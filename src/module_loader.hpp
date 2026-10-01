#pragma once
#include <string>
#include <vector>
#include <dlfcn.h>
#include <sys/stat.h>

class ModuleLoader {
public:
    explicit ModuleLoader(const std::string& dir);
    ~ModuleLoader();
    std::string call(const std::string& func, const std::string& args);
    void reloadIfChanged();

private:
    struct Module {
        std::string path;
        void* handle = nullptr;
        time_t mtime = 0;
        char* (*execute)(const char*, const char*) = nullptr;
    };

    std::string dir_;
    std::vector<Module> modules_;

    void loadAll();
    void unload(Module& m);
};