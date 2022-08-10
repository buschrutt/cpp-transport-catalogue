#include "json_reader.h"


namespace json_reader {

    std::string TrashDelete (std::string source_string){
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

    std::string JsonFileRead(const std::string &f_name) {
        std::string f_clear_data;
        std::string f_data;
        std::string f_line;
        std::ifstream json_i_stream (f_name);
        if (json_i_stream.is_open()){
            while (std::getline(json_i_stream, f_line)){
                f_data += f_line;
            }
        }
        return TrashDelete(f_data);
    }

    void JsonFileWrite(const json_lib::Document& json_doc, const std::string& f_name){
        std::ofstream json_o_stream (f_name);
        if (json_o_stream.is_open()){
            Print(json_doc, json_o_stream);;
        }
    }

}