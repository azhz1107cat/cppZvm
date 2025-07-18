#ifndef OBJECT_META_DATA_H
#define OBJECT_META_DATA_H

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include "ZvmOpcodes.hpp"

// 前向声明
struct ZataObject;
struct Function;
struct Class;
struct Instanced;

// 基类：所有对象的基类
struct ZataObject {
    virtual ~ZataObject() = default;

    enum class Type {
        FUNCTION,
        CLASS,
        INSTANCED
    };
    Type type;

    explicit ZataObject(Type t) : type(t) {}
};

// 使用指针类型，确保MixedType可默认构造
using MixedType = std::variant<int, bool, std::string, std::shared_ptr<ZataObject>>;

// 函数对象
struct Function final : ZataObject {
    std::vector<MixedType> locals;
    int arg_count = 0;
    std::string name;
    std::vector<MixedType> code;

    Function() : ZataObject(Type::FUNCTION) {}
};

// 类对象
struct Class final : ZataObject {
    int method_count = 0;
    int local_count = 0;
    std::string name;
    std::map<std::string, std::shared_ptr<Function>> methods;
    std::map<std::string, MixedType> attrs;

    Class() : ZataObject(Type::CLASS) {}

    void add_method(const std::string& name, const std::shared_ptr<Function>& method) {
        methods[name] = method;
        method_count++;
    }

    void add_attr(const std::string& name, const MixedType& value) {
        attrs[name] = value;
    }

    [[nodiscard]] MixedType get_attr(const std::string& name) const {
        auto it = attrs.find(name);
        if (it != attrs.end()) {
            return it->second;
        }
        return std::shared_ptr<ZataObject>(nullptr); // 返回空指针
    }

    [[nodiscard]] std::shared_ptr<Instanced> instantiate() const;

    [[nodiscard]] std::shared_ptr<Function> get_method(const std::string& name) const {
        const auto it = methods.find(name);
        return it != methods.end() ? it->second : nullptr;
    }
};

// 实例对象
struct Instanced final : ZataObject {
    std::shared_ptr<Class> class_obj;
    std::map<std::string, MixedType> fields;

    explicit Instanced(std::shared_ptr<Class> cls)
        : ZataObject(Type::INSTANCED), class_obj(std::move(cls)) {}

    [[nodiscard]] MixedType get_field(const std::string& name) const {
        const auto it = fields.find(name);
        if (it != fields.end()) {
            return it->second;
        }
        return class_obj->get_attr(name); // 会返回空指针
    }

    void set_field(const std::string& name, const MixedType& value) {
        fields[name] = value;
    }

    [[nodiscard]] std::shared_ptr<Function> get_method(const std::string& name) const {
        return class_obj->get_method(name);
    }
};

inline std::shared_ptr<Instanced> Class::instantiate() const {
    return std::make_shared<Instanced>(std::make_shared<Class>(*this));
}

#endif // OBJECT_META_DATA_H