#include <stdint.h>


char * welcome = "Hello World\n";

int
foo(int a, int b)
{
    return a + 1;
}

void
kernel_init(void)
{
    int idx = 1;
    if (idx == 1) {
        *(uint8_t *)0xb8000 = 'c';
    }
    for (idx = 0xe0000001; ((int32_t)idx) <= ((int32_t)0xf0000012); idx++) {
        foo(idx, idx + 1);
    }
}
