#include <algorithm>
//#include <mmcobj.h>
#include <string>
#include "json_reader.h"
#include "json_builder.h"
#include "transport_router.h"
//#include "log_duration.h"

using namespace std::literals;

namespace json_reader {

    renderer::RenderSettings JSONReader::RenderSettingsBuilder(const json::Document& json_doc){
        //LOG_DURATION("RenderSettingsBuilder"s);
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

    domain::RoutingSettings JSONReader::RoutingSettingsBuilder(const json::Document& json_doc) {
        //LOG_DURATION("RoutingSettingsBuilder"s);
        domain::RoutingSettings rs;
        if (json_doc.GetRoot().AsDict().count("routing_settings"s) > 0) {
            auto json_rs = json_doc.GetRoot().AsDict().at("routing_settings"s);
            rs.bus_wait_time = json_rs.AsDict().at("bus_wait_time").AsInt();
            rs.bus_velocity = json_rs.AsDict().at("bus_velocity").AsInt();
        }
        return rs;
    }

    void JSONReader::DBBuilder(){
        //LOG_DURATION("DBBuilder"s);
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
        //LOG_DURATION("JsonResponseBuilder"s);
        router::CustomRouteFinder router(this->routing_settings_, catalogue_);
        json::Builder j_builder{};
        if (json_doc_.GetRoot().AsDict().count("stat_requests"s) > 0 ){
            auto db_request_arr = json_doc_.GetRoot().AsDict().at("stat_requests"s);
            j_builder.StartArray(); // %%%%% start root array
            if (!std::count(db_request_arr.AsArray().begin(), db_request_arr.AsArray().end(), nullptr)){
                for (const auto& db_request : db_request_arr.AsArray()){
                    auto node = db_request.AsDict().at("type"s);
                    if(node.AsString() == "Bus"s){
                        j_builder.StartDict();
                        std::string bus_name = db_request.AsDict().at("name"s).AsString();
                        if ((int) catalogue_.BusStopCount(bus_name) == -1){
                            j_builder
                                    .Key("request_id"s).Value((int) db_request.AsDict().at("id"s).AsInt())
                                    .Key("error_message"s).Value("not found"s);
                        } else {
                            j_builder
                                    .Key("request_id"s).Value((int) db_request.AsDict().at("id"s).AsInt())
                                    .Key("stop_count"s).Value((int) catalogue_.BusStopCount(bus_name))
                                    .Key("unique_stop_count"s).Value((int)catalogue_.BusUniqStopCount(bus_name))
                                    .Key("route_length"s).Value((double) catalogue_.BusRouteLength(bus_name).first)
                                    .Key("curvature"s).Value((double) catalogue_.BusRouteLength(bus_name).second);
                        }
                        j_builder.EndDict();
                    } else if(node.AsString() == "Stop"s){
                        j_builder.StartDict();
                        std::string stop_name = db_request.AsDict().at("name"s).AsString();
                        if (catalogue_.GetStopBuses(stop_name).count(nullptr)){
                            j_builder
                                    .Key("request_id"s).Value(db_request.AsDict().at("id"s).AsInt())
                                    .Key("error_message"s).Value("not found"s);
                        } else {
                            json::Array buffer_arr;
                            std::set<std::string> sort_buses;
                            j_builder
                                    .Key("request_id"s).Value(db_request.AsDict().at("id"s).AsInt())
                                    .Key("buses"s).StartArray();
                            for (const auto value : catalogue_.GetStopBuses(stop_name)){
                                sort_buses.insert(value->name);// %%%%%% ??????
                            }
                            for (const auto& value : sort_buses){
                                j_builder.Value(value);
                                buffer_arr.emplace_back(value);
                            }
                            j_builder.EndArray();
                        }
                        j_builder.EndDict();
                    } else if (node.AsString() == "Map"s){
                        j_builder.StartDict();
                        renderer::MapRenderer map_renderer;
                        json::Node svg_map;
                        svg_map = map_renderer.DrawSvgMap(catalogue_, this->render_settings_);
                        j_builder
                                .Key("request_id"s).Value(db_request.AsDict().at("id"s).AsInt())
                                .Key("map"s).Value(svg_map.AsString());
                        j_builder.EndDict();
                    } else if (node.AsString() == "Route"s){
                        j_builder.StartDict();
                        std::string from_name = db_request.AsDict().at("from"s).AsString();
                        std::string to_name = db_request.AsDict().at("to"s).AsString();
                        std::pair<double, std::vector<std::variant<domain::Wait, domain::Ride>>> found_route = router.RouteSearch(from_name, to_name, catalogue_);
                        j_builder.Key("request_id").Value(db_request.AsDict().at("id"s).AsInt());
                        if (found_route.first == -1.0){
                            j_builder.Key("error_message"s).Value("not found"s);
                        } else {
                            j_builder.Key("total_time").Value(found_route.first);
                            j_builder.Key("items"s).StartArray();
                            for (auto edge : found_route.second){
                                if (std::holds_alternative<domain::Wait>(edge)){
                                    j_builder.StartDict()
                                            .Key("type"s).Value("Wait")
                                            .Key("stop_name").Value(std::get<domain::Wait>(edge).stop->name)
                                            .Key("time"s).Value(std::get<domain::Wait>(edge).time)
                                            .EndDict();
                                } else {
                                    j_builder.StartDict()
                                            .Key("type"s).Value("Bus")
                                            .Key("bus").Value(std::get<domain::Ride>(edge).bus->name)
                                            .Key("time"s).Value(std::get<domain::Ride>(edge).time)
                                            .Key("span_count"s).Value(std::get<domain::Ride>(edge).span_count)
                                            .EndDict();
                                }
                            }
                            j_builder.EndArray();
                        }
                        j_builder.EndDict();
                    }
                }
            }
            j_builder.EndArray(); // %%%%% end root array
        }
        result_json_doc_ = json::Document{j_builder.Build()};
        return result_json_doc_;
    }
}