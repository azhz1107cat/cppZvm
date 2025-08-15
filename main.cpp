#include <fstream>

#include "include/ZataVM.hpp"
#include <pybind11/pybind11.h>

#include "include/models/Errors.hpp"
#include "include/utils/Utils.hpp"
#include <pybind11/stl.h>  // 必须包含！

#include "builtins/builtins_type.hpp"

namespace py = pybind11;


PYBIND11_MODULE(cppZvm, m) {
    m.doc() = "Zata虚拟机Python绑定";
    py::class_<Context>(m, "Context")
            .def_readonly("file_path", &Context::file_path)
            .def_readonly("file_content", &Context::file_content)
            .def_readonly("file_path", &Context::file_name)
            .def_readonly("file_mtime", &Context::file_mtime);

    // 执行字节码函数
    m.def("execute_zmod",
        [](const std::shared_ptr<ZataModule>& module,
            const std::vector<Context>& contexts
        )
        -> std::vector<ZataObjectPtr> {
            try {
                Utils::enable_ansi_escape();
                init_type_system();

                ZataVirtualMachine vm(module, contexts);
                auto result_stack = vm.run();
                return Utils::stack_to_vector(result_stack);
            } catch (const std::exception& e) {
                throw py::value_error("Error from Zata Vm (GCC raised): " + std::string(e.what()));
            } catch (...) {
                throw py::value_error("Unknown Error from Zata Vm (GCC raised)");
            }
        },
        "执行Zata模块并返回结果列表",
        py::arg("module"), py::arg("contexts")
    );

    // 绑定 ZataUserType
    py::class_<ZataUserType, ZataMetaType, std::shared_ptr<ZataUserType>>(m, "ZataUserType")
        .def(py::init<>())
        .def_readwrite("object_type", &ZataUserType::object_type)
        .def_readwrite("type_new", &ZataUserType::type_new)
        .def_readwrite("type_init", &ZataUserType::type_init)
        .def_readwrite("type_add", &ZataUserType::type_add)
        .def_readwrite("type_sub", &ZataUserType::type_sub)
        .def_readwrite("type_mul", &ZataUserType::type_mul)
        .def_readwrite("type_div", &ZataUserType::type_div)
        .def_readwrite("type_mod", &ZataUserType::type_mod)
        .def_readwrite("type_neg", &ZataUserType::type_neg)
        .def_readwrite("type_eq", &ZataUserType::type_eq)
        .def_readwrite("type_weq", &ZataUserType::type_weq)
        .def_readwrite("type_gt", &ZataUserType::type_gt)
        .def_readwrite("type_lt", &ZataUserType::type_lt)
        .def_readwrite("type_ge", &ZataUserType::type_ge)
        .def_readwrite("type_le", &ZataUserType::type_le)
        .def_readwrite("type_bit_and", &ZataUserType::type_bit_and)
        .def_readwrite("type_bit_not", &ZataUserType::type_bit_not)
        .def_readwrite("type_bit_or", &ZataUserType::type_bit_or)
        .def_readwrite("type_bit_xor", &ZataUserType::type_bit_xor)
        .def_readwrite("type_nil", &ZataUserType::type_nil)
        .def_readwrite("type_str", &ZataUserType::type_str)
        .def_readwrite("type_getitem", &ZataUserType::type_getitem)
        .def_readwrite("type_setitem", &ZataUserType::type_setitem)
        .def_readwrite("type_delitem", &ZataUserType::type_delitem)
        .def_readwrite("type_call", &ZataUserType::type_call)
        .def_readwrite("type_del", &ZataUserType::type_del);

    // 6. 绑定 ZataCodeObject（继承ZataObject）
    py::class_<ZataCodeObject, ZataObject, std::shared_ptr<ZataCodeObject>>(m, "ZataCodeObject")
        .def(py::init<>())
        .def_readwrite("locals", &ZataCodeObject::locals)
        .def_readwrite("consts", &ZataCodeObject::consts)
        .def_readwrite("co_code", &ZataCodeObject::co_code)
        .def_readwrite("line_map", &ZataCodeObject::line_map);

    // 7. 绑定 ZataModule（继承ZataObject）
    py::class_<ZataModule, ZataObject, std::shared_ptr<ZataModule>>(m, "ZataModule")
        .def(py::init<>())
        .def_readwrite("object_name", &ZataModule::object_name)
        .def_readwrite("module_path", &ZataModule::module_path)
        .def_readwrite("names", &ZataModule::names)
        .def_readwrite("attrs", &ZataModule::attrs)
        .def_readwrite("code", &ZataModule::code)
        .def_readwrite("exports", &ZataModule::exports);

    // 8. 绑定 ZataFunction（继承ZataObject）
    py::class_<ZataFunction, ZataObject, std::shared_ptr<ZataFunction>>(m, "ZataFunction")
        .def(py::init<>())
        .def_readwrite("object_name", &ZataFunction::object_name)
        .def_readwrite("arg_count", &ZataFunction::arg_count)
        .def_readwrite("code", &ZataFunction::code)
        .def_readwrite("free_vars_names", &ZataFunction::free_vars_names)
        .def_readwrite("free_vars", &ZataFunction::free_vars);

    // 9. 绑定 ZataClass（继承ZataObject）
    py::class_<ZataClass, ZataObject, std::shared_ptr<ZataClass>>(m, "ZataClass")
        .def(py::init<>())
        .def_readwrite("object_name", &ZataClass::object_name)
        .def_readwrite("parent_class", &ZataClass::parent_class)
        .def_readwrite("names", &ZataClass::names)
        .def_readwrite("attrs", &ZataClass::attrs);

    // 10. 绑定 ZataInstance（继承ZataObject）
    py::class_<ZataInstance, ZataObject, std::shared_ptr<ZataInstance>>(m, "ZataInstance")
        .def(py::init<>())
        .def_readwrite("object_type", &ZataInstance::object_type)
        .def_readwrite("ref_class", &ZataInstance::ref_class)
        .def_readwrite("names", &ZataInstance::names)
        .def_readwrite("fields", &ZataInstance::fields);

    // 11. 绑定内置数据类型（均继承ZataBuiltinsClass）
    // 字符串对象
    py::class_<ZataString, ZataBuiltinsClass, std::shared_ptr<ZataString>>(m, "ZataString")
        .def(py::init<>())
        .def_readwrite("val", &ZataString::val); // 覆盖ZataBuiltinsClass的val（C++允许隐藏）

    // 整数对象
    py::class_<ZataInt, ZataBuiltinsClass, std::shared_ptr<ZataInt>>(m, "ZataInt")
        .def(py::init<>())
        .def_readwrite("val", &ZataInt::val);

    // 长整数对象
    py::class_<ZataInt64, ZataBuiltinsClass, std::shared_ptr<ZataInt64>>(m, "ZataInt64")
        .def(py::init<>())
        .def_readwrite("val", &ZataInt64::val);

    // 无限整数对象
    py::class_<ZataInfInt, ZataBuiltinsClass, std::shared_ptr<ZataInfInt>>(m, "ZataInfInt")
        .def(py::init<>())
        .def_readwrite("is_negative", &ZataInfInt::is_negative)
        .def_readwrite("digits", &ZataInfInt::digits)
        .def_readonly_static("BASE", &ZataInfInt::BASE)
        .def_readonly_static("BASE_DIGITS", &ZataInfInt::BASE_DIGITS);

    // 浮点数对象
    py::class_<ZataFloat, ZataBuiltinsClass, std::shared_ptr<ZataFloat>>(m, "ZataFloat")
        .def(py::init<>())
        .def_readwrite("val", &ZataFloat::val);

    // 长浮点数对象
    py::class_<ZataFloat64, ZataBuiltinsClass, std::shared_ptr<ZataFloat64>>(m, "ZataFloat64")
        .def(py::init<>())
        .def_readwrite("val", &ZataFloat64::val);

    // 安全小数对象
    py::class_<ZataDec, ZataBuiltinsClass, std::shared_ptr<ZataDec>>(m, "ZataDec")
        .def(py::init<>())
        .def_readwrite("is_negative", &ZataDec::is_negative)
        .def_readwrite("int_digits", &ZataDec::int_digits)
        .def_readwrite("frac_digits", &ZataDec::frac_digits)
        .def_readonly_static("BASE", &ZataDec::BASE)
        .def_readonly_static("BASE_DIGITS", &ZataDec::BASE_DIGITS);

    // 列表对象
    py::class_<ZataList, ZataBuiltinsClass, std::shared_ptr<ZataList>>(m, "ZataList")
        .def(py::init<>())
        .def_readwrite("items", &ZataList::items)
        .def_readwrite("size", &ZataList::size);

    // 字典对象
    py::class_<ZataDict, ZataBuiltinsClass, std::shared_ptr<ZataDict>>(m, "ZataDict")
        .def(py::init<>())
        .def_readwrite("key_val", &ZataDict::key_val);

    // 元组对象
    py::class_<ZataTuple, ZataBuiltinsClass, std::shared_ptr<ZataTuple>>(m, "ZataTuple")
        .def(py::init<>())
        .def_readwrite("items", &ZataTuple::items);

    // 记录对象
    py::class_<ZataRecord, ZataBuiltinsClass, std::shared_ptr<ZataRecord>>(m, "ZataRecord")
        .def(py::init<>())
        .def_readwrite("attrs", &ZataRecord::attrs);

    // 状态对象
    py::class_<ZataState, ZataBuiltinsClass, std::shared_ptr<ZataState>>(m, "ZataState")
        .def(py::init<>())
        .def_readwrite("val", &ZataState::val);

    // 绑定整数创建函数
    m.def("create_int", &create_int,
          "创建整数对象",
          py::arg("val"));

    // 绑定字符串创建函数
    m.def("create_str", &create_str,
          "创建字符串对象",
          py::arg("val"));

    // 绑定列表创建函数
    m.def("create_list", &create_list,
          "创建空列表对象");

    // 绑定长整数创建函数
    m.def("create_int64", &create_int64,
          "创建长整数对象",
          py::arg("val"));

    // 绑定浮点数创建函数
    m.def("create_float", &create_float,
          "创建浮点数对象",
          py::arg("val"));

    // 绑定双精度浮点数创建函数
    m.def("create_float64", &create_float64,
          "创建双精度浮点数对象",
          py::arg("val"));

    // 绑定字典创建函数
    m.def("create_dict", &create_dict,
          "创建空字典对象");

    // 绑定元组创建函数
    m.def("create_tuple",&create_tuple,"创建空元组对象");
}