#ifndef VMDEPS_HPP
#define VMDEPS_HPP
#include <vector>

#include "../models/Objects.hpp"

// 调用帧
struct CallFrame {
    int pc = 0;
    std::vector<ZataObjectPtr> locals;
    int return_address = 0;
    std::string function_name;
    std::vector<ZataObjectPtr> constant_pool;
    std::vector<int> code;
};

enum BinaryCalc {

};

enum UnaryCalc {

};

// 上下文, 用于报错
struct Context{
    std::string file_path;
    std::string file_content;
};

// 块
struct Block {
    std::string name = "";
};

#endif //VMDEPS_HPP
