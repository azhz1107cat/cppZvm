#ifndef ZVM_OPCODES_H
#define ZVM_OPCODES_H
#include <ios>
#include <iostream>
#include <windows.h>

#include "builtins_type.hpp"
#include "models/Errors.hpp"

inline ZataObjectPtr zata_print(const std::vector<ZataObjectPtr>& arguments) {
    auto target = std::dynamic_pointer_cast<ZataBuiltinsClass>(arguments[0]);
    if (!target) {
        zata_vm_error_thrower({}, ZataError{
            .name = "ZataTypeError",
            .message = "print() argument must be a built-in object",
            .error_code = 0
        });
    }

    // 检查是否有type_str方法
    if (!target->object_type || !target->object_type->type_str) {
        zata_vm_error_thrower({}, ZataError{
            .name = "ZataRunTimeError",
            .message = "Can't print object without __str__ method",
            .error_code = 0
        });
    }

    // 调用type_str获取字符串表示
    auto str_obj = target->object_type->type_str({target});
    if (!str_obj) {
        zata_vm_error_thrower({}, ZataError{
            .name = "ZataRunTimeError",
            .message = "__str__ method returned null",
            .error_code = 0
        });
    }

    // 打印
    auto str_val = std::dynamic_pointer_cast<ZataString>(str_obj);
    if (!str_val) {
        zata_vm_error_thrower({}, ZataError{
            .name = "ZataTypeError",
            .message = "__str__ method must return a string",
            .error_code = 0
        });
    }
    std::cout << str_val->val << std::endl;

    auto none = std::make_shared<ZataState>();
    none->val = 2;
    return none;
}

inline ZataObjectPtr zata_input(const std::vector<ZataObjectPtr>& arguments) {
    const auto prompt = std::dynamic_pointer_cast<ZataString>(arguments[0]);
    std::cout << prompt->val;
    std::string input;
    std::getline(std::cin, input);

    auto result = create_str(input);
    return result;
}

inline ZataObjectPtr zata_now(const std::vector<ZataObjectPtr>& arguments) {
    #ifdef _WIN32
    LARGE_INTEGER freq;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    // 转换为秒（计数器值 / 频率）
    auto second = (float)((double)counter.QuadPart / (double)freq.QuadPart);
    auto result = create_float(second);
    return result;

    #else
    // 类 Unix 平台
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);  // 使用单调时钟
    return (float)ts.tv_sec + (float)ts.tv_nsec / 1000000000.0f;
    #endif
}
#endif
