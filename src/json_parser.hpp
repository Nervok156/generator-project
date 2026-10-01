#pragma once
#include <string>
#include <unordered_map>

class JsonParser {
public:
    static std::unordered_map<std::string, std::string> parseFile(const std::string& path);
};