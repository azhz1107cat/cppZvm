#ifndef ZATA_VM_H
#define ZATA_VM_H

#include <algorithm>
#include <array>
#include <vector>
#include <stack>
#include <string>
#include <memory>
#include <unordered_map>
#include <variant>
#include "Objects.hpp"

#include "ZvmOpcodes.hpp"

using ItemOfStack = std::variant<int, bool, std::string, std::shared_ptr<ZataObject>, uint64_t>;

// 调用帧
struct CallFrame {
     int pc = 0;
     std::vector<ItemOfStack> locals;
     int return_address = 0;
     std::string function_name;
};

// 虚拟机
class ZataVirtualMachine {
    std::stack<ItemOfStack>   op_stack;
    std::stack<CallFrame>   call_stack;

    std::vector<ItemOfStack>  locals;
    std::vector<ItemOfStack>  globals;
    std::vector<ItemOfStack>  constant_pool;

    std::vector<codeMember> code;
    std::stack<ZataFunction> current_function;
    std::unordered_map<uint64_t, ItemOfStack> memory;
    uint64_t next_memory_address = 0x1000;  // 从0x1000开始（避开低地址）
    int pc = 0;
    bool running = false;

public:
    ZataVirtualMachine()
        : pc(0) {
        op_stack = std::stack<ItemOfStack>();
        call_stack = std::stack<CallFrame>();
        locals.clear();
        globals.clear();
        constant_pool.clear();
        code.clear();
        current_function = std::stack<ZataFunction>();
    }

    // 带参数的构造函数：可初始化常量池和全局变量
    explicit ZataVirtualMachine(const std::vector<ItemOfStack>& constants)
        : ZataVirtualMachine() {  // 委托给默认构造函数
        this->constant_pool = constants;  // 初始化常量池
    }

