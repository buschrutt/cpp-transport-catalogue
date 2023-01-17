#include "serialization.h"
#include "json_builder.h"

using namespace std::literals;

namespace data_serialization {

    void Serializer::ToFileSerializer() {
        //LOG_DURATION("RequestSerializer"s);
        //json_reader::JSONReader::RenderSettingsBuilder(json_doc_);
        //json_reader::JSONReader::RoutingSettingsBuilder(json_doc_);
        if (json_doc_.GetRoot().AsDict().count("serialization_settings"s) > 0){
            proto_serialization::TransportCatalogue proto_catalogue;
            auto db_settings_dict = json_doc_.GetRoot().AsDict().at("serialization_settings"s);
            auto file_name = db_settings_dict.AsDict().at("file"s);
            const std::string& file_path = "db_files/" + file_name.AsString();
            std::ofstream out_file(file_path, std::ios::binary);

            // %%%%% %%%%% Reading JSON %%%%% %%%%%
            if (json_doc_.GetRoot().AsDict().count("base_requests"s) > 0){
                auto db_request_arr = json_doc_.GetRoot().AsDict().at("base_requests"s);
                if (std::count(db_request_arr.AsArray().begin(), db_request_arr.AsArray().end(), nullptr)){
                    return;
                }
                for (const auto& db_request : db_request_arr.AsArray()){
                    auto node = db_request.AsDict().at("type"s);
                    if(node.AsString() == "Bus"s){
                        proto_serialization::Bus bus;
                        for (const auto& stop : db_request.AsDict().at("stops"s).AsArray()){
                            if (stop.IsString()){
                                bus.mutable_stop_ids()->Add(SetStopByName(stop.AsString()));
                            }
                        }
                        bus.set_name(db_request.AsDict().at("name"s).AsString());
                        bus.set_is_roundtrip(db_request.AsDict().at("is_roundtrip"s).AsBool());
                        proto_catalogue.mutable_buses()->Add(std::move(bus));
                    } else if (node.AsString() == "Stop"s){
                        std::string stop_name = db_request.AsDict().at("name"s).AsString();
                        int stop_id = SetStopByName(stop_name);
                        proto_serialization::Stop stop;
                        proto_serialization::Distance distance{};
                        stop.set_name(stop_name);
                        stop.set_id(stop_id);
                        stop.set_latitude((double)db_request.AsDict().at("latitude"s).AsDouble());
                        stop.set_longitude((double)db_request.AsDict().at("longitude"s).AsDouble());
                        proto_catalogue.mutable_stops()->Add(std::move(stop));
                        if(db_request.AsDict().count("road_distances"s) > 0){
                            for (auto [key, length] : db_request.AsDict().at("road_distances"s).AsDict()){
                                distance.set_stop_from_id(stop_id);
                                distance.set_stop_to_id(SetStopByName(key));
                                distance.set_distance((double)length.AsDouble());
                                proto_catalogue.mutable_distances()->Add(std::move(distance));
                            }
                        }
                    } else {
                        continue;
                    }
                }
            }
            // End of %%%%% %%%%% Reading JSON %%%%% %%%%%

            // %%%%% %%%%% Serializing %%%%% %%%%%
            proto_catalogue.SerializeToOstream(&out_file);

        } else {
            return;
        }
    }

    int Serializer::SetStopByName(const std::string& stop_name) {
        if (stop_ids_.count(stop_name) == 0) {
            CURRENT_ID_++;
            stop_ids_.insert({stop_name, CURRENT_ID_});
        }
        return stop_ids_.at(stop_name);
    }

    json::Document Serializer::FromFileDeserializer() {
        if (json_doc_.GetRoot().AsDict().count("serialization_settings"s) > 0) {
            proto_serialization::TransportCatalogue proto_catalogue;
            auto db_settings_dict = json_doc_.GetRoot().AsDict().at("serialization_settings"s);
            auto file_name = db_settings_dict.AsDict().at("file"s);
            const std::string& file_path = "db_files/" + file_name.AsString();
            std::ifstream in_file(file_path, std::ios::binary);
            proto_catalogue.ParseFromIstream(&in_file);
            for (const auto& stop : proto_catalogue.stops()){
                geo::Coordinates coordinates{};
                coordinates.lat = stop.latitude();
                coordinates.lng = stop.longitude();
                catalogue_.SetStop(stop.name(), coordinates);
                id_stops_.insert({stop.id(), stop.name()});
            }
            std::string from; std::string to;
            for (const auto& distance : proto_catalogue.distances()){
                catalogue_.SetStopDistance(id_stops_.at(distance.stop_from_id()), id_stops_.at(distance.stop_to_id()), distance.distance());
            }
            for (const auto& bus : proto_catalogue.buses()){
                std::vector<std::string> stop_data;
                for (auto stop_id : bus.stop_ids()){
                    stop_data.push_back(id_stops_.at(stop_id));
                }
                std::reverse(stop_data.begin(), stop_data.end());
                catalogue_.SetBus(bus.name(), stop_data, !bus.is_roundtrip());
            }

            // %%%%% %%%%% json::Document building %%%%% %%%%%
            json_reader::JSONReader json_data(json_doc_, catalogue_);
            return json_data.JsonResponseBuilder();
        }
        return json::Document({});
    }
}
