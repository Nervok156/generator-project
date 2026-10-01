#include "module_loader.hpp"
#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace fs = std::filesystem;

ModuleLoader::ModuleLoader(const std::string& dir) : dir_(dir) {
    loadAll();
}

ModuleLoader::~ModuleLoader() {
    for (auto& m : modules_) unload(m);
}

void ModuleLoader::unload(Module& m) {
    if (m.handle) {
        dlclose(m.handle);
        m.handle = nullptr;
        m.execute = nullptr;
    }
}

void ModuleLoader::loadAll() {
    if (!fs::exists(dir_)) {
        std::cerr << "Warning: modules dir not found: " << dir_ << "\n";
        return;
    }
    for (auto& entry : fs::directory_iterator(dir_)) {
        if (entry.path().extension() == ".so") {
            Module m;
            m.path = entry.path().string();
            struct stat st;
            if (stat(m.path.c_str(), &st) == 0) m.mtime = st.st_mtime;

            m.handle = dlopen(m.path.c_str(), RTLD_LAZY | RTLD_LOCAL);
            if (!m.handle) {
                std::cerr << "Warning: cannot load " << m.path << ": " << dlerror() << "\n";
                continue;
            }
            m.execute = reinterpret_cast<char* (*)(const char*, const char*)>(
                dlsym(m.handle, "execute"));
            if (!m.execute) {
                std::cerr << "Warning: module " << m.path << " has no execute()\n";
                dlclose(m.handle);
                continue;
            }
            modules_.push_back(m);
        }
    }
}

void ModuleLoader::reloadIfChanged() {
    for (auto& m : modules_) {
        struct stat st;
        if (stat(m.path.c_str(), &st) == 0 && st.st_mtime != m.mtime) {
            std::cerr << "Reloading module: " << m.path << "\n";
            unload(m);
            m.mtime = st.st_mtime;
            m.handle = dlopen(m.path.c_str(), RTLD_LAZY | RTLD_LOCAL);
            if (!m.handle) {
                std::cerr << "Warning: cannot reload " << m.path << ": " << dlerror() << "\n";
                continue;
            }
            m.execute = reinterpret_cast<char* (*)(const char*, const char*)>(
                dlsym(m.handle, "execute"));
            if (!m.execute) {
                std::cerr << "Warning: reloaded module " << m.path << " has no execute()\n";
                unload(m);
            }
        }
    }

    if (!fs::exists(dir_)) return;
    for (auto& entry : fs::directory_iterator(dir_)) {
        if (entry.path().extension() == ".so") {
            std::string path = entry.path().string();
            bool found = false;
            for (auto& m : modules_) if (m.path == path) { found = true; break; }
            if (!found) {
                Module m;
                m.path = path;
                struct stat st;
                if (stat(m.path.c_str(), &st) == 0) m.mtime = st.st_mtime;
                m.handle = dlopen(m.path.c_str(), RTLD_LAZY | RTLD_LOCAL);
                if (!m.handle) continue;
                m.execute = reinterpret_cast<char* (*)(const char*, const char*)>(
                    dlsym(m.handle, "execute"));
                if (!m.execute) { dlclose(m.handle); continue; }
                modules_.push_back(m);
            }
        }
    }
}

std::string ModuleLoader::call(const std::string& func, const std::string& args) {
    reloadIfChanged();
    for (auto& m : modules_) {
        if (!m.execute) continue;
        char* res = m.execute(func.c_str(), args.c_str());
        if (res) {
            std::string out(res);
            free(res);
            return out;
        }
    }
    throw std::runtime_error("No module handled function: " + func);
}