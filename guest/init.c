#include <stdint.h>


char * welcome = "Hello World\n";

int
foo(int a, int b)
{
    return a + b;
}

void
kernel_init(void)
{
    int idx = 1;
    if (idx == 1) {
        *(uint8_t *)0xb8000 = 'c';
    }
    for (idx = 0xe0000001; ((uint32_t)idx) <= ((uint32_t)0xe0000012); idx++) {
        foo(idx, idx + 1);
    }
}
