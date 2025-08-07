#ifndef ZVM_OPCODES_H
#define ZVM_OPCODES_H
#include <ios>
#include <iostream>
#include <windows.h>

extern "C" inline void zata_print(const std::vector<ZataElem>& arguments) {
    const auto target_text = std::dynamic_pointer_cast<ZataString>(arguments[0]);
    if (!target_text) {
        zata_vm_error_thrower({},ZataError{
                        .name = "ZataRunTimeError",
                        .message = "Can't not print a object without __str__ method",
                        .error_code = 0
                    });
    }
    std::cout << target_text << std::endl;
}

extern "C" inline ZataString zata_input(const std::vector<ZataElem>& arguments) {
    std::cout << arguments[0];
    std::string input;
    std::getline(std::cin, input);
    auto result = ZataString();
    result.val = input;
    return result;
}

extern "C" inline ZataFloat zata_now(const std::vector<ZataElem>& arguments) {
    #ifdef _WIN32
    LARGE_INTEGER freq;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    // 转换为秒（计数器值 / 频率）
    auto second = (float)((double)counter.QuadPart / (double)freq.QuadPart);
    auto result = ZataFloat();
    result.val = second;
    return result;

    #else
    // 类 Unix 平台
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);  // 使用单调时钟
    return (float)ts.tv_sec + (float)ts.tv_nsec / 1000000000.0f;
    #endif
}
#endif
