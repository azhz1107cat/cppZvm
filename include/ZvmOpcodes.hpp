#ifndef OPCODE_H
#define OPCODE_H


namespace Opcode {

    constexpr int B_CALC = 0x01;  // 通用二元运算   <pattern>
    // 0 -> add
    // 1 -> sub
    // 2 -> mul
    // 3 -> div
    // 4 -> mod
    // 5 -> eq
    // 6 -> weq
    // 7 -> lt
    // 8 -> gt
    // 9 -> le
    // 10 -> ge
    // 11 -> and
    // 12 -> or
    // 13 -> not
    // 14 -> bit_and
    // 15 -> bit_or
    // 16 -> bit_xor

    constexpr int U_CALC = 0x02;  // 通用一元运算   <pattern>
    // 0 -> neg
    // 1 -> not
    // 2 -> bit_not

    // 加载/存储指令
    constexpr int LOAD_CONST = 0x20;    // 从常量池加载常量到栈 <index in consts>
    constexpr int LOAD_LOCAL = 0x21;    // 从变量加载值到栈    <index in names>
    constexpr int STORE_LOCAL = 0x22;   // 从栈存储值到变量    <index in names>
    constexpr int LOAD_GLOBAL = 0x23;   // 加载全局变量       <index in names>
    constexpr int STORE_GLOBAL = 0x24;  // 存储全局变量       <index in names>
    constexpr int LOAD_CLOSURE = 0x25;  // 加载闭包变量       <index in names>
    constexpr int SWAP = 0x26;          // 交换栈顶值
    constexpr int DUP = 0x27;           // 复制栈顶值
    constexpr int POP = 0x28;           // 弹出栈顶值并丢弃
    constexpr int LOAD_SLL = 0x29;      // 加载c/c++静态链接库 <index in consts>

    // 控制流
    constexpr int JMP = 0x30;         // 无条件跳转    <offset>
    constexpr int JMP_IF_TRUE = 0x31; // 如果为真跳转  <offset>
    constexpr int JMP_IF_FALSE = 0x32;// 如果为假跳转  <offset>
    constexpr int CALL = 0x33;        // 调用函数     <arg_count>
    constexpr int RET = 0x34;         // 从函数返回
    constexpr int NOP = 0x35;         // 占位符

    // 对象操作
    constexpr int MAKE_INSTANCE = 0x40;     // 创建新对象  <index in consts>
    constexpr int GET_ATTR = 0x41;   // 获取对象属性  <index in names>
    constexpr int SET_ATTR = 0x42;   // 设置对象属性  <index in names>
    constexpr int GET_ITER  = 0x43;  // 获取迭代器
    constexpr int NEXT_ITER  = 0x44; // 迭代器自增

    // 内存操作
    constexpr int ALLOC = 0x50;       // 分配内存
    constexpr int FREE = 0x51;        // 释放内存
    constexpr int LOAD_MEM = 0x52;    // 从内存加载
    constexpr int STORE_MEM = 0x53;   // 存储到内存

    // 异常处理
    constexpr int SETUP_FINALLY = 0x54;     // 设置finally块 <offset>
    constexpr int TRY_CATCH_START = 0x55;   // 进入try-catch块 <offset>
    constexpr int TRY_FINALLY_START = 0x56;     // 进入try-finally块 <offset>
    constexpr int SETUP_CATCH = 0x56;       // 进入catch块 <index in consts>
    constexpr int END_FINALLY = 0x57;       // 结束finally块
    constexpr int BS_POP = 0x58;            // 弹出块栈顶值并丢弃  BS -> BlockStack
    constexpr int THROW = 0x59;             // 抛出栈顶异常

    // 专门指令(用于优化)
    constexpr int LIST_APPEND = 0x60;
    constexpr int LIST_EXTEND = 0x61;
    constexpr int DICT_SETITEM = 0x62;
    constexpr int DICT_UPDATE = 0x63;
    constexpr int GET_LEN = 0x64;
    constexpr int IS_INSTANCE = 0x65;

    // 特殊指令
    constexpr int HALT = 0xFF;     // 终止执行
}

#endif // OPCODE_H