#include "transport_router.h"
//#include "log_duration.h"

namespace router{

    CustomRouteFinder::CustomRouteFinder(domain::RoutingSettings routing_settings, catalogue::TransportCatalogue & catalogue){
        speed_factor_ = 1 / ((routing_settings.bus_velocity * 1000) / 60);
        wait_factor_ = routing_settings.bus_wait_time;
        GetVertexes(catalogue);
        GetEdges(catalogue);
        BuildRouter();
    }

    size_t CustomRouteFinder::GetLastId(){
        return vertex_ids_.size();
    }

    void CustomRouteFinder::GetVertexes(catalogue::TransportCatalogue & catalogue){
        std::map<std::string, domain::Stop*> all_stops = catalogue.GetAllStops();
        for (const auto& stop : all_stops){
            size_t id = GetLastId();
            stop_data_[stop.second] = {id + 1, id + 2, stop.second};
            vertex_ids_.push_back(id + 1);
            vertex_ids_.push_back(id + 2);
        }
    }

    void CustomRouteFinder::RouteEdgeBuild(std::vector<domain::Stop*> route, catalogue::TransportCatalogue & catalogue, domain::Bus* bus){
        auto outer_runner = route.begin();
        while (outer_runner != route.end()){
            auto inner_runner = outer_runner;
            inner_runner++;
            while (inner_runner != route.end()){
                domain::EdgeData edge_data;
                edge_data.origin_id = stop_data_.at(*outer_runner).vertex_out_id;
                edge_data.destination_id = stop_data_.at(*inner_runner).vertex_in_id;
                edge_data.weight = GetEdgeWeight(outer_runner, inner_runner, catalogue).first;
                edge_data.span_count = (int) GetEdgeWeight(outer_runner, inner_runner, catalogue).second;
                edge_data.is_stop_edge = false;
                edge_data.bus = bus;
                edge_data_.push_back(edge_data);
                inner_runner++;
            }
            outer_runner++;
        }
    }

    void CustomRouteFinder::GetEdges(catalogue::TransportCatalogue & catalogue) {
        // in stop edges making
        for (auto [stop, stop_data]: stop_data_){
            edge_data_.push_back({stop_data.vertex_in_id, stop_data.vertex_out_id, wait_factor_, true, stop_data.stop});
        }
        // route edges making
        std::map<std::string, domain::Bus*> all_buses = catalogue.GetAllBuses();
        for (auto& bus : all_buses){
            auto route = bus.second->route;
            if (bus.second->is_chain){
                RouteEdgeBuild(route, catalogue, bus.second);
                std::reverse(route.begin(), route.end());
                RouteEdgeBuild(route, catalogue, bus.second);
            } else {
                std::reverse(route.begin(), route.end());
                RouteEdgeBuild(route, catalogue, bus.second);
            }
        }
    }

    void CustomRouteFinder::BuildRouter(){
        //LOG_DURATION("BuildRouter"s);
        domain::Wait wait{};
        domain::Ride ride{};
        auto * current_graph = new graph::DirectedWeightedGraph<double>(stop_data_.size() * 2 + 1);
        for (auto e : edge_data_){
            graph::Edge<double> edge = {e.origin_id, e.destination_id, e.weight};
            size_t id = current_graph->AddEdge(edge);
            if (e.is_stop_edge){
                wait = {e.origin_stop, e.weight};
                edges_[id] = wait;
            } else {
                ride = {e.bus, e.weight, e.span_count};
                edges_[id] = ride;
            }
        }
        auto * current_router = new graph::Router<double>(*current_graph);
        router_ = current_router;
    }

    std::pair<size_t, size_t> CustomRouteFinder::GetFromToId(catalogue::TransportCatalogue & catalogue, const std::string & from_name, const std::string & to_name){
        return {stop_data_.at(catalogue.ReturnStop(from_name)).vertex_in_id, stop_data_.at(catalogue.ReturnStop(to_name)).vertex_in_id};
    }

    std::vector<std::variant<domain::Wait, domain::Ride>> CustomRouteFinder::GetSearchResult(const std::optional<graph::Router<double>::RouteInfo>& route_info) {
        std::vector<std::variant<domain::Wait, domain::Ride>> ride_data;
        for (auto section_id : route_info->edges){
            ride_data.push_back(edges_.at(section_id));
        }
        return ride_data;
    }

    std::pair<double, std::vector<std::variant<domain::Wait, domain::Ride>>> CustomRouteFinder::RouteSearch(const std::string & from_name, const std::string & to_name, catalogue::TransportCatalogue & catalogue) {
        if (from_name == to_name){
            return {0.0, {}};
        }
        std::pair<size_t, size_t> from_to_id = GetFromToId(catalogue, from_name, to_name);
        std::optional<graph::Router<double>::RouteInfo> route_info = router_->BuildRoute(from_to_id.first, from_to_id.second);
        if (!route_info){
            return{-1.0, {}};
        }
        return {route_info->weight, GetSearchResult(route_info)};
    }

}