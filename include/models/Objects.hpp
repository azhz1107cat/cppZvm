#ifndef ZATA_OBJECTS_H
#define ZATA_OBJECTS_H

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include "utils/Utils.hpp"

inline size_t get_uuid() {
    static size_t uuid_counter = 0;
    return uuid_counter++;
}

struct ZataType;

// 基类
struct ZataObject {
    std::shared_ptr<ZataType> object_type;
    size_t object_id;
    ZataObject() : object_type(nullptr), object_id(get_uuid()) {}
    virtual ~ZataObject() = default;
};

using ZataObjectPtr = std::shared_ptr<ZataObject>;

// 字节码对象
struct ZataCodeObject final : ZataObject {
    std::vector<std::string> names;
    std::vector<ZataObjectPtr> consts;
    std::vector<int> co_code; // co -> code_object
    std::vector<std::pair<int, int>> line_map; // line_in_zata_file , line_in_code(max)
};

// 模块对象
struct ZataModule final : ZataObject {
    std::string object_name;
    std::string module_path;
    std::unordered_map<std::string, ZataObjectPtr> globals;
    std::shared_ptr<ZataCodeObject> code;
};

// 函数对象
struct ZataFunction final : ZataObject {
    std::string object_name;
    int arg_count = 0;
    std::shared_ptr<ZataCodeObject> code;
};

// 类对象
struct ZataClass final : ZataObject {
    std::string object_name;

    std::vector<std::shared_ptr<ZataClass>> parent_class;
    std::unordered_map<std::string, std::shared_ptr<ZataObject>> attrs;
};

// 实例对象
struct ZataInstance final : ZataObject {
    std::shared_ptr<ZataClass> ref_class;
    std::unordered_map<std::string,ZataObjectPtr> fields;
};

// 内置类型对象
struct ZataType final : ZataObject {
    using ZataCppFnPtr = std::function<ZataObjectPtr(const std::vector<ZataObjectPtr>&)>;
    std::weak_ptr<ZataType> object_type;
    ZataCppFnPtr type_new;
    ZataCppFnPtr type_init;

    ZataCppFnPtr type_add;
    ZataCppFnPtr type_sub;
    ZataCppFnPtr type_mul;
    ZataCppFnPtr type_div;
    ZataCppFnPtr type_mod;
    ZataCppFnPtr type_neg;
    ZataCppFnPtr type_eq;
    ZataCppFnPtr type_weq;
    ZataCppFnPtr type_gt;
    ZataCppFnPtr type_lt;
    ZataCppFnPtr type_ge;
    ZataCppFnPtr type_le;
    ZataCppFnPtr type_bit_and;
    ZataCppFnPtr type_bit_not;
    ZataCppFnPtr type_bit_or;
    ZataCppFnPtr type_bit_xor;

    ZataCppFnPtr type_nil;
    ZataCppFnPtr type_str;
    ZataCppFnPtr type_getitem;
    ZataCppFnPtr type_setitem;
    ZataCppFnPtr type_delitem;
    ZataCppFnPtr type_call;
    ZataCppFnPtr type_del;
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
    bool is_negative;  // false -> +n , true -> -n
    std::vector<uint64_t> digits;
    static constexpr uint64_t BASE = 1000000000;
    static constexpr int BASE_DIGITS = 9;
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
    bool is_negative;
    std::vector<uint64_t> int_digits;
    std::vector<uint64_t> frac_digits;
    static constexpr uint64_t BASE = 1000000000;
    static constexpr int BASE_DIGITS = 9;

};

// 列表
struct ZataList final : ZataObject {
    std::vector<ZataObjectPtr> items;
    size_t size;
};

// 字典
struct ZataDict final : ZataObject {
    std::unordered_map<ZataObjectPtr, ZataObjectPtr> key_val;
};

// 元组
struct ZataTuple final : ZataObject {
    std::vector<ZataObjectPtr> items;
};


// 记录
struct ZataRecord final : ZataObject {
    std::unordered_map<std::string, ZataObjectPtr> attrs;
};

// 状态
struct ZataState final : ZataObject {
    // 0 => False
    // 1 => True
    // 2 => None
    // 3 => Nofound
    short val;
};

#endif // ZATA_OBJECTS_H
