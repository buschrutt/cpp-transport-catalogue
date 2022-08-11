#include "json.h"
#include "json_reader.h"
#include <utility>
#include <sstream>

using namespace std;

namespace json_lib {

    namespace {

        //Node LoadNode(istream& input);
        Node LoadArray(const std::string& input_string);

        std::string SpaceDelete(std::string s) {
            s = s.substr(s.find_first_not_of(' '));
            s = s.substr(0, s.find_last_not_of(' ') + 1);
            return s;
        }

        Node LoadString(const std::string& input_string) {
            std::string s = input_string;
            if (s == "null"s){
                return Node{};
            } else if (s == "true"s || s == "false"s){
                if (s == "true"s){
                    return Node{true};
                } else {
                    return Node{false};
                }
            } else {
                if (s[0] == '\"' && s[s.size() - 1] != '\"'){
                    throw json_lib::ParsingError ("ParsingError"s);
                }
                return Node{s};
            }
        }

        Node LoadNumber(const std::string& s){
            if (s.find('.') == string::npos && s.find('e') == string::npos && s.find('E') == string::npos) {
                return {stoi(s)};
            } else {
                return {stod(s)};
            }
        }

        std::string FindMapString (size_t k, const std::string& s){
            std::string s_return;
            size_t step_count = 0;
            for (size_t i = k; i < s.size(); i++){
                if (s[i] == '{'){
                    step_count++;
                }
                if (s[i] == '}'){
                    step_count--;
                }
                s_return.push_back(s[i]);
                if (step_count == 0){
                    break;
                }
            }
            return s_return;
        }

        std::string FindArrayString (size_t k, const std::string& s){
            std::string s_return;
            size_t step_count = 0;
            for (size_t i = k; i < s.size(); i++){
                if (s[i] == '['){
                    step_count++;
                }
                if (s[i] == ']'){
                    step_count--;
                }
                s_return.push_back(s[i]);
                if (step_count == 0){
                    break;
                }
            }
            return s_return;
        }

        Dict LoadDict(const std::string& input_string){
            std::string s = input_string;
            std::string buffer;
            std::string node_key;
            Dict result_dict;
            for(size_t i = 1; i < s.size(); i++){
                while (s[i] != ','){
                    if (s[i] == '}'){
                        break;
                    }
                    if (s[i] == ':'){
                        node_key = SpaceDelete(buffer);
                        buffer.clear();
                        i++;
                        while (s[i] == ' '){
                            i++;
                        }
                        // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                        if (s[i] == '{'){
                            std::string map_string = FindMapString(i, s);
                            node_key = node_key.substr(node_key.find_first_not_of('\"'), node_key.find_last_not_of('\"'));
                            result_dict.emplace(node_key, (LoadDict(map_string)));
                            i += map_string.size();
                            while (s[i] == ' '){
                                i++;
                            }
                            break;
                        }
                        // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                        // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                        if (s[i] == '['){
                            std::string array_string = FindArrayString(i, s);
                            node_key = node_key.substr(node_key.find_first_not_of('\"'), node_key.find_last_not_of('\"'));
                            result_dict.emplace(node_key, (LoadArray(array_string)));
                            i += array_string.size();
                            while (s[i] == ' '){
                                i++;
                            }
                            break;
                        }
                        // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    }
                    buffer.push_back(s[i]);
                    i++;
                }
                if (buffer.empty()){
                    continue;
                }
                buffer = SpaceDelete(buffer);
                if (isdigit(buffer[0]) || buffer[0] == '-'){
                    node_key = node_key.substr(node_key.find_first_not_of('\"'), node_key.find_last_not_of('\"'));
                    result_dict.emplace(node_key, LoadNumber(buffer));
                } else {
                    node_key = node_key.substr(node_key.find_first_not_of('\"'), node_key.find_last_not_of('\"'));
                    if (buffer.find('\"') != string::npos){
                        buffer = buffer.substr(buffer.find_first_not_of('\"'), buffer.find_last_not_of('\"'));
                    }
                    result_dict.emplace(node_key, LoadString(buffer));
                }
                buffer.clear();
            }
            return result_dict;
        }

