#include "transport_catalogue.h"
#include "json_reader.h"
#include "json_builder.h"
#include "json.h"
#include "domain.h"
#include <iostream>

class Builder;

using namespace std::literals;
using namespace std;

int main() {
    json::Print(
            json::Document{
                    json::Builder{}
                            .StartDict()
                            .Key("key1"s).Value(123)
                            .Key("key2"s).Value("value2"s)
                            .Key("key3"s).StartArray()
                            .Value(456)
                            .StartDict().EndDict()
                            .StartDict()
                            .Key(""s)
                            .Value(nullptr)
                            .EndDict()
                            .Value(""s)
                            .EndArray()
                            .EndDict()
                            .Build()
            },
            cout
    );
    cout << endl;

    json::Print(
            json::Document{
                    json::Builder{}
                            .Value("just a string"s)
                            .Build()
            },
            cout
    );
    cout << endl;
}


/*
 *
 * catalogue::TransportCatalogue catalogue;
    //json_lib::Document json_doc = json_lib::JsonFileLoad("json_source.json"s);
    json_lib::Document json_doc = json_lib::JsonConsoleLoad(std::cin);
    json_reader::JSONReader json_data(json_doc, catalogue);
    json_data.DBBuilder();
    json_lib::Document json_result = json_data.JsonResponseBuilder();
    //json_lib::JsonFileWrite(json_result, "json_result.json"s);
    json_lib::JsonConsoleOutput(json_result);
    //renderer::DrawSvgMap(catalogue, render_settings);
 */