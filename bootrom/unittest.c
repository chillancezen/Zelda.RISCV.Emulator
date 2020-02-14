/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <unittest.h>


static void
test_arithmetic_instruction(void)
{
    // add Instruction
    TEST_R_R_R(add, 0x00000000, 0x00000000, 0x00000000);
    TEST_R_R_R(add, 0x00000002, 0x00000001, 0x00000001);
    TEST_R_R_R(add, 0x0000000a, 0x00000003, 0x00000007);
    TEST_R_R_R(add, 0xffff8000, 0x00000000, 0xffff8000);
    TEST_R_R_R(add, 0x80000000, 0x80000000, 0x00000000);
    TEST_R_R_R(add, 0x7fff8000, 0x80000000, 0xffff8000);

    TEST_R_R_R(add, 0x00007fff, 0x00000000, 0x00007fff );
    TEST_R_R_R(add, 0x7fffffff, 0x7fffffff, 0x00000000 );
    TEST_R_R_R(add, 0x80007ffe, 0x7fffffff, 0x00007fff );

    TEST_R_R_R(add, 0x80007fff, 0x80000000, 0x00007fff );
    TEST_R_R_R(add, 0x7fff7fff, 0x7fffffff, 0xffff8000 );

    TEST_R_R_R(add, 0xffffffff, 0x00000000, 0xffffffff );
    TEST_R_R_R(add, 0x00000000, 0xffffffff, 0x00000001 );
    TEST_R_R_R(add, 0xfffffffe, 0xffffffff, 0xffffffff );

    TEST_R_R_R(add, 0x80000000, 0x00000001, 0x7fffffff );
    // and instruction
    TEST_R_R_R(and, 0x0f000f00, 0xff00ff00, 0x0f0f0f0f);
    TEST_R_R_R(and, 0x00f000f0, 0x0ff00ff0, 0xf0f0f0f0);
    TEST_R_R_R(and, 0x000f000f, 0x00ff00ff, 0x0f0f0f0f);
    TEST_R_R_R(and, 0xf000f000, 0xf00ff00f, 0xf0f0f0f0);
    //or instruction
    TEST_R_R_R(or, 0xff0fff0f, 0xff00ff00, 0x0f0f0f0f);
    TEST_R_R_R(or, 0xfff0fff0, 0x0ff00ff0, 0xf0f0f0f0);
    TEST_R_R_R(or, 0x0fff0fff, 0x00ff00ff, 0x0f0f0f0f);
    TEST_R_R_R(or, 0xf0fff0ff, 0xf00ff00f, 0xf0f0f0f0);
    // sll instruction
    TEST_R_R_R(sll, 0x00000001, 0x00000001, 0  );
    TEST_R_R_R(sll, 0x00000002, 0x00000001, 1  );
    TEST_R_R_R(sll, 0x00000080, 0x00000001, 7  );
    TEST_R_R_R(sll, 0x00004000, 0x00000001, 14 );
    TEST_R_R_R(sll, 0x80000000, 0x00000001, 31 );

    TEST_R_R_R(sll, 0xffffffff, 0xffffffff, 0  );
    TEST_R_R_R(sll, 0xfffffffe, 0xffffffff, 1  );
    TEST_R_R_R(sll, 0xffffff80, 0xffffffff, 7  );
    TEST_R_R_R(sll, 0xffffc000, 0xffffffff, 14 );
    TEST_R_R_R(sll, 0x80000000, 0xffffffff, 31 );

    TEST_R_R_R(sll, 0x21212121, 0x21212121, 0  );
    TEST_R_R_R(sll, 0x42424242, 0x21212121, 1  );
    TEST_R_R_R(sll, 0x90909080, 0x21212121, 7  );
    TEST_R_R_R(sll, 0x48484000, 0x21212121, 14 );
    TEST_R_R_R(sll, 0x80000000, 0x21212121, 31 );

    TEST_R_R_R(sll, 0x21212121, 0x21212121, 0xffffffc0 );
    TEST_R_R_R(sll, 0x42424242, 0x21212121, 0xffffffc1 );
    TEST_R_R_R(sll, 0x90909080, 0x21212121, 0xffffffc7 );
    TEST_R_R_R(sll, 0x48484000, 0x21212121, 0xffffffce );
    // slt instruction
    TEST_R_R_R(slt, 0, 0x00000000, 0x00000000 );
    TEST_R_R_R(slt, 0, 0x00000001, 0x00000001 );
    TEST_R_R_R(slt, 1, 0x00000003, 0x00000007 );
    TEST_R_R_R(slt, 0, 0x00000007, 0x00000003 );

    TEST_R_R_R(slt, 0, 0x00000000, 0xffff8000 );
    TEST_R_R_R(slt, 1, 0x80000000, 0x00000000 );
    TEST_R_R_R(slt, 1, 0x80000000, 0xffff8000 );

    TEST_R_R_R(slt, 1, 0x00000000, 0x00007fff );
    TEST_R_R_R(slt, 0, 0x7fffffff, 0x00000000 );
    TEST_R_R_R(slt, 0, 0x7fffffff, 0x00007fff );

    TEST_R_R_R(slt, 1, 0x80000000, 0x00007fff );
    TEST_R_R_R(slt, 0, 0x7fffffff, 0xffff8000 );

    TEST_R_R_R(slt, 0, 0x00000000, 0xffffffff );
    TEST_R_R_R(slt, 1, 0xffffffff, 0x00000001 );
    TEST_R_R_R(slt, 0, 0xffffffff, 0xffffffff );
    // sltu instruction
    TEST_R_R_R(sltu, 0, 0x00000000, 0x00000000 );
    TEST_R_R_R(sltu, 0, 0x00000001, 0x00000001 );
    TEST_R_R_R(sltu, 1, 0x00000003, 0x00000007 );
    TEST_R_R_R(sltu, 0, 0x00000007, 0x00000003 );

    TEST_R_R_R(sltu, 1, 0x00000000, 0xffff8000 );
    TEST_R_R_R(sltu, 0, 0x80000000, 0x00000000 );
    TEST_R_R_R(sltu, 1, 0x80000000, 0xffff8000 );

    TEST_R_R_R(sltu, 1, 0x00000000, 0x00007fff );
    TEST_R_R_R(sltu, 0, 0x7fffffff, 0x00000000 );
    TEST_R_R_R(sltu, 0, 0x7fffffff, 0x00007fff );

    TEST_R_R_R(sltu, 0, 0x80000000, 0x00007fff );
    TEST_R_R_R(sltu, 1, 0x7fffffff, 0xffff8000 );

    TEST_R_R_R(sltu, 1, 0x00000000, 0xffffffff );
    TEST_R_R_R(sltu, 0, 0xffffffff, 0x00000001 );
    TEST_R_R_R(sltu, 0, 0xffffffff, 0xffffffff );
    // sra instruction
    TEST_R_R_R(sra, 0x80000000, 0x80000000, 0  );
    TEST_R_R_R(sra, 0xc0000000, 0x80000000, 1  );
    TEST_R_R_R(sra, 0xff000000, 0x80000000, 7  );
    TEST_R_R_R(sra, 0xfffe0000, 0x80000000, 14 );
    TEST_R_R_R(sra, 0xffffffff, 0x80000001, 31 );

    TEST_R_R_R(sra, 0x7fffffff, 0x7fffffff, 0  );
    TEST_R_R_R(sra, 0x3fffffff, 0x7fffffff, 1  );
    TEST_R_R_R(sra, 0x00ffffff, 0x7fffffff, 7  );
    TEST_R_R_R(sra, 0x0001ffff, 0x7fffffff, 14 );
    TEST_R_R_R(sra, 0x00000000, 0x7fffffff, 31 );

    TEST_R_R_R(sra, 0x81818181, 0x81818181, 0  );
    TEST_R_R_R(sra, 0xc0c0c0c0, 0x81818181, 1  );
    TEST_R_R_R(sra, 0xff030303, 0x81818181, 7  );
    TEST_R_R_R(sra, 0xfffe0606, 0x81818181, 14 );
    TEST_R_R_R(sra, 0xffffffff, 0x81818181, 31 );


    TEST_R_R_R(sra, 0x81818181, 0x81818181, 0xffffffc0 );
    TEST_R_R_R(sra, 0xc0c0c0c0, 0x81818181, 0xffffffc1 );
    TEST_R_R_R(sra, 0xff030303, 0x81818181, 0xffffffc7 );
    TEST_R_R_R(sra, 0xfffe0606, 0x81818181, 0xffffffce );
    TEST_R_R_R(sra, 0xffffffff, 0x81818181, 0xffffffff );
    // srl instruction
    TEST_R_R_R(srl, 0x80000000, 0x80000000, 0  );
    TEST_R_R_R(srl, 0x40000000, 0x80000000, 1  );
    TEST_R_R_R(srl, 0x1000000, 0x80000000, 7  );
    TEST_R_R_R(srl, 0x20000, 0x80000000, 14 );
    TEST_R_R_R(srl, 0x1, 0x80000001, 31 );

    TEST_R_R_R(srl, 0xffffffff, 0xffffffff, 0  );
    TEST_R_R_R(srl, 0x7FFFFFFF, 0xffffffff, 1  );
    TEST_R_R_R(srl, 0x1FFFFFF, 0xffffffff, 7  );
    TEST_R_R_R(srl, 0x3FFFF, 0xffffffff, 14 );
    TEST_R_R_R(srl, 0x1, 0xffffffff, 31 );

    TEST_R_R_R(srl, 0x21212121, 0x21212121, 0  );
    TEST_R_R_R(srl, 0x10909090, 0x21212121, 1  );
    TEST_R_R_R(srl, 0x424242, 0x21212121, 7  );
    TEST_R_R_R(srl, 0x8484, 0x21212121, 14 );
    TEST_R_R_R(srl, 0x0, 0x21212121, 31 );

    TEST_R_R_R(srl, 0x21212121, 0x21212121, 0xffffffc0 );
    TEST_R_R_R(srl, 0x10909090, 0x21212121, 0xffffffc1 );
    TEST_R_R_R(srl, 0x00424242, 0x21212121, 0xffffffc7 );
    TEST_R_R_R(srl, 0x00008484, 0x21212121, 0xffffffce );
    TEST_R_R_R(srl, 0x00000000, 0x21212121, 0xffffffff );
    // xor instruction
    TEST_R_R_R(xor, 0xf00ff00f, 0xff00ff00, 0x0f0f0f0f );
    TEST_R_R_R(xor, 0xff00ff00, 0x0ff00ff0, 0xf0f0f0f0 );
    TEST_R_R_R(xor, 0x0ff00ff0, 0x00ff00ff, 0x0f0f0f0f );
    TEST_R_R_R(xor, 0x00ff00ff, 0xf00ff00f, 0xf0f0f0f0 );
    // sub instruction
    TEST_R_R_R(sub, 0x00000000, 0x00000000, 0x00000000 );
    TEST_R_R_R(sub, 0x00000000, 0x00000001, 0x00000001 );
    TEST_R_R_R(sub, 0xfffffffc, 0x00000003, 0x00000007 );

    TEST_R_R_R(sub, 0x00008000, 0x00000000, 0xffff8000 );
    TEST_R_R_R(sub, 0x80000000, 0x80000000, 0x00000000 );
    TEST_R_R_R(sub, 0x80008000, 0x80000000, 0xffff8000 );

    TEST_R_R_R(sub, 0xffff8001, 0x00000000, 0x00007fff );
    TEST_R_R_R(sub, 0x7fffffff, 0x7fffffff, 0x00000000 );
    TEST_R_R_R(sub, 0x7fff8000, 0x7fffffff, 0x00007fff );

    TEST_R_R_R(sub, 0x7fff8001, 0x80000000, 0x00007fff );
    TEST_R_R_R(sub, 0x80007fff, 0x7fffffff, 0xffff8000 );

    TEST_R_R_R(sub, 0x00000001, 0x00000000, 0xffffffff );
    TEST_R_R_R(sub, 0xfffffffe, 0xffffffff, 0x00000001 );
    TEST_R_R_R(sub, 0x00000000, 0xffffffff, 0xffffffff );
}

