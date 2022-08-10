#include <algorithm>
#include "json_reader.h"


namespace json_reader {

    void DBBuilder(const json_lib::Document& json_doc, catalogue::TransportCatalogue& catalogue){
        if (json_doc.GetRoot().AsMap().count("base_requests") > 0 ){
            auto db_request_arr = json_doc.GetRoot().AsMap().at("base_requests");
            for (const auto& db_request : db_request_arr.AsArray()){
                auto dd = db_request.AsMap().at("type");
                if(dd.AsString() == "Bus"){
                    std::vector<std::string> stop_data;
                    for (const auto& stop : db_request.AsMap().at("stops").AsArray()){
                        stop_data.push_back(stop.AsString());
                    }
                    std::reverse(stop_data.begin(),stop_data.end());
                    catalogue.SetBus(db_request.AsMap().at("name").AsString()
                                     , stop_data
                                     , db_request.AsMap().at("is_roundtrip").AsBool());
                } else {
                    geo::Coordinates coordinates{};
                    coordinates.lat = db_request.AsMap().at("latitude").AsDouble();
                    coordinates.lng = db_request.AsMap().at("longitude").AsDouble();
                    catalogue.SetStop(db_request.AsMap().at("name").AsString()
                                      , coordinates);
                    for (auto [key, length] : db_request.AsMap().at("road_distances").AsMap()){
                        catalogue.SetStopDistance(db_request.AsMap().at("name").AsString()
                                        , key
                                        , length.AsDouble());
                    }
                }
            }
        }
    }


}