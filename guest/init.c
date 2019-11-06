#include <stdint.h>


void
kernel_init(void)
{
    *(uint8_t *)0xb8000 = 'c';
}
