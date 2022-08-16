#include "transport_catalogue.h"

namespace catalogue {

    std::unordered_map<std::string, TransportCatalogue::Stop> TransportCatalogue::GetAllStops(){
        return stops_;
    }

    std::map<std::string, TransportCatalogue::Bus> TransportCatalogue::GetAllBuses(){
        return buses_;
    }

    void TransportCatalogue::SetStop(const std::string& stop_name, geo::Coordinates coordinates){
        stops_[stop_name].coordinates = coordinates;
    }

    void TransportCatalogue::SetStopDistance(const std::string& origin_name, const std::string& destination_name, double length){
        stops_[origin_name].distances[destination_name] = length;
    }

    void TransportCatalogue::SetBus(const std::string& bus_name, const std::vector<std::string>& route, bool if_chain){
        buses_[bus_name].is_chain = if_chain;
        buses_[bus_name].route = route;
        for (const std::string& stop_name : route){
            stops_[stop_name].buses.insert(bus_name);
        }
    }

    std::set<std::string> TransportCatalogue::GetStopBuses(const std::string& stop_name){
        std::set<std::string> dummy;
        if (stops_.count(stop_name) < 1){
            dummy.insert(":");
            return dummy;
        }
        return stops_.at(stop_name).buses;
    }

    size_t TransportCatalogue::BusStopCount(const std::string &bus_name){
        if (buses_.count(bus_name) < 1){
            return 0;
        }
        if (buses_.at(bus_name).is_chain){
            return buses_.at(bus_name).route.size() + (buses_.at(bus_name).route.size() - 1);
        } else {
            return buses_.at(bus_name).route.size();
        }
    }

    size_t TransportCatalogue::BusUniqStopCount(const std::string &bus_name){
        std::set<std::string_view> uniq_stops;
        for (const std::string_view stop : buses_.at(bus_name).route){
            uniq_stops.insert(stop);
        }
        return uniq_stops.size();
    }

    std::pair<double, double> TransportCatalogue::BusRouteLength(const std::string &bus_name){
        double route_length = 0.0;
        double chord_route_length = 0.0;
        std::pair<std::string, std::string> key;
        geo::Coordinates c_origin{};
        geo::Coordinates c_destination{};
        for (size_t i = 1; i < buses_.at(bus_name).route.size(); i++){
            key.first = buses_.at(bus_name).route[i];
            key.second = buses_.at(bus_name).route[i - 1];
            if (stops_.at(key.first).distances.count(key.second) < 1){
                route_length += stops_.at(key.second).distances.at(key.first);
            } else {
                route_length += stops_.at(key.first).distances.at(key.second);
            }
            c_destination = stops_.at(buses_.at(bus_name).route[i]).coordinates;
            c_origin = stops_.at(buses_.at(bus_name).route[i-1]).coordinates;
            chord_route_length += ComputeDistance(c_origin, c_destination);
            if (buses_.at(bus_name).is_chain){
                if (stops_.at(key.second).distances.count(key.first) < 1){
                    route_length += stops_.at(key.first).distances.at(key.second);
                } else {
                    route_length += stops_.at(key.second).distances.at(key.first);
                }
            }
        }
        if (buses_.at(bus_name).is_chain){
            chord_route_length += chord_route_length;
        }
        return {route_length, route_length / chord_route_length};
    }

}