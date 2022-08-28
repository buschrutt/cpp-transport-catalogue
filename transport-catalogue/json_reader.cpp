#include <algorithm>
//#include <mmcobj.h>
#include <string>
#include "json_reader.h"

using namespace std::literals;

namespace json_reader {

    renderer::RenderSettings JSONReader::RenderSettingsBuilder(const json::Document& json_doc){
        renderer::RenderSettings rs;
        if (json_doc.GetRoot().AsDict().count("render_settings"s) > 0 ){
            auto json_rs = json_doc.GetRoot().AsDict().at("render_settings"s);
            rs.bus_label_font_size = json_rs.AsDict().at("bus_label_font_size").AsInt();
            rs.stop_label_font_size = json_rs.AsDict().at("stop_label_font_size").AsInt();
            rs.width = json_rs.AsDict().at("width").AsDouble();
            rs.height = json_rs.AsDict().at("height").AsDouble();
            rs.padding = json_rs.AsDict().at("padding").AsDouble();
            rs.stop_radius = json_rs.AsDict().at("stop_radius").AsDouble();
            rs.line_width = json_rs.AsDict().at("line_width").AsDouble();
            rs.underlayer_width = json_rs.AsDict().at("underlayer_width").AsDouble();
            rs.bus_label_offset.first = json_rs.AsDict().at("bus_label_offset").AsArray()[0].AsDouble();
            rs.bus_label_offset.second = json_rs.AsDict().at("bus_label_offset").AsArray()[1].AsDouble();
            rs.stop_label_offset.first = json_rs.AsDict().at("stop_label_offset").AsArray()[0].AsDouble();
            rs.stop_label_offset.second = json_rs.AsDict().at("stop_label_offset").AsArray()[1].AsDouble();
            // %%%%%%%%%% %%%%%%%%%% Color underlayer_color %%%%%%%%%% %%%%%%%%%%
            if (json_rs.AsDict().at("underlayer_color").IsArray()){
                if (json_rs.AsDict().at("underlayer_color").AsArray().size() == 3){
                    rs.underlayer_color = svg::Rgb(json_rs.AsDict().at("underlayer_color").AsArray()[0].AsInt()
                            ,json_rs.AsDict().at("underlayer_color").AsArray()[1].AsInt()
                            ,json_rs.AsDict().at("underlayer_color").AsArray()[2].AsInt());
                } else {
                    rs.underlayer_color = svg::Rgba(json_rs.AsDict().at("underlayer_color").AsArray()[0].AsInt()
                            ,json_rs.AsDict().at("underlayer_color").AsArray()[1].AsInt()
                            ,json_rs.AsDict().at("underlayer_color").AsArray()[2].AsInt()
                            ,json_rs.AsDict().at("underlayer_color").AsArray()[3].AsDouble());
                }
            } else {
                rs.underlayer_color = json_rs.AsDict().at("underlayer_color").AsString();
            }
            // %%%%%%%%%% %%%%%%%%%% std::vector<Color> color_palette %%%%%%%%%% %%%%%%%%%%
            for (const auto& color : json_rs.AsDict().at("color_palette").AsArray()){
                if (color.IsArray()){
                    if (color.AsArray().size() == 3){
                        rs.color_palette.emplace_back(svg::Rgb(color.AsArray()[0].AsInt()
                                ,color.AsArray()[1].AsInt()
                                ,color.AsArray()[2].AsInt()));
                    } else {
                        rs.color_palette.emplace_back(svg::Rgba(color.AsArray()[0].AsInt()
                                ,color.AsArray()[1].AsInt()
                                ,color.AsArray()[2].AsInt()
                                ,color.AsArray()[3].AsDouble()));
                    }
                } else {
                    rs.color_palette.emplace_back(color.AsString());
                }
            }
            // END OF %%%%%%%%%% %%%%%%%%%% std::vector<Color> color_palette %%%%%%%%%% %%%%%%%%%%
        }
        return rs;
    }

