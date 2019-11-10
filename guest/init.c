#include <stdint.h>


char * welcome = "Hello World\n";

void
kernel_init(void)
{
    *(uint8_t *)0xb8000 = 'c';
}
