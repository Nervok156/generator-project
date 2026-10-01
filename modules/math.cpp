#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>

extern "C" char* execute(const char* func, const char* args) {
    if (std::strcmp(func, "sum") != 0) return nullptr;

    std::string s(args);
    size_t comma = s.find(',');
    if (comma == std::string::npos) return nullptr;

    try {
        double a = std::stod(s.substr(0, comma));
        double b = std::stod(s.substr(comma + 1));
        double res = a + b;

        std::ostringstream oss;
        oss << res;
        std::string out = oss.str();

        char* ret = static_cast<char*>(std::malloc(out.size() + 1));
        std::strcpy(ret, out.c_str());
        return ret;
    } catch (...) {
        return nullptr;
    }
}