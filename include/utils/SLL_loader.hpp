#pragma once

#include <windows.h>
#include <unordered_map>
#include <vector>
#include <any>
#include <stdexcept>
#include <string>
#include <memory>

// 定义函数指针类型
using DllFunction = std::any(*)(const std::vector<std::any>&);

// 函数映射类
class FunctionMap {
private:
    HMODULE dll_handle_;
    std::unordered_map<std::string, DllFunction> functions_;

    // 自定义删除器，用于管理DLL句柄
    struct DllHandleDeleter {
        void operator()(HMODULE h) const {
            if (h != nullptr) {
                FreeLibrary(h);
            }
        }
    };

    std::unique_ptr<void, DllHandleDeleter> dll_handle_wrapper_;

public:
    // 构造函数
    FunctionMap(HMODULE handle, const std::unordered_map<std::string, DllFunction>& functions)
        : dll_handle_(handle), functions_(functions), dll_handle_wrapper_(handle) {}

    // 禁止复制构造和赋值
    FunctionMap(const FunctionMap&) = delete;
    FunctionMap& operator=(const FunctionMap&) = delete;

    // 允许移动构造和赋值
    FunctionMap(FunctionMap&&) = default;
    FunctionMap& operator=(FunctionMap&&) = default;

    // 通过函数名获取函数
    DllFunction operator[](const std::string& function_name) const {
        auto it = functions_.find(function_name);
        if (it == functions_.end()) {
            throw std::runtime_error("Function '" + function_name + "' not found in DLL");
        }
        return it->second;
    }

    // 检查是否包含某个函数
    bool contains(const std::string& function_name) const {
        return functions_.find(function_name) != functions_.end();
    }

    // 获取已加载的函数名列表
    std::vector<std::string> get_function_names() const {
        std::vector<std::string> names;
        for (const auto& pair : functions_) {
            names.push_back(pair.first);
        }
        return names;
    }
};

// 加载动态链接库并获取指定的函数
inline FunctionMap load_sll(const std::string& path, const std::vector<std::string>& all_fn) {
    // 加载DLL
    HMODULE hModule = LoadLibraryA(path.c_str());
    if (hModule == nullptr) {
        throw std::runtime_error("Failed to load DLL: " + path +
                                ". Error code: " + std::to_string(GetLastError()));
    }

    std::unordered_map<std::string, DllFunction> functions;

    try {
        // 加载所有指定的函数
        for (const std::string& fn_name : all_fn) {
            auto func = reinterpret_cast<DllFunction>(GetProcAddress(hModule, fn_name.c_str()));
            if (func == nullptr) {
                throw std::runtime_error("Failed to find function '" + fn_name +
                                        "' in DLL. Error code: " + std::to_string(GetLastError()));
            }
            functions[fn_name] = func;
        }
    }
    catch (...) {
        // 如果加载函数失败，释放DLL并重新抛出异常
        FreeLibrary(hModule);
        throw;
    }

    return FunctionMap(hModule, functions);
}
