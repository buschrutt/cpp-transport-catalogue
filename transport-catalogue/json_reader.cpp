#include <algorithm>
//#include <mmcobj.h>
#include <string>
#include "json_reader.h"

using namespace std::literals;

namespace json_reader {

    void DBBuilder(const json_lib::Document& json_doc, catalogue::TransportCatalogue& catalogue){
        if (json_doc.GetRoot().AsMap().count("base_requests"s) > 0){
            auto db_request_arr = json_doc.GetRoot().AsMap().at("base_requests"s);
            if (std::count(db_request_arr.AsArray().begin(), db_request_arr.AsArray().end(), nullptr)){
                return;
            }
            for (const auto& db_request : db_request_arr.AsArray()){
                auto node = db_request.AsMap().at("type"s);
                if(node.AsString() == "Bus"s){
                    std::vector<std::string> stop_data;
                    for (const auto& stop : db_request.AsMap().at("stops"s).AsArray()){
                        if (stop.IsString()){
                            stop_data.push_back(stop.AsString());
                        }
                    }
                    std::reverse(stop_data.begin(),stop_data.end());
                    catalogue.SetBus(db_request.AsMap().at("name"s).AsString()
                                     , stop_data
                                     , (bool)!db_request.AsMap().at("is_roundtrip"s).AsBool());
                } else if (node.AsString() == "Stop"s){
                    geo::Coordinates coordinates{};
                    coordinates.lat = (double)db_request.AsMap().at("latitude"s).AsDouble();
                    coordinates.lng = (double)db_request.AsMap().at("longitude"s).AsDouble();
                    catalogue.SetStop(db_request.AsMap().at("name"s).AsString()
                                      , coordinates);
                    if(db_request.AsMap().count("road_distances"s) > 0){
                        for (auto [key, length] : db_request.AsMap().at("road_distances"s).AsMap()){
                            catalogue.SetStopDistance(db_request.AsMap().at("name"s).AsString()
                                    , key
                                    , (double)length.AsDouble());
                        }
                    }
                } else {
                    continue;
                }
            }
        }
    }

    json_lib::Document JsonResponseBuilder(const json_lib::Document& json_doc, catalogue::TransportCatalogue& catalogue){
        json_lib::Array json_arr;
        json_lib::Dict json_pair;
        if (json_doc.GetRoot().AsMap().count("stat_requests"s) > 0 ){
            auto db_request_arr = json_doc.GetRoot().AsMap().at("stat_requests"s);
            if (!std::count(db_request_arr.AsArray().begin(), db_request_arr.AsArray().end(), nullptr)){
                for (const auto& db_request : db_request_arr.AsArray()){
                    auto node = db_request.AsMap().at("type"s);
                    if(node.AsString() == "Bus"s){
                        std::string bus_name = db_request.AsMap().at("name"s).AsString();
                        if (catalogue.BusStopCount(bus_name) == 0){
                            json_arr.emplace_back(json_lib::Dict{
                                    {"request_id"s, (int) db_request.AsMap().at("id"s).AsInt(),},
                                    {"error_message"s, "not found"s}
                            });
                        } else {
                            json_arr.emplace_back(json_lib::Dict{
                                    {"request_id"s, (int) db_request.AsMap().at("id"s).AsInt()},
                                    {"stop_count"s, (int) catalogue.BusStopCount(bus_name)},
                                    {"unique_stop_count"s, (int)catalogue.BusUniqStopCount(bus_name)},
                                    {"route_length"s, (double) catalogue.BusRouteLength(bus_name).first},
                                    {"curvature"s, (double) catalogue.BusRouteLength(bus_name).second}
                            });
                        }
                    } else {
                        std::string stop_name = db_request.AsMap().at("name"s).AsString();
                        if (catalogue.GetStopBuses(stop_name).count(":") > 0){
                            json_arr.emplace_back(json_lib::Dict{
                                    {"request_id"s, db_request.AsMap().at("id"s).AsInt(),},
                                    {"error_message"s, "not found"s}
                            });
                        } else {
                            json_lib::Array buffer_arr;
                            for (const std::string& value : catalogue.GetStopBuses(stop_name)){
                                buffer_arr.emplace_back(value);
                            }
                            json_arr.emplace_back(json_lib::Dict{
                                    {"request_id"s, db_request.AsMap().at("id"s).AsInt()},
                                    {"buses"s, buffer_arr}
                            });
                        }
                    }
                }
            }

        }
        return json_lib::Document{json_arr};
    }

