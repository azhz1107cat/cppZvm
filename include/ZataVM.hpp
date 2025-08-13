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
#include "utils/SLL_loader.hpp"
#include "vm_deps/VmModels.hpp"

#include "vm_deps/ZvmOpcodes.hpp"

// 虚拟机
class ZataVirtualMachine {
    std::stack<ZataObjectPtr>    op_stack;
    std::stack<CallFrame>        call_stack;
    std::stack<Block>            block_stack;

    std::vector<ZataObjectPtr>   locals;
    std::vector<ZataObjectPtr>   globals;
    std::vector<ZataObjectPtr>   constant_pool;

    std::shared_ptr<ZataModule>  module;
    std::vector<Context>         contexts;
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

    std::stack<ZataObjectPtr> run() {
        CallFrame frame{
            .pc = this->pc,
            .locals = this->locals,
            .return_address = this->pc,
            .name = module->object_name,
        };
        this->call_stack.push(frame);
        this->exec(this->module->code);
        return this->op_stack;
    }

    std::stack<ZataObjectPtr> exec(const std::shared_ptr<ZataCodeObject>& code_object) {
        this->running = true;
        this->locals = code_object->locals;
        this->constant_pool = code_object->consts;
        auto co_code = code_object->co_code;

        while(this->running) {

            if (this->pc >= co_code.size()){
                this->running = false;
                break;
            }

            const int opcode = co_code[this->pc];
            this->pc += 1;

            switch (opcode) {
            case Opcode::B_CALC: {
                int pattern = co_code[this->pc];
                this->pc += 1;
                auto b = this->op_stack.top();
                this->op_stack.pop();
                auto a = this->op_stack.top();
                this->op_stack.pop();

                auto b_ptr = std::dynamic_pointer_cast<ZataBuiltinsClass>(b);
                auto a_ptr = std::dynamic_pointer_cast<ZataBuiltinsClass>(a);
                if (!a_ptr || !b_ptr) {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "B_CALC opcode: can not use on the type which is not a builtins type",
                        .error_code = 0
                    });
                }

                switch (pattern) {
                    case 0:  // add（加法）
                        this->op_stack.emplace(a_ptr->object_type->type_add({a_ptr, b_ptr}));
                        break;
                    case 1:  // sub（减法）
                        this->op_stack.emplace(a_ptr->object_type->type_sub({a_ptr, b_ptr}));
                        break;
                    case 2:  // mul（乘法）
                        this->op_stack.emplace(a_ptr->object_type->type_mul({a_ptr, b_ptr}));
                        break;
                    case 3:  // div（除法）
                        this->op_stack.emplace(a_ptr->object_type->type_div({a_ptr, b_ptr}));
                        break;
                    case 4:  // mod（取模）
                        this->op_stack.emplace(a_ptr->object_type->type_mod({a_ptr, b_ptr}));
                        break;
                    case 5:  // eq（等于）
                        this->op_stack.emplace(a_ptr->object_type->type_eq({a_ptr, b_ptr}));
                        break;
                    case 6:  // weq（弱等于）
                        this->op_stack.emplace(a_ptr->object_type->type_weq({a_ptr, b_ptr}));
                        break;
                    case 7:  // lt（小于）
                        this->op_stack.emplace(a_ptr->object_type->type_lt({a_ptr, b_ptr}));
                        break;
                    case 8:  // gt（大于）
                        this->op_stack.emplace(a_ptr->object_type->type_gt({a_ptr, b_ptr}));
                        break;
                    case 9:  // le（小于等于）
                        this->op_stack.emplace(a_ptr->object_type->type_le({a_ptr, b_ptr}));
                        break;
                    case 10: // ge（大于等于）
                        this->op_stack.emplace(a_ptr->object_type->type_ge({a_ptr, b_ptr}));
                        break;
                    case 11: // bit_and（按位与）
                        this->op_stack.emplace(a_ptr->object_type->type_bit_and({a_ptr, b_ptr}));
                        break;
                    case 12: // bit_or（按位或）
                        this->op_stack.emplace(a_ptr->object_type->type_bit_or({a_ptr, b_ptr}));
                        break;
                    case 13: // bit_xor（按位异或）
                        this->op_stack.emplace(a_ptr->object_type->type_bit_xor({a_ptr, b_ptr}));
                        break;
                    default: {
                        zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "Unknown binary pattern opcode",
                        .error_code = 0
                    });
                    }
                }

