#include "json_parser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

static void skipWs(const std::string& s, size_t& i) {
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
}

static std::string parseString(const std::string& s, size_t& i) {
    std::string out;
    if (i >= s.size() || s[i] != '"') throw std::runtime_error("expected string");
    ++i;
    while (i < s.size() && s[i] != '"') {
        if (s[i] == '\\' && i + 1 < s.size()) {
            ++i;
            switch (s[i]) {
                case 'n': out += '\n'; break;
                case 't': out += '\t'; break;
                case 'r': out += '\r'; break;
                case '\\': out += '\\'; break;
                case '"': out += '"'; break;
                default: out += s[i]; break;
            }
        } else {
            out += s[i];
        }
        ++i;
    }
    if (i >= s.size()) throw std::runtime_error("unterminated string");
    ++i;
    return out;
}

std::unordered_map<std::string, std::string> JsonParser::parseFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open JSON: " + path);
    std::stringstream ss;
    ss << f.rdbuf();
    std::string s = ss.str();

    std::unordered_map<std::string, std::string> data;
    size_t i = 0;
    skipWs(s, i);
    if (i >= s.size() || s[i] != '{') throw std::runtime_error("JSON must be object");
    ++i;

    while (true) {
        skipWs(s, i);
        if (i < s.size() && s[i] == '}') { ++i; break; }
        std::string key = parseString(s, i);
        skipWs(s, i);
        if (i >= s.size() || s[i] != ':') throw std::runtime_error("expected :");
        ++i;
        skipWs(s, i);

        std::string value;
        if (s[i] == '"') {
            value = parseString(s, i);
        } else if (s[i] == '[') {
            ++i;
            while (true) {
                skipWs(s, i);
                if (i < s.size() && s[i] == ']') { ++i; break; }
                if (!value.empty()) value += '\n';
                value += parseString(s, i);
                skipWs(s, i);
                if (i < s.size() && s[i] == ',') { ++i; continue; }
                if (i < s.size() && s[i] == ']') { ++i; break; }
                throw std::runtime_error("expected , or ]");
            }
        } else {
            size_t start = i;
            while (i < s.size() && s[i] != ',' && s[i] != '}') ++i;
            value = s.substr(start, i - start);
            while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) value.pop_back();
            while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) value.erase(value.begin());
        }
        data[key] = value;
        skipWs(s, i);
        if (i < s.size() && s[i] == ',') { ++i; continue; }
        if (i < s.size() && s[i] == '}') { ++i; break; }
        throw std::runtime_error("expected , or }");
    }
    return data;
}