    renderer::RenderSettings RenderSettingsBuilder(const json_lib::Document& json_doc){
        renderer::RenderSettings rs;
        if (json_doc.GetRoot().AsMap().count("render_settings"s) > 0 ){
            auto json_rs = json_doc.GetRoot().AsMap().at("render_settings"s);
            rs.bus_label_font_size = json_rs.AsMap().at("bus_label_font_size").AsInt();
            rs.stop_label_font_size = json_rs.AsMap().at("stop_label_font_size").AsInt();
            rs.width = json_rs.AsMap().at("width").AsDouble();
            rs.height = json_rs.AsMap().at("height").AsDouble();
            rs.padding = json_rs.AsMap().at("padding").AsDouble();
            rs.stop_radius = json_rs.AsMap().at("stop_radius").AsDouble();
            rs.line_width = json_rs.AsMap().at("line_width").AsDouble();
            rs.underlayer_width = json_rs.AsMap().at("underlayer_width").AsDouble();
            rs.bus_label_offset.first = json_rs.AsMap().at("bus_label_offset").AsArray()[0].AsDouble();
            rs.bus_label_offset.second = json_rs.AsMap().at("bus_label_offset").AsArray()[1].AsDouble();
            rs.stop_label_offset.second = json_rs.AsMap().at("stop_label_offset").AsArray()[0].AsDouble();
            rs.stop_label_offset.second = json_rs.AsMap().at("stop_label_offset").AsArray()[1].AsDouble();
            // %%%%%%%%%% %%%%%%%%%% Color underlayer_color %%%%%%%%%% %%%%%%%%%%
            if (json_rs.AsMap().at("underlayer_color").IsArray()){
                std::string color_code;
                std::string capacity;
                if (json_rs.AsMap().at("underlayer_color").AsArray().size() == 4){
                    capacity = "," + std::to_string(json_rs.AsMap().at("underlayer_color").AsArray()[3].AsDouble()) + ")";
                    color_code = "rgba(";
                } else {
                    color_code = "rgb(";
                    capacity = ")";
                }
                for (int i = 0; i < 3; i++){
                    if (i != 0){color_code += ",";}
                    color_code += std::to_string(json_rs.AsMap().at("underlayer_color").AsArray()[i].AsInt());
                }
                color_code += capacity;
                rs.underlayer_color = color_code;
            } else {
                rs.underlayer_color = json_rs.AsMap().at("underlayer_color").AsString();
            }
            // %%%%%%%%%% %%%%%%%%%% std::vector<Color> color_palette %%%%%%%%%% %%%%%%%%%%
            for (const auto& color : json_rs.AsMap().at("color_palette").AsArray()){
                if (color.IsArray()){
                    std::string color_code;
                    std::string capacity;
                    if (color.AsArray().size() == 4){
                        capacity = "," + std::to_string(color.AsArray()[3].AsDouble()) + ")";
                        color_code = "rgba(";
                    } else {
                        color_code = "rgb(";
                        capacity = ")";
                    }
                    for (int i = 0; i < 3; i++){
                        if (i != 0){color_code += ",";}
                        color_code += std::to_string(color.AsArray()[i].AsInt());
                    }
                    color_code += capacity;
                    rs.color_palette.emplace_back(color_code);
                } else {
                    rs.color_palette.emplace_back(color.AsString());
                }
            }
            // END OF %%%%%%%%%% %%%%%%%%%% std::vector<Color> color_palette %%%%%%%%%% %%%%%%%%%%
        }
        return rs;
    }

}