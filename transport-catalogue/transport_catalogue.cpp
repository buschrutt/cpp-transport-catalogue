#include "transport_catalogue.h"

namespace catalogue {

    const std::map<std::string, domain::Stop*>& TransportCatalogue::GetAllStops(){
        return stops_;
    }

    const std::map<std::string, domain::Bus*>& TransportCatalogue::GetAllBuses(){
        return buses_;
    }

    const double& TransportCatalogue::GetDistance(std::pair<domain::Stop*, domain::Stop*> key_pair) {
        if (distances_.count(key_pair) > 0) {
            return distances_.at(key_pair);
        } else {
            return distances_.at({key_pair.second, key_pair.first});
        }
    }

    domain::Stop* TransportCatalogue::ReturnStop(const std::string& stop_name){
        if (stops_.count(stop_name) == 0){
            domain::Stop stop;
            stop.name = stop_name;
            stops_[stop_name] = &source_stops_.emplace_back(stop);
        }
        return stops_[stop_name];
    }

    domain::Bus* TransportCatalogue::ReturnBus(const std::string& bus_name){
        if (buses_.count(bus_name) == 0){
            domain::Bus bus;
            bus.name = bus_name;
            buses_[bus_name] = &source_buses_.emplace_back(bus);
        }
        return buses_[bus_name];
    }

    void TransportCatalogue::SetStop(const std::string& stop_name, geo::Coordinates coordinates){
        ReturnStop(stop_name)->coordinates = coordinates;
    }

    void TransportCatalogue::SetStopDistance(const std::string& origin_name, const std::string& destination_name, double length){
        distances_[{ReturnStop(origin_name), ReturnStop(destination_name)}] = length;
    }

    void TransportCatalogue::SetBus(const std::string& bus_name, const std::vector<std::string>& route, bool if_chain){
        ReturnBus(bus_name)->is_chain = if_chain;
        for (const std::string& stop_name : route){
            ReturnBus(bus_name)->route.emplace_back(ReturnStop(stop_name));
            ReturnStop(stop_name)->buses.insert(ReturnBus(bus_name));
        }
    }

    std::set<domain::Bus*> TransportCatalogue::GetStopBuses(const std::string& stop_name){
        std::set<domain::Bus*> dummy;
        if (stops_.count(stop_name) == 0){
            dummy.insert(nullptr);
            return dummy;
        }
        return stops_.at(stop_name)->buses;
    }

    size_t TransportCatalogue::BusStopCount(const std::string &bus_name){
        if (buses_.count(bus_name) < 1){
            return -1;
        }
        if (buses_.at(bus_name)->route.empty()){
            return 0;
        }
        if (buses_.at(bus_name)->is_chain){
            return buses_.at(bus_name)->route.size() + (buses_.at(bus_name)->route.size() - 1);
        } else {
            return buses_.at(bus_name)->route.size();
        }
    }

    size_t TransportCatalogue::BusUniqStopCount(const std::string &bus_name){
        std::set<domain::Stop*> uniq_stops;
        for (const auto stop : buses_.at(bus_name)->route){
            uniq_stops.insert(stop);
        }
        return uniq_stops.size();
    }

    std::pair<double, double> TransportCatalogue::BusRouteLength(const std::string &bus_name){
        double route_length = 0.0;
        double chord_route_length = 0.0;
        std::pair<domain::Stop*, domain::Stop*> key;
        geo::Coordinates c_origin{};
        geo::Coordinates c_destination{};
        for (size_t i = 1; i < buses_.at(bus_name)->route.size(); i++){
            key.first = buses_.at(bus_name)->route[i]; //destination
            key.second = buses_.at(bus_name)->route[i - 1]; //origin
            if (distances_.count({key.first, key.second}) ==0){
                route_length += distances_.at({key.second, key.first});
            } else {
                route_length += distances_.at({key.first, key.second});
            }
            c_destination = key.first->coordinates;
            c_origin = key.second->coordinates;
            chord_route_length += ComputeDistance(c_origin, c_destination);
            if (buses_.at(bus_name)->is_chain){
                if (distances_.count({key.second, key.first}) ==0){
                    route_length += distances_.at({key.first, key.second});
                } else {
                    route_length += distances_.at({key.second, key.first});
                }
            }
        }
        if (buses_.at(bus_name)->is_chain){
            chord_route_length += chord_route_length;
        }
        return {route_length, route_length / chord_route_length};
    }

}