#include <stdint.h>


char * welcome = "Hello RISC-V\n";

int
foo(int a, int b)
{
    return a + b;
}


unsigned int strlen(const char * str)
{
    uint32_t ret = 0;
    while(*str) {
        ret++;
        str++;
    }
    return ret;
}

void
raw_puts(const char * text)
{
    uint8_t * ptr = (uint8_t *)text;
    while(*ptr) {
        *(uint8_t *)0x10000000 = *ptr++;
    }
}
unsigned int count_decoded_length(const char *encoded) {

    unsigned int len = strlen(encoded), padding = 0;

    if (encoded[len - 1] == '=' && encoded[len - 2] == '=')
        padding = 2;
    else if (encoded[len - 1] == '=')
        padding = 1;

    return (len * 3) / 4 - padding;
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
    count_decoded_length(welcome);
    {
        uint8_t * ptr = (uint8_t *)welcome;
        while (*ptr) {
            //*ptr = *ptr + 1;
            ptr++;
        }
    }
    raw_puts(welcome);
    raw_puts("\n");
}

