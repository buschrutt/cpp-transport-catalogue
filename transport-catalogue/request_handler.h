#pragma once

#include <string>

namespace handler {

    using namespace std::literals;

    class CustomRouteFinder{
    public:
        explicit CustomRouteFinder(domain::RoutingSettings routing_settings){
            speed_factor_ = 1 / ((routing_settings.bus_velocity * 1000) / 60);
            wait_factor_ = routing_settings.bus_wait_time;
        };

        size_t GetLastId(){
            return vertex_ids_.size();
        }

        void GetVertexes(catalogue::TransportCatalogue & catalogue){
            std::map<std::string, domain::Stop*> all_stops = catalogue.GetAllStops();
            std::map<std::string, domain::Bus*> all_buses = catalogue.GetAllBuses();
            for (const auto& stop : all_stops){
                size_t id = GetLastId();
                vertex_ids_[id + 1] = {id + 1, {}, stop.second};
                start_ids_[stop.second] = id + 1;
                stop_vertexes_[stop.second].push_back(&vertex_ids_.at(id + 1));
            }
            for (const auto& [bus_name, bus] : all_buses){
                std::vector<domain::VertexData*> bus_vertexes;
                for (size_t i = bus->route.size(); i > 0; i--){
                    size_t id = GetLastId();
                    vertex_ids_[id + 1] = {id + 1, bus, bus->route[i - 1]};
                    bus_vertexes.push_back(&vertex_ids_.at(id + 1));
                    stop_vertexes_[bus->route[i - 1]].push_back(&vertex_ids_.at(id + 1));
                }
                if (bus->is_chain){
                    for (size_t i = 0; i < bus->route.size(); i++){
                        size_t id = GetLastId();
                        vertex_ids_[id + 1] = {id + 1, bus, bus->route[i]};
                        bus_vertexes.push_back(&vertex_ids_.at(id + 1));
                        stop_vertexes_[bus->route[i]].push_back(&vertex_ids_.at(id + 1));
                    }
                }
                bus_route_vertexes_[bus] = bus_vertexes;
            }
        }

        void GetEdges(catalogue::TransportCatalogue & catalogue) {
            // in stop edges making
            for (auto [stop, vertexes]: stop_vertexes_) {
                for (size_t i = 1; i < vertexes.size(); i++) {
                    edges_.push_back({vertexes[0]->id, vertexes[i]->id, wait_factor_});
                    edges_.push_back({vertexes[i]->id, vertexes[0]->id, 0});
                }
            }
            // route edges making
            for (auto [bus, vertexes]: bus_route_vertexes_) {
                for (size_t i = 0; i < vertexes.size() - 1; i++) {
                    if (bus->is_chain && vertexes[i]->stop == vertexes[i + 1]->stop){
                        edges_.push_back({vertexes[i]->id, vertexes[i + 1]->id, 0});
                    } else {
                        auto id_weight = catalogue.GetDistance({vertexes[i]->stop, vertexes[i + 1]->stop}) * speed_factor_;
                        edges_.push_back({vertexes[i]->id, vertexes[i + 1]->id, id_weight});
                    }
                }
            }
        }

        std::pair<size_t, size_t> GetFromToId(catalogue::TransportCatalogue & catalogue, const std::string & from_name, const std::string & to_name){
            return {start_ids_.at(catalogue.ReturnStop(from_name)), start_ids_.at(catalogue.ReturnStop(to_name))};
        }

        std::vector<std::variant<domain::Wait, domain::Ride>> GetSearchResult(const std::optional<graph::Router<double>::RouteInfo>& route_info) {
            std::vector<std::variant<domain::Wait, domain::Ride>> ride_data;
            domain::Wait wait{};
            domain::Ride ride{};
            double time = 0;
            int span_count = 0;
            domain::Bus* bus{};
            for (auto section : route_info->edges){
                if (edges_.at(section).weight != 0 ){
                    if (vertex_ids_.at(edges_.at(section).from).stop == vertex_ids_.at(edges_.at(section).to).stop){
                        if (!ride_data.empty()){
                            ride = {bus, time, span_count};
                            ride_data.emplace_back(ride);
                            time = 0;
                            span_count = 0;
                        }
                        wait = {vertex_ids_.at(edges_.at(section).from).stop, edges_.at(section).weight};
                        ride_data.emplace_back(wait);
                        bus = vertex_ids_.at(edges_.at(section).to).bus;
                    } else {
                        time += edges_.at(section).weight;
                        span_count++;
                    }
                }
            }
            ride = {bus, time, span_count};
            ride_data.emplace_back(ride);
            return ride_data;
        }

        std::pair<double, std::vector<std::variant<domain::Wait, domain::Ride>>> RouteSearch(const std::string & from_name, const std::string & to_name, catalogue::TransportCatalogue & catalogue) {
            if (from_name == to_name){
                return {0.0, {}};
            }
            GetVertexes(catalogue);
            GetEdges(catalogue);
            std::pair<size_t, size_t> from_to_id = GetFromToId(catalogue, from_name, to_name);
            graph::DirectedWeightedGraph<double> current_graph(vertex_ids_.size() + 1);
            for (auto edge : edges_){
                size_t id = current_graph.AddEdge(edge);
                graph_edges_[id] = &edge;
            }
            graph::Router current_router(current_graph);
            std::optional<graph::Router<double>::RouteInfo> route_info = current_router.BuildRoute(from_to_id.first, from_to_id.second);
            if (!route_info){
                return{-1.0, {}};
            }
            return {route_info->weight, GetSearchResult(route_info)};
        }

    private:
        double speed_factor_;
        double wait_factor_;
        std::map<size_t, domain::VertexData> vertex_ids_;
        std::map<domain::Bus*, std::vector<domain::VertexData*>> bus_route_vertexes_;
        std::map<domain::Stop*, std::vector<domain::VertexData*>> stop_vertexes_;
        std::map<domain::Stop*, size_t> start_ids_;
        std::map<size_t, graph::Edge<double>*> graph_edges_;
        std::vector<graph::Edge<double>> edges_;
    };
}