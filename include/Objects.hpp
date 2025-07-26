#ifndef ZATA_OBJECTS_H
#define ZATA_OBJECTS_H

#include <string>
#include <vector>
#include <utility>
#include <any>
#include <memory>
#include <variant>

using codeMember = int;

// 基类：所有Zata对象的共同祖先
struct ZataObject {
    virtual ~ZataObject() = default; // 虚析构函数确保多态正确析构
};

// 模块对象
struct ZataModule final : ZataObject {
    std::string module_name;
    int local_count = 0;
    std::vector<codeMember> bytecode;
    std::vector<std::any> consts;
    std::vector<std::pair<int, int>> map_of_line_num;
};

// 函数对象
struct ZataFunction final : ZataObject {
    std::string function_name;
    int arg_count = 0;
    int local_count = 0;
    std::vector<std::any> consts;
    std::vector<codeMember> bytecode;
    std::vector<std::pair<int, int>> map_of_line_num;
};

// 类对象
struct ZataClass final : ZataObject {
    std::string class_name;

    std::vector<std::shared_ptr<ZataClass>> parent_class;
    std::vector<std::any> consts;
    std::vector<codeMember> bytecode;

    std::vector<std::any> fields;
    std::vector<std::pair<int, int>> map_of_line_num;
};

struct ZataInstanced final : ZataObject {
    std::string class_name;

    std::vector<std::any> fields;
    std::vector<std::pair<int, int>> map_of_line_num;
};

// 字符串对象
struct ZataString final : ZataObject {
    std::string value;
};

// 整数对象
struct ZataInt final : ZataObject {
    int value;
};

// 浮点数对象
struct ZataFloat final : ZataObject {
    double value;
};

#endif // ZATA_OBJECTS_H
