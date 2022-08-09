#include "old_/input_reader.h"
#include "old_/stat_reader.h"
#include "transport_catalogue.h"

int main() {
    catalogue::TransportCatalogue catalogue;
    input_reader::dbw_data_input(catalogue, std::cin);
    stat_reader::dbr_data_input(catalogue, std::cin);
    return 0;
}






/*
  "base_requests": [
    {
      "type": "Stop",
      "name": "Tolstopaltsevo",
      "latitude": 55.611087,
      "longitude": 37.20829,
      "road_distances": {"Marushkino": 3900}
    },
    {
      "type": "Stop",
      "name": "Marushkino",
      "latitude": 55.595884,
      "longitude": 37.209755,
      "road_distances": {"Rasskazovka": 9900, "Marushkino": 9900}
    },
    {
      "type": "Bus",
      "name": "256",
      "stops": ["Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"],
      "is_roundtrip": true
    },
    {
      "type": "Bus",
      "name": "750",
      "stops": ["Tolstopaltsevo", "Marushkino", "Marushkino", "Rasskazovka"],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "name": "Rasskazovka",
      "latitude": 55.632761,
      "longitude": 37.333324,
      "road_distances": {"Marushkino": 9500}
    },
    {
      "type": "Stop",
      "name": "Biryulyovo Zapadnoye",
      "latitude": 55.574371,
      "longitude": 37.6517,
      "road_distances": {"Rossoshanskaya ulitsa": 7500, "Biryusinka": 1800, "Universam": 2400}
    },
    {
      "type": "Stop",
      "name": "Biryusinka",
      "latitude": 55.581065,
      "longitude": 37.64839,
      "road_distances": {"Universam": 750}
    },
    {
      "type": "Stop",
      "name": "Universam",
      "latitude": 55.587655,
      "longitude": 37.645687,
      "road_distances": {"Rossoshanskaya ulitsa": 5600, "Biryulyovo Tovarnaya": 900}
    },
    {
      "type": "Stop",
      "name": "Biryulyovo Tovarnaya",
      "latitude": 55.592028,
      "longitude": 37.653656,
      "road_distances": {"Biryulyovo Passazhirskaya": 1300}
    },
    {
      "type": "Stop",
      "name": "Biryulyovo Passazhirskaya",
      "latitude": 55.580999,
      "longitude": 37.659164,
      "road_distances": {"Biryulyovo Zapadnoye": 1200}
    },
    {
      "type": "Bus",
      "name": "828",
      "stops": ["Biryulyovo Zapadnoye", "Universam", "Rossoshanskaya ulitsa", "Biryulyovo Zapadnoye"],
      "is_roundtrip": true
    },
    {
      "type": "Stop",
      "name": "Rossoshanskaya ulitsa",
      "latitude": 55.595579,
      "longitude": 37.605757
    },
    {
      "type": "Stop",
      "name": "Prazhskaya",
      "latitude": 55.611678,
      "longitude": 37.603831
    }
  ],

  "stat_requests": [
    { "id": 1, "type": "Bus", "name": "256" },
    { "id": 1, "type": "Bus", "name": "750" },
    { "id": 1, "type": "Bus", "name": "751" },
    { "id": 1, "type": "Stop", "name": "Samara" },
    { "id": 1, "type": "Stop", "name": "Prazhskaya" },
    { "id": 1, "type": "Stop", "name": "Biryulyovo Zapadnoye" }
  ]
  */