#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <sstream>


namespace svg {

    class Object;

    struct Rgb {
    public:
        explicit Rgb(uint8_t red_in = 0, uint8_t green_in = 0, uint8_t blue_in = 0){
            red = (uint8_t) red_in;
            green = (uint8_t) green_in;
            blue = (uint8_t) blue_in;
        }
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    struct Rgba {
    public:
        explicit Rgba(uint8_t red_in = 0, uint8_t green_in = 0, uint8_t blue_in = 0, double opacity_in = 1.0)
                : red(red_in), green(green_in), blue(blue_in), opacity (opacity_in) {}
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        double opacity;

    };

    using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;
    inline const Color NoneColor{"none"};

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    std::ostream& operator<< (std::ostream & out, const StrokeLineCap & line_cap);

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    struct SolutionPrinter {
        void operator()(std::monostate) const {
            std::cout << "none";
        }
        void operator()(std::string color) const {
            std::cout << color;
        }
        void operator()(Rgb rgb) const {
            std::cout << "rgb(" << unsigned(rgb.red) << "," << unsigned(rgb.green) << "," << unsigned(rgb.blue) << ")";
        }
        void operator()(Rgba rgba) const {
            std::cout << "rgba(" << unsigned(rgba.red) << "," << unsigned(rgba.green) << "," << unsigned(rgba.blue) << "," << rgba.opacity << ")";
        }
    };

    std::ostream& operator<< (std::ostream &out, const StrokeLineJoin & line_join);
    std::ostream& operator<< (std::ostream & out, const Color & color);

    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeWidth(double width) {
            stroke_width_ = width;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            line_cap_ = line_cap;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            line_join_ = line_join;
            return AsOwner();
        }


    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << R"( fill=")" << *fill_color_ << R"(")";
            }
            if (stroke_color_) {
                out << R"( stroke=")" << *stroke_color_ << R"(")";
            }
            if (stroke_width_) {
                out << R"( stroke-width=")" << *stroke_width_ << R"(")";
            }
            if (line_cap_) {
                out << R"( stroke-linecap=")" << *line_cap_ << R"(")";
            }
            if (line_join_) {
                out << R"( stroke-linejoin=")" << *line_join_ << R"(")";
            }
        }

    private:
        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;
        std::optional<double> stroke_width_;
        Owner& AsOwner() {
            return static_cast<Owner&>(*this);
        }
    };

    class ObjectContainer { // %%%%%%%%%%%%%%% INTERFACE
    public:
        template <typename Obj>
        void Add(Obj obj) {
            AddPtr(std::make_unique<Obj>(std::move(obj)));
        }

        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    };

    class Drawable { // %%%%%%%%%%%%%%% INTERFACE
    public:
        virtual ~Drawable() = default;

        virtual void Draw(ObjectContainer& object_container) const = 0;
    };

    struct Point {
        Point() = default;
        Point(double x, double y)
                : x(x)
                , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    struct RenderContext {
        RenderContext(std::ostream& out)
                : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
                : out(out)
                , indent_step(indent_step)
                , indent(indent) {
        }

        [[nodiscard]] RenderContext Indented() const {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };


    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */

    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

    private:
        void RenderObject(const RenderContext& context) const override;
        std::vector<Point> points_;

    };

    class Text : public Object, public PathProps<Text> {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point pos_ = {0.0, 0.0};
        Point offset_ = {0.0, 0.0};
        uint32_t size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;

    };

    class Document : public ObjectContainer {
    public:

        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;

    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };

}  // namespace svg


/*

 #include "svg.h"
#include <cmath>

using namespace std::literals;
using namespace svg;

namespace shapes {

    class Snowman : public svg::Drawable {
    public:
        Snowman(Point head_center, double head_r)
                : head_center_(head_center)
                , head_r_(head_r) {}

        void Draw(svg::ObjectContainer& container) const override {
            double legs_shift = head_center_.y + head_r_ * 2 + head_r_ * 3;
            double body_shift = head_center_.y + head_r_ * 2;
            container.Add(svg::Circle().SetCenter({head_center_.x, legs_shift}).SetRadius(2 * head_r_)
                                  .SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
            container.Add(svg::Circle().SetCenter({head_center_.x, body_shift}).SetRadius(1.5 * head_r_)
                                  .SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
            container.Add(svg::Circle().SetCenter(head_center_).SetRadius(head_r_)
                                  .SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        }

    private:
        Point head_center_;
        double head_r_;
    };

} // namespace shapes


template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);


    [[maybe_unused]] uint8_t Lerp(uint8_t from, uint8_t to, double t) {
        return static_cast<uint8_t>(std::round((to - from) * t + from));
    }

// Выполняет линейную интерполяцию Rgb цвета от from до to в зависимости от параметра t

    [[maybe_unused]] svg::Rgb Lerp(svg::Rgb from, svg::Rgb to, double t) {
        return {Lerp(from.red, to.red, t), Lerp(from.green, to.green, t), Lerp(from.blue, to.blue, t)};
    }

}

int main() {
    using namespace svg;
    using namespace std;

    Color none_color;
    cout << none_color << endl; // none

    Color purple{"purple"s};
    cout << purple << endl; // purple

    Color rgb = Rgb{100, 200, 255};
    cout << rgb << endl; // rgb(100,200,255)

    Color rgba = Rgba{100, 200, 255, 0.5};
    cout << rgba << endl; // rgba(100,200,255,0.5)

    Circle c;
    c.SetRadius(3.5).SetCenter({1.0, 2.0});
    c.SetFillColor(rgba);
    c.SetStrokeColor(purple);

    Document doc;
    doc.Add(std::move(c));
    doc.Render(cout);
}

 */