#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main() {
    catalogue::TransportCatalogue catalogue;
    input_reader::dbw_data_input(catalogue, std::cin);
    stat_reader::dbr_data_input(catalogue, std::cin);
    return 0;
}