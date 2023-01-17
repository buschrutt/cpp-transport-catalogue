#include "transport_catalogue.h"
#include "serialization.h"
#include "json.h"
#include "transport_router.h"
#include <iostream>
//#include "log_duration.h"

using namespace std::literals;
using namespace std;

int main() {
    catalogue::TransportCatalogue catalogue;

    //json::Document json_doc = json::JsonFileLoad("json_files/json_source.json"s);
    json::Document json_make_db_doc = json::JsonFileLoad("json_files/json_make_base.json"s);
    //json::Document json_make_db_doc = json::JsonConsoleLoad(std::cin);
    //json_reader::JSONReader json_data(json_doc, catalogue);
    //json_data.DBBuilder();

    data_serialization::Serializer serializer = data_serialization::Serializer(json_make_db_doc);
    serializer.ToFileSerializer();

    json::Document json_process_doc = json::JsonFileLoad("json_files/json_process_requests.json"s);
    data_serialization::Serializer deserializer = data_serialization::Serializer(json_process_doc, catalogue);
    json::Document json_result = deserializer.FromFileDeserializer();
    //deserializer.FromFileDeserializerConsole();
    //json::Document json_result = json_data.JsonResponseBuilder();
    json::JsonFileWrite(json_result, "json_files/json_result.json"s);
    //json::JsonConsoleOutput(json_result);


    // %%%%%%%%%% %%%%%%%%%% custom route finder %%%%%%%%%% %%%%%%%%%%

    // %%%%%%%%%% %%%%%%%%%% map renderer %%%%%%%%%% %%%%%%%%%%
    //renderer::RenderSettings render_settings = json_reader::JSONReader::RenderSettingsBuilder (json_doc);
    //renderer::MapRenderer map_renderer = renderer::MapRenderer();
    //map_renderer.DrawSvgMap(catalogue, render_settings);
}