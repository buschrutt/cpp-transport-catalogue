#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

namespace json_reader {

    class JSONReader{
    public:
        // %%%%%%%%%% %%%%%%%%%% constructor %%%%%%%%%% %%%%%%%%%%
        JSONReader(json_lib::Document  json_doc, catalogue::TransportCatalogue& catalogue)
        : json_doc_(std::move(json_doc))
        , catalogue_(catalogue)
        , result_json_doc_({}){}
        // %%%%%%%%%% %%%%%%%%%% static get settings method %%%%%%%%%% %%%%%%%%%%
        static renderer::RenderSettings RenderSettingsBuilder(const json_lib::Document& json_doc);
        // %%%%%%%%%% %%%%%%%%%% get from json data method %%%%%%%%%% %%%%%%%%%%
        void DBBuilder();
        // %%%%%%%%%% %%%%%%%%%% response section reading and response formation %%%%%%%%%% %%%%%%%%%%
        json_lib::Document JsonResponseBuilder();
        // %%%%%%%%%% %%%%%%%%%% private %%%%%%%%%% %%%%%%%%%%
    private:
        const json_lib::Document json_doc_;
        catalogue::TransportCatalogue catalogue_;
        json_lib::Document result_json_doc_;
        renderer::RenderSettings render_settings_;
    };
}