#ifndef ZVM_OPCODES_H
#define ZVM_OPCODES_H

enum class Opcode : unsigned char {
    // 算术运算
    ADD = 0x01,   // 加法 a+b
    SUB = 0x02,   // 减法 a-b
    MUL = 0x03,   // 乘法 a*b
    DIV = 0x04,   // 除法 a/b
    MOD = 0x05,   // 取模 a%b
    NEG = 0x06,   // 取相反数 -a

    // 逻辑运算
    EQ_EQ = 0x10, // 等于 a==b
    NE = 0x11,    // 不等于 a!=b
    LT = 0x12,    // 小于 a<b
    GT = 0x13,    // 大于 a>b
    LE = 0x14,    // 小于等于 a<=b
    GE = 0x15,    // 大于等于 a>=b
    AND = 0x16,   // 逻辑与 a&&b
    OR = 0x17,    // 逻辑或 a||b
    NOT = 0x18,   // 逻辑非 !a

    // 加载/存储指令
    LOAD_CONST = 0x20,  // 从常量池加载常量到栈
    LOAD_VAR = 0x21,    // 从变量加载值到栈
    STORE_VAR = 0x22,   // 从栈存储值到变量
    LOAD_GLOBAL = 0x23, // 加载全局变量
    STORE_GLOBAL = 0x24,// 存储全局变量
    SWAP = 0x25,        // 交换栈顶值
    DUP = 0x26,         // 复制栈顶值
    PUSH = 0x27,        // 将整数(不是int对象,是纯i16数字)压入栈顶
    POP = 0x28,         // 弹出栈顶值并丢弃

    // 控制流
    JMP = 0x30,         // 无条件跳转
    JMP_IF_TRUE = 0x31, // 如果为真跳转
    JMP_IF_FALSE = 0x32,// 如果为假跳转
    CALL = 0x33,        // 调用函数
    RET = 0x34,         // 从函数返回
    BRK = 0x35,         // 中断执行
    CONT = 0x36,        // 继续执行
    NOP = 0x37,         // 占位符

    // 对象操作
    NEW_OBJ = 0x40,     // 创建新对象
    GET_FIELD = 0x41,   // 获取对象字段
    SET_FIELD = 0x42,   // 设置对象字段
    CALL_METHOD = 0x43, // 调用对象方法

    // 内存操作
    ALLOC = 0x50,       // 分配内存
    FREE = 0x51,        // 释放内存
    LOAD_MEM = 0x52,    // 从内存加载
    STORE_MEM = 0x53,   // 存储到内存

    // IO
    WRITE = 0x60,
    READ = 0x61,

    // 二进制
    BIT_AND = 0x70, // 按位与
    BIT_OR = 0x71,  // 按位或
    BIT_XOR = 0x72, // 按位异或
    SHL = 0x73,     // 左移
    SHR = 0x74,     // 右移

    // 特殊指令
    HALT = 0xFF     // 终止执行
};

#endif // ZVM_OPCODES_H
