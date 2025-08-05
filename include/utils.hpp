#ifndef UTILS_HPP
#define UTILS_HPP
#include <fstream>

#include "ZataVM.hpp"

namespace Utils {
    // 将stack转换为vector
    inline std::vector<ItemOfStack> stack_to_vector(const std::stack<ItemOfStack>& stack) {
        std::vector<ItemOfStack> result;
        auto temp_stack = stack;

        while (!temp_stack.empty()) {
            result.push_back(temp_stack.top());
            temp_stack.pop();
        }

        std::ranges::reverse(result);
        return result;
    }

    inline auto read_zir(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "failed to open file: " << filename << std::endl;
            return std::vector<unsigned char>{};
        }

        file.seekg(0, std::ios::end);
        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<unsigned char> buffer(size);

        file.read(reinterpret_cast<char*>(buffer.data()), size);

        if (!file) {
            std::cerr << "Unknown error that we only read" << file.gcount() << "b, instead of" << size << " b" << std::endl;
            return std::vector<unsigned char>{}; // 返回空vector表示错误
        }

        file.close();
        return buffer;
    }

    inline void enable_ansi_escape() {
    #ifdef _WIN32
    #include <windows.h>
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;

        DWORD mode = 0;
        if (!GetConsoleMode(hOut, &mode)) return;

        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, mode);
    #endif
    }
}

#endif //UTILS_HPP
