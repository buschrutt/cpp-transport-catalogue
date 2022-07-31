#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include "transport_catalogue.h"

void dbr_request_processing (const std::string& input_string, TransportCatalogue& catalogue);

void dbr_data_input (TransportCatalogue& catalogue, std::istream& input);