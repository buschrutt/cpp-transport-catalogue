#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include "../transport_catalogue.h"

namespace input_reader {

    void dbw_parse_request (const std::string& input_string, catalogue::TransportCatalogue& catalogue);

    std::istream& dbw_data_input(catalogue::TransportCatalogue &catalogue, std::istream& input);

}