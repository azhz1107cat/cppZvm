#include <fstream>

#include "include/ZataVM.hpp"
#include <pybind11/pybind11.h>

#include "Errors.hpp"
#include "utils.hpp"
#include <pybind11/stl.h>  // 必须包含！


void execute_bytecode(const std::shared_ptr<ZataModule>& module)
{
    Utils::enable_ansi_escape();

    ZataVirtualMachine vm(module->consts);
    vm.run(module->bytecode);
}

namespace py = pybind11;

PYBIND11_MODULE(cppZvm, m) {
    m.doc() = "Zata虚拟机Python绑定";
    // 基类绑定
    py::class_<ZataObject, std::shared_ptr<ZataObject>>(m, "ZataObject")
        .def(py::init<>());

    // 模块对象绑定
    py::class_<ZataModule, ZataObject, std::shared_ptr<ZataModule>>(m, "ZataModule")
        .def(py::init<>())
        .def_readwrite("module_name", &ZataModule::module_name)
        .def_readwrite("local_count", &ZataModule::local_count)
        .def_readwrite("bytecode", &ZataModule::bytecode)
        .def_readwrite("consts", &ZataModule::consts)
        .def_readwrite("line_number_map", &ZataModule::line_number_map);

    // 函数对象绑定
    py::class_<ZataFunction, ZataObject, std::shared_ptr<ZataFunction>>(m, "ZataFunction")
        .def(py::init<>())
        .def_readwrite("function_name", &ZataFunction::function_name)
        .def_readwrite("arg_count", &ZataFunction::arg_count)
        .def_readwrite("local_count", &ZataFunction::local_count)
        .def_readwrite("consts", &ZataFunction::consts)
        .def_readwrite("bytecode", &ZataFunction::bytecode)
        .def_readwrite("line_number_map", &ZataFunction::line_number_map);

    // 类对象绑定
    py::class_<ZataClass, ZataObject, std::shared_ptr<ZataClass>>(m, "ZataClass")
        .def(py::init<>())
        .def_readwrite("class_name", &ZataClass::class_name)
        .def_readwrite("parent_class", &ZataClass::parent_class)
        .def_readwrite("consts", &ZataClass::consts)
        .def_readwrite("fields", &ZataClass::fields)
        .def_readwrite("line_number_map", &ZataClass::line_number_map);

    // 实例对象绑定
    py::class_<ZataInstance, ZataObject, std::shared_ptr<ZataInstance>>(m, "ZataInstance")
        .def(py::init<>())
        .def_readwrite("ref_class", &ZataInstance::ref_class)
        .def_readwrite("fields", &ZataInstance::fields);

    // 字符串对象绑定
    py::class_<ZataString, ZataObject, std::shared_ptr<ZataString>>(m, "ZataString")
        .def(py::init<>())
        .def_readwrite("val", &ZataString::val);

    // 整数对象绑定
    py::class_<ZataInt, ZataObject, std::shared_ptr<ZataInt>>(m, "ZataInt")
        .def(py::init<>())
        .def_readwrite("val", &ZataInt::val);

    // 长整数绑定
    py::class_<ZataInt64, ZataObject, std::shared_ptr<ZataInt64>>(m, "ZataInt64")
        .def(py::init<>())
        .def_readwrite("val", &ZataInt64::val);

    // 无限整数绑定
    py::class_<ZataInfInt, ZataObject, std::shared_ptr<ZataInfInt>>(m, "ZataInfInt")
        .def(py::init<>())
        .def_readwrite("val", &ZataInfInt::val);

    // 浮点数对象绑定
    py::class_<ZataFloat, ZataObject, std::shared_ptr<ZataFloat>>(m, "ZataFloat")
        .def(py::init<>())
        .def_readwrite("val", &ZataFloat::val);

    // 长浮点数对象绑定
    py::class_<ZataFloat64, ZataObject, std::shared_ptr<ZataFloat64>>(m, "ZataFloat64")
        .def(py::init<>())
        .def_readwrite("val", &ZataFloat64::val);

    // 安全小数绑定
    py::class_<ZataDec, ZataObject, std::shared_ptr<ZataDec>>(m, "ZataDec")
        .def(py::init<>())
        .def_readwrite("val", &ZataDec::val);

    //状态对象绑定
    py::class_<ZataState, ZataObject, std::shared_ptr<ZataState>>(m, "ZataState")
    .def(py::init<>())
    .def_readwrite("val", &ZataState::val);

    m.def("execute_bytecode", &execute_bytecode,
          "执行字节码并返回最终栈状态或错误时栈状态",
          py::arg("module"));
}