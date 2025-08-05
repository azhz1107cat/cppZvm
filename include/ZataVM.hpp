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

#include "Errors.hpp"
#include "Objects.hpp"

#include "ZvmOpcodes.hpp"

// 调用帧
struct CallFrame {
     int pc = 0;
     std::vector<ZataElem> locals;
     int return_address = 0;
     std::string function_name;
     std::vector<ZataElem> constant_pool;
     std::vector<int> code;
};

// 虚拟机
class ZataVirtualMachine {
    std::stack<ZataElem>   op_stack;
    std::stack<CallFrame>   call_stack;

    std::vector<ZataElem>  locals;
    std::vector<ZataElem>  globals;
    std::vector<ZataElem>  constant_pool;

    std::vector<int> code;
    std::stack<std::shared_ptr<ZataFunction>> current_function;
    std::unordered_map<size_t, std::shared_ptr<ZataInstance>> memory;
    uint64_t next_memory_address = 0x1000;  // 避开低地址
    int pc = 0;
    bool running = false;

public:
    ZataVirtualMachine() = default;

    explicit ZataVirtualMachine(const std::vector<ZataElem>& constants)
        : ZataVirtualMachine() {  // 委托给默认构造函数
        this->constant_pool = constants;  // 初始化常量池
    }

    std::stack<ZataElem> run(const std::vector<int> &main_code) {
        this->code = main_code;
        this->running = true;
        while(this->running) {
            const std::vector<int>& current_code = !this->current_function.empty()
                                                   ? this->current_function.top()->bytecode
                                                   : this->code;

            if (this->pc >= current_code.size()){
                this->running = false;
                break;
            }

            const int opcode = current_code[this->pc];
            this->pc += 1;

            switch (opcode) {
            // case Opcode::CALC: {
            //     auto b = std::get<int>(this->op_stack.top());
            //     this->op_stack.pop();
            //     auto a = std::get<int>(this->op_stack.top());
            //     this->op_stack.pop();
            //     this->op_stack.emplace(a + b);
            //     break;
            // }
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
                ZataElem val = this->op_stack.top();
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
                ZataElem val = this->op_stack.top();
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
                ZataElem cond = this->op_stack.top();
                this->op_stack.pop();

                int condition = 2;
                std::shared_ptr<ZataBool> bool_obj_ptr = dynamic_pointer_cast<ZataBool>(cond);
                if(bool_obj_ptr) {
                    condition = bool_obj_ptr->value;
                }else {
                    zata_vm_error_thrower();
                }

                if (condition != 1) {
                    this->pc += offset;
                } else {
                    this->pc += 1;
                }
                break;
            }
            case Opcode::JMP_IF_TRUE: {
                int offset = current_code[this->pc];
                ZataElem cond = this->op_stack.top();
                this->op_stack.pop();

                int condition = 2;
                std::shared_ptr<ZataBool> bool_obj_ptr = dynamic_pointer_cast<ZataBool>(cond);
                if(bool_obj_ptr) {
                    condition = bool_obj_ptr->value;
                }else {
                    zata_vm_error_thrower();
                }

                if (condition == 1) {
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

                ZataElem fn = this->constant_pool[fn_addr];

                if (!std::dynamic_pointer_cast<ZataFunction>(fn)) {
                    zata_vm_error_thrower("CALL opcode: function is not a ZataObject");
                }

                auto fn_ptr = std::dynamic_pointer_cast<ZataFunction>(fn);

                if (!fn_ptr) {
                    zata_vm_error_thrower("CALL opcode: object is not a Function");
                }

                std::vector<int> function_code = fn_ptr->bytecode;

                std::vector<ZataElem> fns_locals(fn_ptr->local_count);
                std::vector<ZataElem> args;

                for(int i = 0; i < arg_count; ++i) {
                    args.push_back(this->op_stack.top());
                    this->op_stack.pop();
                }
                std::ranges::reverse(args);

                for(int i = 0; i < arg_count; ++i) {
                    fns_locals[i] = args[i];
                }

                CallFrame frame{
                    .pc = this->pc,
                    .locals = this->locals,
                    .code = this->code,
                    .return_address = this->pc,
                    .function_name = fn_ptr->function_name,
                    .constant_pool = this->constant_pool
                };
                this->call_stack.push(frame);

                this->locals = fns_locals;
                this->current_function.emplace(fn_ptr);
                this->code = fn_ptr->bytecode;
                this->pc = 0;
                this->constant_pool = fn_ptr->consts;
                break;
            }
            case Opcode::RET: {
                if (!this->call_stack.empty()) {
                    CallFrame frame = this->call_stack.top();
                    this->call_stack.pop();

                    this->code = frame.code;
                    this->pc = frame.return_address;
                    this->locals = frame.locals;
                    this->constant_pool = frame.constant_pool;

                    if (!this->current_function.empty()) {
                        this->current_function.pop();
                    }
                } else {
                    zata_vm_error_thrower("RET opcode: call stack is empty");
                }
                break;
            }
            case Opcode::NEW_OBJ: {
                int class_addr = current_code[this->pc];
                this->pc += 1;

                ZataElem class_obj = this->constant_pool[class_addr];

                std::shared_ptr<ZataClass> class_ptr = std::dynamic_pointer_cast<ZataClass>(class_obj);

                if (!class_ptr) {
                    throw std::runtime_error("NEW_OBJ opcode: object is not a Class");
                }

                std::shared_ptr<ZataInstance> class_instance = std::make_shared<ZataInstance>();;
                class_instance->ref_class = class_ptr;
                class_instance->fields.resize(class_ptr->fields.size());
                this->op_stack.emplace(class_instance);
                break;
            }
            case Opcode::SET_FIELD: {
                int field_addr = current_code[this->pc];
                this->pc += 1;

                ZataElem value = this->op_stack.top();
                this->op_stack.pop();

                ZataElem obj = this->op_stack.top();
                this->op_stack.pop();

                std::shared_ptr<ZataInstance> instance = std::dynamic_pointer_cast<ZataInstance>(obj);
                if (!instance) {
                    zata_vm_error_thrower("SET_FIELD opcode: object is not an Instance");
                }
                instance->fields[field_addr] = value;
                break;
            }
            case Opcode::GET_FIELD: {
                int field_addr = current_code[this->pc];
                this->pc += 1;

                ZataElem obj = this->op_stack.top();
                this->op_stack.pop();

                std::shared_ptr<ZataInstance> instance_ptr = std::dynamic_pointer_cast<ZataInstance>(obj);
                if (instance_ptr) {
                    this->op_stack.emplace(instance_ptr->fields[field_addr]);
                    break;
                }

                // 再尝试转换为ZataClass
                std::shared_ptr<ZataClass> class_ptr = std::dynamic_pointer_cast<ZataClass>(obj);
                if (class_ptr) {
                    this->op_stack.emplace(class_ptr->fields[field_addr]);
                    break;
                }

                zata_vm_error_thrower("GET_FIELD: object is not ZataInstance or ZataClass");
                break;
            }
            case Opcode::CALL_METHOD: {
                int fn_addr = current_code[this->pc];
                int arg_count = current_code[this->pc + 1];
                this->pc += 2;

                ZataElem fn = this->constant_pool[fn_addr];
                std::shared_ptr<ZataFunction> fn_ptr = std::dynamic_pointer_cast<ZataFunction>(fn);

                if (!fn_ptr) {
                    zata_vm_error_thrower("CALL_METHOD opcode: function is not a zata function");
                }

                ZataElem obj = this->op_stack.top();
                this->op_stack.pop();

                std::shared_ptr<ZataInstance> instance = std::dynamic_pointer_cast<ZataInstance>(obj);
                if (!instance) {
                    zata_vm_error_thrower("CALL_METHOD opcode: top of stack is not an object");
                }

                // 处理函数参数
                std::vector<ZataElem> args;
                for (int i = 0; i < arg_count; ++i) {
                    args.push_back(this->op_stack.top());
                    this->op_stack.pop();
                }
                std::ranges::reverse(args);
                std::vector<ZataElem> fns_locals(fn_ptr->local_count);
                fns_locals[0] = obj;

                for (int i = 0; i < arg_count; ++i) {
                    fns_locals[i + 1] = args[i];  // 后续位置存放参数
                }

                // 保存调用帧
                CallFrame frame{
                    .pc = this->pc,
                    .locals = this->locals,
                    .code = this->code,
                    .return_address = this->pc,
                    .function_name = fn_ptr->function_name,
                    .constant_pool = this->constant_pool
                };
                this->call_stack.push(frame);

                this->locals = fns_locals;
                this->current_function.push(fn_ptr);
                this->pc = 0;
                this->code = fn_ptr->bytecode;
                this->constant_pool = fn_ptr->consts;
                break;
            }
            case Opcode::POP: {
                if(!this->op_stack.empty()) {
                    this->op_stack.pop();
                } else {
                    zata_vm_error_thrower("POP opcode: stack underflow");
                }
                break;
            }
            case Opcode::DUP: {
                if(!this->op_stack.empty()) {
                    ZataElem a = this->op_stack.top();
                    this->op_stack.emplace(a);
                } else {
                    zata_vm_error_thrower("DUP opcode: stack underflow");
                }
                break;
            }
            /*case Opcode::ALLOC: {
                // ToDo : Make sure use stack or code
                ZataElem size_item = current_code[this->pc];
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

                memory[addr] = ZataElem{};

                op_stack.emplace(addr);
                break;
            }
            case Opcode::FREE: {
                ZataElem addr_item = op_stack.top();
                op_stack.pop();
                if (!std::holds_alternative<uint64_t>(addr_item)) {
                    throw std::runtime_error("FREE opcode: address must be uint64_t");
                }
                uint64_t addr = std::get<uint64_t>(addr_item);

                // 检查地址是否存在
                if (!memory.contains(addr)) {
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
                ZataElem addr_item = op_stack.top();
                op_stack.pop();
                if (!std::holds_alternative<uint64_t>(addr_item)) {
                    throw std::runtime_error("LOAD_MEM opcode: address must be uint64_t");
                }
                uint64_t addr = std::get<uint64_t>(addr_item);

                // 检查地址是否存在
                if (!memory.contains(addr)) {
                    throw std::runtime_error("LOAD_MEM opcode: invalid address (not allocated)");
                }

                // 将地址中的数据压入栈
                ZataElem value = memory[addr];
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
                ZataElem value = op_stack.top();
                op_stack.pop();
                ZataElem addr_item = op_stack.top();
                op_stack.pop();

                if (!std::holds_alternative<uint64_t>(addr_item)) {
                    throw std::runtime_error("STORE_MEM opcode: address must be uint64_t");
                }
                uint64_t addr = std::get<uint64_t>(addr_item);

                // 检查地址是否已分配（也可允许存储到新地址，视需求而定）
                if (!memory.contains(addr)) {
                    throw std::runtime_error("STORE_MEM opcode: invalid address (not allocated)");
                }

                // 存储数据到内存
                memory[addr] = value;
                break;
            }*/
            case Opcode::HALT: {
                this->running = false;
                break;
            }
            default: {
                zata_vm_error_thrower("Unknown opcode: " + std::to_string(opcode));
            }
            }
        }

        return this->op_stack;
    }
};

#endif // ZATA_VM_H
