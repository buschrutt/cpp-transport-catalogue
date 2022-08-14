#include "stat_reader.h"

using namespace std;

namespace stat_reader {

    void dbr_parse_request (const std::string& input_string, catalogue::TransportCatalogue& catalogue){
        if (input_string.substr(0, 4) == "Bus "){
            std::string bus_name = input_string.substr(4);
            if (catalogue.BusStopCount(bus_name) == 0){
                std::cout << input_string << ": not found";
                std::cout << std::endl;
                return;
            }
            std::cout << input_string << ": ";
            std::cout << catalogue.BusStopCount(bus_name) << " stops on route, ";
            std::cout << catalogue.BusUniqStopCount(bus_name) << " unique stops, ";
            std::cout << catalogue.BusRouteLength(bus_name).first << " route length, ";
            std::cout << catalogue.BusRouteLength(bus_name).second << " curvature";
            std::cout << std::endl;
        } else {
            std::string stop_name = input_string.substr(5);
            std::set<std::string> value = catalogue.GetStopBuses(stop_name);
            if (value.count(":") > 0){
                std::cout << input_string << ": not found";
                std::cout << std::endl;
                return;
            }
            if (value.empty()){
                std::cout << input_string << ": no buses";
                std::cout << std::endl;
                return;
            }
            std::cout << input_string << ": buses";
            for (const std::string& bus_name : catalogue.GetStopBuses(stop_name)){
                std::cout << " " << bus_name;
            }
            std::cout << std::endl;
        }
    }

    [[maybe_unused]] istream& dbr_data_input (catalogue::TransportCatalogue& catalogue, std::istream& input) {
        int request_count;
        std::string input_string;
        getline (input, input_string);
        request_count = stoi(input_string);
        for (int i = 0; i < request_count; i++){
            getline (input, input_string);
            dbr_parse_request (input_string, catalogue);
        }
        return input;
    }

}