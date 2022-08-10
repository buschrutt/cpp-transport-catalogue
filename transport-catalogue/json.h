#pragma once
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <variant>

namespace json_lib {

    class Node;
// Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node() : value_(nullptr) {};
        Node(std::nullptr_t) : value_(nullptr) {};
        Node(int value) : value_(value) {};
        Node(double value) : value_(value) {};
        Node(std::string value) : value_(value) {};
        Node(bool value) : value_(value) {};
        Node(Array value) : value_(value) {};
        Node(Dict value) : value_(value) {};
        //Node(Value value) : value_(std::move(value)) {};

        // %%%%%%%%%%  Type Check  %%%%%%%%%%

        bool IsInt() const {if(std::holds_alternative<int>(value_)){return true;} else {return false;}}

        bool IsDouble() const {if(std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_)){return true;} else {return false;}}

        bool IsPureDouble() const {if(std::holds_alternative<double>(value_)){return true;} else {return false;}}

        bool IsBool() const {if(std::holds_alternative<bool>(value_)){return true;} else {return false;}}

        bool IsString() const {if(std::holds_alternative<std::string>(value_)){return true;} else {return false;}}

        bool IsNull() const {if(std::holds_alternative<std::nullptr_t>(value_)){return true;} else {return false;}}

        bool IsArray() const {if(std::holds_alternative<Array>(value_)){return true;} else {return false;}}

        bool IsMap() const {if(std::holds_alternative<Dict>(value_)){return true;} else {return false;}}

        // %%%%%%%%%%  Get Value  %%%%%%%%%%

        int AsInt() const {
            if (IsInt()){
                return std::get<int>(value_);
            } else {
                throw std::logic_error ("Invalid type");
            }
        }

        bool AsBool() const{
            if (IsBool()){
                return std::get<bool>(value_);
            } else {
                throw std::logic_error ("Invalid type");
            }
        }

        double AsDouble() const {
            if (IsDouble()){
                if (IsPureDouble()){
                    return std::get<double>(value_);
                } else {
                    return (double) std::get<int>(value_);
                }
            } else {
                throw std::logic_error ("Invalid type");
            }
        }

        const std::string& AsString() const{
            if (IsString()){
                return std::get<std::string>(value_);
            } else {
                throw std::logic_error ("Invalid type");
            }
        }

        const Array& AsArray() const{
            if (IsArray()){
                return std::get<Array>(value_);
            } else {
                throw std::logic_error ("Invalid type");
            }
        }

        const Dict& AsMap() const {
            if (IsMap()){
                return std::get<Dict>(value_);
            } else {
                throw std::logic_error ("Invalid type");
            }
        }

    private:
        Value value_;
    };

    class Document {
    public:
        explicit Document(Node root);

        [[nodiscard]] const Node& GetRoot() const;

    private:
        Node root_;
    };

    std::string SpaceDelete(std::string s);

    Node LoadString(const std::string& input_string);

    Node LoadNumber(const std::string& input_string);

    std::string FindMapString (size_t k, const std::string& s);

    std::string FindArrayString (size_t k, const std::string& s);

    Dict LoadDict(const std::string& input_string);

    Node LoadArray(const std::string& input_string);

    Document JsonDbBuilder(const std::string& f_clear_data);

    void Print(const Document& doc, std::ostream& output);

    bool operator== (const Node & l, const Node & r);

    bool operator!= (const Node & l, const Node & r);

    bool operator== (const Document & l, const Document & r);

    bool operator!= (const Document & l, const Document & r);

}  // namespace json