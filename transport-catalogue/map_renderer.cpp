#include "map_renderer.h"

#include <utility>
#include <cmath>

using namespace std::literals;

namespace renderer {

    renderer::MapRenderer::RenderSettings::RenderSettings() = default;

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    namespace shapes {
        class Poly : public svg::Drawable {
        public:
            explicit Poly(svg::Polyline poly): poly_(std::move(poly)) {}
            void Draw(svg::ObjectContainer &container) const override {
                container.Add(poly_);
            }
        private:
            svg::Polyline poly_;
        };
    }

    void DrawSvgMap(catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer::RenderSettings& settings){
        std::vector<geo::Coordinates> geo_coords;
        std::set <std::string> result_order;
        std::map <std::string, svg::Polyline> MapPolyLines;
        std::vector<std::unique_ptr<svg::Drawable>> picture;
        svg::Document doc;
        // %%%%%%%%%% %%%%%%%%%% result order & scale finding %%%%%%%%%% %%%%%%%%%%
        for (const auto& bus : catalogue.GetAllBuses()){
            result_order.insert(bus.first);
            for (auto & stop_name : bus.second.route){
                geo_coords.emplace_back(catalogue.GetAllStops().at(stop_name).coordinates);
            }
        }
        const SphereProjector proj(geo_coords.begin(), geo_coords.end(), settings.width, settings.height, settings.padding);
        // %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
        int color_count = 0;
        for (const auto& route : result_order){
            std::vector<std::string> bus = catalogue.GetAllBuses().at(route).route;
            if (bus.empty()){ continue;}
            std::vector<std::string> direction1 = bus;
            reverse(direction1.begin(),direction1.end());
            svg::Polyline polyline;
            for (const auto& stop : direction1){
                polyline.AddPoint(proj(catalogue.GetAllStops().at(stop).coordinates))
                        .SetFillColor(svg::NoneColor)
                        .SetStrokeColor(settings.color_palette[color_count % settings.color_palette.size()])
                        .SetStrokeWidth(settings.line_width)
                        .SetStrokeLineCap(static_cast<svg::StrokeLineCap>(1))
                        .SetStrokeLineJoin(static_cast<svg::StrokeLineJoin>(5));
            }
            if (catalogue.GetAllBuses().at(route).is_chain) {
                for (const auto& stop : bus){
                    if (bus[0] == stop){ continue;}
                    polyline.AddPoint(proj(catalogue.GetAllStops().at(stop).coordinates))
                            .SetFillColor(svg::NoneColor)
                            .SetStrokeColor(settings.color_palette[color_count % settings.color_palette.size()])
                            .SetStrokeWidth(settings.line_width)
                            .SetStrokeLineCap(static_cast<svg::StrokeLineCap>(1))
                            .SetStrokeLineJoin(static_cast<svg::StrokeLineJoin>(5));
                }
            }
            MapPolyLines[route] = polyline;
            color_count++;
        }
        // %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
        for (auto [key, poly] : MapPolyLines){
            picture.emplace_back(std::make_unique<shapes::Poly>(poly));
        }
        DrawPicture(picture, doc);
        doc.Render(std::cout);
    }

}

/*
 *
<?xml version="1.0" encoding="UTF-8" ?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1">
  <polyline points="99.2283,329.5 50,232.18 99.2283,329.5" fill="none" stroke="green" stroke-width="14"
  stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="550,190.051 279.22,50 333.61,269.08 550,190.051" fill="none" stroke="rgb(255,160,0)"
  stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
</svg>
 */