#pragma once
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include "geo.h"


class TransportCatalogue{

public:

    void SetStop(const std::string& bus_stop, Coordinates coordinates){
        stops_data_[bus_stop] = coordinates;
        stop_to_buses_[bus_stop];
    }
    void SetStopDistance(const std::string& origin_name, const std::string& destination_name, double length){
        stop_distances_[{origin_name, destination_name}] = length;
    }
    void SetBus(const std::string& bus_name, const std::vector<std::string>& bus, bool if_chain){
        bus_types_[bus_name] = if_chain;
        buses_data_[bus_name] = bus;
        for (const std::string& stop : bus){
            stop_to_buses_[stop].insert(bus_name);
        }
    }

    std::set<std::string> GetStopBuses(const std::string& stop_name){
        std::set<std::string> dummy;
        if (stop_to_buses_.count(stop_name) < 1){
            dummy.insert(":");
            return dummy;
        }
        return stop_to_buses_.at(stop_name);
    }

    size_t BusStopCount(const std::string &bus_name){
        if (buses_data_.count(bus_name) < 1){
            return 0;
        }
        if (bus_types_.at(bus_name)){
            return buses_data_.at(bus_name).size() + (buses_data_.at(bus_name).size() - 1);
        } else {
            return buses_data_.at(bus_name).size();
        }
    };

    size_t BusUniqStopCount(const std::string &bus_name){
        std::set<std::string> uniq_stops;
        for (const std::string& stop : buses_data_.at(bus_name)){
            uniq_stops.insert(stop);
        }
        return uniq_stops.size();
    };

    std::pair<double, double> BusRouteLength(const std::string &bus_name){
        double route_length = 0.0;
        std::pair<std::string, std::string> key;
        for (int i = 1; i < buses_data_.at(bus_name).size(); i++){
            key.second = buses_data_.at(bus_name)[i];
            key.first = buses_data_.at(bus_name)[i - 1];
            if (stop_distances_.count(key) < 1){
                route_length += stop_distances_.at({key.second, key.first});
            } else {
                route_length += stop_distances_.at(key);
            }
            if (bus_types_.at(bus_name)){
                if (stop_distances_.count({key.second, key.first}) < 1){
                    route_length += stop_distances_.at(key);
                } else {
                    route_length += stop_distances_.at({key.second, key.first});
                }
            }
        }
        Coordinates c_origin{};
        Coordinates c_destination{};
        double chord_route_length = 0.0;
        for (int i = 1; i < buses_data_.at(bus_name).size(); i++){
            c_destination = stops_data_[buses_data_.at(bus_name)[i]];
            c_origin = stops_data_[buses_data_.at(bus_name)[i-1]];
            chord_route_length += ComputeDistance(c_origin, c_destination);
        }
        if (bus_types_.at(bus_name)){
            chord_route_length += chord_route_length;
        }
        return {route_length, route_length / chord_route_length};
    };

private:
    std::unordered_map<std::string, Coordinates> stops_data_;
    std::unordered_map<std::string, std::set<std::string>> stop_to_buses_;
    std::map<std::pair<std::string, std::string>, double> stop_distances_;
    std::unordered_map<std::string, std::vector<std::string>> buses_data_;
    std::unordered_map<std::string, bool> bus_types_;
};