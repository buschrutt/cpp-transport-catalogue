// %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
//json_lib
//json_lib::Document json_lib::JsonFileLoad(std::string f_path);
//void json_lib::JsonFileWrite(std::string f_path);
//json_lib::Document json_lib::JsonBuilder(std::string f_clear_data);
//std::string json_lib::JsonTrashDelete(std::string s_source);
//void JsonPrintRare (const Document& d1, std::ostream& output);

// ?? bool json_lib::JsonEquals(json_lib::Document d1, json_lib::Document d1,);

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

        Node() : value_(nullptr) {}
        Node(std::nullptr_t) : value_(nullptr) {}
        Node(int value) : value_(value) {}
        Node(double value) : value_(value) {}
        Node(std::string value) : value_(value) {}
        Node(bool value) : value_(value) {}
        Node(Array value) : value_(std::move(value)) {};
        Node(Dict value) : value_(std::move(value)) {};

        // %%%%%%%%%%  Type Check  %%%%%%%%%%

        [[nodiscard]] bool IsInt() const;

        [[nodiscard]] bool IsDouble() const;

        [[nodiscard]] bool IsPureDouble() const;

        [[nodiscard]] bool IsBool() const;

        [[nodiscard]] bool IsString() const;

        [[nodiscard]] bool IsNull() const;

        [[nodiscard]] bool IsArray() const;

        [[nodiscard]] bool IsMap() const;

        // %%%%%%%%%%  Get Value  %%%%%%%%%%

        [[nodiscard]] int AsInt() const;

        [[nodiscard]] bool AsBool() const;

        [[nodiscard]] double AsDouble() const;

        [[nodiscard]] const std::string& AsString() const;

        [[nodiscard]] const Array& AsArray() const;

        [[nodiscard]] const Dict& AsMap() const;

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

    // %%%%%%%%%%  json_lib methods  %%%%%%%%%%

    Node LoadString(std::istream& input);

    Node LoadNumber(std::istream& input);

    Dict LoadDict(std::istream& input);

    Node LoadArray(std::istream& input);

    Document JsonFileLoad(const std::string& f_path);

    //Document Load(std::istream& input);

    void JsonOutput (const Document& doc, std::ostream& output);

    Document JsonBuilder(std::istream& input);

    //Document JsonFileLoad(const std::string& f_path);

    Document JsonConsoleLoad(std::istream& input);

    void JsonConsoleOutput(const Document& doc);

    void JsonFileWrite(const Document& doc, const std::string& f_path);

    bool operator== (const Node & l, const Node & r);

    bool operator!= (const Node & l, const Node & r);

    bool operator== (const Document & l, const Document & r);

    bool operator!= (const Document & l, const Document & r);

}  // namespace json_lib