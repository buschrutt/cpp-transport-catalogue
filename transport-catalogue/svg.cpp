#include "svg.h"
#include <utility>

namespace svg {

    using namespace std::literals;

    std::ostream& operator<< (std::ostream & out, const StrokeLineCap & line_cap) {
        if (line_cap == StrokeLineCap::BUTT){
            return out << "butt";
        } else if (line_cap == StrokeLineCap::ROUND){
            return out << "round";
        } else {
            return out << "square";
        }
    }

    std::ostream& operator<< (std::ostream & out, const Color & color) {
        visit(SolutionPrinter{}, color);
        return out;
    }

    std::ostream& operator<< (std::ostream &out, const StrokeLineJoin & line_join) {
        if (line_join == StrokeLineJoin::ARCS){
            return out << "arcs";
        } else if (line_join == StrokeLineJoin::BEVEL){
            return out << "bevel";
        } else if (line_join == StrokeLineJoin::MITER){
            return out << "miter";
        } else if (line_join == StrokeLineJoin::MITER_CLIP){
            return out << "miter-clip";
        } else {
            return out << "round";
        }
    }

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }


// %%%%%%%%%% Circle %%%%%%%%%%

    Circle& Circle::SetCenter(Point center)  {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)  {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << R"(<circle cx=")" << center_.x << R"(" cy=")" << center_.y << R"(" )";
        out << R"(r=")" << radius_ << R"(")";
        RenderAttrs(context.out);
        out << " />"sv;
    }

// %%%%%%%%%% Polyline %%%%%%%%%%
    Polyline& Polyline::AddPoint(Point point){
        points_.emplace_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        bool is_first = true;
        out << R"(<polyline points=")";
        for (auto point : points_){
            if (is_first){
                out << point.x << "," << point.y;
                is_first = false;
            } else {
                out << " " << point.x << "," << point.y;
            }
        }
        out << "\"";
        RenderAttrs(context.out);
        out << R"( />)";
    }

// %%%%%%%%%% Text %%%%%%%%%%

    Text& Text::SetPosition(Point pos){
        pos_ = pos;
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size) {
        size_ = size;
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << R"(<text)";
        RenderAttrs(context.out);
        out << R"( x=")" << pos_.x << R"(" y=")" << pos_.y << R"(" )";
        out << R"(dx=")" << offset_.x << R"(" dy=")" << offset_.y << R"(" )";
        out << R"(font-size=")" << size_;
        if (!font_family_.empty()){
            out << R"(" font-family=")" << font_family_;
        }
        if (!font_weight_.empty())   {
            out << R"(" font-weight=")" << font_weight_;
        }
        out << R"(">)";
        out << data_;
        out << "</text>"sv;
    }

// %%%%%%%%%% Document %%%%%%%%%%

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        std::cout << R"(<?xml version="1.0" encoding="UTF-8" ?>)" << std::endl;
        std::cout << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)" << std::endl;
        for (const auto & object : objects_){
            RenderContext context(out);
            object->Render(context);
        }
        std::cout << "</svg>"sv;
    }

}  // namespace svg