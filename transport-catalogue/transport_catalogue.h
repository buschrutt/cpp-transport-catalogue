#pragma once
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <deque>
#include "geo.h"
#include "domain.h"
#include "router.h"

namespace catalogue {

    class TransportCatalogue{

    public:

        domain::Stop* ReturnStop(const std::string& stop_name);

        domain::Bus* ReturnBus(const std::string& stop_name);

        const std::map<std::string, domain::Stop*> & GetAllStops();

        const std::map<std::string, domain::Bus*>& GetAllBuses();

        const double& GetDistance(std::pair<domain::Stop*, domain::Stop*>);

        void SetStop(const std::string& bus_stop, geo::Coordinates coordinates);

        void SetStopDistance(const std::string& origin_name, const std::string& destination_name, double length);

        void SetBus(const std::string& bus_name, const std::vector<std::string>& bus, bool if_chain);

        std::set<domain::Bus*> GetStopBuses(const std::string& stop_name);

        size_t BusStopCount(const std::string &bus_name);

        size_t BusUniqStopCount(const std::string &bus_name);

        std::pair<double, double> BusRouteLength(const std::string &bus_name);

    private:
        std::map<std::string, domain::Bus*> buses_;
        std::map<std::string, domain::Stop*> stops_;
        std::deque<domain::Stop> source_stops_;
        std::deque<domain::Bus> source_buses_;
        std::map<std::pair<domain::Stop*, domain::Stop*>, double> distances_;
    };

}