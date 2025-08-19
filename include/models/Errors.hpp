#ifndef ZATA_ERRORS_H
#define ZATA_ERRORS_H
#include <iostream>
#include <stack>
#include <string>

#include "../vm_deps/CallFrame.hpp"

struct ZataError {
    std::string name = "ZataError";
    std::string message;
    int error_code = 0;
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
    std::cout << Fore::RED << "\n-- [ Trace Back ] --" << Fore::RESET << std::endl;

    while (!current_call_stack.empty()) {
        CallFrame current_call_frame = current_call_stack.top();

        std::cout << Fore::RED << " at function " << current_call_frame.name << Fore::RESET << std::endl;

        current_call_stack.pop();
    }

    std::cout << Fore::RED << "\n-- [ Infos ] --" << Fore::RESET << std::endl;
    std::cout << Fore::RED <<  error_class.name << ":" << error_class.message << " err_code=" << error_class.error_code << Fore::RESET << std::endl;

    std::cout << Fore::RED << "\n-- [ End ] --" << Fore::RESET  << std::endl;
    exit(error_class.error_code);
};

#endif // ZATA_ERRORS_H
