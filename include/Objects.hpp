#ifndef ZATA_OBJECTS_H
#define ZATA_OBJECTS_H

#include <string>
#include <vector>
#include <utility>
#include <memory>


// 基类
struct ZataObject {
    virtual ~ZataObject() = default;
};

using ZataElem = std::shared_ptr<ZataObject>;

// 模块对象
struct ZataModule final : ZataObject {
    std::string module_name;
    int local_count = 0;
    std::vector<int> bytecode;
    std::vector<ZataElem> consts;
    std::vector<std::pair<int, int>> line_number_map;
};

// 函数对象
struct ZataFunction final : ZataObject {
    std::string function_name;
    int arg_count = 0;
    int local_count = 0;
    std::vector<ZataElem> consts;
    std::vector<int> bytecode;
    std::vector<std::pair<int, int>> line_number_map;
};

// 类对象
struct ZataClass final : ZataObject {
    std::string class_name;

    std::vector<std::shared_ptr<ZataClass>> parent_class;
    std::vector<ZataObject> consts;

    std::vector<std::shared_ptr<ZataObject>> fields;
    std::vector<std::pair<int, int>> line_number_map;
};

// 实例对象
struct ZataInstance final : ZataObject {
    std::shared_ptr<ZataClass> ref_class = nullptr;
    std::vector<ZataElem> fields;
};

// 字符串对象
struct ZataString final : ZataObject {
    std::string val;
};

// 整数对象
struct ZataInt final : ZataObject {
    int val;
};

// 长整数
struct ZataInt64 final : ZataObject {
    long long val;
};

// 无限整数
struct ZataInfInt final : ZataObject {
    std::string val;
};

// 浮点数对象
struct ZataFloat final : ZataObject {
    float val;
};

// 长浮点数对象
struct ZataFloat64 final : ZataObject {
    double val;
};

// 安全小数
struct ZataDec final : ZataObject {
    std::string val;
};

struct ZataState final : ZataObject {
    // 1 => True
    // 2 => False
    // 3 => None
    // 4 => Nofound
    int val;
};

// 调用帧
struct CallFrame {
    int pc = 0;
    std::vector<ZataElem> locals;
    int return_address = 0;
    std::string function_name;
    std::vector<ZataElem> constant_pool;
    std::vector<int> code;
};

// 内存帧
struct Allocation {
    uint64_t size;
    ZataElem data;
};


#endif // ZATA_OBJECTS_H
