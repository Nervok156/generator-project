#include "template_parser.hpp"
#include "json_parser.hpp"
#include "module_loader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <unistd.h>

int main(int argc, char** argv) {
    std::string templatePath, dataPath, outputPath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--template" && i + 1 < argc) templatePath = argv[++i];
        else if (arg == "--data" && i + 1 < argc) dataPath = argv[++i];
        else if (arg == "--output" && i + 1 < argc) outputPath = argv[++i];
        else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: generator --template <file> --data <file> --output <file>\n";
            return 0;
        }
    }

    if (templatePath.empty() || dataPath.empty() || outputPath.empty()) {
        std::cerr << "Usage: generator --template <file> --data <file> --output <file>\n";
        return 1;
    }

    try {
        // 1. Читаем данные из JSON
        auto data = JsonParser::parseFile(dataPath);

        // 2. Читаем шаблон
        std::ifstream tf(templatePath);
        if (!tf) throw std::runtime_error("Cannot open template: " + templatePath);
        std::stringstream ss;
        ss << tf.rdbuf();
        std::string tmpl = ss.str();

        // 3. Определяем каталог модулей рядом с исполняемым файлом
        std::string moduleDir = "modules";
        char exeBuf[4096];
        ssize_t n = readlink("/proc/self/exe", exeBuf, sizeof(exeBuf) - 1);
        if (n > 0) {
            exeBuf[n] = '\0';
            std::filesystem::path exePath(exeBuf);
            moduleDir = (exePath.parent_path() / "modules").string();
        }

        // 4. Загружаем модули
        ModuleLoader loader(moduleDir);

        // 5. Рендерим шаблон
        TemplateParser parser(loader);
        std::string result = parser.render(tmpl, data);

        // 6. Пишем результат
        std::ofstream of(outputPath);
        if (!of) throw std::runtime_error("Cannot write output: " + outputPath);
        of << result;

        std::cout << "Generated: " << outputPath << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}