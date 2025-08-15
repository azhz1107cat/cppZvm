#ifndef ZATA_OBJECTS_H
#define ZATA_OBJECTS_H

#include <any>
#include <atomic>
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
#include <memory>

inline size_t get_uuid() {
    static std::atomic_size_t uuid_counter(0);
    return uuid_counter++;
}

struct ZataMetaType;
struct ZataBuiltinsType;
struct ZataUserType;

struct ZataObject;
using ZataObjectPtr = std::shared_ptr<ZataObject>;

// 基类
struct ZataObject {
    std::shared_ptr<ZataMetaType> object_type;
    const size_t object_id;
    ZataObject() : object_type(nullptr), object_id(get_uuid()) {}
    virtual ~ZataObject() = default;

protected:
    ZataObject(const ZataObject&) = default;
    bool operator==(const ZataObjectPtr& other) const {
        if (this->object_id == other->object_id) {
            return true;
        }return false;
    }
};

// 字节码对象
struct ZataCodeObject final : ZataObject {
    std::vector<ZataObjectPtr> locals{};
    std::vector<ZataObjectPtr> consts;
    std::vector<int> co_code; // co -> code_object
    std::vector<std::pair<int, int>> line_map; // line_in_zata_file , line_in_code(max)
};

// 模块对象
struct ZataModule final : ZataObject {
    std::string object_name;
    std::string module_path;
    std::vector<std::string> names;
    std::unordered_map<std::string, std::shared_ptr<ZataObject>> attrs;
    std::shared_ptr<ZataCodeObject> code;
    std::vector<std::string> exports{};
};

// 函数对象
struct ZataFunction final : ZataObject {
    std::string object_name;
    int arg_count = 0;
    std::shared_ptr<ZataCodeObject> code;
    std::vector<std::string> free_vars_names{};
    std::unordered_map<std::string, std::shared_ptr<ZataObject>> free_vars{};
};

// 类对象
struct ZataClass final : ZataObject {
    std::string object_name;

    std::vector<std::shared_ptr<ZataClass>> parent_class;
    std::vector<std::string> names;
    std::unordered_map<std::string, std::shared_ptr<ZataObject>> attrs;
};

// 实例对象
struct ZataInstance final : ZataObject {
    std::shared_ptr<ZataUserType> object_type;
    std::shared_ptr<ZataClass> ref_class;
    std::vector<std::string> names;
    std::unordered_map<std::string,ZataObjectPtr> fields;
};

struct ZataMetaType : ZataObject {
    using ZataAny = std::any;
    std::weak_ptr<ZataBuiltinsType> object_type;
    ZataAny type_new;
    ZataAny type_init;

    ZataAny type_add;
    ZataAny type_sub;
    ZataAny type_mul;
    ZataAny type_div;
    ZataAny type_mod;
    ZataAny type_neg;
    ZataAny type_eq;
    ZataAny type_weq;
    ZataAny type_gt;
    ZataAny type_lt;
    ZataAny type_ge;
    ZataAny type_le;
    ZataAny type_bit_and;
    ZataAny type_bit_not;
    ZataAny type_bit_or;
    ZataAny type_bit_xor;

    ZataAny type_nil;
    ZataAny type_str;
    ZataAny type_getitem;
    ZataAny type_setitem;
    ZataAny type_delitem;
    ZataAny type_call;
    ZataAny type_del;
};

// 内置类型对象
struct ZataBuiltinsType final : ZataMetaType {
    using ZataCppFnPtr = std::function<ZataObjectPtr(const std::vector<ZataObjectPtr>&)>;
    std::weak_ptr<ZataBuiltinsType> object_type;
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

struct ZataUserType final : ZataMetaType {
    using ZataFnPtr = std::shared_ptr<ZataFunction>;
    std::weak_ptr<ZataBuiltinsType> object_type;
    ZataFnPtr type_new;
    ZataFnPtr type_init;

    ZataFnPtr type_add;
    ZataFnPtr type_sub;
    ZataFnPtr type_mul;
    ZataFnPtr type_div;
    ZataFnPtr type_mod;
    ZataFnPtr type_neg;
    ZataFnPtr type_eq;
    ZataFnPtr type_weq;
    ZataFnPtr type_gt;
    ZataFnPtr type_lt;
    ZataFnPtr type_ge;
    ZataFnPtr type_le;
    ZataFnPtr type_bit_and;
    ZataFnPtr type_bit_not;
    ZataFnPtr type_bit_or;
    ZataFnPtr type_bit_xor;

    ZataFnPtr type_nil;
    ZataFnPtr type_str;
    ZataFnPtr type_getitem;
    ZataFnPtr type_setitem;
    ZataFnPtr type_delitem;
    ZataFnPtr type_call;
    ZataFnPtr type_del;
};

struct ZataBuiltinsClass : ZataObject {
    std::shared_ptr<ZataBuiltinsType> object_type;
    std::any val;
    ~ZataBuiltinsClass() override = default;
};

// 字符串对象
struct ZataString final : ZataBuiltinsClass {
    std::string val;
};

// 整数对象
struct ZataInt final : ZataBuiltinsClass {
    int val;
};

// 长整数
struct ZataInt64 final : ZataBuiltinsClass {
    long long val;
};

// 无限整数
struct ZataInfInt final : ZataBuiltinsClass {
    bool is_negative;  // false -> +n , true -> -n
    std::vector<uint64_t> digits;
    static constexpr uint64_t BASE = 1000000000;
    static constexpr int BASE_DIGITS = 9;
};

// 浮点数对象
struct ZataFloat final : ZataBuiltinsClass {
    float val;
};

// 长浮点数对象
struct ZataFloat64 final : ZataBuiltinsClass {
    double val;
};

// 安全小数
struct ZataDec final : ZataBuiltinsClass {
    bool is_negative;
    std::vector<uint64_t> int_digits;
    std::vector<uint64_t> frac_digits;
    static constexpr uint64_t BASE = 1000000000;
    static constexpr int BASE_DIGITS = 9;

};

// 列表
struct ZataList final : ZataBuiltinsClass {
    std::vector<ZataObjectPtr> items;
    size_t size;
};

// 字典
struct ZataDict final : ZataBuiltinsClass {
    std::unordered_map<ZataObjectPtr, ZataObjectPtr> key_val;
};

// 元组
struct ZataTuple final : ZataBuiltinsClass {
    std::vector<ZataObjectPtr> items;
};


// 记录
struct ZataRecord final : ZataBuiltinsClass {
    std::unordered_map<std::string, ZataObjectPtr> attrs;
};

// 状态
struct ZataState final : ZataBuiltinsClass {
    // 布尔假     False = 0
    // 布尔真     True = 1
    // 空值       None = 2
    // 未找到     NotFound = 3
    int val = 2;
};

#endif // ZATA_OBJECTS_H
