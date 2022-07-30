#pragma once
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include "geo.h"

class TransportCatalogue{

public:

    void SetStop(const std::string& bus_stop, Coordinates coordinates);

    void SetStopDistance(const std::string& origin_name, const std::string& destination_name, double length);

    void SetBus(const std::string& bus_name, const std::vector<std::string>& bus, bool if_chain);

    std::set<std::string> GetStopBuses(const std::string& stop_name);

    size_t BusStopCount(const std::string &bus_name);

    size_t BusUniqStopCount(const std::string &bus_name);

    std::pair<double, double> BusRouteLength(const std::string &bus_name);

private:
    std::unordered_map<std::string, Coordinates> stops_data_;
    std::unordered_map<std::string, std::set<std::string>> stop_to_buses_;
    std::map<std::pair<std::string, std::string>, double> stop_distances_;
    std::unordered_map<std::string, std::vector<std::string>> buses_data_;
    std::unordered_map<std::string, bool> bus_types_;
};