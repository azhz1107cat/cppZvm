#include "include/ZataVM.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // 用于自动转换std::vector

namespace py = pybind11;
using MixedType = std::variant<int, bool, std::string, std::shared_ptr<ZataObject>>;

// 将stack转换为vector
std::vector<MixedType> stack_to_vector(const std::stack<MixedType>& stack) {
    std::vector<MixedType> result;
    auto temp_stack = stack;

    while (!temp_stack.empty()) {
        result.push_back(temp_stack.top());
        temp_stack.pop();
    }

    // 反转以保持原始顺序
    std::ranges::reverse(result);
    return result;
}

std::vector<MixedType> execute_bytecode(
    const std::vector<MixedType>& bytecode,
    const std::vector<MixedType>& constants,
    const std::vector<MixedType>& globals
) {
    ZataVirtualMachine vm(constants, globals);
    const auto final_stack = vm.run(bytecode);

    // 转换为vector返回给Python
    return stack_to_vector(final_stack);
}

int main() {
    std::cout << "Hello World!" << std::endl;
    return 0;
}

// 修改为：模块名 = 目标名（cppZvm）
PYBIND11_MODULE(cppZvm, m) {
    m.doc() = "Zata虚拟机Python绑定";

    m.def("execute_bytecode", &execute_bytecode,
          "执行字节码并返回最终栈状态",
          py::arg("bytecode"), py::arg("constants"), py::arg("globals"));
}