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
        // Remove db_files\ path before tests
        void ToFileSerializer();

        // %%%%%%%%%% %%%%%%%%%% Deserialization from file db_files\transport_catalogue.db %%%%%%%%%% %%%%%%%%%%
        // Remove db_files\ path before tests
        json::Document FromFileDeserializer();

        // %%%%%%%%%% %%%%%%%%%% Supporting funcs %%%%%%%%%% %%%%%%%%%%
        void StopAndBusDataSerializer(catalog_proto::TransportCatalogue & proto_catalogue);

        void RenderSettingsSerializer(catalog_proto::TransportCatalogue & proto_catalogue);

        void RoutingSettingsSerializer(catalog_proto::TransportCatalogue & proto_catalogue);

        int SetStopByName(const std::string& stop_name);

        void StopAndBusDataSet(const catalog_proto::TransportCatalogue& proto_catalogue);

        void RenderSettingsSet(const catalog_proto::TransportCatalogue& proto_catalogue);

        void RoutingSettingsSet(const catalog_proto::TransportCatalogue& proto_catalogue);

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
