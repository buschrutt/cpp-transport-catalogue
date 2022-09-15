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

    struct StopData {
        size_t vertex_in_id{};
        size_t vertex_out_id{};
        domain::Stop* stop{};
    };

    struct EdgeData {
        size_t origin_id{};
        size_t destination_id{};
        double weight{};
        bool is_stop_edge = true;
        domain::Stop* origin_stop{};
        //domain::Stop* destination_stop{};
        domain::Bus* bus{};
        int span_count = 0;


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