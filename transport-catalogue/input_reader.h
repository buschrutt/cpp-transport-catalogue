#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include "transport_catalogue.h"

void dbw_request_parsing (const std::string& input_string, TransportCatalogue& catalogue);

void dbw_data_input(TransportCatalogue &catalogue, std::istream& input);
