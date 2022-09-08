#pragma once
#include <string>
#include <vector>
#include <set>
#include "geo.h"
#include "json.h"

namespace domain {

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

    struct RoutingSettings {
        int bus_wait_time{};
        double bus_velocity{};
    };

    struct VertexData {
        size_t id{};
        domain::Bus* bus{};
        domain::Stop* stop{};
    };

    struct Wait {
        Stop* stop;
        double time;
    };

    struct Ride {
        Bus* bus;
        double time;
        int span_count;
    };

}