#include "map_renderer.h"
#include <utility>
#include <cmath>

using namespace std::literals;

namespace renderer {

    renderer::RenderSettings::RenderSettings() = default;

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    class Poly : public svg::Drawable {
    public:
        explicit Poly(svg::Polyline poly): poly_(std::move(poly)) {}
        void Draw(svg::ObjectContainer &container) const override {
            container.Add(poly_);
        }
    private:
        svg::Polyline poly_;
    };

    void DrawSvgMap(catalogue::TransportCatalogue& catalogue, const renderer::RenderSettings& settings){
        std::vector<geo::Coordinates> geo_coords;
        std::vector<std::unique_ptr<svg::Drawable>> picture;
        svg::Document doc;
        // %%%%%%%%%% %%%%%%%%%% result order & scale finding %%%%%%%%%% %%%%%%%%%%
        for (const auto& stop : catalogue.GetAllStops()){
            if (!stop.second.buses.empty()){
                geo_coords.emplace_back(stop.second.coordinates);
            }
        }
        const SphereProjector proj(geo_coords.begin(), geo_coords.end(), settings.width, settings.height, settings.padding);
        // %%%%%%%%%% %%%%%%%%%% poly lines & picture formation %%%%%%%%%% %%%%%%%%%%
        int color_count = 0;
        for (const auto& [key, value] : catalogue.GetAllBuses()){
            if (value.route.empty()){ continue;}
            svg::Polyline polyline;
            for (auto itr_stop = value.route.end(); itr_stop != value.route.begin();){
                itr_stop--;
                polyline.AddPoint(proj(catalogue.GetAllStops().at(*itr_stop).coordinates))
                        .SetFillColor(svg::NoneColor)
                        .SetStrokeColor(settings.color_palette[color_count % settings.color_palette.size()])
                        .SetStrokeWidth(settings.line_width)
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            }
            if (value.is_chain) { // if is chain back-direction point adding
                for (auto itr_stop = value.route.begin(); itr_stop != value.route.end(); itr_stop++){
                    if (value.route[0] == *itr_stop){ continue;}
                    polyline.AddPoint(proj(catalogue.GetAllStops().at(*itr_stop).coordinates))
                            .SetFillColor(svg::NoneColor)
                            .SetStrokeColor(settings.color_palette[color_count % settings.color_palette.size()])
                            .SetStrokeWidth(settings.line_width)
                            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                }
            }
            picture.emplace_back(std::make_unique<Poly>(polyline)); // poly-lines unique_ptr
            color_count++;
        }
        // %%%%%%%%%% %%%%%%%%%% draw & render %%%%%%%%%% %%%%%%%%%%
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