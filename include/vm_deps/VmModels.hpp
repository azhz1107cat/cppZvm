#ifndef VM_DEPS_HPP
#define VM_DEPS_HPP
#include <vector>

#include "../models/Objects.hpp"
#include "builtins/builtins_functions.hpp"


// 上下文, 用于报错
struct Context{
    std::string file_path;
    std::string file_content;
};

// 块
struct Block {
    std::string name;
};

inline std::unordered_map<std::string,std::function<ZataObjectPtr(const std::vector<ZataObjectPtr>&)>> BuiltinsFunction = {
    {"print", zata_print},
    {"input", zata_input},
    {"now", zata_now}
};

#endif //VM_DEPS_HPP
