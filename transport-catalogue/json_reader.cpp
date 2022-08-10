#include <algorithm>
//#include <mmcobj.h>
#include <string>
#include "json_reader.h"


namespace json_reader {

    void DBBuilder(const json_lib::Document& json_doc, catalogue::TransportCatalogue& catalogue){
        if (json_doc.GetRoot().AsMap().count("base_requests") > 0 ){
            auto db_request_arr = json_doc.GetRoot().AsMap().at("base_requests");
            for (const auto& db_request : db_request_arr.AsArray()){
                auto node = db_request.AsMap().at("type");
                if(node.AsString() == "Bus"){
                    std::vector<std::string> stop_data;
                    for (const auto& stop : db_request.AsMap().at("stops").AsArray()){
                        stop_data.push_back(stop.AsString());
                    }
                    std::reverse(stop_data.begin(),stop_data.end());
                    catalogue.SetBus(db_request.AsMap().at("name").AsString()
                                     , stop_data
                                     , !db_request.AsMap().at("is_roundtrip").AsBool());
                } else {
                    geo::Coordinates coordinates{};
                    coordinates.lat = db_request.AsMap().at("latitude").AsDouble();
                    coordinates.lng = db_request.AsMap().at("longitude").AsDouble();
                    catalogue.SetStop(db_request.AsMap().at("name").AsString()
                                      , coordinates);
                    if(db_request.AsMap().count("road_distances") > 0){
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

    json_lib::Document JsonResponseBuilder(const json_lib::Document& json_doc, catalogue::TransportCatalogue& catalogue){
        json_lib::Array json_arr;
        json_lib::Dict json_pair;
        if (json_doc.GetRoot().AsMap().count("stat_requests") > 0 ){
            auto db_request_arr = json_doc.GetRoot().AsMap().at("stat_requests");
            for (const auto& db_request : db_request_arr.AsArray()){
                auto node = db_request.AsMap().at("type");
                if(node.AsString() == "Bus"){
                    std::string bus_name = db_request.AsMap().at("name").AsString();
                    if (catalogue.BusStopCount(bus_name) == 0){
                        json_arr.emplace_back(json_lib::Dict{
                                {"request_id", db_request.AsMap().at("id").AsInt(),},
                                {"error_message", "not found"}
                        });
                    } else {
                        json_arr.emplace_back(json_lib::Dict{
                                {"request_id:", db_request.AsMap().at("id").AsInt()},
                                {"stop_count:", (int) catalogue.BusStopCount(bus_name)},
                                {"unique_stop_count:", (int)catalogue.BusUniqStopCount(bus_name)},
                                {"route_length:", catalogue.BusRouteLength(bus_name).first},
                                {"curvature:", catalogue.BusRouteLength(bus_name).second}
                        });
                    }
                } else {
                    std::string stop_name = db_request.AsMap().at("name").AsString();
                    if (catalogue.GetStopBuses(stop_name).count(":") > 0){
                        json_arr.emplace_back(json_lib::Dict{
                                {"request_id", db_request.AsMap().at("id").AsInt(),},
                                {"error_message", "not found"}
                        });
                    } else {
                        json_lib::Array buffer_arr;
                        for (const std::string& value : catalogue.GetStopBuses(stop_name)){
                            buffer_arr.emplace_back(value);
                        }
                        json_arr.emplace_back(json_lib::Dict{
                                {"request_id:", db_request.AsMap().at("id").AsInt()},
                                {"buses", buffer_arr}
                        });
                    }
                }
            }
        }
        return json_lib::Document{json_arr};
    }

    void func(std::stringstream & s){

    }

}