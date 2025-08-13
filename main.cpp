#include <fstream>

#include "include/ZataVM.hpp"
#include <pybind11/pybind11.h>

#include "include/models/Errors.hpp"
#include "include/utils/Utils.hpp"
#include <pybind11/stl.h>  // 必须包含！


std::vector<ZataObjectPtr> execute_bytecode(
    const std::shared_ptr<ZataModule>& module,
    const std::vector<Context> &contexts
){
    std::stack<ZataObjectPtr> result;
    try {
        Utils::enable_ansi_escape();

        ZataVirtualMachine vm(module,contexts);
        result = vm.run();
    } catch (const std::exception& e) {
        std::cout << Fore::RED << "Error from Zata Vm (GCC raised): " << e.what() << Fore::RESET << std::endl;
        exit(-1);
    }

    return Utils::stack_to_vector(result);
}

namespace py = pybind11;


PYBIND11_MODULE(cppZvm, m) {
    m.doc() = "Zata虚拟机Python绑定";

    // 基类绑定（不允许直接实例化）
    py::class_<ZataObject, std::shared_ptr<ZataObject>>(m, "ZataObject")
        .def(py::init<>())
        .def_readonly("object_id", &ZataObject::object_id)
        .def_readwrite("object_type", &ZataObject::object_type);

    // 字节码对象绑定
    py::class_<ZataCodeObject, ZataObject, std::shared_ptr<ZataCodeObject>>(m, "ZataCodeObject")
        .def(py::init<>())
        .def_readwrite("consts", &ZataCodeObject::consts)
        .def_readwrite("co_code", &ZataCodeObject::co_code)
        .def_readwrite("line_map", &ZataCodeObject::line_map);

    // 模块对象绑定（修复成员映射错误）
    py::class_<ZataModule, ZataObject, std::shared_ptr<ZataModule>>(m, "ZataModule")
        .def(py::init<>())
        .def_readwrite("module_name", &ZataModule::object_name)  // 对应 object_name
        .def_readwrite("module_path", &ZataModule::module_path)
        .def_readwrite("globals", &ZataModule::globals)          // 全局变量映射
        .def_readwrite("code", &ZataModule::code);               // 关联的字节码对象

    // 函数对象绑定（修复成员错误）
    py::class_<ZataFunction, ZataObject, std::shared_ptr<ZataFunction>>(m, "ZataFunction")
        .def(py::init<>())
        .def_readwrite("function_name", &ZataFunction::object_name)
        .def_readwrite("arg_count", &ZataFunction::arg_count)
        .def_readwrite("code", &ZataFunction::code);  // 关联的字节码对象

    // 类对象绑定（修复成员名称）
    py::class_<ZataClass, ZataObject, std::shared_ptr<ZataClass>>(m, "ZataClass")
        .def(py::init<>())
        .def_readwrite("class_name", &ZataClass::object_name)
        .def_readwrite("parent_class", &ZataClass::parent_class)  // 父类列表
        .def_readwrite("attrs", &ZataClass::attrs);               // 类属性

    // 实例对象绑定（正确）
    py::class_<ZataInstance, ZataObject, std::shared_ptr<ZataInstance>>(m, "ZataInstance")
        .def(py::init<>())
        .def_readwrite("ref_class", &ZataInstance::ref_class)  // 关联的类
        .def_readwrite("fields", &ZataInstance::fields);       // 实例字段

    // 内置类型对象绑定（ZataBuiltinsType）
    py::class_<ZataBuiltinsType, ZataObject, std::shared_ptr<ZataBuiltinsType>>(m, "ZataBuiltinsType")
        .def(py::init<>())
        .def_readwrite("object_type", &ZataBuiltinsType::object_type)  // 弱引用自身类型
        ;

    // 字符串对象绑定（正确）
    py::class_<ZataString, ZataObject, std::shared_ptr<ZataString>>(m, "ZataString")
        .def(py::init<>())
        .def_readwrite("val", &ZataString::val);

    // 整数对象绑定（正确）
    py::class_<ZataInt, ZataObject, std::shared_ptr<ZataInt>>(m, "ZataInt")
        .def(py::init<>())
        .def_readwrite("val", &ZataInt::val);

    // 长整数绑定（正确）
    py::class_<ZataInt64, ZataObject, std::shared_ptr<ZataInt64>>(m, "ZataInt64")
        .def(py::init<>())
        .def_readwrite("val", &ZataInt64::val);

    // 无限整数绑定（修复成员错误：无 val，改为 is_negative 和 digits）
    py::class_<ZataInfInt, ZataObject, std::shared_ptr<ZataInfInt>>(m, "ZataInfInt")
        .def(py::init<>())
        .def_readwrite("is_negative", &ZataInfInt::is_negative)
        .def_readwrite("digits", &ZataInfInt::digits)
        .def_readonly_static("BASE", &ZataInfInt::BASE)
        .def_readonly_static("BASE_DIGITS", &ZataInfInt::BASE_DIGITS);

    // 浮点数对象绑定（正确）
    py::class_<ZataFloat, ZataObject, std::shared_ptr<ZataFloat>>(m, "ZataFloat")
        .def(py::init<>())
        .def_readwrite("val", &ZataFloat::val);

    // 长浮点数对象绑定（正确）
    py::class_<ZataFloat64, ZataObject, std::shared_ptr<ZataFloat64>>(m, "ZataFloat64")
        .def(py::init<>())
        .def_readwrite("val", &ZataFloat64::val);

    // 安全小数绑定（修复成员错误：无 val，改为具体成员）
    py::class_<ZataDec, ZataObject, std::shared_ptr<ZataDec>>(m, "ZataDec")
        .def(py::init<>())
        .def_readwrite("is_negative", &ZataDec::is_negative)
        .def_readwrite("int_digits", &ZataDec::int_digits)
        .def_readwrite("frac_digits", &ZataDec::frac_digits)
        .def_readonly_static("BASE", &ZataDec::BASE)
        .def_readonly_static("BASE_DIGITS", &ZataDec::BASE_DIGITS);

    // 列表绑定
    py::class_<ZataList, ZataObject, std::shared_ptr<ZataList>>(m, "ZataList")
        .def(py::init<>())
        .def_readwrite("items", &ZataList::items)
        .def_readwrite("size", &ZataList::size);

    // 字典绑定
    py::class_<ZataDict, ZataObject, std::shared_ptr<ZataDict>>(m, "ZataDict")
        .def(py::init<>())
        .def_readwrite("key_val", &ZataDict::key_val);

    // 元组绑定
    py::class_<ZataTuple, ZataObject, std::shared_ptr<ZataTuple>>(m, "ZataTuple")
        .def(py::init<>())
        .def_readwrite("items", &ZataTuple::items);

    // 记录绑定
    py::class_<ZataRecord, ZataObject, std::shared_ptr<ZataRecord>>(m, "ZataRecord")
        .def(py::init<>())
        .def_readwrite("attrs", &ZataRecord::attrs);

    // 状态对象绑定（正确）
    py::class_<ZataState, ZataObject, std::shared_ptr<ZataState>>(m, "ZataState")
        .def(py::init<>())
        .def_readwrite("val", &ZataState::val);
}