#ifndef ZATA_VM_H
#define ZATA_VM_H

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include "Object_meta_data.hpp"

#include "ZvmOpcodes.hpp"

using MixedType = std::variant<int, bool, std::string, std::shared_ptr<ZataObject>>;

// 调用帧
struct CallFrame {
     int pc = 0;
     std::vector<MixedType> locals;
     int return_address = 0;
};

// 虚拟机
class ZataVirtualMachine {
    std::stack<MixedType>   op_stack;
    std::stack<CallFrame>   call_stack;

    std::vector<MixedType>  locals;
    std::vector<MixedType>  globals;
    std::vector<MixedType>  constant_pool;

    std::vector<MixedType> code;
    std::stack<ZataFunction> current_function;
    int pc = 0;
    bool running = false;

public:
    ZataVirtualMachine()
        : pc(0), running(false) {
        // 初始化容器（可选，STL容器默认会自动初始化）
        op_stack = std::stack<MixedType>();
        call_stack = std::stack<CallFrame>();
        locals.clear();
        globals.clear();
        constant_pool.clear();
        code.clear();
        current_function = std::stack<ZataFunction>();
    }

    // 带参数的构造函数：可初始化常量池和全局变量
    explicit ZataVirtualMachine(const std::vector<MixedType>& constants)
        : ZataVirtualMachine() {  // 委托给默认构造函数
        this->constant_pool = constants;  // 初始化常量池
    }

