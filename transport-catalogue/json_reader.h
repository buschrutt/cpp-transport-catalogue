#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "json.h"

namespace json_reader {

    std::string TrashDelete (std::string source_string);

    std::string JsonFileRead(const std::string &file_name);

    void JsonFileWrite(const json_lib::Document& json_doc, const std::string& file_name);

}