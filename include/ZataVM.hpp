#ifndef ZATA_VM_H
#define ZATA_VM_H

#include <algorithm>
#include <vector>
#include <stack>
#include <string>
#include <memory>
#include <unordered_map>

#include "models/Errors.hpp"
#include "models/Objects.hpp"
#include "builtins/builtins_functions.hpp"

#include "vm_deps/ZvmOpcodes.hpp"

// 虚拟机
class ZataVirtualMachine {
    std::stack<ZataObjectPtr>    op_stack;
    std::stack<CallFrame>        call_stack;
    std::stack<Block>            block_stack;

    std::vector<ZataObjectPtr>  locals;
    std::vector<ZataObjectPtr>  globals;
    std::vector<ZataObjectPtr>  constant_pool;

    std::shared_ptr<ZataModule> module;
    std::vector<Context>  contexts;
    int pc = 0;
    bool running = false;

public:
    ZataVirtualMachine(
        const std::shared_ptr<ZataModule>& _module,
        const std::vector<Context>& _contexts)
    {
        this->module = _module;
        this->contexts = _contexts;
    }

    void exec_module() {

    }

    std::stack<ZataObjectPtr> run(const std::vector<int>& co_code) {
        this->running = true;
        while(this->running) {

            if (this->pc >= co_code.size()){
                this->running = false;
                break;
            }

            const int opcode = co_code[this->pc];
            this->pc += 1;

            switch (opcode) {
            case Opcode::B_CALC: {
                auto b = this->op_stack.top();
                this->op_stack.pop();
                auto a = this->op_stack.top();
                this->op_stack.pop();
                break;
            }
            case Opcode::U_CALC: {

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
                int const_addr = co_code[this->pc];
                this->pc += 1;
                this->op_stack.emplace(this->constant_pool[const_addr]);
                break;
            }
            case Opcode::LOAD_LOCAL: {
                int var_addr = co_code[this->pc];
                this->pc += 1;
                this->op_stack.emplace(this->locals[var_addr]);
                break;
            }
            case Opcode::STORE_LOCAL: {
                int var_addr = co_code[this->pc];
                this->pc += 1;
                ZataObjectPtr val = this->op_stack.top();
                this->op_stack.pop();
                this->locals[var_addr] = val;
                break;
            }
            case Opcode::LOAD_GLOBAL: {
                int var_addr = co_code[this->pc];
                this->pc += 1;
                this->op_stack.emplace(this->globals[var_addr]);
                break;
            }
            case Opcode::STORE_GLOBAL: {
                int var_addr = co_code[this->pc];
                this->pc += 1;
                ZataObjectPtr val = this->op_stack.top();
                this->op_stack.pop();
                this->globals[var_addr] = val;
                break;
            }
            case Opcode::JMP: {
                int offset = co_code[this->pc];
                this->pc += offset;
                break;
            }
            case Opcode::JMP_IF_FALSE: {
                int offset = co_code[this->pc];
                ZataObjectPtr cond = this->op_stack.top();
                this->op_stack.pop();

                int condition = 2;
                std::shared_ptr<ZataState> bool_obj_ptr = dynamic_pointer_cast<ZataState>(cond);
                if(bool_obj_ptr) {
                    condition = bool_obj_ptr->val;
                }else {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "Top of the stack is not a bool object",
                        .error_code = 0
                    });
                }

                if (condition != 1) {
                    this->pc += offset;
                } else {
                    this->pc += 1;
                }
                break;
            }
            case Opcode::JMP_IF_TRUE: {
                int offset = co_code[this->pc];
                ZataObjectPtr cond = this->op_stack.top();
                this->op_stack.pop();

                int condition = 2;
                std::shared_ptr<ZataState> bool_obj_ptr = dynamic_pointer_cast<ZataState>(cond);
                if(bool_obj_ptr) {
                    condition = bool_obj_ptr->val;
                }else {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "Top of the stack is not a bool object",
                        .error_code = 0
                    });
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
                int arg_count = co_code[this->pc];
                this->pc += 1;

                ZataObjectPtr fn = this->op_stack.top();
                this->op_stack.pop();

                if (!std::dynamic_pointer_cast<ZataFunction>(fn)) {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "CALL opcode: function is not a ZataObject",
                        .error_code = 0
                    });
                }

                auto fn_ptr = std::dynamic_pointer_cast<ZataFunction>(fn);

                if (!fn_ptr) {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "CALL opcode: object is not a Function",
                        .error_code = 0
                    });
                }

                std::vector<int> function_code = fn_ptr->bytecode;

                std::vector<ZataObjectPtr> fns_locals(fn_ptr->local_count);
                std::vector<ZataObjectPtr> args;

                for(int i = 0; i < arg_count; ++i) {
                    args.push_back(this->op_stack.top());
                    this->op_stack.pop();
                }
                std::ranges::reverse(args);

                for(int i = 0; i < arg_count; ++i) {
                    fns_locals[i] = args[i];
                }

                if (fn_ptr->function_name == "print"){zata_print(args);break;}
                if (fn_ptr->function_name == "input"){zata_input(args);break;}
                if (fn_ptr->function_name == "now"){zata_now(args);break;}

                CallFrame frame{
                    .pc = this->pc,
                    .locals = this->locals,
                    .return_address = this->pc,
                    .function_name = fn_ptr->function_name,
                    .constant_pool = this->constant_pool,
                    .code = this->code,

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
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "RET opcode: call stack is empty",
                        .error_code = 0
                    });
                }
                break;
            }
            case Opcode::MAKE_INSTANCE: {
                int class_addr = co_code[this->pc];
                this->pc += 1;

                ZataObjectPtr class_obj = this->constant_pool[class_addr];

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
            case Opcode::SET_ATTR: {
                int field_addr = co_code[this->pc];
                this->pc += 1;

                ZataObjectPtr value = this->op_stack.top();
                this->op_stack.pop();

                ZataObjectPtr obj = this->op_stack.top();
                this->op_stack.pop();

                std::shared_ptr<ZataInstance> instance = std::dynamic_pointer_cast<ZataInstance>(obj);
                if (!instance) {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "SET_FIELD opcode: object is not an Instance",
                        .error_code = 0
                    });
                }
                instance->attrs[field_addr] = value;
                break;
            }
            case Opcode::GET_ATTR: {
                int field_addr = co_code[this->pc];
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

                zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "GET_FIELD: object is not ZataInstance or ZataClass",
                        .error_code = 0
                    });
                break;
            }
            case Opcode::POP: {
                if(!this->op_stack.empty()) {
                    this->op_stack.pop();
                } else {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "POP opcode: stack underflow",
                        .error_code = 0
                    });
                }
                break;
            }
            case Opcode::DUP: {
                if(!this->op_stack.empty()) {
                    ZataObjectPtr a = this->op_stack.top();
                    this->op_stack.emplace(a);
                } else {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "DUP opcode: stack underflow",
                        .error_code = 0
                    });
                }
                break;
            }
            case Opcode::LOAD_SLL: {

            }
            case Opcode::HALT: {
                this->running = false;
                break;
            }
            default: {
                zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "Unknown opcode: " + std::to_string(opcode),
                        .error_code = 0
                    });
            }
            }
        }

        return this->op_stack;
    }
};

#endif // ZATA_VM_H