    std::stack<MixedType> run(const std::vector<MixedType> &main_code) {
        this->code = main_code;
        this->running = true;
        while(this->running) {
            std::vector<MixedType> current_code =  !this->current_function.empty() ? this->current_function.top().code : this->code;

            if (this->pc >= current_code.size()){
                this->running = false;
                break;
            }

            const int opcode = std::get<int>(current_code[this->pc]);
            this->pc += 1;

            if (opcode == static_cast<int>(Opcode::ADD)) {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a + b);
            }
            else if(opcode == static_cast<int>(Opcode::SUB)){
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a - b);
            }
            else if(opcode == static_cast<int>(Opcode::MUL)){
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a * b);
            }
            else if(opcode == static_cast<int>(Opcode::DIV)){
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                if(b == 0) throw std::runtime_error("Division by zero");
                this->op_stack.emplace(a / b);
            }
            else if(opcode == static_cast<int>(Opcode::MOD)){
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                if(b == 0) throw std::runtime_error("Modulo by zero");
                this->op_stack.emplace(a % b);
            }
            else if(opcode == static_cast<int>(Opcode::EQ_EQ)){
                auto b = this->op_stack.top();
                this->op_stack.pop();
                auto a = this->op_stack.top();
                this->op_stack.pop();
                bool result = false;

                if(a.index() == b.index()) { // 类型相同才比较
                    if(std::holds_alternative<int>(a)) {
                        result = std::get<int>(a) == std::get<int>(b);
                    }
                }

                this->op_stack.emplace(result);
            }
            else if(opcode == static_cast<int>(Opcode::NE)){
                auto b = this->op_stack.top();
                this->op_stack.pop();
                auto a = this->op_stack.top();
                this->op_stack.pop();
                bool result = true;

                if(a.index() == b.index()) {
                    if(std::holds_alternative<int>(a)) {
                        result = std::get<int>(a) != std::get<int>(b);
                    }
                }

                this->op_stack.emplace(result);
            }
            else if(opcode == static_cast<int>(Opcode::LT)){
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a < b);
            }
            else if(opcode == static_cast<int>(Opcode::GT)){
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a > b);
            }
            else if(opcode == static_cast<int>(Opcode::LE)){
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a <= b);
            }
            else if(opcode == static_cast<int>(Opcode::GE)){
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a >= b);
            }
            else if(opcode == static_cast<int>(Opcode::AND)){
                auto b = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a && b);
            }
            else if(opcode == static_cast<int>(Opcode::OR)){
                auto b = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a || b);
            }
            else if(opcode == static_cast<int>(Opcode::NOT)){
                auto a = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(!a);
            }
            else if(opcode == static_cast<int>(Opcode::NEG)){
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(-a);
            }
            else if(opcode == static_cast<int>(Opcode::SWAP)){
                auto b = this->op_stack.top();
                this->op_stack.pop();
                auto a = this->op_stack.top();
                this->op_stack.pop();
                this->op_stack.emplace(b);
                this->op_stack.emplace(a);
            }
            else if(opcode == static_cast<int>(Opcode::LOAD_CONST)) {
                int const_addr = std::get<int>(current_code[this->pc]);
                this->pc += 1;
                this->op_stack.emplace(this->constant_pool[const_addr]);
            }
            else if(opcode == static_cast<int>(Opcode::LOAD_VAR)) {
                int var_addr = std::get<int>(current_code[this->pc]);
                this->pc += 1;
                this->op_stack.emplace(this->locals[var_addr]);
            }
            else if(opcode == static_cast<int>(Opcode::STORE_VAR)) {
                int var_addr = std::get<int>(current_code[this->pc]);
                this->pc += 1;
                MixedType val = this->op_stack.top();
                this->op_stack.pop();
                this->locals[var_addr] = val;
            }
            else if(opcode == static_cast<int>(Opcode::LOAD_GLOBAL)) {
                int var_addr = std::get<int>(current_code[this->pc]);
                this->pc += 1;
                this->op_stack.emplace(this->globals[var_addr]);
            }
            else if(opcode == static_cast<int>(Opcode::STORE_GLOBAL)) {
                int var_addr = std::get<int>(current_code[this->pc]);
                this->pc += 1;
                MixedType val = this->op_stack.top();
                this->op_stack.pop();
                this->globals[var_addr] = val;
            }
            else if(opcode == static_cast<int>(Opcode::JMP)) {
                int offset = std::get<int>(current_code[this->pc]);
                this->pc += offset;
            }
            else if(opcode == static_cast<int>(Opcode::JMP_IF_FALSE)) {
                int offset = std::get<int>(current_code[this->pc]);
                MixedType cond = this->op_stack.top();
                this->op_stack.pop();

                bool condition = false;
                if(std::holds_alternative<bool>(cond)) {
                    condition = std::get<bool>(cond);
                }

                if (!condition) {
                    this->pc += offset;
                } else {
                    this->pc += 1;
                }
            }
            else if(opcode == static_cast<int>(Opcode::JMP_IF_TRUE)) {
                int offset = std::get<int>(current_code[this->pc]);
                MixedType cond = this->op_stack.top();
                this->op_stack.pop();

                bool condition = false;
                if(std::holds_alternative<bool>(cond)) {
                    condition = std::get<bool>(cond);
                }

                if (condition) {
                    this->pc += offset;
                } else {
                    this->pc += 1;
                }
            }
            else if(opcode == static_cast<int>(Opcode::NOP)) {
                // 空操作
            }
            else if(opcode == static_cast<int>(Opcode::CALL)) {
                int fn_addr = std::get<int>(current_code[this->pc]);
                int arg_count = std::get<int>(current_code[this->pc + 1]);
                this->pc += 2;

                MixedType fn = this->constant_pool[fn_addr];

                if (!std::holds_alternative<std::shared_ptr<ZataObject>>(fn)) {
                    throw std::runtime_error("CALL opcode: function is not a ZataObject");
                }
                auto zata_obj_ptr = std::get<std::shared_ptr<ZataObject>>(fn);

                auto fn_ptr = std::dynamic_pointer_cast<ZataFunction>(zata_obj_ptr);
                if (!fn_ptr) throw std::runtime_error("CALL opcode: object is not a Function");

                std::vector<MixedType> function_code = fn_ptr->code;

                std::vector<MixedType> fns_locals(arg_count);
                std::vector<MixedType> args;

                for(int i = 0; i < arg_count; ++i) {
                    args.push_back(this->op_stack.top());
                    this->op_stack.pop();
                }
                std::ranges::reverse(args);

                for(int i = 0; i < arg_count; ++i) {
                    fns_locals[i] = args[i];
                }

                CallFrame frame;
                frame.pc = this->pc;
                frame.locals = this->locals;
                frame.return_address = this->pc;
                this->call_stack.push(frame);

                this->locals = fns_locals;
                this->current_function.push(*fn_ptr);
                this->pc = 0;
            }
            else if(opcode == static_cast<int>(Opcode::RET)) {
                if (!this->call_stack.empty()) {
                    CallFrame frame = this->call_stack.top();
                    this->call_stack.pop();

                    this->pc = frame.return_address;
                    this->locals = frame.locals;

                    if (!this->current_function.empty()) {
                        this->current_function.pop();
                    }
                } else {
                    throw std::runtime_error("RET opcode: call stack is empty");
                }
            }
            else if(opcode == static_cast<int>(Opcode::NEW_OBJ)) {
                int class_addr = std::get<int>(current_code[this->pc]);
                this->pc += 1;

                MixedType class_obj = this->constant_pool[class_addr];

                // 检查是否为 shared_ptr<ZataObject>
                if(!std::holds_alternative<std::shared_ptr<ZataObject>>(class_obj)) throw std::runtime_error("NEW_OBJ opcode: invalid class type");

                auto zata_obj_ptr = std::get<std::shared_ptr<ZataObject>>(class_obj);

                // 使用 dynamic_pointer_cast 将基类指针转换为子类指针
                auto class_ptr = std::dynamic_pointer_cast<ZataClass>(zata_obj_ptr);
                if (!class_ptr) {
                    throw std::runtime_error("NEW_OBJ opcode: object is not a Class");
                }

                auto class_instance = class_ptr->instantiate();
                this->op_stack.emplace(class_instance);
            }
            else if(opcode == static_cast<int>(Opcode::SET_FIELD)) {
                int field_name_addr = std::get<int>(current_code[this->pc]);
                this->pc += 1;

                MixedType field_name = this->constant_pool[field_name_addr];
                if(!std::holds_alternative<std::string>(field_name)) {
                    throw std::runtime_error("SET_FIELD opcode: invalid field name type");
                }

                MixedType value = this->op_stack.top();
                this->op_stack.pop();
                MixedType obj = this->op_stack.top();
                this->op_stack.pop();

                // 检查是否为 shared_ptr<ZataObject>
                if(!std::holds_alternative<std::shared_ptr<ZataObject>>(obj)) {
                    throw std::runtime_error("SET_FIELD opcode: object is not a ZataObject");
                }

                // 获取智能指针
                auto ptr = std::get<std::shared_ptr<ZataObject>>(obj);

                // 转换为 Instanced 指针
                auto instance = std::dynamic_pointer_cast<Instanced>(ptr);
                if (!instance) {
                    throw std::runtime_error("SET_FIELD opcode: object is not an instance");
                }

                // 设置字段值
                instance->set_field(std::get<std::string>(field_name), value);
            }
            else if(opcode == static_cast<int>(Opcode::GET_FIELD)) {
                // TODO:...
            }
            else if(opcode == static_cast<int>(Opcode::CALL_METHOD)) {
                int fn_addr = std::get<int>(current_code[this->pc]);
                int arg_count = std::get<int>(current_code[this->pc + 1]);
                this->pc += 2;

                MixedType fn = this->constant_pool[fn_addr];

                // 检查函数是否为 ZataObject 智能指针
                if (!std::holds_alternative<std::shared_ptr<ZataObject>>(fn)) {
                    throw std::runtime_error("CALL_METHOD opcode: function is not a ZataObject");
                }
                auto zata_obj_ptr = std::get<std::shared_ptr<ZataObject>>(fn);

                // 转换为 Function 类型
                auto fn_ptr = std::dynamic_pointer_cast<Function>(zata_obj_ptr);
                if (!fn_ptr) {
                    throw std::runtime_error("CALL_METHOD opcode: object is not a Function");
                }

                // 提取栈中的实例对象（this 指针）
                MixedType obj = this->op_stack.top();
                this->op_stack.pop();
                if (!std::holds_alternative<std::shared_ptr<ZataObject>>(obj)) {
                    throw std::runtime_error("CALL_METHOD opcode: top of stack is not an object");
                }

                // 将对象指针转换为 Instanced 实例
                auto obj_ptr = std::get<std::shared_ptr<ZataObject>>(obj);
                auto instance = std::dynamic_pointer_cast<Instanced>(obj_ptr);
                if (!instance) {
                    throw std::runtime_error("CALL_METHOD opcode: object is not an instance");
                }

                // 查找方法
                auto method = instance->get_method(fn_ptr->name);  // 注意使用 -> 调用指针方法
                if (!method) {
                    throw std::runtime_error("CALL_METHOD opcode: method not found: " + fn_ptr->name);
                }

                // 处理函数参数
                std::vector<MixedType> args;
                for (int i = 0; i < arg_count; ++i) {
                    args.push_back(this->op_stack.top());
                    this->op_stack.pop();
                }
                std::ranges::reverse(args);  // 反转参数顺序以匹配声明顺序

                // 准备函数局部变量（包含实例对象作为第一个参数，即 this）
                std::vector<MixedType> fns_locals;
                fns_locals.reserve(arg_count + 1);
                fns_locals.push_back(obj);  // 将实例对象作为第一个局部变量（this）
                for (int i = 0; i < arg_count; ++i) {
                    fns_locals.push_back(args[i]);
                }

                // 保存调用帧
                CallFrame frame;
                frame.pc = this->pc;
                frame.locals = this->locals;
                frame.return_address = this->pc;
                this->call_stack.push(frame);

                // 切换到新函数
                this->locals = fns_locals;
                this->current_function.push(*method);  // 注意：应压入查找到的 method 而非原 fn_ptr
                this->pc = 0;
            }
            else if(opcode == static_cast<int>(Opcode::PUSH)) {
                MixedType num = current_code[this->pc];
                this->pc += 1;
                this->op_stack.emplace(num);
            }
            else if(opcode == static_cast<int>(Opcode::POP)) {
                if(!this->op_stack.empty()) {
                    this->op_stack.pop();
                } else {
                    throw std::runtime_error("POP opcode: stack underflow");
                }
            }
            else if(opcode == static_cast<int>(Opcode::DUP)) {
                if(!this->op_stack.empty()) {
                    MixedType a = this->op_stack.top();
                    this->op_stack.emplace(a);
                } else {
                    throw std::runtime_error("DUP opcode: stack underflow");
                }
            }
            else if(opcode == static_cast<int>(Opcode::ALLOC)) {
                //TODO:...
            }
            else if(opcode == static_cast<int>(Opcode::FREE)) {
                //TODO:...
            }
            else if(opcode == static_cast<int>(Opcode::LOAD_MEM)) {
                //TODO:...
            }
            else if(opcode == static_cast<int>(Opcode::STORE_MEM)) {
                //TODO:...
            }
            else if(opcode == static_cast<int>(Opcode::BIT_AND)) {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a & b);
            }
            else if(opcode == static_cast<int>(Opcode::BIT_OR)) {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a | b);
            }
            else if(opcode == static_cast<int>(Opcode::BIT_XOR)) {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a ^ b);
            }
            else if(opcode == static_cast<int>(Opcode::SHL)) {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a << b);
            }
            else if(opcode == static_cast<int>(Opcode::SHR)) {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a >> b);
            }
            else if(opcode == static_cast<int>(Opcode::HALT)) {
                this->running = false;
            }
            else {
                throw std::runtime_error("Unknown opcode: " + std::to_string(opcode));
            }
        }

        return this->op_stack;
    }
};

#endif // ZATA_VM_H