    std::stack<ItemOfStack> run(const std::vector<codeMember> &main_code) {
        this->code = main_code;
        this->running = true;
        while(this->running) {
            std::vector<codeMember> current_code =  !this->current_function.empty() ? this->current_function.top().bytecode : this->code;

            if (this->pc >= current_code.size()){
                this->running = false;
                break;
            }

            const int opcode = current_code[this->pc];
            this->pc += 1;

            switch (opcode) {
            case Opcode::ADD: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a + b);
                break;
            }
            case Opcode::SUB: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a - b);
                break;
            }
            case Opcode::MUL: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a * b);
                break;
            }
            case Opcode::DIV: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                if(b == 0) throw std::runtime_error("Division by zero");
                this->op_stack.emplace(a / b);
                break;
            }
            case Opcode::MOD: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                if(b == 0) throw std::runtime_error("Modulo by zero");
                this->op_stack.emplace(a % b);
                break;
            }
            case Opcode::EQ_EQ: {
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
                break;
            }
            case Opcode::NE: {
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
                break;
            }
            case Opcode::LT: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a < b);
                break;
            }
            case Opcode::GT: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a > b);
                break;
            }
            case Opcode::LE: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a <= b);
                break;
            }
            case Opcode::GE: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a >= b);
                break;
            }
            case Opcode::AND: {
                auto b = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a && b);
                break;
            }
            case Opcode::OR: {
                auto b = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a || b);
                break;
            }
            case Opcode::NOT: {
                auto a = std::get<bool>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(!a);
                break;
            }
            case Opcode::NEG: {
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(-a);
                break;
            }
            case Opcode::SWAP: {
                auto b = this->op_stack.top();
                this->op_stack.pop();
                auto a = this->op_stack.top();
                this->op_stack.pop();
                this->op_stack.emplace(b);
                this->op_stack.emplace(a);
                break;
            }
            case Opcode::LOAD_CONST: {
                int const_addr = current_code[this->pc];
                this->pc += 1;
                this->op_stack.emplace(this->constant_pool[const_addr]);
                break;
            }
            case Opcode::LOAD_VAR: {
                int var_addr = current_code[this->pc];
                this->pc += 1;
                this->op_stack.emplace(this->locals[var_addr]);
                break;
            }
            case Opcode::STORE_VAR: {
                int var_addr = current_code[this->pc];
                this->pc += 1;
                ItemOfStack val = this->op_stack.top();
                this->op_stack.pop();
                this->locals[var_addr] = val;
                break;
            }
            case Opcode::LOAD_GLOBAL: {
                int var_addr = current_code[this->pc];
                this->pc += 1;
                this->op_stack.emplace(this->globals[var_addr]);
                break;
            }
            case Opcode::STORE_GLOBAL: {
                int var_addr = current_code[this->pc];
                this->pc += 1;
                ItemOfStack val = this->op_stack.top();
                this->op_stack.pop();
                this->globals[var_addr] = val;
                break;
            }
            case Opcode::JMP: {
                int offset = current_code[this->pc];
                this->pc += offset;
                break;
            }
            case Opcode::JMP_IF_FALSE: {
                int offset = current_code[this->pc];
                ItemOfStack cond = this->op_stack.top();
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
                break;
            }
            case Opcode::JMP_IF_TRUE: {
                int offset = current_code[this->pc];
                ItemOfStack cond = this->op_stack.top();
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
                break;
            }
            case Opcode::NOP: {
                // 空操作
                break;
            }
            case Opcode::CALL: {
                int fn_addr = current_code[this->pc];
                int arg_count = current_code[this->pc + 1];
                this->pc += 2;

                ItemOfStack fn = this->constant_pool[fn_addr];

                if (!std::holds_alternative<std::shared_ptr<ZataObject>>(fn)) {
                    throw std::runtime_error("CALL opcode: function is not a ZataObject");
                }
                auto zata_obj_ptr = std::get<std::shared_ptr<ZataObject>>(fn);

                auto fn_ptr = std::dynamic_pointer_cast<ZataFunction>(zata_obj_ptr);
                if (!fn_ptr) throw std::runtime_error("CALL opcode: object is not a Function");

                std::vector<int> function_code = fn_ptr->bytecode;

                std::vector<ItemOfStack> fns_locals(arg_count);
                std::vector<ItemOfStack> args;

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
                frame.function_name = fn_ptr->function_name;
                this->call_stack.push(frame);

                this->locals = fns_locals;
                this->current_function.push(*fn_ptr);
                this->code = fn_ptr->bytecode;
                this->pc = 0;
                break;
            }
            case Opcode::RET: {
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
                break;
            }
            case Opcode::NEW_OBJ: {
                int class_addr = current_code[this->pc];
                this->pc += 1;

                ItemOfStack class_obj = this->constant_pool[class_addr];

                // 检查是否为 shared_ptr<ZataObject>
                if(!std::holds_alternative<std::shared_ptr<ZataObject>>(class_obj)) throw std::runtime_error("NEW_OBJ opcode: invalid class type");

                auto zata_obj_ptr = std::get<std::shared_ptr<ZataObject>>(class_obj);

                // 使用 dynamic_pointer_cast 将基类指针转换为子类指针
                auto class_ptr = std::dynamic_pointer_cast<ZataClass>(zata_obj_ptr);
                if (!class_ptr) {
                    throw std::runtime_error("NEW_OBJ opcode: object is not a Class");
                }

                /* TODO : if (!class_prt->local){
                 *    this->run(class_ptr->bytecode);
                 * }
                 */
                auto class_instance = class_ptr;
                this->op_stack.emplace(class_instance);
                break;
            }
            case Opcode::SET_FIELD: {
                int field_addr = current_code[this->pc];
                this->pc += 1;

                ItemOfStack value = this->op_stack.top();
                this->op_stack.pop();
                ItemOfStack obj = this->op_stack.top();
                this->op_stack.pop();

                // 检查是否为 shared_ptr<ZataObject>
                if(!std::holds_alternative<std::shared_ptr<ZataObject>>(obj)) {
                    throw std::runtime_error("SET_FIELD opcode: object is not a ZataObject");
                }

                // 获取智能指针
                auto ptr = std::get<std::shared_ptr<ZataObject>>(obj);

                // 转换为 Instanced 指针
                auto instance = std::dynamic_pointer_cast<ZataInstanced>(ptr);
                if (!instance) {
                    throw std::runtime_error("SET_FIELD opcode: object is not an instance");
                }

                // 设置字段值
                // TODO: ...
                break;
            }
            case Opcode::GET_FIELD: {
                // TODO:...
                break;
            }
            case Opcode::CALL_METHOD: {
                int fn_addr = current_code[this->pc];
                int arg_count = current_code[this->pc + 1];
                this->pc += 2;

                ItemOfStack fn = this->constant_pool[fn_addr];

                // 检查函数是否为 ZataObject 智能指针
                if (!std::holds_alternative<std::shared_ptr<ZataObject>>(fn)) {
                    throw std::runtime_error("CALL_METHOD opcode: function is not a ZataObject");
                }
                auto zata_obj_ptr = std::get<std::shared_ptr<ZataObject>>(fn);

                // 转换为 Function 类型
                auto fn_ptr = std::dynamic_pointer_cast<ZataFunction>(zata_obj_ptr);
                if (!fn_ptr) {
                    throw std::runtime_error("CALL_METHOD opcode: object is not a Function");
                }

                // 提取栈中的实例对象（this 指针）
                ItemOfStack obj = this->op_stack.top();
                this->op_stack.pop();
                if (!std::holds_alternative<std::shared_ptr<ZataObject>>(obj)) {
                    throw std::runtime_error("CALL_METHOD opcode: top of stack is not an object");
                }

                // 将对象指针转换为 Instanced 实例
                auto obj_ptr = std::get<std::shared_ptr<ZataObject>>(obj);
                auto instance = std::dynamic_pointer_cast<ZataInstanced>(obj_ptr);
                if (!instance) {
                    throw std::runtime_error("CALL_METHOD opcode: object is not an instance");
                }

                // 查找方法
                /* TODO ...
                if (!method) {
                    throw std::runtime_error("CALL_METHOD opcode: method not found: " + fn_ptr->function_name);
                }

                // 处理函数参数
                std::vector<ItemOfStack> args;
                for (int i = 0; i < arg_count; ++i) {
                    args.push_back(this->op_stack.top());
                    this->op_stack.pop();
                }
                std::ranges::reverse(args);  // 反转参数顺序以匹配声明顺序

                // 准备函数局部变量（包含实例对象作为第一个参数，即 this）
                std::vector<ItemOfStack> fns_locals;
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
                this->pc = 0;*/
                break;
            }
            case Opcode::POP: {
                if(!this->op_stack.empty()) {
                    this->op_stack.pop();
                } else {
                    throw std::runtime_error("POP opcode: stack underflow");
                }
                break;
            }
            case Opcode::DUP: {
                if(!this->op_stack.empty()) {
                    ItemOfStack a = this->op_stack.top();
                    this->op_stack.emplace(a);
                } else {
                    throw std::runtime_error("DUP opcode: stack underflow");
                }
                break;
            }
            case Opcode::ALLOC: {
                // ToDo : Make sure use stack or code
                ItemOfStack size_item = current_code[this->pc];
                this->pc += 1;
                if (!std::holds_alternative<int>(size_item)) {
                    throw std::runtime_error("ALLOC opcode: size must be integer");
                }
                int size = std::get<int>(size_item);
                if (size <= 0) {
                    throw std::runtime_error("ALLOC opcode: size must be positive");
                }

                uint64_t addr = next_memory_address;
                next_memory_address += size;

                memory[addr] = ItemOfStack{};

                op_stack.emplace(addr);
                break;
            }
            case Opcode::FREE: {
                ItemOfStack addr_item = op_stack.top();
                op_stack.pop();
                if (!std::holds_alternative<uint64_t>(addr_item)) {
                    throw std::runtime_error("FREE opcode: address must be uint64_t");
                }
                uint64_t addr = std::get<uint64_t>(addr_item);

                // 检查地址是否存在
                if (memory.find(addr) == memory.end()) {
                    throw std::runtime_error("FREE opcode: invalid address (not allocated)");
                }

                // 释放内存（从map中删除）
                memory.erase(addr);
                break;
            }
            case Opcode::LOAD_MEM: {
                // 功能：从指定地址加载数据到栈
                // 栈输入：[address]（要读取的地址，uint64_t）
                // 栈输出：[value]（地址中存储的数据）

                if (op_stack.empty()) {
                    throw std::runtime_error("LOAD_MEM opcode: stack underflow (need address)");
                }

                // 获取地址
                ItemOfStack addr_item = op_stack.top();
                op_stack.pop();
                if (!std::holds_alternative<uint64_t>(addr_item)) {
                    throw std::runtime_error("LOAD_MEM opcode: address must be uint64_t");
                }
                uint64_t addr = std::get<uint64_t>(addr_item);

                // 检查地址是否存在
                if (memory.find(addr) == memory.end()) {
                    throw std::runtime_error("LOAD_MEM opcode: invalid address (not allocated)");
                }

                // 将地址中的数据压入栈
                ItemOfStack value = memory[addr];
                op_stack.emplace(value);
                break;
            }
            case Opcode::STORE_MEM: {
                // 功能：将栈顶数据存储到指定地址
                // 栈输入：[value, address]（先弹出value，再弹出address）
                // 栈输出：无

                if (op_stack.size() < 2) {
                    throw std::runtime_error("STORE_MEM opcode: stack underflow (need value and address)");
                }

                // 弹出值和地址（注意栈的顺序：先弹值，再弹地址）
                ItemOfStack value = op_stack.top();
                op_stack.pop();
                ItemOfStack addr_item = op_stack.top();
                op_stack.pop();

                if (!std::holds_alternative<uint64_t>(addr_item)) {
                    throw std::runtime_error("STORE_MEM opcode: address must be uint64_t");
                }
                uint64_t addr = std::get<uint64_t>(addr_item);

                // 检查地址是否已分配（也可允许存储到新地址，视需求而定）
                if (memory.find(addr) == memory.end()) {
                    throw std::runtime_error("STORE_MEM opcode: invalid address (not allocated)");
                }

                // 存储数据到内存
                memory[addr] = value;
                break;
            }
            case Opcode::BIT_AND: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a & b);
                break;
            }
            case Opcode::BIT_OR: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a | b);
                break;
            }
            case Opcode::BIT_XOR: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a ^ b);
                break;
            }
            case Opcode::SHL: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a << b);
                break;
            }
            case Opcode::SHR: {
                auto b = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                auto a = std::get<int>(this->op_stack.top());
                this->op_stack.pop();
                this->op_stack.emplace(a >> b);
                break;
            }
            case Opcode::HALT: {
                this->running = false;
                break;
            }
            default: {
                throw std::runtime_error("Unknown opcode: " + std::to_string(opcode));
            }
            }
        }

        return this->op_stack;
    }
};

#endif // ZATA_VM_H