        Node LoadArray(const std::string& input_string){
            std::string s = input_string;
            std::string node_s;
            Array result_array;
            for(size_t i = 1; i < s.size(); i++){
                while (s[i] == ' '){
                    i++;
                }
                // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                if (s[i] == '{'){
                    std::string map_string = FindMapString(i, s);
                    result_array.push_back(LoadDict(map_string));
                    i += map_string.size();
                    while (s[i] == ' '){
                        i++;
                    }
                }
                // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                if (s[i] == '['){
                    std::string array_string = FindArrayString(i, s);
                    result_array.push_back(LoadArray(array_string));
                    i += array_string.size();
                    while (s[i] == ' '){
                        i++;
                    }
                }
                // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                while (s[i] != ','){
                    if (s[i] == ']'){
                        break;
                    }
                    node_s.push_back(s[i]);
                    i++;
                }
                if (!node_s.empty()){
                    node_s = SpaceDelete(node_s);
                    if (isdigit(node_s[0]) || node_s[0] == '-'){
                        result_array.push_back(LoadNumber(node_s));
                    } else {
                        if (node_s.find('\"') != string::npos){
                            node_s = node_s.substr(node_s.find_first_not_of('\"'), node_s.find_last_not_of('\"'));
                        }
                        result_array.push_back(LoadString(node_s));
                    }
                    node_s.clear();
                }
            }
            return result_array;
        }

        Node LoadNode(const std::string& node_string) {
            std::string_view node_w = node_string;
            if (node_w == "nul"s || node_w == "["s || node_w == "]"s || node_w == "{"s
                || node_w == "}"s || node_w == "tru"s || node_w == "fals"s){
                throw json_lib::ParsingError ("ParsingError"s);
            }
            if (isdigit(node_w[0]) || node_w[0] == '-'){
                return LoadNumber(node_string);
            } else if ((node_w[0]) == '['){
                return LoadArray(node_string);
            } else if ((node_w[0]) == '{'){
                return LoadDict(node_string);
            } else {
                return LoadString(node_string);
            }
        }
    }  // namespace

    void JsonOutput (const Document& doc, std::ostream& output) {
        if (doc.GetRoot().IsNull()){
            output << "null"s;
        }
        if (doc.GetRoot().IsString()){
            std::string result = "\""s;
            std::string origin = doc.GetRoot().AsString();
            auto itr = origin.begin();
            while (itr != origin.end()){
                if (*itr == '\r'){
                    result.push_back('\\');
                    result.push_back('r');
                    itr++;
                    continue;
                }
                if (*itr == '\n'){
                    result.push_back('\\');
                    result.push_back('n');
                    itr++;
                    continue;
                }
                if (*itr == '\"'){
                    result.push_back('\\');
                    result.push_back('"');
                    itr++;
                    continue;
                }
                if (*itr == '\\'){
                    std::string s = R"(\\)"s;
                    result += s;
                    itr++;
                    continue;
                }
                result.push_back(*itr);
                itr++;
            }
            result.push_back('\"');
            output << result;
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

    Document JsonBuilder(const std::string& f_clear_data) {
        return Document{LoadNode(f_clear_data)};
    }

    std::string JsonTrashDelete (std::string source_string){
        std::string result_string;
        auto itr = source_string.begin();
        auto itr_step = itr;
        while (itr != source_string.end()){
            if (*itr == '\\'){
                itr_step = itr;
                itr_step++;
                if (*itr_step == 't'){
                    result_string.push_back('\t');
                }
                if (*itr_step == 'r'){
                    result_string.push_back('\r');
                }
                if (*itr_step == 'n'){
                    result_string.push_back('\n');
                }
                if (*itr_step == '\\'){
                    result_string.push_back('\\');
                }
                if (*itr_step == '"'){
                    result_string.push_back('\"');
                }
                itr++;
            } else if (*itr != '\t' && *itr != '\n' && *itr != '\r' && *itr != '\\'){
                result_string.push_back(*itr);
            }
            itr++;
        }
        result_string = result_string.substr(result_string.find_first_not_of(' '));
        result_string = result_string.substr(0, result_string.find_last_not_of(' ') + 1);
        if (result_string[0] == '\"' && result_string[result_string.size() - 1] == '\"'){
            result_string = result_string.substr(1, result_string.size() - 2);
        }
        return result_string;
    }

    Document JsonFileLoad(const std::string& f_path){
        std::string f_clear_data;
        std::string f_data;
        std::string f_line;
        std::ifstream json_i_stream (f_path);
        if (json_i_stream.is_open()){
            while (std::getline(json_i_stream, f_line)){
                f_data += f_line;
            }
        } else {
            std::cerr << "No file found"s << std::endl;
        }
        return JsonBuilder(JsonTrashDelete(f_data));
    }

    Document JsonConsoleLoad(std::istream& input){
        char c;
        int b_count = 0;
        std::string cin_data;
        while (input >> c){
            if (c == '{'){
                b_count++;
            }
            if (c == '}'){
                b_count--;
                if (b_count == 0){
                    cin_data.push_back(c);
                    break;
                }
            }
            cin_data.push_back(c);
        }
        return {JsonBuilder(JsonTrashDelete({cin_data}))};
    }

    void JsonConsoleOutput(const Document& doc){
        std::ostringstream out;
        json_lib::JsonOutput(Document{doc.GetRoot()}, out);
        cout << out.str();
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
        if(l.IsNull() && r.IsNull()){return false;}
        if(l.IsInt() && r.IsInt()){if(l.AsInt() == r.IsInt()){return false;}}
        if(l.IsPureDouble() && r.IsPureDouble()){if(l.AsInt() == r.IsInt()){return false;}}
        if(l.IsBool() && r.IsBool()){if(l.AsInt() == r.IsInt()){return false;}}
        if(l.IsString() && r.IsString()){if(l.AsInt() == r.IsInt()){return false;}}
        if(l.IsArray() && r.IsArray()){if(l.AsInt() == r.IsInt()){return false;}}
        if(l.IsMap() && r.IsMap()){if(l.AsInt() == r.IsInt()){return false;}}
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