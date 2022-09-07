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
            for (const auto& [stop_name, stop] : all_stops){
                std::vector<domain::VertexData*> vertexes;
                for (auto bus_through_stop : stop->buses){
                    size_t id = GetLastId();
                    vertex_ids_[id + 1] = {id + 1, bus_through_stop, stop};
                    vertexes.emplace_back(&vertex_ids_.at(id + 1));
                    if (bus_through_stop->is_chain){
                        vertex_ids_[id + 2] = {id + 2, bus_through_stop, stop, true};
                        vertexes.emplace_back(&vertex_ids_.at(id + 2));
                    }
                }
                auto* fake_bus = new domain::Bus {"fake bus"s, false, {}};
                size_t id = GetLastId();
                vertex_ids_[id + 1] = {id + 1, fake_bus, stop};
                fake_bus_ids_[stop] = id + 1;
                vertexes.emplace_back(&vertex_ids_.at(id + 1));
                stop_vertexes_[stop] = vertexes;
            }
        }

        void GetEdges(catalogue::TransportCatalogue & catalogue){
            // in stop edges making
            for (auto [stop, vertexes] : stop_vertexes_){
                domain::VertexData* fake_vertex = &vertex_ids_.at(fake_bus_ids_.at(stop));
                for (auto vertex : vertexes){
                    stop_bus_id_[stop][vertex->bus][vertex->is_way_back] = vertex->id;
                    if (vertex != fake_vertex){
                        edges_.push_back({fake_vertex->id, vertex->id, wait_factor_});
                        edges_.push_back({vertex->id, fake_vertex->id, 0});
                    }
                }
            }
            // route edges making
            std::map<std::string, domain::Bus*> all_buses = catalogue.GetAllBuses();
            for (const auto& [bus_name, bus] : all_buses){
                for (size_t i = 0; i < bus->route.size() - 1; i++){
                    if (bus->is_chain){
                        size_t first_id = stop_bus_id_.at(bus->route[i]).at(bus).at(false);
                        size_t second_id = stop_bus_id_.at(bus->route[i + 1]).at(bus).at(false);
                        size_t first_back_id = stop_bus_id_.at(bus->route[i + 1]).at(bus).at(true);
                        size_t second_back_id = stop_bus_id_.at(bus->route[i]).at(bus).at(true);
                        auto id_weight = catalogue.GetDistance({bus->route[i], bus->route[i + 1]}) * speed_factor_;
                        edges_.push_back({first_id, second_id, id_weight});
                        auto id_back_weight = catalogue.GetDistance({bus->route[i + 1], bus->route[i]}) * speed_factor_;
                        edges_.push_back({first_back_id, second_back_id, id_back_weight});
                        if (second_id == stop_bus_id_.at(bus->route[bus->route.size() - 1]).at(bus).at(false)){
                            edges_.push_back({second_id, first_back_id, 0});
                        }
                    } else {
                        size_t first_id = stop_bus_id_.at(bus->route[i + 1]).at(bus).at(false);
                        size_t second_id = stop_bus_id_.at(bus->route[i]).at(bus).at(false);
                        auto id_weight = catalogue.GetDistance({bus->route[i + 1], bus->route[i]}) * speed_factor_;
                        if (second_id == stop_bus_id_.at(bus->route[0]).at(bus).at(false)) {
                            edges_.push_back({first_id, fake_bus_ids_.at(bus->route[0]), id_weight});
                        } else {
                            edges_.push_back({first_id, second_id, id_weight});
                        }
                    }
                }
            }
        }

        std::pair<size_t, size_t> GetFromToId(catalogue::TransportCatalogue & catalogue, const std::string & from_name, const std::string & to_name){
            return {fake_bus_ids_.at(catalogue.ReturnStop(from_name)), fake_bus_ids_.at(catalogue.ReturnStop(to_name))};
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
        std::map<domain::Stop*, size_t> fake_bus_ids_;
        std::map<size_t, graph::Edge<double>*> graph_edges_;
        std::map<domain::Stop*, std::map<domain::Bus*, std::map<bool, size_t>>> stop_bus_id_;
        std::vector<graph::Edge<double>> edges_;
        std::map<size_t, domain::VertexData> vertex_ids_;
        std::map<domain::Stop*, std::vector<domain::VertexData*>> stop_vertexes_;
    };
}