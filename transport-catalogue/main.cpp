//#include "old_/input_reader.h"
//#include "old_/stat_reader.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "json.h"

using namespace std::literals;

int main() {

    catalogue::TransportCatalogue catalogue;
    //dbw_data_input(catalogue);
    //dbr_data_input(catalogue);
    json_lib::Document json_doc = json_lib::JsonFileLoad("json_source.json"s);
    //json_lib::Document json_doc = json_lib::JsonConsoleLoad(std::cin);
    json_reader::DBBuilder(json_doc, catalogue);
    renderer::MapRenderer::RenderSettings render_settings = json_reader::RenderSettingsBuilder (json_doc);
    //json_lib::Document json_result = json_reader::JsonResponseBuilder(json_doc, catalogue);
    //json_lib::JsonFileWrite(json_result, "json_result.json"s);
    //json_lib::JsonConsoleOutput(json_result);
    renderer::DrawSvgMap(catalogue, render_settings);

    return 0;
}


/*
<?xml version="1.0" encoding="UTF-8" ?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1">
  <polyline points="99.2283,329.5 50,232.18 99.2283,329.5" fill="none" stroke="green" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="550,190.051 279.22,50 333.61,269.08 550,190.051" fill="none" stroke="rgb(255,160,0)" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
</svg>
  */

