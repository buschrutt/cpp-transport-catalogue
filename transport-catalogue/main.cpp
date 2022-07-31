#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main() {
    TransportCatalogue catalogue;
    dbw_data_input(catalogue, std::cin);
    dbr_data_input(catalogue, std::cin);
    return 0;
}