void
test_arithmetic_immediate_instruction(void)
{
    //andi instruction
    TEST_R_R_I(addi, 0x00000000, 0x00000000, 0x000 );
    TEST_R_R_I(addi, 0x00000002, 0x00000001, 0x001 );
    TEST_R_R_I(addi, 0x0000000a, 0x00000003, 0x007 );

    TEST_R_R_I(addi, 0xfffff800, 0x00000000, -0x800 );
    TEST_R_R_I(addi, 0x80000000, 0x80000000, 0x000 );
    TEST_R_R_I(addi, 0x7ffff800, 0x80000000, -0x800 );

    TEST_R_R_I(addi, 0x000007ff, 0x00000000, 0x7ff );
    TEST_R_R_I(addi, 0x7fffffff, 0x7fffffff, 0x000 );
    TEST_R_R_I(addi, 0x800007fe, 0x7fffffff, 0x7ff );

    TEST_R_R_I(addi, 0x800007ff, 0x80000000, 0x7ff );
    TEST_R_R_I(addi, 0x7ffff7ff, 0x7fffffff, -0x800 );

    TEST_R_R_I(addi, 0xffffffff, 0x00000000, -0x1 );
    TEST_R_R_I(addi, 0x00000000, 0xffffffff, 0x001 );
    TEST_R_R_I(addi, 0xfffffffe, 0xffffffff, -0x1 );

    TEST_R_R_I(addi, 0x80000000, 0x7fffffff, 0x001 );

    // andi instruction
    TEST_R_R_I(andi, 0xff00ff00, 0xff00ff00, -0xf1 );
    TEST_R_R_I(andi, 0x000000f0, 0x0ff00ff0, 0x0f0 );
    TEST_R_R_I(andi, 0x0000000f, 0x00ff00ff, 0x70f );
    TEST_R_R_I(andi, 0x00000000, 0xf00ff00f, 0x0f0 );
    // ori instruction
    TEST_R_R_I(ori, 0xffffff0f, 0xff00ff00, -0xf1 );
    TEST_R_R_I(ori, 0x0ff00ff0, 0x0ff00ff0, 0x0f0 );
    TEST_R_R_I(ori, 0x00ff07ff, 0x00ff00ff, 0x70f );
    TEST_R_R_I(ori, 0xf00ff0ff, 0xf00ff00f, 0x0f0 );
    //slli instruction
    TEST_R_R_I(slli, 0x00000001, 0x00000001, 0  );
    TEST_R_R_I(slli, 0x00000002, 0x00000001, 1  );
    TEST_R_R_I(slli, 0x00000080, 0x00000001, 7  );
    TEST_R_R_I(slli, 0x00004000, 0x00000001, 14 );
    TEST_R_R_I(slli, 0x80000000, 0x00000001, 31 );

    TEST_R_R_I(slli, 0xffffffff, 0xffffffff, 0  );
    TEST_R_R_I(slli, 0xfffffffe, 0xffffffff, 1  );
    TEST_R_R_I(slli, 0xffffff80, 0xffffffff, 7  );
    TEST_R_R_I(slli, 0xffffc000, 0xffffffff, 14 );
    TEST_R_R_I(slli, 0x80000000, 0xffffffff, 31 );

    TEST_R_R_I(slli, 0x21212121, 0x21212121, 0  );
    TEST_R_R_I(slli, 0x42424242, 0x21212121, 1  );
    TEST_R_R_I(slli, 0x90909080, 0x21212121, 7  );
    TEST_R_R_I(slli, 0x48484000, 0x21212121, 14 );
    TEST_R_R_I(slli, 0x80000000, 0x21212121, 31 );
    //slti instruction
    TEST_R_R_I(slti, 0, 0x00000000, 0x000 );
    TEST_R_R_I(slti, 0, 0x00000001, 0x001 );
    TEST_R_R_I(slti, 1, 0x00000003, 0x007 );
    TEST_R_R_I(slti, 0, 0x00000007, 0x003 );

    TEST_R_R_I(slti, 0, 0x00000000, -0x800 );
    TEST_R_R_I(slti, 1, 0x80000000, 0x000 );
    TEST_R_R_I(slti, 1, 0x80000000, -0x800 );

    TEST_R_R_I(slti, 1, 0x00000000, 0x7ff );
    TEST_R_R_I(slti, 0, 0x7fffffff, 0x000 );
    TEST_R_R_I(slti, 0, 0x7fffffff, 0x7ff );

    TEST_R_R_I(slti, 1, 0x80000000, 0x7ff );
    TEST_R_R_I(slti, 0, 0x7fffffff, -0x800 );

    TEST_R_R_I(slti, 0, 0x00000000, -0x1 );
    TEST_R_R_I(slti, 1, 0xffffffff, 0x001 );
    TEST_R_R_I(slti, 0, 0xffffffff, -0x1 );
    //sltiu instruction
    TEST_R_R_I(sltiu, 0, 0x00000000, 0x000 );
    TEST_R_R_I(sltiu, 0, 0x00000001, 0x001 );
    TEST_R_R_I(sltiu, 1, 0x00000003, 0x007 );
    TEST_R_R_I(sltiu, 0, 0x00000007, 0x003 );

    TEST_R_R_I(sltiu, 1, 0x00000000, -0x800 );
    TEST_R_R_I(sltiu, 0, 0x80000000, 0x000 );
    TEST_R_R_I(sltiu, 1, 0x80000000, -0x800 );

    TEST_R_R_I(sltiu, 1, 0x00000000, 0x7ff );
    TEST_R_R_I(sltiu, 0, 0x7fffffff, 0x000 );
    TEST_R_R_I(sltiu, 0, 0x7fffffff, 0x7ff );

    TEST_R_R_I(sltiu, 0, 0x80000000, 0x7ff );
    TEST_R_R_I(sltiu, 1, 0x7fffffff, -0x800 );

    TEST_R_R_I(sltiu, 1, 0x00000000, -0x1 );
    TEST_R_R_I(sltiu, 0, 0xffffffff, 0x001 );
    TEST_R_R_I(sltiu, 0, 0xffffffff, -0x1 );

    //srai instruction
    TEST_R_R_I(srai, 0x00000000, 0x00000000, 0  );
    TEST_R_R_I(srai, 0xc0000000, 0x80000000, 1  );
    TEST_R_R_I(srai, 0xff000000, 0x80000000, 7  );
    TEST_R_R_I(srai, 0xfffe0000, 0x80000000, 14 );
    TEST_R_R_I(srai, 0xffffffff, 0x80000001, 31 );

    TEST_R_R_I(srai, 0x7fffffff, 0x7fffffff, 0  );
    TEST_R_R_I(srai, 0x3fffffff, 0x7fffffff, 1  );
    TEST_R_R_I(srai, 0x00ffffff, 0x7fffffff, 7  );
    TEST_R_R_I(srai, 0x0001ffff, 0x7fffffff, 14 );
    TEST_R_R_I(srai, 0x00000000, 0x7fffffff, 31 );

    TEST_R_R_I(srai, 0x81818181, 0x81818181, 0  );
    TEST_R_R_I(srai, 0xc0c0c0c0, 0x81818181, 1  );
    TEST_R_R_I(srai, 0xff030303, 0x81818181, 7  );
    TEST_R_R_I(srai, 0xfffe0606, 0x81818181, 14 );
    TEST_R_R_I(srai, 0xffffffff, 0x81818181, 31 );
    // srli instruction
    TEST_R_R_I(srli, 0x80000000, 0x80000000, 0  );
    TEST_R_R_I(srli, 0x40000000, 0x80000000, 1  );
    TEST_R_R_I(srli, 0x1000000, 0x80000000, 7  );
    TEST_R_R_I(srli, 0x20000, 0x80000000, 14 );
    TEST_R_R_I(srli, 0x1, 0x80000001, 31 );

    TEST_R_R_I(srli, 0xffffffff, 0xffffffff, 0  );
    TEST_R_R_I(srli, 0x7FFFFFFF, 0xffffffff, 1  );
    TEST_R_R_I(srli, 0x1FFFFFF, 0xffffffff, 7  );
    TEST_R_R_I(srli, 0x3FFFF, 0xffffffff, 14 );
    TEST_R_R_I(srli, 0x1, 0xffffffff, 31 );

    TEST_R_R_I(srli, 0x21212121, 0x21212121, 0  );
    TEST_R_R_I(srli, 0x10909090, 0x21212121, 1  );
    TEST_R_R_I(srli, 0x424242, 0x21212121, 7  );
    TEST_R_R_I(srli, 0x8484, 0x21212121, 14 );
    TEST_R_R_I(srli, 0x0, 0x21212121, 31 );
    // xori instruction
    TEST_R_R_I(xori, 0xff00f00f, 0x00ff0f00, -0xf1 );
    TEST_R_R_I(xori, 0x0ff00f00, 0x0ff00ff0, 0x0f0 );
    TEST_R_R_I(xori, 0x00ff0ff0, 0x00ff08ff, 0x70f );
    TEST_R_R_I(xori, 0xf00ff0ff, 0xf00ff00f, 0x0f0 );
}

