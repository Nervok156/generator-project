#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

extern "C" char* execute(const char* func, const char* args) {
    if (std::strcmp(func, "csv") != 0) return nullptr;

    std::ifstream f(args);
    if (!f) return nullptr;

    std::stringstream ss;
    ss << f.rdbuf();
    std::string content = ss.str();

    char* ret = static_cast<char*>(std::malloc(content.size() + 1));
    std::strcpy(ret, content.c_str());
    return ret;
}