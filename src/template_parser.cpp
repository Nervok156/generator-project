#include "template_parser.hpp"
#include <stdexcept>
#include <cctype>

static std::string trim(const std::string& s) {
    size_t start = 0, end = s.size();
    while (start < end && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

TemplateParser::TemplateParser(ModuleLoader& loader) : loader_(loader) {}

std::string TemplateParser::render(
    const std::string& tmpl,
    const std::unordered_map<std::string, std::string>& data) {
    std::string out;
    size_t pos = 0;
    while (pos < tmpl.size()) {
        size_t start = tmpl.find("{{", pos);
        if (start == std::string::npos) {
            out += tmpl.substr(pos);
            break;
        }
        out += tmpl.substr(pos, start - pos);
        size_t end = tmpl.find("}}", start + 2);
        if (end == std::string::npos) {
            throw std::runtime_error("Unclosed {{ in template");
        }
        std::string expr = trim(tmpl.substr(start + 2, end - start - 2));
        out += resolve(expr, data);
        pos = end + 2;
    }
    return out;
}

std::string TemplateParser::resolve(
    const std::string& expr,
    const std::unordered_map<std::string, std::string>& data) {
    size_t paren = expr.find('(');
    if (paren != std::string::npos) {
        size_t close = expr.rfind(')');
        if (close == std::string::npos || close < paren) {
            throw std::runtime_error("Invalid function call: " + expr);
        }
        std::string func = trim(expr.substr(0, paren));
        std::string args = expr.substr(paren + 1, close - paren - 1);
        return loader_.call(func, args);
    }
    auto it = data.find(expr);
    if (it != data.end()) return it->second;
    throw std::runtime_error("Unknown variable: " + expr);
}