#define _(cond) {                                                              \
    if (!(cond)) {                                                             \
        goto fail;                                                             \
    }                                                                          \
}


static void
test_memory_operation(void)
{

    static uint8_t chunk[8192];
    int idx = 0;
    int32_t read_value = 0;
    // byte
    for (idx = 0; idx < 8192; idx++) {
        SB(&chunk[idx], 0xfe);
        LB(&chunk[idx], read_value);
        _(read_value == 0xfffffffe);
        LBU(&chunk[idx], read_value);
        _(read_value == 0xfe);
    }
    // half word
    for (idx = 0; idx < 8192; idx += 2) {
        SH(&chunk[idx], 0xdeadbeef);
        LBU(&chunk[idx], read_value);
        _(read_value == 0xef);
        LB(&chunk[idx], read_value);
        _(read_value == 0xffffffef);
        LBU(&chunk[idx + 1], read_value);
        _(read_value == 0xbe);
        LB(&chunk[idx + 1], read_value);
        _(read_value == 0xffffffbe);
        LH(&chunk[idx], read_value);
        _(read_value == 0xffffbeef);
        LHU(&chunk[idx], read_value);
        _(read_value == 0xbeef);
    }
    // word
    for (idx = 0; idx < 8192; idx += 4) {
        SW(&chunk[idx], 0xdeadbeef);
        LBU(&chunk[idx], read_value);
        _(read_value == 0xef);
        LBU(&chunk[idx + 1], read_value);
        _(read_value == 0xbe);
        LBU(&chunk[idx + 2], read_value);
        _(read_value == 0xad);
        LBU(&chunk[idx + 3], read_value);
        _(read_value == 0xde);

        LB(&chunk[idx], read_value);
        _(read_value == 0xffffffef);
        LB(&chunk[idx + 1], read_value);
        _(read_value == 0xffffffbe);
        LB(&chunk[idx + 2], read_value);
        _(read_value == 0xffffffad);
        LB(&chunk[idx + 3], read_value);
        _(read_value == 0xffffffde);

        LHU(&chunk[idx], read_value);
        _(read_value == 0xbeef);
        LHU(&chunk[idx + 2], read_value);
        _(read_value == 0xdead);
      
        LH(&chunk[idx], read_value);
        _(read_value == 0xffffbeef);
        LH(&chunk[idx + 2], read_value);
        _(read_value == 0xffffdead);

        LH(&chunk[idx + 1], read_value);
        _(read_value == 0xffffadbe);
        LHU(&chunk[idx + 1], read_value);
        _(read_value == 0xadbe);

        LW(&chunk[idx], read_value);
        _(read_value == 0xdeadbeef);

    }
    printk(ANSI_COLOR_GREEN"PASS"ANSI_COLOR_RESET" memory operations\n");
    return;
    fail:
        printk(ANSI_COLOR_RED"FAIL"ANSI_COLOR_RESET" memory operations\n");
}

