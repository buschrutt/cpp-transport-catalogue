#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include "transport_catalogue.h"

void dbw_request_parsing (const std::string& input_string, TransportCatalogue& catalogue) {
    if (input_string.substr(0, 4) == "Bus "){
        std::vector<std::string> bus_data;
        auto pos = input_string.find_first_of(':');
        std::string bus_name = input_string.substr(4, pos - 4);
        std::string value_string = input_string.substr(pos + 1);
        if (value_string.find_last_of('>') != std::string::npos){
            std::string word_to_push;
            for (char char_value : value_string){
                if (char_value != '>'){
                    word_to_push += char_value;
                } else {
                    bus_data.push_back(word_to_push.substr(1, word_to_push.find_last_not_of(' ')));
                    word_to_push.clear();
                }
            }
            bus_data.push_back(word_to_push.substr(1));
            catalogue.SetBus(bus_name, bus_data, false);
        } else {
            std::string word_to_push;
            for (char char_value : value_string){
                if (char_value != '-'){
                    word_to_push += char_value;
                } else {
                    bus_data.push_back(word_to_push.substr(1, word_to_push.find_last_not_of(' ')));
                    word_to_push.clear();
                }
            }
            bus_data.push_back(word_to_push.substr(1));
            catalogue.SetBus(bus_name, bus_data, true);
        }
    } else if (input_string.substr(0, 4) == "Stop"){
        Coordinates coordinates{};
        auto colon_pos = input_string.find(':');
        std::string stop_name = input_string.substr(5, colon_pos - 5);
        std::string value_string = input_string.substr(colon_pos + 1);
        std::string word_to_push;
        std::vector<double> vector_coordinates;
        int count = 0;
        for (char char_value : value_string){
            if (char_value != ','){
                word_to_push += char_value;
            } else {
                if(count < 2){
                    vector_coordinates.push_back(std::stod(word_to_push.substr(1)));
                    word_to_push.clear();
                    count++;
                    if(count == 2){
                        coordinates.lat = vector_coordinates[0];
                        coordinates.lng = vector_coordinates[1];
                        catalogue.SetStop(stop_name, coordinates);
                    }
                } else {
                    catalogue.SetStopDistance(stop_name, word_to_push.substr(word_to_push.find('m') + 5),
                                              stod(word_to_push.substr(1, word_to_push.find('m') - 1)));
                    word_to_push.clear();
                }
            }
        }
        if(count < 2){
            vector_coordinates.push_back(std::stod(word_to_push.substr(1)));
            coordinates.lat = vector_coordinates[0];
            coordinates.lng = vector_coordinates[1];
            catalogue.SetStop(stop_name, coordinates);
        } else {
            catalogue.SetStopDistance(stop_name, word_to_push.substr(word_to_push.find('m') + 5),
                                      stod(word_to_push.substr(1, word_to_push.find('m') - 1)));
        }
    }
}

void dbw_data_input (TransportCatalogue& catalogue) {
    int request_count;
    std::string input_string;
    getline (std::cin, input_string);
    request_count = stoi(input_string);
    for (int i = 0; i < request_count; i++){
        getline (std::cin, input_string);
        dbw_request_parsing (input_string, catalogue);
    }
}