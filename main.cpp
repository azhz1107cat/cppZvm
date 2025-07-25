#include <fstream>

#include "include/ZataVM.hpp"
#include <pybind11/pybind11.h>

#include "Errors.hpp"
#include "utils.hpp"


std::vector<ItemOfStack> execute_bytecode(
    const std::vector<codeMember>& bytecode,
    const std::vector<ItemOfStack>& constants
) {

    enable_ansi_escape();
    ZataVirtualMachine vm(constants);
    const auto current_stack = vm.run(bytecode);

    // 转换为vector返回给Python
    return stack_to_vector(current_stack);
}


PYBIND11_MODULE(cppZvm, m) {
    m.doc() = "Zata虚拟机Python绑定";

    m.def("execute_bytecode", &execute_bytecode,
          "执行字节码并返回最终栈状态或错误时栈状态",
          pybind11::arg("bytecode"));
}