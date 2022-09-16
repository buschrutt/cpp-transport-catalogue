#include "domain.h"
#include "graph.h"
#include "transport_catalogue.h"

namespace router {

    using namespace std::literals;

    class CustomRouteFinder{
    public:
        explicit CustomRouteFinder(domain::RoutingSettings routing_settings, catalogue::TransportCatalogue & catalogue);

        template<typename Itr>
        std::pair<double, size_t> GetEdgeWeight(Itr itr_begin, Itr itr_end, catalogue::TransportCatalogue & catalogue) {
            double weight = 0.0;
            size_t span_count = 0;
            auto runner = itr_begin;
            auto ahead_runner = itr_begin;
            ahead_runner++;
            while (runner != itr_end){
                weight += catalogue.GetDistance({*runner, *ahead_runner});
                span_count++;
                runner++;
                ahead_runner++;
            }
            return {weight * speed_factor_, span_count};
        }

        size_t GetLastId();

        void GetVertexes(catalogue::TransportCatalogue & catalogue);

        void RouteEdgeBuild(std::vector<domain::Stop*> route, catalogue::TransportCatalogue & catalogue, domain::Bus* bus);

        void GetEdges(catalogue::TransportCatalogue & catalogue);

        void BuildRouter();

        std::pair<size_t, size_t> GetFromToId(catalogue::TransportCatalogue & catalogue, const std::string & from_name, const std::string & to_name);

        std::vector<std::variant<domain::Wait, domain::Ride>> GetSearchResult(const std::optional<graph::Router<double>::RouteInfo>& route_info);

        std::pair<double, std::vector<std::variant<domain::Wait, domain::Ride>>> RouteSearch(const std::string & from_name, const std::string & to_name, catalogue::TransportCatalogue & catalogue);

    private:
        double speed_factor_;
        double wait_factor_;
        std::map<domain::Stop*, domain::StopData> stop_data_;
        std::vector<domain::EdgeData> edge_data_;
        std::vector<size_t> vertex_ids_;
        std::map<size_t, domain::EdgeData*> graph_edges_;
        std::map<size_t, std::variant<domain::Wait, domain::Ride>> edges_;
        graph::Router<double>* router_{};
    };

}