                if (nullptr == this->op_stack.top()) {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataTypeError",
                        .message = "<object id="+std::to_string(a_ptr->object_id)+">can not support op "+std::to_string(pattern),
                        .error_code = 0
                    });
                }
                break;
            }
            case Opcode::U_CALC: {
                int pattern = co_code[this->pc];
                this->pc += 1;
                auto a = this->op_stack.top();
                this->op_stack.pop();


                auto a_ptr = std::dynamic_pointer_cast<ZataBuiltinsClass>(a);
                if (!a_ptr) {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "U_CALC opcode: can not use on the type which is not a builtins type",
                        .error_code = 0
                    });
                }

                switch (pattern) {
                    case 0:
                        this->op_stack.emplace(a_ptr->object_type->type_neg({a_ptr}));
                        break;
                    case 1:
                        this->op_stack.emplace(a_ptr->object_type->type_bit_not({a_ptr}));
                        break;
                    default: {
                        zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "Unknown binary pattern opcode",
                        .error_code = 0
                    });
                    }if (nullptr == this->op_stack.top()) {
                        zata_vm_error_thrower(this->call_stack ,ZataError{
                            .name = "ZataTypeError",
                            .message = "<object id="+std::to_string(a->object_id)+">can not support op "+std::to_string(pattern),
                            .error_code = 0
                        });
                    }
                }
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
                    condition = static_cast<int>(bool_obj_ptr->val);
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
                    condition = static_cast<int>(bool_obj_ptr->val);
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

                // Prepare arguments
                std::vector<ZataObjectPtr> fns_locals;
                std::vector<ZataObjectPtr> args;

                for(int i = 0; i < arg_count; ++i) {
                    args.push_back(this->op_stack.top());
                    this->op_stack.pop();
                }
                std::ranges::reverse(args);

                for(int i = 0; i < arg_count; ++i) {
                    fns_locals.push_back(args[i]);
                }

                auto fn_ptr = std::dynamic_pointer_cast<ZataFunction>(fn);

                if (!fn_ptr) {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "CALL opcode: function is not a Zata Callable Object",
                        .error_code = 0
                    });
                }

                // Check is in builtins
                auto it = BuiltinsFunction.find(fn_ptr->object_name);
                if (it != BuiltinsFunction.end()) {
                    this->op_stack.emplace(it->second(args));
                    break;
                }

                fn_ptr->code->locals = fns_locals;
                auto function_code_object = fn_ptr->code;

                CallFrame frame{
                    .pc = this->pc,
                    .locals = this->locals,
                    .return_address = this->pc,
                    .name = fn_ptr->object_name,

                };
                this->call_stack.push(frame);

                this->exec(fn_ptr->code);
                break;
            }
            case Opcode::RET: {
                if (!this->call_stack.empty()) {
                    CallFrame frame = this->call_stack.top();
                    this->call_stack.pop();

                    this->pc = frame.return_address;
                    this->locals = frame.locals;

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
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "NEW_OBJ opcode: object is not a Zata Class",
                        .error_code = 0
                    });
                }

                std::shared_ptr<ZataInstance> class_instance = std::make_shared<ZataInstance>();
                this->exec(class_instance->object_type->type_new->code);
                class_instance->ref_class = class_ptr;
                class_instance->fields = {};
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
                        .message = "SET_ATTR opcode: object is not an Instance",
                        .error_code = 0
                    });
                }

                auto name = instance->names.at(field_addr);
                instance->fields[name] = value;
                break;
            }
            case Opcode::GET_ATTR: {
                int field_addr = co_code[this->pc];
                this->pc += 1;

                ZataObjectPtr obj = this->op_stack.top();
                this->op_stack.pop();

                std::shared_ptr<ZataInstance> instance_ptr = std::dynamic_pointer_cast<ZataInstance>(obj);
                if (instance_ptr) {
                    auto name = instance_ptr->names.at(field_addr);
                    this->op_stack.emplace(instance_ptr->fields[name]);
                    break;
                }

                // 再尝试转换为ZataClass
                std::shared_ptr<ZataClass> class_ptr = std::dynamic_pointer_cast<ZataClass>(obj);
                if (class_ptr) {
                    auto name = class_ptr->names.at(field_addr);
                    this->op_stack.emplace(instance_ptr->fields[name]);
                    break;
                }

                zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "GET_ATTR: object is not ZataInstance or ZataClass",
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
                int fn_addr = co_code[this->pc];
                int arg_count = co_code[this->pc+1];
                this->pc += 2;

                ZataObjectPtr sll_module = this->op_stack.top();
                this->op_stack.pop();

                auto sll_module_ptr = std::dynamic_pointer_cast<ZataModule>(sll_module);

                if (!sll_module_ptr) {
                    zata_vm_error_thrower(this->call_stack ,ZataError{
                        .name = "ZataRunTimeError",
                        .message = "LOAD_SLL opcode: sll module not in consts",
                        .error_code = 0
                    });
                }

                // Prepare arguments
                std::vector<ZataObjectPtr> args;
                for(int i = 0; i < arg_count; ++i) {
                    args.push_back(this->op_stack.top());
                    this->op_stack.pop();
                }
                std::ranges::reverse(args);

                auto functions  = load_sll(sll_module_ptr->module_path, sll_module_ptr->exports);
                auto fn_name = sll_module_ptr->exports.at(fn_addr);
                auto result = functions[fn_name](args);
                this->op_stack.emplace(result);
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
