#pragma once
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "geo.h"

class TransportCatalogue{

    struct Bus{
        bool is_chain;
        std::vector<std::string> route;
    };

    struct Stop{
        Coordinates coordinates;
        std::unordered_map<std::string, double> distances;
        std::set<std::string> buses;
    };

public:

    void SetStop(const std::string& bus_stop, Coordinates coordinates);

    void SetStopDistance(const std::string& origin_name, const std::string& destination_name, double length);

    void SetBus(const std::string& bus_name, const std::vector<std::string>& bus, bool if_chain);

    std::set<std::string> GetStopBuses(const std::string& stop_name);

    size_t BusStopCount(const std::string &bus_name);

    size_t BusUniqStopCount(const std::string &bus_name);

    std::pair<double, double> BusRouteLength(const std::string &bus_name);

private:
    std::unordered_map<std::string, Bus> buses_;
    std::unordered_map<std::string, Stop> stops_;
};