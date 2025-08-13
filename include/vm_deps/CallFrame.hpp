#ifndef CALL_FRAME_HPP
#define CALL_FRAME_HPP
#include <string>

#include "models/Objects.hpp"

// 调用帧
struct CallFrame {
    int pc = 0;
    std::vector<ZataObjectPtr> locals;
    int return_address = 0;
    std::string name;
};


#endif //CALL_FRAME_HPP
