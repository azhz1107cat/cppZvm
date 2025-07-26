#ifndef ZATA_ERRORS_H
#define ZATA_ERRORS_H
#include <iostream>
#include <stack>
#include <string>
#include "ZataVM.hpp"

struct ZataError {
    std::string message;
    int error_code = 0;
    std::string name = "ZataError";

    // 构造函数方便初始化
    explicit ZataError(std::string msg = "", int code = 0)
        : message(std::move(msg)), error_code(code) {}
};

struct ZataCallStackError : public ZataError {
    std::string name = "ZataCallStackError";
    using ZataError::ZataError; // 继承构造函数
};

struct ZataOpStackError : public ZataError {
    std::string name = "ZataOpStackError";
    using ZataError::ZataError;
};

struct ZataCalcError : public ZataError {
    std::string name = "ZataCalcError";
    using ZataError::ZataError;
};

struct ZataRuntimeError : public ZataError {
    std::string name = "ZataRuntimeError";
    using ZataError::ZataError;
};

struct ZataMemoryError : public ZataError {
    std::string name = "ZataMemoryError";
    using ZataError::ZataError;
};

struct ZataIOError : public ZataError {
    std::string name = "ZataIOError";
    using ZataError::ZataError;
};

struct ZataBytesCodeError : public ZataError {
    std::string name = "ZataBytesCodeError";
    using ZataError::ZataError;
};

struct ZataTypeError : public ZataError {
    std::string name = "ZataTypeError";
    using ZataError::ZataError;
};

namespace Fore {
    const std::string RESET = "\033[0m";

    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";

    const std::string LIGHT_BLACK = "\033[90m";
    const std::string LIGHT_RED = "\033[91m";
    const std::string LIGHT_GREEN = "\033[92m";
    const std::string LIGHT_YELLOW = "\033[93m";
    const std::string LIGHT_BLUE = "\033[94m";
    const std::string LIGHT_MAGENTA = "\033[95m";
    const std::string LIGHT_CYAN = "\033[96m";
    const std::string LIGHT_WHITE = "\033[97m";
}

inline void zata_vm_error_thrower(std::stack<CallFrame> current_call_stack,
                                  const ZataError& error_class){
    std::cout << "-- [ Trace Back ] --" << std::endl;

    while (!current_call_stack.empty()) {
        CallFrame current_call_frame = current_call_stack.top();

        std::cout << " at function " << current_call_frame.function_name << std::endl;

        current_call_stack.pop();
    }

    std::cout << "-- [ Infos ] --" << std::endl;
    std::cout << error_class.name << "[" <<error_class.error_code << "]"
    << ":" << error_class.message << std::endl;

    std::cout << "-- [ End ] --" << std::endl;
};

#endif // ZATA_ERRORS_H
