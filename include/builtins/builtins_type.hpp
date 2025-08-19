#ifndef BUILTINS_TYPE_HPP
#define BUILTINS_TYPE_HPP
#include <iostream>
#include <memory>
#include <vector>
#include <string>  // 补充字符串处理头文件

#include "models/Objects.hpp"

// 提前声明魔术方法（避免未定义错误）
inline ZataObjectPtr int_add(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int_sub(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int_mul(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int_div(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int_eq(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr str_add(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr str_eq(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int64_add(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int64_sub(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int64_eq(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float_add(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float_sub(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float_eq(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float64_add(const std::vector<ZataObjectPtr>& args);

inline ZataObjectPtr int_gt(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int_lt(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int64_gt(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int64_lt(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float_gt(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float_lt(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float64_gt(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float64_lt(const std::vector<ZataObjectPtr>& args);

inline ZataObjectPtr dict_getitem(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr dict_setitem(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr tuple_getitem(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr list_add(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr list_getitem(const std::vector<ZataObjectPtr>& args);

// 提前声明type_str方法
inline ZataObjectPtr int_str(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr str_str(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr int64_str(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float_str(const std::vector<ZataObjectPtr>& args);
inline ZataObjectPtr float64_str(const std::vector<ZataObjectPtr>& args);


// -------------------------- 类型绑定 --------------------------

// 整数类型绑定
inline auto int_type = std::make_shared<ZataBuiltinsType>();
inline void bind_int_type() {
    int_type->type_add = int_add;
    int_type->type_sub = int_sub;
    int_type->type_mul = int_mul;
    int_type->type_div = int_div;
    int_type->type_eq = int_eq;
    int_type->type_str = int_str;  // 绑定type_str
    int_type->type_gt = int_gt;
    int_type->type_lt = int_lt;
    int_type->type_str = int_str;
}

// 字符串类型绑定
inline auto str_type = std::make_shared<ZataBuiltinsType>();
inline void bind_str_type() {
    str_type->type_add = str_add;
    str_type->type_eq = str_eq;
    str_type->type_str = str_str;  // 绑定type_str
}

// 列表类型绑定
inline auto list_type = std::make_shared<ZataBuiltinsType>();
inline void bind_list_type() {
    list_type->type_add = list_add;
    list_type->type_getitem = list_getitem;
    // 若实现了list_str，需在此绑定：list_type->type_str = list_str;
}

// 长整数类型绑定
inline auto int64_type = std::make_shared<ZataBuiltinsType>();
inline void bind_int64_type() {
    int64_type->type_add = int64_add;
    int64_type->type_sub = int64_sub;
    int64_type->type_eq = int64_eq;
    int64_type->type_str = int64_str;  // 绑定type_str
    int64_type->type_gt = int64_gt;
    int64_type->type_lt = int64_lt;
}

// 浮点数类型绑定
inline auto float_type = std::make_shared<ZataBuiltinsType>();
inline void bind_float_type() {
    float_type->type_add = float_add;
    float_type->type_sub = float_sub;
    float_type->type_eq = float_eq;
    float_type->type_str = float_str;  // 绑定type_str
    float_type->type_gt = float_gt;
    float_type->type_lt = float_lt;
    float_type->type_str = float_str;
}

// 双精度浮点数类型绑定
inline auto float64_type = std::make_shared<ZataBuiltinsType>();
inline void bind_float64_type() {
    float64_type->type_add = float64_add;
    float64_type->type_gt = float64_gt;
    float64_type->type_lt = float64_lt;
    float64_type->type_str = float64_str;  // 绑定type_str
}

// 字典类型绑定
inline auto dict_type = std::make_shared<ZataBuiltinsType>();
inline void bind_dict_type() {
    dict_type->type_getitem = dict_getitem;
    dict_type->type_setitem = dict_setitem;
    // 若实现了dict_str，需在此绑定：dict_type->type_str = dict_str;
}

// 元组类型绑定
inline auto tuple_type = std::make_shared<ZataBuiltinsType>();
inline void bind_tuple_type() {
    tuple_type->type_getitem = tuple_getitem;
    // 若实现了tuple_str，需在此绑定：tuple_type->type_str = tuple_str;
}

// 扩展初始化函数（包含所有类型）
inline void init_type_system() {
    bind_int_type();       // 整数
    bind_int64_type();     // 长整数
    bind_float_type();     // 浮点数
    bind_float64_type();   // 双精度浮点数
    bind_str_type();       // 字符串
    bind_list_type();      // 列表
    bind_dict_type();      // 字典
    bind_tuple_type();     // 元组
}

// -------------------------- 对象创建函数 --------------------------

// 创建整数对象
inline std::shared_ptr<ZataInt> create_int(int val) {
    auto obj = std::make_shared<ZataInt>();
    obj->val = val;
    obj->object_type = int_type;
    return obj;
}

// 创建字符串对象
inline std::shared_ptr<ZataString> create_str(const std::string& val) {
    auto obj = std::make_shared<ZataString>();
    obj->val = val;
    obj->object_type = str_type;
    return obj;
}

// 创建列表对象
inline std::shared_ptr<ZataList> create_list() {
    auto obj = std::make_shared<ZataList>();
    obj->size = 0;
    obj->object_type = list_type;
    return obj;
}

// 创建长整数对象
inline std::shared_ptr<ZataInt64> create_int64(long long val) {
    auto obj = std::make_shared<ZataInt64>();
    obj->val = val;
    obj->object_type = int64_type;
    return obj;
}

// 创建浮点数对象
inline std::shared_ptr<ZataFloat> create_float(float val) {
    auto obj = std::make_shared<ZataFloat>();
    obj->val = val;
    obj->object_type = float_type;
    return obj;
}

// 创建双精度浮点数对象
inline std::shared_ptr<ZataFloat64> create_float64(double val) {
    auto obj = std::make_shared<ZataFloat64>();
    obj->val = val;
    obj->object_type = float64_type;
    return obj;
}

// 创建字典对象
inline std::shared_ptr<ZataDict> create_dict() {
    auto obj = std::make_shared<ZataDict>();
    obj->object_type = dict_type;
    return obj;
}

// 创建元组对象（从向量初始化）
inline std::shared_ptr<ZataTuple> create_tuple(const std::vector<ZataObjectPtr>& items) {
    auto obj = std::make_shared<ZataTuple>();
    obj->items = items;
    obj->object_type = tuple_type;
    return obj;
}

// -------------------------- 魔术方法实现 --------------------------

// 整数加法
inline ZataObjectPtr int_add(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataInt>();
    result->val = self->val + other->val;
    result->object_type = int_type;  // 补充结果的类型绑定
    return result;
}

// 整数减法
inline ZataObjectPtr int_sub(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataInt>();
    result->val = self->val - other->val;
    result->object_type = int_type;  // 补充结果的类型绑定
    return result;
}

// 整数乘法
inline ZataObjectPtr int_mul(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataInt>();
    result->val = self->val * other->val;
    result->object_type = int_type;  // 补充结果的类型绑定
    return result;
}

// 整数除法
inline ZataObjectPtr int_div(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt>(args[1]);
    if (!self || !other || other->val == 0) return nullptr;

    auto result = std::make_shared<ZataInt>();
    result->val = self->val / other->val;
    result->object_type = int_type;  // 补充结果的类型绑定
    return result;
}

// 整数相等比较
inline ZataObjectPtr int_eq(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val == other->val) ? 1 : 0;
    // 假设ZataState有默认object_type绑定，若没有需补充：result->object_type = state_type;
    return result;
}


inline ZataObjectPtr int_gt(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val > other->val) ? 1 : 0;
    return result;
}


inline ZataObjectPtr int_lt(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val < other->val) ? 1 : 0;
    return result;
}

// 字符串加法（拼接）
inline ZataObjectPtr str_add(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataString>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataString>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataString>();
    result->val = self->val + other->val;
    result->object_type = str_type;  // 补充结果的类型绑定
    return result;
}

// 字符串相等比较
inline ZataObjectPtr str_eq(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataString>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataString>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val == other->val) ? 1 : 0;
    return result;
}

// 长整数（ZataInt64）运算
inline ZataObjectPtr int64_add(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt64>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt64>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataInt64>();
    result->val = self->val + other->val;
    result->object_type = int64_type;  // 补充结果的类型绑定
    return result;
}

inline ZataObjectPtr int64_sub(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt64>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt64>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataInt64>();
    result->val = self->val - other->val;
    result->object_type = int64_type;  // 补充结果的类型绑定
    return result;
}

inline ZataObjectPtr int64_eq(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt64>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt64>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val == other->val) ? 1 : 0;
    return result;
}

inline ZataObjectPtr int64_gt(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt64>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt64>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val > other->val) ? 1 : 0;
    return result;
}

inline ZataObjectPtr int64_lt(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt64>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataInt64>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val < other->val) ? 1 : 0;
    return result;
}

// 浮点数（ZataFloat）运算
inline ZataObjectPtr float_add(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataFloat>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataFloat>();
    result->val = self->val + other->val;
    result->object_type = float_type;  // 补充结果的类型绑定
    return result;
}

inline ZataObjectPtr float_sub(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataFloat>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataFloat>();
    result->val = self->val - other->val;
    result->object_type = float_type;  // 补充结果的类型绑定
    return result;
}

inline ZataObjectPtr float_eq(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataFloat>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val == other->val) ? 1 : 0;
    return result;
}

// float 大于：self > other
inline ZataObjectPtr float_gt(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataFloat>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val > other->val) ? 1 : 0;
    return result;
}

// float 小于：self < other
inline ZataObjectPtr float_lt(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataFloat>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val < other->val) ? 1 : 0;
    return result;
}

// 双精度浮点数（ZataFloat64）运算
inline ZataObjectPtr float64_add(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat64>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataFloat64>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataFloat64>();
    result->val = self->val + other->val;
    result->object_type = float64_type;  // 补充结果的类型绑定
    return result;
}

// float64 大于：self > other
inline ZataObjectPtr float64_gt(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat64>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataFloat64>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val > other->val) ? 1 : 0;
    return result;
}

// float64 小于：self < other
inline ZataObjectPtr float64_lt(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat64>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataFloat64>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataState>();
    result->val = (self->val < other->val) ? 1 : 0;
    return result;
}

// 字典（ZataDict）操作
inline ZataObjectPtr dict_getitem(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataDict>(args[0]);
    if (!self) return nullptr;

    auto it = self->key_val.find(args[1]);
    return (it != self->key_val.end()) ? it->second : nullptr;
}

inline ZataObjectPtr dict_setitem(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataDict>(args[0]);
    if (!self) return nullptr;

    self->key_val[args[1]] = args[2];
    auto result = std::make_shared<ZataState>();
    result->val = 1;  // 成功状态
    return result;
}

// 元组（ZataTuple）操作
inline ZataObjectPtr tuple_getitem(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataTuple>(args[0]);
    auto index = std::dynamic_pointer_cast<ZataInt>(args[1]);
    if (!self || !index) return nullptr;

    int idx = index->val;
    if (idx < 0 || static_cast<size_t>(idx) >= self->items.size()) {
        return nullptr;
    }
    return self->items[idx];
}

// 列表加法
inline ZataObjectPtr list_add(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataList>(args[0]);
    auto other = std::dynamic_pointer_cast<ZataList>(args[1]);
    if (!self || !other) return nullptr;

    auto result = std::make_shared<ZataList>();
    result->items = self->items;
    result->items.insert(result->items.end(), other->items.begin(), other->items.end());
    result->size = result->items.size();
    result->object_type = list_type;  // 补充结果的类型绑定
    return result;
}

// 列表索引访问
inline ZataObjectPtr list_getitem(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataList>(args[0]);
    auto index = std::dynamic_pointer_cast<ZataInt>(args[1]);
    if (!self || !index) return nullptr;

    int idx = index->val;
    if (idx < 0 || static_cast<size_t>(idx) >= self->items.size()) {
        return nullptr;
    }
    return self->items[idx];
}

// -------------------------- 字符串转换（type_str）方法 --------------------------

// 整数转字符串
inline ZataObjectPtr int_str(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt>(args[0]);
    if (!self) return nullptr;

    auto result = std::make_shared<ZataString>();
    result->val = std::to_string(self->val);
    result->object_type = str_type;
    return result;
}

// 字符串转字符串（返回自身值）
inline ZataObjectPtr str_str(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataString>(args[0]);
    if (!self) return nullptr;

    auto result = std::make_shared<ZataString>();
    result->val = self->val;
    result->object_type = str_type;
    return result;
}

// 长整数（ZataInt64）转字符串
inline ZataObjectPtr int64_str(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataInt64>(args[0]);
    if (!self) return nullptr;

    auto result = std::make_shared<ZataString>();
    result->val = std::to_string(self->val);
    result->object_type = str_type;
    return result;
}

// 浮点数（ZataFloat）转字符串
inline ZataObjectPtr float_str(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat>(args[0]);
    if (!self) return nullptr;

    auto result = std::make_shared<ZataString>();
    result->val = std::to_string(self->val);
    result->object_type = str_type;
    return result;
}

// 双精度浮点数（ZataFloat64）转字符串
inline ZataObjectPtr float64_str(const std::vector<ZataObjectPtr>& args) {
    auto self = std::dynamic_pointer_cast<ZataFloat64>(args[0]);
    if (!self) return nullptr;

    auto result = std::make_shared<ZataString>();
    result->val = std::to_string(self->val);
    result->object_type = str_type;
    return result;
}

#endif //BUILTINS_TYPE_HPP
