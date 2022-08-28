#include "json.h"
#include "json_reader.h"
#include <utility>
#include <sstream>
#include <iomanip>

using namespace std;

namespace json {

    Node LoadNode(istream& input);

    Node LoadArray(istream& input);

    // %%%%%%%%%%  Node class Type Check  %%%%%%%%%%

    bool Node::IsInt() const {if(std::holds_alternative<int>(value_)){return true;} else {return false;}}

    bool Node::IsDouble() const {if(std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_)){return true;} else {return false;}}

    bool Node::IsPureDouble() const {if(std::holds_alternative<double>(value_)){return true;} else {return false;}}

    bool Node::IsBool() const {if(std::holds_alternative<bool>(value_)){return true;} else {return false;}}

    bool Node::IsString() const {if(std::holds_alternative<std::string>(value_)){return true;} else {return false;}}

    bool Node::IsNull() const {if(std::holds_alternative<std::nullptr_t>(value_)){return true;} else {return false;}}

    bool Node::IsArray() const {if(std::holds_alternative<Array>(value_)){return true;} else {return false;}}

    bool Node::IsMap() const {if(std::holds_alternative<Dict>(value_)){return true;} else {return false;}}

    // %%%%%%%%%%  Node class Get Value  %%%%%%%%%%

    int Node::AsInt() const {
        if (IsInt()){
            return std::get<int>(value_);
        } else {
            throw std::logic_error ("Invalid type");
        }
    }

    bool Node::AsBool() const{
        if (IsBool()){
            return std::get<bool>(value_);
        } else {
            throw std::logic_error ("Invalid type");
        }
    }

    double Node::AsDouble() const {
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

    const std::string& Node::AsString() const{
        if (IsString()){
            return std::get<std::string>(value_);
        } else {
            throw std::logic_error ("Invalid type");
        }
    }

    const Array& Node::AsArray() const{
        if (IsArray()){
            return std::get<Array>(value_);
        } else {
            throw std::logic_error ("Invalid type");
        }
    }

    const Dict& Node::AsMap() const {
        if (IsMap()){
            return std::get<Dict>(value_);
        } else {
            throw std::logic_error ("Invalid type");
        }
    }

    Node LoadString(istream& input) {
        bool flag_quotes_err = true;
        char c;
        string s;
        while (input.get(c)) {
            if (c == '"'){flag_quotes_err = false; break;}
            if (c == '\\'){
                input.get(c);
                if (c == 'r'){s.push_back('\r'); continue;} else
                if (c == 'n'){s.push_back('\n'); continue;} else
                if (c == 't'){s.push_back('\t'); continue;} else
                if (c == '\"'){s += R"(")"s; continue;} else
                if (c == '\\'){s += R"(\)"s; continue;}
            }
            if ((c != '\t' && c != '\r' && c != '\n' && c != '\"' && c != '\\')){
                s.push_back(c);
            }
        }
        if (flag_quotes_err){
            throw ParsingError ("ParsingError E0: flag_quotes_err");
        }
        return {move(s)};
    }

    Node LoadNumber(istream& input){
        char c;
        string s;
        while (input >> c && c != ',' && c != ']' && c != '}'){
            s.push_back(c);
        }
        if (c == ']' || c == '}') {input.putback(c);}
        if (s == "nul" || s == "tru" || s == "fals"){
            throw ParsingError ("ParsingError E1: nul, tru, fals");
        }
        if (s == "null"){
            return Node{};
        } else  if (s == "true"s){
            return Node{true};
        } else if (s == "false"s){
            return Node{false};
        }
        if (!(s[0] == '-' || isdigit(s[0]))){
            throw ParsingError ("ParsingError4");
        }
        if (s.find('.') == string::npos && s.find('+') == string::npos && s.find('e') == string::npos && s.find('E') == string::npos) {
            return {stoi(s)};
        } else {
            double r = stod(s);
            return r;
        }
    }

    Dict LoadDict(istream& input){
        char c;
        Dict m;
        std::string n_key;
        Node value;
        while (input.get(c) && c != '}') {
            if (c == ':') {
                m.insert({move(n_key), LoadNode(input)});
                continue;
            }
            if (c == '\"'){
                n_key = LoadString(input).AsString();
            }
        }
        return m;
    }

    Node LoadArray(istream& input){
        char c;
        Array a;
        while (input >> c && c != ']'){
            if (c != ','){
                input.putback(c);
            }
            a.push_back(move(LoadNode(input)));
        }
        if (c != ']'){ throw ParsingError ("ParsingError E8:"); }
        if (a.empty()){
            a.push_back({nullptr});
        }
        return a;
    }

    Node LoadNode(istream& input) {
        char c;
        input >> c;
        if (c == '['){
            return LoadArray(input);
        } else if (c == '{'){
            return LoadDict(input);
        } else if (c == '"') {
            return LoadString(input);
        } else {
            if (c == ']' || c == '}'){
                throw ParsingError ("ParsingError E2: ] } --");
            }
            input.putback(c);
            return LoadNumber(input);
        }
    }

    void JsonOutput (const Document& doc, std::ostream& output) {
        if (doc.GetRoot().IsNull()){
            output << "null";
        }
        if (doc.GetRoot().IsInt()){
            output << doc.GetRoot().AsInt();
        }
        if (doc.GetRoot().IsPureDouble()){
            output << doc.GetRoot().AsDouble();
        }
        if (doc.GetRoot().IsBool()){
            if (doc.GetRoot().AsBool()){
                output << "true"s;
            } else {
                output << "false"s;
            }
        }
        if (doc.GetRoot().IsString()){
            std::string r = "\"";
            std::string origin = doc.GetRoot().AsString();
            auto itr = origin.begin();
            while (itr != origin.end()){
                if (*itr == '\r'){ r.push_back('\\'); r.push_back('r'); itr++; continue; }
                if (*itr == '\n'){ r.push_back('\\'); r.push_back('n'); itr++; continue; }
                if (*itr == '\"'){ r.push_back('\\'); r.push_back('"'); itr++; continue; }
                if (*itr == '\\'){ std::string s = R"(\\)"s; r += s; itr++; continue; }
                r.push_back(*itr);
                itr++;
            }
            r.push_back('\"');
            output << r;
        }
        if (doc.GetRoot().IsArray()){
            bool is_first_node = true;
            output << "["s;
            for (const auto& node : doc.GetRoot().AsArray()){
                if (is_first_node){
                    is_first_node = false;
                } else {
                    output << ","s;
                }
                JsonOutput(Document{node}, output);
            }
            output << "]"s;
        }
        if (doc.GetRoot().IsMap()){
            bool is_first_pair = true;
            output << "{"s;
            for (auto [key, value] : doc.GetRoot().AsMap()){
                if (is_first_pair){
                    is_first_pair = false;
                } else {
                    output << ", "s;
                }
                JsonOutput(Document{key}, output);
                output << ": "s;
                JsonOutput(Document{value}, output);
            }
            output << "}"s;
        }
    }

    Document::Document(Node root)
            : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document JsonBuilder(istream& input) {
        return Document{LoadNode(input)};
    }

    Document JsonFileLoad(const std::string& f_path){
        std::string f_clear_data;
        std::string f_data;
        std::string f_line;
        std::ifstream json_i_stream (f_path);
        if (json_i_stream.is_open()){
            return JsonBuilder(json_i_stream);
        } else {
            std::cerr << "No file found"s << std::endl;
        }
        return JsonBuilder(json_i_stream);
    }

    Document JsonConsoleLoad(std::istream& input){
        return {JsonBuilder(input)};
    }

    void JsonConsoleOutput(const Document& doc){
        std::ostringstream out;
        json::JsonOutput(Document{doc.GetRoot()}, out);
        cout << out.str();
    }

    void Print(const Document& doc, std::ostream& output) {
        if(output){
            JsonConsoleOutput(doc);
        }
    }

    void JsonFileWrite(const Document& doc, const std::string& f_path){
        std::ofstream json_o_stream (f_path);
        if (json_o_stream.is_open()){
            JsonOutput(doc, json_o_stream);
        } else {
            std::cerr << "No file found"s << std::endl;
        }
    }

    bool operator== (const Node & l, const Node & r){
        if(l.IsNull() && r.IsNull()){return true;}
        if(l.IsInt() && r.IsInt()){if(l.AsInt() == r.AsInt()){return true;}}
        if(l.IsPureDouble() && r.IsPureDouble()){if(l.AsDouble() == r.AsDouble()){return true;}}
        if(l.IsBool() && r.IsBool()){if(l.AsBool() == r.AsBool()){return true;}}
        if(l.IsString() && r.IsString()){if(l.AsString() == r.AsString()){return true;}}
        if(l.IsArray() && r.IsArray()){if(l.AsArray() == r.AsArray()){return true;}}
        if(l.IsMap() && r.IsMap()){if(l.AsMap() == r.AsMap()){return true;}}
        return false;
    }

    bool operator!= (const Node & l, const Node & r){
        if(l == r){return false;}
        return true;
    }

    bool operator== (const Document & l, const Document & r){
        if (l.GetRoot() == r.GetRoot()){
            return true;
        }
        return false;
    }

    bool operator!= (const Document & l, const Document & r){
        if (l.GetRoot() == r.GetRoot()){
            return false;
        }
        return true;
    }

}  // namespace json