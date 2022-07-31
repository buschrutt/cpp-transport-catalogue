#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include "transport_catalogue.h"

namespace stat_reader {

    void dbr_parse_request (const std::string& input_string, catalogue::TransportCatalogue& catalogue);

    void dbr_data_input (catalogue::TransportCatalogue& catalogue, std::istream& input);

}