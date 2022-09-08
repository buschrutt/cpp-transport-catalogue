#include "transport_catalogue.h"
#include "json_reader.h"
#include "json_builder.h"
#include "json.h"
#include "domain.h"
#include "map_renderer.h"
#include "transport_router.h"
#include <iostream>

using namespace std::literals;
using namespace std;

int main() {
    catalogue::TransportCatalogue catalogue;
    json::Document json_doc = json::JsonFileLoad("json_source.json"s);
    //json::Document json_doc = json::JsonConsoleLoad(std::cin);
    json_reader::JSONReader json_data(json_doc, catalogue);
    json_data.DBBuilder();
    json::Document json_result = json_data.JsonResponseBuilder();
    json::JsonFileWrite(json_result, "json_result.json"s);
    //json::JsonConsoleOutput(json_result);

    // %%%%%%%%%% %%%%%%%%%% custom route finder %%%%%%%%%% %%%%%%%%%%

    // %%%%%%%%%% %%%%%%%%%% map renderer %%%%%%%%%% %%%%%%%%%%
    //renderer::RenderSettings render_settings = json_reader::JSONReader::RenderSettingsBuilder (json_doc);
    //renderer::MapRenderer map_renderer = renderer::MapRenderer();
    //map_renderer.DrawSvgMap(catalogue, render_settings);
}