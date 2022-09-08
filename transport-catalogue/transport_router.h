#include "domain.h"
#include "graph.h"
#include "transport_catalogue.h"

namespace router {

    using namespace std::literals;

    class CustomRouteFinder{
    public:
        explicit CustomRouteFinder(domain::RoutingSettings routing_settings);

        size_t GetLastId(){
            return vertex_ids_.size();
        }

        void GetVertexes(catalogue::TransportCatalogue & catalogue);

        void GetEdges(catalogue::TransportCatalogue & catalogue);

        std::pair<size_t, size_t> GetFromToId(catalogue::TransportCatalogue & catalogue,
                                              const std::string & from_name, const std::string & to_name);

        std::vector<std::variant<domain::Wait, domain::Ride>> GetSearchResult(const
                                                                              std::optional<graph::Router<double>::RouteInfo>& route_info);

        std::pair<double, std::vector<std::variant<domain::Wait, domain::Ride>>> RouteSearch(const std::string &
        from_name, const std::string & to_name, catalogue::TransportCatalogue & catalogue);

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