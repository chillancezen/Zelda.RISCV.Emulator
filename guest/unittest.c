/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <unittest.h>


static void
test_add_instruction(void)
{
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
}


static void
test_and_instruction(void)
{
    TEST_R_R_R(and, 0x0f000f00, 0xff00ff00, 0x0f0f0f0f);
    TEST_R_R_R(and, 0x00f000f0, 0x0ff00ff0, 0xf0f0f0f0);
    TEST_R_R_R(and, 0x000f000f, 0x00ff00ff, 0x0f0f0f0f);
    TEST_R_R_R(and, 0xf000f000, 0xf00ff00f, 0xf0f0f0f0);
}

static void
test_or_instruction(void)
{
    TEST_R_R_R(or, 0xff0fff0f, 0xff00ff00, 0x0f0f0f0f);
    TEST_R_R_R(or, 0xfff0fff0, 0x0ff00ff0, 0xf0f0f0f0);
    TEST_R_R_R(or, 0x0fff0fff, 0x00ff00ff, 0x0f0f0f0f);
    TEST_R_R_R(or, 0xf0fff0ff, 0xf00ff00f, 0xf0f0f0f0);
}

static void
test_sll_instruction(void)
{
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
}

static void
test_slt_instruction(void)
{
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
}

static void
test_sltu_instruction(void)
{
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
}
static void
test_sra_instruction(void)
{
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
}

static void
test_srl_instruction(void)
{
#define TEST_SRL(v, a)                                                         \
  //TEST_R_R_R(srl, ((v) & (((1 << (32 - 1)) << 1) - 1)) >> (a), v, a)
    TEST_SRL(0x80000000, 0  );
    TEST_SRL(0x80000000, 1  );
    TEST_SRL(0x80000000, 7  );
    TEST_SRL(0x80000000, 14 );
    TEST_SRL(0x80000001, 31 );

    TEST_SRL(0xffffffff, 0  );
    TEST_SRL(0xffffffff, 1  );
    TEST_SRL(0xffffffff, 7  );
    TEST_SRL(0xffffffff, 14 );
    TEST_SRL(0xffffffff, 31 );

    TEST_SRL(0x21212121, 0  );
    TEST_SRL(0x21212121, 1  );
    TEST_SRL(0x21212121, 7  );
    TEST_SRL(0x21212121, 14 );
    TEST_SRL(0x21212121, 31 );

    TEST_R_R_R(srl, 0x21212121, 0x21212121, 0xffffffc0 );
    TEST_R_R_R(srl, 0x10909090, 0x21212121, 0xffffffc1 );
    TEST_R_R_R(srl, 0x00424242, 0x21212121, 0xffffffc7 );
    TEST_R_R_R(srl, 0x00008484, 0x21212121, 0xffffffce );
    TEST_R_R_R(srl, 0x00000000, 0x21212121, 0xffffffff );
}

static void
test_xor_instruction(void)
{
    TEST_R_R_R(xor, 0xf00ff00f, 0xff00ff00, 0x0f0f0f0f );
    TEST_R_R_R(xor, 0xff00ff00, 0x0ff00ff0, 0xf0f0f0f0 );
    TEST_R_R_R(xor, 0x0ff00ff0, 0x00ff00ff, 0x0f0f0f0f );
    TEST_R_R_R(xor, 0x00ff00ff, 0xf00ff00f, 0xf0f0f0f0 );
}
void
unit_test(void)
{
    test_add_instruction();
    test_and_instruction();
    test_or_instruction();
    test_sll_instruction();
    test_slt_instruction();
    test_sltu_instruction();
    test_sra_instruction();
    test_srl_instruction();
    test_xor_instruction();
}
