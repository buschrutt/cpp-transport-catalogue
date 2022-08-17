#include "map_renderer.h"
#include <utility>
#include <cmath>

using namespace std::literals;

namespace renderer {

    renderer::RenderSettings::RenderSettings() = default;

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    class PolyClass : public svg::Drawable {
    public:
        explicit PolyClass(svg::Polyline poly): poly_(std::move(poly)) {}
        void Draw(svg::ObjectContainer &container) const override {
            container.Add(poly_);
        }
    private:
        svg::Polyline poly_;
    };

    class TextClass : public svg::Drawable {
    public:
        explicit TextClass(svg::Text text): text_(std::move(text)) {}
        void Draw(svg::ObjectContainer &container) const override {
            container.Add(text_);
        }
    private:
        svg::Text text_;
    };

    class CircleClass : public svg::Drawable {
    public:
        explicit CircleClass(svg::Circle circle): circle_(std::move(circle)) {}
        void Draw(svg::ObjectContainer &container) const override {
            container.Add(circle_);
        }
    private:
        svg::Circle circle_;
    };

    void DrawRoutes(catalogue::TransportCatalogue& catalogue, const renderer::RenderSettings& settings
                    , const SphereProjector& proj, std::vector<std::unique_ptr<svg::Drawable>>& picture){
        // %%%%%%%%%% %%%%%%%%%% poly lines & picture formation %%%%%%%%%% %%%%%%%%%%
        int color_count = 0;
        for (const auto& [key, value] : catalogue.GetAllBuses()){
            if (value->route.empty()){ continue;}
            svg::Polyline polyline;
            for (size_t i = value->route.size(); i > 0;){
                i--;
                polyline.AddPoint(proj(value->route[i]->coordinates))
                        .SetFillColor(svg::NoneColor)
                        .SetStrokeColor(settings.color_palette[color_count % settings.color_palette.size()])
                        .SetStrokeWidth(settings.line_width)
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            }
            if (value->is_chain) { // if is chain back-direction point adding
                for (size_t j = 1; j < value->route.size(); j++){
                    polyline.AddPoint(proj(value->route[j]->coordinates))
                            .SetFillColor(svg::NoneColor)
                            .SetStrokeColor(settings.color_palette[color_count % settings.color_palette.size()])
                            .SetStrokeWidth(settings.line_width)
                            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                }
            }
            picture.emplace_back(std::make_unique<PolyClass>(polyline)); // poly-lines unique_ptr
            color_count++;
        }
    }

    void DrawRouteNames(catalogue::TransportCatalogue& catalogue, const renderer::RenderSettings& settings
                        , const SphereProjector& proj, std::vector<std::unique_ptr<svg::Drawable>>& picture){
        // %%%%%%%%%% %%%%%%%%%% text & picture formation %%%%%%%%%% %%%%%%%%%%
        int color_count = 0;
        size_t k, index;
        for (const auto& [key, value] : catalogue.GetAllBuses()) {
            if (value->route.empty()) { continue; }
            value->is_chain ? k = 4 : k = 2;
            for (size_t i = 0; i < k; i++){
                svg::Text text;
                if (i % 2 == 0){
                    text.SetFillColor(settings.underlayer_color);
                    text.SetStrokeColor(settings.underlayer_color);
                    text.SetStrokeWidth(settings.underlayer_width);
                    text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                    text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                } else {
                    text.SetFillColor(settings.color_palette[color_count]);
                }
                i > 1 ? index = 0 : index = value->route.size() - 1;
                text.SetPosition(proj(value->route[index]->coordinates));
                text.SetOffset({settings.bus_label_offset.first, settings.bus_label_offset.second});
                text.SetFontSize(settings.bus_label_font_size);
                text.SetFontFamily("Verdana").SetFontWeight("bold").SetData(key);
                picture.emplace_back(std::make_unique<TextClass>(text)); // text unique_ptr
            }
            color_count++;
        }
    }

    void DrawStopPoints(catalogue::TransportCatalogue& catalogue, const renderer::RenderSettings& settings
    , const SphereProjector& proj, std::vector<std::unique_ptr<svg::Drawable>>& picture){
        for (const auto& stop : catalogue.GetAllStops()){
            if (!stop.second->buses.empty()){
                svg::Circle circle;
                circle.SetCenter(proj(stop.second->coordinates))
                        .SetRadius((settings.stop_radius))
                        .SetFillColor("white");
                picture.emplace_back(std::make_unique<CircleClass>(circle)); // stop unique_ptr
            }
        }
    }

    void DrawStopNames(catalogue::TransportCatalogue& catalogue, const renderer::RenderSettings& settings
            , const SphereProjector& proj, std::vector<std::unique_ptr<svg::Drawable>>& picture){
        for (const auto& stop : catalogue.GetAllStops()){
            if (!stop.second->buses.empty()){
                for (size_t i = 0; i < 2; i++){
                    svg::Text text;
                    if (i == 0){
                        text.SetFillColor(settings.underlayer_color)
                        .SetStrokeColor(settings.underlayer_color)
                        .SetStrokeWidth(settings.underlayer_width)
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                    } else {
                        text.SetFillColor("black");
                    }
                    text.SetPosition(proj(stop.second->coordinates))
                            .SetOffset({settings.stop_label_offset.first, settings.stop_label_offset.second})
                            .SetFontSize(settings.stop_label_font_size)
                            .SetFontFamily("Verdana")
                            .SetData(stop.first);
                    picture.emplace_back(std::make_unique<TextClass>(text)); // stop unique_ptr
                }
            }
        }
    }

    void DrawSvgMap(catalogue::TransportCatalogue& catalogue, const renderer::RenderSettings& settings){
        std::vector<geo::Coordinates> geo_coords;
        std::vector<std::unique_ptr<svg::Drawable>> picture;
        svg::Document doc;

        // %%%%%%%%%% %%%%%%%%%% scale finding %%%%%%%%%% %%%%%%%%%%
        for (const auto& stop : catalogue.GetAllStops()){
            if (!stop.second->buses.empty()){
                geo_coords.emplace_back(stop.second->coordinates);
            }
        }
        const SphereProjector proj(geo_coords.begin(), geo_coords.end(), settings.width, settings.height, settings.padding);
        // %%%%%%%%%% %%%%%%%%%% poly lines & picture formation %%%%%%%%%% %%%%%%%%%%


        DrawRoutes(catalogue, settings, proj, picture);

        DrawRouteNames(catalogue, settings, proj, picture);

        DrawStopPoints(catalogue, settings, proj, picture);

        DrawStopNames(catalogue, settings, proj, picture);

        // %%%%%%%%%% %%%%%%%%%% draw & render %%%%%%%%%% %%%%%%%%%%
        DrawPicture(picture, doc);
        doc.Render(std::cout);

    }

}


/*


 */