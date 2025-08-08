#ifndef OPCODE_H
#define OPCODE_H


namespace Opcode {
    // 算术运算
    // constexpr int CALC = 0x00;
    // constexpr int ADD = 0x01    // 加法 a+b
    // constexpr int SUB = 0x02;   // 减法 a-b
    // constexpr int MUL = 0x03;   // 乘法 a*b
    // constexpr int DIV = 0x04;   // 除法 a/b
    // constexpr int MOD = 0x05;   // 取模 a%b
    // constexpr int NEG = 0x06;   // 取相反数 -a

    // 二进制
    // constexpr int BIT_AND = 0x70; // 按位与
    // constexpr int BIT_OR = 0x71;  // 按位或
    // constexpr int BIT_XOR = 0x72; // 按位异或
    // constexpr int SHL = 0x73;     // 左移
    // constexpr int SHR = 0x74;     // 右移

    // 逻辑运算
    // constexpr int EQ_EQ = 0x10; // 等于 a==b
    // constexpr int NE = 0x11;    // 不等于 a!=b
    // constexpr int LT = 0x12;    // 小于 a<b
    // constexpr int GT = 0x13;    // 大于 a>b
    // constexpr int LE = 0x14;    // 小于等于 a<=b
    // constexpr int GE = 0x15;    // 大于等于 a>=b
    // constexpr int AND = 0x16;   // 逻辑与 a&&b
    // constexpr int OR = 0x17;    // 逻辑或 a||b
    // constexpr int NOT = 0x18;   // 逻辑非 !a

    // 加载/存储指令
    constexpr int LOAD_CONST = 0x20;  // 从常量池加载常量到栈
    constexpr int LOAD_VAR = 0x21;    // 从变量加载值到栈
    constexpr int STORE_VAR = 0x22;   // 从栈存储值到变量
    constexpr int LOAD_GLOBAL = 0x23; // 加载全局变量
    constexpr int STORE_GLOBAL = 0x24;// 存储全局变量
    constexpr int SWAP = 0x25;        // 交换栈顶值
    constexpr int DUP = 0x26;         // 复制栈顶值
    constexpr int PUSH = 0x27;        // 将整数压入栈顶
    constexpr int POP = 0x28;         // 弹出栈顶值并丢弃
    constexpr int LOAD_SLL = 0x29;    // 加载c/c++静态链接库

    // 控制流
    constexpr int JMP = 0x30;         // 无条件跳转
    constexpr int JMP_IF_TRUE = 0x31; // 如果为真跳转
    constexpr int JMP_IF_FALSE = 0x32;// 如果为假跳转
    constexpr int CALL = 0x33;        // 调用函数
    constexpr int RET = 0x34;         // 从函数返回
    constexpr int BRK = 0x35;         // 中断执行
    constexpr int CONT = 0x36;        // 继续执行
    constexpr int NOP = 0x37;         // 占位符

    // 对象操作
    constexpr int NEW_OBJ = 0x40;     // 创建新对象
    constexpr int GET_FIELD = 0x41;   // 获取对象字段
    constexpr int SET_FIELD = 0x42;   // 设置对象字段
    constexpr int CALL_METHOD = 0x43; // 调用对象方法

    // 内存操作
    constexpr int ALLOC = 0x50;       // 分配内存
    constexpr int FREE = 0x51;        // 释放内存
    constexpr int LOAD_MEM = 0x52;    // 从内存加载
    constexpr int STORE_MEM = 0x53;   // 存储到内存

    // 特殊指令
    constexpr int HALT = 0xFF;     // 终止执行
}

#endif // OPCODE_H