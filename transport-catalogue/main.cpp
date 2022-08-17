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
    renderer::RenderSettings render_settings = json_reader::RenderSettingsBuilder (json_doc);
    json_lib::Document json_result = json_reader::JsonResponseBuilder(json_doc, catalogue);
    json_lib::JsonFileWrite(json_result, "json_result.json"s);
    //json_lib::JsonConsoleOutput(json_result);
    //renderer::DrawSvgMap(catalogue, render_settings);

    return 0;
}
