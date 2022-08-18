#include "transport_catalogue.h"
#include "json_reader.h"
#include "json.h"
#include "domain.h"

using namespace std::literals;

int main() {

    catalogue::TransportCatalogue catalogue;
    json_lib::Document json_doc = json_lib::JsonFileLoad("json_source.json"s);
    //json_lib::Document json_doc = json_lib::JsonConsoleLoad(std::cin);
    json_reader::JSONReader json_data(json_doc, catalogue);
    json_data.DBBuilder();
    json_lib::Document json_result = json_data.JsonResponseBuilder();
    //json_lib::JsonFileWrite(json_result, "json_result.json"s);
    json_lib::JsonConsoleOutput(json_result);
    //renderer::DrawSvgMap(catalogue, render_settings);

    return 0;
}