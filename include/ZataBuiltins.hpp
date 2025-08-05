#ifndef ZVM_OPCODES_H
#define ZVM_OPCODES_H
#include <ios>
#include <iostream>
#include <windows.h>

extern "C" inline void zata_print(const std::string& values,
    const std::string& sep,
    const std::string& end) {

    std::cout << values << sep << end;
}

extern "C" inline std::string zata_input(const std::string& proper) {
    std::cout << proper;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

extern "C" inline float now() {
    #ifdef _WIN32
    LARGE_INTEGER freq;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    // 转换为秒（计数器值 / 频率）
    return (float)((double)counter.QuadPart / (double)freq.QuadPart);

    #else
    // 类 Unix 平台
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);  // 使用单调时钟
    return (float)ts.tv_sec + (float)ts.tv_nsec / 1000000000.0f;
    #endif
}
#endif
