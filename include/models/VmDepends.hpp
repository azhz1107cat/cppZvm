#ifndef VMDEPS_HPP
#define VMDEPS_HPP
#include <vector>

#include "Objects.hpp"

// 调用帧
struct CallFrame {
    int pc = 0;
    std::vector<ZataObjectPtr> locals;
    int return_address = 0;
    std::string function_name;
    std::vector<ZataObjectPtr> constant_pool;
    std::vector<int> code;
};

// 内存帧
struct Allocation {
    uint64_t size;
    ZataObjectPtr data;
};

#endif //VMDEPS_HPP
