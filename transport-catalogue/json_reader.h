#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "request_handler.h"

namespace json_reader {

    class JSONReader{
    public:
        // %%%%%%%%%% %%%%%%%%%% constructor %%%%%%%%%% %%%%%%%%%%
        JSONReader(json::Document  json_doc, catalogue::TransportCatalogue& catalogue)
        : json_doc_(std::move(json_doc))
        , catalogue_(catalogue)
        , result_json_doc_({}){
            routing_settings_ = json_reader::JSONReader::RoutingSettingsBuilder(json_doc_);
        }
        // %%%%%%%%%% %%%%%%%%%% static get render settings method %%%%%%%%%% %%%%%%%%%%
        static renderer::RenderSettings RenderSettingsBuilder(const json::Document& json_doc);
        // %%%%%%%%%% %%%%%%%%%% static get routing settings method %%%%%%%%%% %%%%%%%%%%
        static domain::RoutingSettings RoutingSettingsBuilder(const json::Document& json_doc);
        // %%%%%%%%%% %%%%%%%%%% get from json data method %%%%%%%%%% %%%%%%%%%%
        void DBBuilder();
        // %%%%%%%%%% %%%%%%%%%% response section reading and response formation %%%%%%%%%% %%%%%%%%%%
        json::Document JsonResponseBuilder();
        // %%%%%%%%%% %%%%%%%%%% private %%%%%%%%%% %%%%%%%%%%
    private:
        const json::Document json_doc_;
        catalogue::TransportCatalogue catalogue_;
        json::Document result_json_doc_;
        renderer::RenderSettings render_settings_;
        domain::RoutingSettings routing_settings_;
    };
}