    void JSONReader::DBBuilder(){
        if (json_doc_.GetRoot().AsDict().count("base_requests"s) > 0){
            auto db_request_arr = json_doc_.GetRoot().AsDict().at("base_requests"s);
            if (std::count(db_request_arr.AsArray().begin(), db_request_arr.AsArray().end(), nullptr)){
                return;
            }
            for (const auto& db_request : db_request_arr.AsArray()){
                auto node = db_request.AsDict().at("type"s);
                if(node.AsString() == "Bus"s){
                    std::vector<std::string> stop_data;
                    for (const auto& stop : db_request.AsDict().at("stops"s).AsArray()){
                        if (stop.IsString()){
                            stop_data.push_back(stop.AsString());
                        }
                    }
                    std::reverse(stop_data.begin(),stop_data.end());
                    catalogue_.SetBus(db_request.AsDict().at("name"s).AsString()
                            , stop_data
                            , (bool)!db_request.AsDict().at("is_roundtrip"s).AsBool());
                } else if (node.AsString() == "Stop"s){
                    geo::Coordinates coordinates{};
                    coordinates.lat = (double)db_request.AsDict().at("latitude"s).AsDouble();
                    coordinates.lng = (double)db_request.AsDict().at("longitude"s).AsDouble();
                    catalogue_.SetStop(db_request.AsDict().at("name"s).AsString()
                            , coordinates);
                    if(db_request.AsDict().count("road_distances"s) > 0){
                        for (auto [key, length] : db_request.AsDict().at("road_distances"s).AsDict()){
                            catalogue_.SetStopDistance(db_request.AsDict().at("name"s).AsString()
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

    json::Document JSONReader::JsonResponseBuilder(){
        json::Array json_arr;
        json::Dict json;
        if (json_doc_.GetRoot().AsDict().count("stat_requests"s) > 0 ){
            auto db_request_arr = json_doc_.GetRoot().AsDict().at("stat_requests"s);
            if (!std::count(db_request_arr.AsArray().begin(), db_request_arr.AsArray().end(), nullptr)){
                for (const auto& db_request : db_request_arr.AsArray()){
                    auto node = db_request.AsDict().at("type"s);
                    if(node.AsString() == "Bus"s){
                        std::string bus_name = db_request.AsDict().at("name"s).AsString();
                        if ((int) catalogue_.BusStopCount(bus_name) == -1){
                            json_arr.emplace_back(json::Dict{
                                    {"request_id"s, (int) db_request.AsDict().at("id"s).AsInt(),},
                                    {"error_message"s, "not found"s}
                            });
                        } else {
                            json_arr.emplace_back(json::Dict{
                                    {"request_id"s, (int) db_request.AsDict().at("id"s).AsInt()},
                                    {"stop_count"s, (int) catalogue_.BusStopCount(bus_name)},
                                    {"unique_stop_count"s, (int)catalogue_.BusUniqStopCount(bus_name)},
                                    {"route_length"s, (double) catalogue_.BusRouteLength(bus_name).first},
                                    {"curvature"s, (double) catalogue_.BusRouteLength(bus_name).second}
                            });
                        }
                    } else if(node.AsString() == "Stop"s){
                        std::string stop_name = db_request.AsDict().at("name"s).AsString();
                        if (catalogue_.GetStopBuses(stop_name).count(nullptr)){
                            json_arr.emplace_back(json::Dict{
                                    {"request_id"s, db_request.AsDict().at("id"s).AsInt(),},
                                    {"error_message"s, "not found"s}
                            });
                        } else {
                            json::Array buffer_arr;
                            std::set<std::string> sort_buses;
                            for (const auto value : catalogue_.GetStopBuses(stop_name)){
                                sort_buses.insert(value->name);// %%%%%% ??????
                            }
                            for (const auto& value : sort_buses){
                                buffer_arr.emplace_back(value);
                            }
                            json_arr.emplace_back(json::Dict{
                                    {"request_id"s, db_request.AsDict().at("id"s).AsInt()},
                                    {"buses"s, buffer_arr}
                            });
                        }
                    } else if (node.AsString() == "Map"s){
                        renderer::MapRenderer map_renderer;
                        json::Node svg_map;
                        svg_map = map_renderer.DrawSvgMap(catalogue_, RenderSettingsBuilder(json_doc_));
                        json_arr.emplace_back(json::Dict{
                                {"request_id"s, db_request.AsDict().at("id"s).AsInt()},
                                {"map"s, svg_map.AsString()}
                        });
                    }
                }
            }

        }
        result_json_doc_ = json::Document{json_arr};
        return result_json_doc_;
    }

}