#pragma once

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <string>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#ifdef _WIN32
using LibraryHandle = void*;
#else
using LibraryHandle = void*;
#endif

// 函数指针类型（使用ZataObjectPtr）
using ZataDllFunction = ZataObjectPtr(*)(const std::vector<ZataObjectPtr>&);

class ZataFunctionMap {
private:
    // 跨平台句柄类型（内部存储为void*）
    using LibraryHandle = void*;

    LibraryHandle lib_handle_;
    std::unordered_map<std::string, ZataDllFunction> functions_;

    // 修复：删除器参数统一为void*
    struct LibraryHandleDeleter {
        void operator()(void* h) const {  // 参数明确为void*
            if (h != nullptr) {
            #ifdef _WIN32
                // Windows下转换为HMODULE后释放
                FreeLibrary(reinterpret_cast<HMODULE>(h));
            #else
                // Linux/macOS下直接释放
                dlclose(h);
            #endif
            }
        }
    };

    // unique_ptr存储void*，删除器参数为void*，类型匹配
    std::unique_ptr<void, LibraryHandleDeleter> lib_handle_wrapper_;

public:
    // 构造函数
    ZataFunctionMap(LibraryHandle handle, const std::unordered_map<std::string, ZataDllFunction>& functions)
        : lib_handle_(handle), functions_(functions), lib_handle_wrapper_(handle) {}

    // 禁止复制，允许移动
    ZataFunctionMap(const ZataFunctionMap&) = delete;
    ZataFunctionMap& operator=(const ZataFunctionMap&) = delete;
    ZataFunctionMap(ZataFunctionMap&&) = default;
    ZataFunctionMap& operator=(ZataFunctionMap&&) = default;

    // 获取函数
    ZataDllFunction operator[](const std::string& function_name) const {
        auto it = functions_.find(function_name);
        if (it == functions_.end()) {
            throw std::runtime_error("Function '" + function_name + "' not found in library");
        }
        return it->second;
    }
};

// 加载动态库
inline ZataFunctionMap load_sll(const std::string& path, const std::vector<std::string>& all_fn) {
    LibraryHandle handle = nullptr;

    #ifdef _WIN32
    // Windows加载DLL（返回HMODULE，本质是void*）
    handle = reinterpret_cast<LibraryHandle>(LoadLibraryA(path.c_str()));
    if (!handle) {
        throw std::runtime_error("Failed to load DLL: " + path +
                                " (Error: " + std::to_string(GetLastError()) + ")");
    }
    #else
    // Linux/macOS加载.so
    handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        throw std::runtime_error("Failed to load .so: " + path +
                                " (Error: " + std::string(dlerror()) + ")");
    }
    #endif

    std::unordered_map<std::string, ZataDllFunction> functions;

    try {
        for (const auto& fn_name : all_fn) {
            ZataDllFunction func = nullptr;
            #ifdef _WIN32
            func = reinterpret_cast<ZataDllFunction>(GetProcAddress(reinterpret_cast<HMODULE>(handle), fn_name.c_str()));
            if (!func) {
                throw std::runtime_error("Function '" + fn_name + "' not found (Error: " +
                                        std::to_string(GetLastError()) + ")");
            }
            #else
            dlerror();  // 清除错误
            func = reinterpret_cast<ZataDllFunction>(dlsym(handle, fn_name.c_str()));
            const char* err = dlerror();
            if (err || !func) {
                throw std::runtime_error("Function '" + fn_name + "' not found (Error: " +
                                        std::string(err ? err : "Unknown") + ")");
            }
            #endif
            functions[fn_name] = func;
        }
    }
    catch (...) {
        // 释放句柄
        if (handle) {
        #ifdef _WIN32
            FreeLibrary(reinterpret_cast<HMODULE>(handle));
        #else
            dlclose(handle);
        #endif
        }
        throw;
    }

    return ZataFunctionMap(handle, functions);
}

