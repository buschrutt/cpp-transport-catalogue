#include "serialization.h"
#include "json_builder.h"

using namespace std::literals;

namespace data_serialization {

    void Serializer::ToFileSerializer() {
        //LOG_DURATION("RequestSerializer"s);

        // Start of %%%%% %%%%% Processing .json %%%%% %%%%%
        if (json_doc_.GetRoot().AsDict().count("serialization_settings"s) > 0){
            proto_serialization::TransportCatalogue proto_catalogue;
            proto_serialization::RenderSettings render_setting;
            auto db_settings_dict = json_doc_.GetRoot().AsDict().at("serialization_settings"s);
            auto file_name = db_settings_dict.AsDict().at("file"s);
            const std::string& file_path = "db_files/" + file_name.AsString();
            std::ofstream out_file(file_path, std::ios::binary);

            // Start of %%%%% %%%%% Getting Render Settings %%%%% %%%%%
            if (json_doc_.GetRoot().AsDict().count("render_settings"s) > 0 ){
                auto json_rs = json_doc_.GetRoot().AsDict().at("render_settings"s);
                proto_catalogue.mutable_render_set()->set_weight(json_rs.AsDict().at("width").AsDouble());
                proto_catalogue.mutable_render_set()->set_height(json_rs.AsDict().at("height").AsDouble());
                proto_catalogue.mutable_render_set()->set_bus_label_font_size(json_rs.AsDict().at("bus_label_font_size").AsInt());
                proto_catalogue.mutable_render_set()->set_stop_label_font_size(json_rs.AsDict().at("stop_label_font_size").AsInt());
                proto_catalogue.mutable_render_set()->set_padding(json_rs.AsDict().at("padding").AsDouble());
                proto_catalogue.mutable_render_set()->set_stop_radius(json_rs.AsDict().at("stop_radius").AsDouble());
                proto_catalogue.mutable_render_set()->set_line_width(json_rs.AsDict().at("line_width").AsDouble());
                proto_catalogue.mutable_render_set()->set_underlayer_width(json_rs.AsDict().at("underlayer_width").AsDouble());
                proto_catalogue.mutable_render_set()->mutable_bus_label_offset()->set_x(json_rs.AsDict().at("bus_label_offset").AsArray()[0].AsDouble());
                proto_catalogue.mutable_render_set()->mutable_bus_label_offset()->set_y(json_rs.AsDict().at("bus_label_offset").AsArray()[1].AsDouble());
                proto_catalogue.mutable_render_set()->mutable_stop_label_offset()->set_x(json_rs.AsDict().at("stop_label_offset").AsArray()[0].AsDouble());
                proto_catalogue.mutable_render_set()->mutable_stop_label_offset()->set_y(json_rs.AsDict().at("stop_label_offset").AsArray()[1].AsDouble());
                // %%%%% Color underlayer_color
                if (json_rs.AsDict().at("underlayer_color").IsArray()){
                    if (json_rs.AsDict().at("underlayer_color").AsArray().size() == 3){
                        proto_catalogue.mutable_render_set()->mutable_underlayer_color()->mutable_rgb()->set_red(json_rs.AsDict().at("underlayer_color").AsArray()[0].AsInt());
                        proto_catalogue.mutable_render_set()->mutable_underlayer_color()->mutable_rgb()->set_green(json_rs.AsDict().at("underlayer_color").AsArray()[1].AsInt());
                        proto_catalogue.mutable_render_set()->mutable_underlayer_color()->mutable_rgb()->set_blue(json_rs.AsDict().at("underlayer_color").AsArray()[2].AsInt());
                    } else {
                        proto_catalogue.mutable_render_set()->mutable_underlayer_color()->mutable_rgba()->set_red(json_rs.AsDict().at("underlayer_color").AsArray()[0].AsInt());
                        proto_catalogue.mutable_render_set()->mutable_underlayer_color()->mutable_rgba()->set_green(json_rs.AsDict().at("underlayer_color").AsArray()[1].AsInt());
                        proto_catalogue.mutable_render_set()->mutable_underlayer_color()->mutable_rgba()->set_blue(json_rs.AsDict().at("underlayer_color").AsArray()[2].AsInt());
                        proto_catalogue.mutable_render_set()->mutable_underlayer_color()->mutable_rgba()->set_opacity(json_rs.AsDict().at("underlayer_color").AsArray()[3].AsDouble());
                    }
                } else {
                    proto_catalogue.mutable_render_set()->mutable_underlayer_color()->set_str(json_rs.AsDict().at("underlayer_color").AsString());
                }
                // %%%%% Repeated Color color_palette
                for (const auto& color : json_rs.AsDict().at("color_palette").AsArray()){
                    if (color.IsArray()){
                        if (color.AsArray().size() == 3){
                            proto_serialization::Color clr;
                            clr.mutable_rgb()->set_red(color.AsArray()[0].AsInt());
                            clr.mutable_rgb()->set_green(color.AsArray()[1].AsInt());
                            clr.mutable_rgb()->set_blue(color.AsArray()[2].AsInt());
                            proto_catalogue.mutable_render_set()->mutable_color_palette()->Add(std::move(clr));
                        } else {
                            proto_serialization::Color clr;
                            clr.mutable_rgba()->set_red(color.AsArray()[0].AsInt());
                            clr.mutable_rgba()->set_green(color.AsArray()[1].AsInt());
                            clr.mutable_rgba()->set_blue(color.AsArray()[2].AsInt());
                            clr.mutable_rgba()->set_opacity(color.AsArray()[3].AsDouble());
                        }
                    } else {
                        proto_catalogue.mutable_render_set()->mutable_color_palette()->Add()->set_str(color.AsString());
                    }
                }
            }
            // End of %%%%% %%%%% Getting Render Settings %%%%% %%%%%

            // Start of %%%%% %%%%% Getting Routing Settings %%%%% %%%%%
            if (json_doc_.GetRoot().AsDict().count("routing_settings"s) > 0) {
                auto json_rs = json_doc_.GetRoot().AsDict().at("routing_settings"s);
                proto_catalogue.mutable_routing_set()->set_bus_wait_time(json_rs.AsDict().at("bus_wait_time").AsInt());
                proto_catalogue.mutable_routing_set()->set_bus_velocity(json_rs.AsDict().at("bus_velocity").AsInt());
            }
            // End of %%%%% %%%%% Getting Routing Settings %%%%% %%%%%

            // Start of %%%%% %%%%% Getting stop and bus data %%%%% %%%%%
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
            // End of %%%%% %%%%% Getting stop and bus data %%%%% %%%%%

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
            proto_catalogue.ParseFromIstream(&in_file); // all db data here

            // Start of %%%%% %%%%% Getting stop and bus data %%%%% %%%%%
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
            // End of %%%%% %%%%% Getting stop and bus data %%%%% %%%%%

            // Start of %%%%% %%%%% Getting Render Settings %%%%% %%%%%
            render_settings_.width = proto_catalogue.render_set().weight();
            render_settings_.height = proto_catalogue.render_set().height();
            render_settings_.bus_label_font_size = proto_catalogue.render_set().bus_label_font_size();
            render_settings_.stop_label_font_size = proto_catalogue.render_set().stop_label_font_size();
            render_settings_.padding = proto_catalogue.render_set().padding();
            render_settings_.stop_radius = proto_catalogue.render_set().stop_radius();
            render_settings_.line_width = proto_catalogue.render_set().line_width();
            render_settings_.underlayer_width = proto_catalogue.render_set().underlayer_width();
            render_settings_.bus_label_offset.first = proto_catalogue.render_set().bus_label_offset().x();
            render_settings_.bus_label_offset.second = proto_catalogue.render_set().bus_label_offset().y();
            render_settings_.stop_label_offset.first = proto_catalogue.render_set().stop_label_offset().x();
            render_settings_.stop_label_offset.second = proto_catalogue.render_set().stop_label_offset().y();
            if (proto_catalogue.render_set().underlayer_color().has_rgba()){
                render_settings_.underlayer_color = svg::Rgba(proto_catalogue.render_set().underlayer_color().rgba().red()
                        ,proto_catalogue.render_set().underlayer_color().rgba().green()
                        ,proto_catalogue.render_set().underlayer_color().rgba().blue()
                        ,proto_catalogue.render_set().underlayer_color().rgba().opacity());
            } else if (proto_catalogue.render_set().underlayer_color().has_rgb()) {
                render_settings_.underlayer_color = svg::Rgb(proto_catalogue.render_set().underlayer_color().rgb().red()
                        ,proto_catalogue.render_set().underlayer_color().rgb().green()
                        ,proto_catalogue.render_set().underlayer_color().rgb().blue());
            } else {
                render_settings_.underlayer_color = proto_catalogue.render_set().underlayer_color().str();
            }
            for (const auto& color : proto_catalogue.render_set().color_palette()){
                if (color.has_rgba()){
                    svg::Color set_color = svg::Rgba(color.rgba().red()
                            , color.rgba().green()
                            , color.rgba().blue()
                            , color.rgba().opacity());
                    render_settings_.color_palette.push_back(set_color);
                } else if (color.has_rgb()) {
                    svg::Color set_color = svg::Rgb(color.rgb().red()
                            , color.rgb().green()
                            ,  color.rgb().blue());
                    render_settings_.color_palette.push_back(set_color);
                } else {
                    svg::Color set_color = color.str();
                    render_settings_.color_palette.push_back(set_color);
                }
            }
            // End of %%%%% %%%%% Getting Render Settings %%%%% %%%%%

            // Start of %%%%% %%%%% Getting Routing Settings %%%%% %%%%%
            routing_settings_.bus_velocity = proto_catalogue.routing_set().bus_velocity();
            routing_settings_.bus_wait_time = proto_catalogue.routing_set().bus_wait_time();
            // End of %%%%% %%%%% Getting Routing Settings %%%%% %%%%%

            // %%%%% %%%%% json::Document building %%%%% %%%%%
            json_reader::JSONReader json_data(json_doc_, catalogue_, render_settings_, routing_settings_);
            return json_data.JsonResponseBuilder();
        }
        return json::Document({});
    }
}
