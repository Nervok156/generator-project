#pragma once
#include <string>
#include <unordered_map>
#include "module_loader.hpp"

class TemplateParser {
public:
    explicit TemplateParser(ModuleLoader& loader);
    std::string render(const std::string& tmpl,
                       const std::unordered_map<std::string, std::string>& data);

private:
    ModuleLoader& loader_;
    std::string resolve(const std::string& expr,
                        const std::unordered_map<std::string, std::string>& data);
};