#undef _

void
test_extension_M_instructions(void)
{
    // div instruction
    TEST_R_R_R(div,  3,  20,   6 );
    TEST_R_R_R(div, -3, -20,   6 );
    TEST_R_R_R(div, -3,  20,  -6 );
    TEST_R_R_R(div,  3, -20,  -6 );
    TEST_R_R_R(div, -1<<31, -1<<31,  1 );
    //TEST_R_R_R(div, -1<<31, -1<<31, -1 );
    #if 0
    TEST_R_R_R(div, -1, -1<<31, 0 );
    TEST_R_R_R(div, -1,      1, 0 );
    TEST_R_R_R(div, -1,      0, 0 );
    #endif
    

    // divu instruction
    TEST_R_R_R(divu,                   3,  20,   6 );
    TEST_R_R_R(divu,           715827879, -20,   6 );
    TEST_R_R_R(divu,                   0,  20,  -6 );
    TEST_R_R_R(divu,                   0, -20,  -6 );
    TEST_R_R_R(divu, -1<<31, -1<<31,  1 );
    //TEST_R_R_R(divu,     0,  -1<<31, -1 );
    #if 0
    TEST_R_R_R(divu, -1, -1<<31, 0 );
    TEST_R_R_R(divu, -1,      1, 0 );
    TEST_R_R_R(divu, -1,      0, 0 );
    #endif

    // mul instruction
    TEST_R_R_R(mul, 0x00001200, 0x00007e00, 0xb6db6db7 );
    TEST_R_R_R(mul, 0x00001240, 0x00007fc0, 0xb6db6db7 );

    TEST_R_R_R(mul, 0x00000000, 0x00000000, 0x00000000 );
    TEST_R_R_R(mul, 0x00000001, 0x00000001, 0x00000001 );
    TEST_R_R_R(mul, 0x00000015, 0x00000003, 0x00000007 );

    TEST_R_R_R(mul, 0x00000000, 0x00000000, 0xffff8000 );
    TEST_R_R_R(mul, 0x00000000, 0x80000000, 0x00000000 );
    TEST_R_R_R(mul, 0x00000000, 0x80000000, 0xffff8000 );

    TEST_R_R_R(mul, 0x0000ff7f, 0xaaaaaaab, 0x0002fe7d );
    TEST_R_R_R(mul, 0x0000ff7f, 0x0002fe7d, 0xaaaaaaab );

    TEST_R_R_R(mul, 0x00000000, 0xff000000, 0xff000000 );

    TEST_R_R_R(mul, 0x00000001, 0xffffffff, 0xffffffff );
    TEST_R_R_R(mul, 0xffffffff, 0xffffffff, 0x00000001 );
    TEST_R_R_R(mul, 0xffffffff, 0x00000001, 0xffffffff );
    //mulh instruction
    TEST_R_R_R(mulh, 0x00000000, 0x00000000, 0x00000000 );
    TEST_R_R_R(mulh, 0x00000000, 0x00000001, 0x00000001 );
    TEST_R_R_R(mulh, 0x00000000, 0x00000003, 0x00000007 );

    TEST_R_R_R(mulh, 0x00000000, 0x00000000, 0xffff8000 );
    TEST_R_R_R(mulh, 0x00000000, 0x80000000, 0x00000000 );
    TEST_R_R_R(mulh, 0x00000000, 0x80000000, 0x00000000 );

    TEST_R_R_R(mulh, 0xffff0081, 0xaaaaaaab, 0x0002fe7d );
    TEST_R_R_R(mulh, 0xffff0081, 0x0002fe7d, 0xaaaaaaab );

    TEST_R_R_R(mulh, 0x00010000, 0xff000000, 0xff000000 );

    TEST_R_R_R(mulh, 0x00000000, 0xffffffff, 0xffffffff );
    TEST_R_R_R(mulh, 0xffffffff, 0xffffffff, 0x00000001 );
    TEST_R_R_R(mulh, 0xffffffff, 0x00000001, 0xffffffff );
    //mulhsu instruction
    TEST_R_R_R(mulhsu, 0x00000000, 0x00000000, 0x00000000 );
    TEST_R_R_R(mulhsu, 0x00000000, 0x00000001, 0x00000001 );
    TEST_R_R_R(mulhsu, 0x00000000, 0x00000003, 0x00000007 );

    TEST_R_R_R(mulhsu, 0x00000000, 0x00000000, 0xffff8000 );
    TEST_R_R_R(mulhsu, 0x00000000, 0x80000000, 0x00000000 );
    TEST_R_R_R(mulhsu, 0x80004000, 0x80000000, 0xffff8000 );

    TEST_R_R_R(mulhsu, 0xffff0081, 0xaaaaaaab, 0x0002fe7d );
    TEST_R_R_R(mulhsu, 0x0001fefe, 0x0002fe7d, 0xaaaaaaab );

    TEST_R_R_R(mulhsu, 0xff010000, 0xff000000, 0xff000000 );

    TEST_R_R_R(mulhsu, 0xffffffff, 0xffffffff, 0xffffffff );
    TEST_R_R_R(mulhsu, 0xffffffff, 0xffffffff, 0x00000001 );
    TEST_R_R_R(mulhsu, 0x00000000, 0x00000001, 0xffffffff );
    //mulhu instruction
    TEST_R_R_R(mulhu, 0x00000000, 0x00000000, 0x00000000 );
    TEST_R_R_R(mulhu, 0x00000000, 0x00000001, 0x00000001 );
    TEST_R_R_R(mulhu, 0x00000000, 0x00000003, 0x00000007 );

    TEST_R_R_R(mulhu, 0x00000000, 0x00000000, 0xffff8000 );
    TEST_R_R_R(mulhu, 0x00000000, 0x80000000, 0x00000000 );
    TEST_R_R_R(mulhu, 0x7fffc000, 0x80000000, 0xffff8000 );

    TEST_R_R_R(mulhu, 0x0001fefe, 0xaaaaaaab, 0x0002fe7d );
    TEST_R_R_R(mulhu, 0x0001fefe, 0x0002fe7d, 0xaaaaaaab );

    TEST_R_R_R(mulhu, 0xfe010000, 0xff000000, 0xff000000 );

    TEST_R_R_R(mulhu, 0xfffffffe, 0xffffffff, 0xffffffff );
    TEST_R_R_R(mulhu, 0x00000000, 0xffffffff, 0x00000001 );
    TEST_R_R_R(mulhu, 0x00000000, 0x00000001, 0xffffffff );
    // rem instruction
    TEST_R_R_R(rem,  2,  20,   6 );
    TEST_R_R_R(rem, -2, -20,   6 );
    TEST_R_R_R(rem,  2,  20,  -6 );
    TEST_R_R_R(rem, -2, -20,  -6 );
    TEST_R_R_R(rem,  0, -1<<31,  1 );
    //TEST_R_R_R(rem,  0, -1<<31, -1 );
    #if 0
    TEST_R_R_R(rem, -1<<31, -1<<31, 0 );
    TEST_R_R_R(rem,      1,      1, 0 );
    TEST_R_R_R(rem,      0,      0, 0 );
    #endif

    //remu instruction
    TEST_R_R_R(remu,   2,  20,   6 );
    TEST_R_R_R(remu,   2, -20,   6 );
    TEST_R_R_R(remu,  20,  20,  -6 );
    TEST_R_R_R(remu, -20, -20,  -6 );
    TEST_R_R_R(remu,      0, -1<<31,  1 );
    //TEST_R_R_R(remu, -1<<31, -1<<31, -1 );
    #if 0
    TEST_R_R_R(remu, -1<<31, -1<<31, 0 );
    TEST_R_R_R(remu,      1,      1, 0 );
    TEST_R_R_R(remu,      0,      0, 0 );
    #endif
}

static void
test_amo_instruction(void)
{
    // amoadd.w instruction
    uint32_t target = 0x12345678;
    uint32_t initial_value = 0;
    __asm__ volatile(
                     "amoadd.w %[INIT_VALUE], %[RS2], (%[RS1]);"
                     :[INIT_VALUE]"=r"(initial_value)
                     :[RS2]"r"(-0x678), [RS1]"r"(&target)
                     :"memory");
    if (initial_value == 0x12345678 && target == 0x12345000) {
        printk(ANSI_COLOR_GREEN"PASS"ANSI_COLOR_RESET" amoadd.w rd, rs2, (rs1)\n");
    }
}

void
unit_test(void)
{
    test_arithmetic_instruction();
    test_arithmetic_immediate_instruction();
    test_extension_M_instructions();
    test_memory_operation();
    test_amo_instruction();
}
