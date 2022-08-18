#pragma once
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <deque>
#include "geo.h"

namespace catalogue {

    class TransportCatalogue{

    public:

        struct Stop;

        struct Bus{
            std::string name;
            bool is_chain;
            std::vector<Stop*> route;
        };

        struct Stop{
            std::string name;
            geo::Coordinates coordinates;
            std::set<Bus*> buses;
        };

        TransportCatalogue::Stop* ReturnStop(const std::string& stop_name);

        TransportCatalogue::Bus* ReturnBus(const std::string& stop_name);

        const std::map<std::string, TransportCatalogue::Stop*> & GetAllStops();

        const std::map<std::string, Bus*>& GetAllBuses();

        void SetStop(const std::string& bus_stop, geo::Coordinates coordinates);

        void SetStopDistance(const std::string& origin_name, const std::string& destination_name, double length);

        void SetBus(const std::string& bus_name, const std::vector<std::string>& bus, bool if_chain);

        std::set<TransportCatalogue::Bus*> GetStopBuses(const std::string& stop_name);

        size_t BusStopCount(const std::string &bus_name);

        size_t BusUniqStopCount(const std::string &bus_name);

        std::pair<double, double> BusRouteLength(const std::string &bus_name);

    private:
        std::map<std::string, Bus*> buses_;
        std::map<std::string, Stop*> stops_;
        std::deque<Stop> source_stops_;
        std::deque<Bus> source_buses_;
        std::map<std::pair<Stop*, Stop*>, double> distances_;
    };

}