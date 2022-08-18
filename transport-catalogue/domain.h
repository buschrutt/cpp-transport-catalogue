#pragma once
#include <string>
#include <vector>
#include <set>
#include "geo.h"

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

}