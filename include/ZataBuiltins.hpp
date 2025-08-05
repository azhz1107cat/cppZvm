#ifndef ZVM_OPCODES_H
#define ZVM_OPCODES_H
#include <ios>
#include <iostream>

extern "C" inline void zata_print(const std::string& values,
    const std::string& sep,
    const std::string& end) {

    std::cout << values << sep << end;
}

extern "C" inline std::string zata_input(const std::string& proper) {
    std::cout << proper;
    std::string input;
    std::getline(std::cin, input);
    return input;
}
#endif
