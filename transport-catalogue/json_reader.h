#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

namespace json_reader {

    void DBBuilder(const json_lib::Document& json_doc, catalogue::TransportCatalogue& catalogue);

    json_lib::Document JsonResponseBuilder(const json_lib::Document& json_doc, catalogue::TransportCatalogue& catalogue);

    renderer::RenderSettings RenderSettingsBuilder(const json_lib::Document& json_doc);

}