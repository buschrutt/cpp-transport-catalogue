#ifndef TRANSPORTCATALOGUE_SERIALIZATION_H
#define TRANSPORTCATALOGUE_SERIALIZATION_H

#include "json.h"
#include "transport_catalogue.h"
#include "json_reader.h"

#include <transport_catalogue.pb.h>

namespace data_serialization {

    class Serializer {
    public:
        // %%%%%%%%%% %%%%%%%%%% constructors %%%%%%%%%% %%%%%%%%%%
        explicit Serializer(json::Document  json_doc)
        : json_doc_(std::move(json_doc))
                , result_json_doc_({}) { }

        explicit Serializer(json::Document  json_doc, catalogue::TransportCatalogue& catalogue)
                : json_doc_(std::move(json_doc))
                , catalogue_(catalogue)
                , result_json_doc_({}) { }
        // %%%%%%%%%% %%%%%%%%%% Serialization to file db_files\transport_catalogue.db %%%%%%%%%% %%%%%%%%%%
        // Remove db_files\ before github
        void ToFileSerializer();
        // %%%%%%%%%% %%%%%%%%%% Deserialization from file db_files\transport_catalogue.db %%%%%%%%%% %%%%%%%%%%
        // Remove db_files\ before GitHub
        json::Document FromFileDeserializer();
        // %%%%%%%%%% %%%%%%%%%% Supporting funcs %%%%%%%%%% %%%%%%%%%%
        // %%%%%%%%%% Get or set and get stop id %%%%%%%%%%
        int SetStopByName(const std::string& stop_name);
        //json::Document JSONBuilder(const std::string& file_path);

        // %%%%%%%%%% %%%%%%%%%% private %%%%%%%%%% %%%%%%%%%%
    private:
        json::Document result_json_doc_;
        catalogue::TransportCatalogue catalogue_;
        const json::Document json_doc_;
        std::map<std::string, int> stop_ids_{};
        std::map<int, std::string> id_stops_{};
        int CURRENT_ID_ = 0;
        renderer::RenderSettings render_settings_{};
        domain::RoutingSettings routing_settings_{};
    };

}

#endif //TRANSPORTCATALOGUE_SERIALIZATION_H
