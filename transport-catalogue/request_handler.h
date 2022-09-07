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
                std::vector<std::pair<domain::VertexData&, domain::VertexData&>> vertex_pairs;
                for (auto bus_through_stop : stop->buses){
                    size_t id = GetLastId();
                    vertex_ids_[id + 1] = {id + 1, bus_through_stop, stop};
                    vertex_ids_[id + 2] = {id + 2, bus_through_stop, stop};
                    vertex_pairs.emplace_back(vertex_ids_.at(id + 1), vertex_ids_.at(id + 2));
                }
                auto* fake_bus = new domain::Bus {"fake bus"s, false, {}};
                size_t id = GetLastId();
                vertex_ids_[id + 1] = {id + 1, fake_bus, stop};
                vertex_ids_[id + 2] = {id + 2, fake_bus, stop};
                first_fake_bus_ids_.push_back(id + 1);
                vertex_pairs.emplace_back(vertex_ids_.at(id + 1), vertex_ids_.at(id + 2));
                stop_vertexes_[stop] = vertex_pairs;
                start_vertex_id_[stop_name] = id + 2;
            }
        }

        void GetEdges(catalogue::TransportCatalogue & catalogue){
            // in stop edges making
            for (auto [stop, vertex_pairs] : stop_vertexes_){
                for (auto vertex_pair : vertex_pairs){
                    bus_zero_edge_[stop][vertex_pair.first.bus] = {vertex_pair.first.id, vertex_pair.second.id};
                    edges_.push_back({vertex_pair.first.id, vertex_pair.second.id, 0});
                    if (vertex_pair.first.bus->is_chain){
                        edges_.push_back({vertex_pair.second.id, vertex_pair.first.id, 0});
                    }
                    for (auto vertex_pair_next : vertex_pairs){
                        if (vertex_pair.first.id != vertex_pair_next.first.id){
                            if (std::count(first_fake_bus_ids_.begin(), first_fake_bus_ids_.end(),vertex_pair_next.first.id) > 0){
                                edges_.push_back({vertex_pair.second.id, vertex_pair_next.first.id, 0});
                            } else {
                                edges_.push_back({vertex_pair.second.id, vertex_pair_next.first.id, wait_factor_});
                            }
                        }
                    }
                }
            }
            // route edges making
            std::map<std::string, domain::Bus*> all_buses = catalogue.GetAllBuses();
            for (const auto& [bus_name, bus] : all_buses){
                for (size_t i = 0; i < bus->route.size() - 1; i++){
                    if (bus->is_chain){
                        size_t first_id = bus_zero_edge_.at(bus->route[i]).at(bus).second;
                        size_t second_id = bus_zero_edge_.at(bus->route[i + 1]).at(bus).first;
                        auto id_weight = catalogue.GetDistance({bus->route[i], bus->route[i + 1]}) * speed_factor_;
                        edges_.push_back({first_id, second_id, id_weight});
                        auto id_back_weight = catalogue.GetDistance({bus->route[i + 1], bus->route[i]}) * speed_factor_;
                        edges_.push_back({second_id, first_id, id_back_weight});
                    } else {
                        size_t first_id = bus_zero_edge_.at(bus->route[i + 1]).at(bus).second;
                        size_t second_id = bus_zero_edge_.at(bus->route[i]).at(bus).first;
                        auto id_weight = catalogue.GetDistance({bus->route[i + 1], bus->route[i]}) * speed_factor_;
                        edges_.push_back({first_id, second_id, id_weight});

                    }
                }
            }
        }

        std::pair<size_t, size_t> GetFromToId(const std::string & from_name, const std::string & to_name){
            return {start_vertex_id_.at(from_name), start_vertex_id_.at(to_name)};
        }

        std::vector<std::variant<domain::Wait, domain::Ride>> GetSearchResult(const std::optional<graph::Router<double>::RouteInfo>& route_info) {
            std::vector<std::variant<domain::Wait, domain::Ride>> ride_data;
            domain::Wait wait{};
            domain::Ride ride{};
            double time = 0;
            int span_count = 0;
            domain::Bus* bus{};
            auto c = route_info->edges.size();
            for (auto section : route_info->edges){
                auto a = section;
                auto b = vertex_ids_.size();
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
            GetVertexes(catalogue);
            GetEdges(catalogue);
            std::pair<size_t, size_t> from_to_id = GetFromToId(from_name, to_name);
            graph::DirectedWeightedGraph<double> current_graph(vertex_ids_.size() + 1);
            for (auto edge : edges_){
                size_t id = current_graph.AddEdge(edge);
                graph_edges_[id] = &edge;
            }
            graph::Router current_router(current_graph);
            std::optional<graph::Router<double>::RouteInfo> route_info = current_router.BuildRoute(from_to_id.first, from_to_id.second);
            if (!route_info){
                return{};
            }

            return {route_info->weight, GetSearchResult(route_info)};
        }

    private:
        double speed_factor_;
        double wait_factor_;
        std::vector<size_t> first_fake_bus_ids_;
        std::map<size_t, graph::Edge<double>*> graph_edges_;
        std::map<std::string, size_t> start_vertex_id_;
        std::map<domain::Stop*, std::map<domain::Bus*, std::pair<size_t , size_t>>> bus_zero_edge_;
        std::vector<graph::Edge<double>> edges_;
        std::map<size_t, domain::VertexData> vertex_ids_;
        std::map<domain::Stop*, std::vector<std::pair<domain::VertexData&, domain::VertexData&>>> stop_vertexes_;
    };
}