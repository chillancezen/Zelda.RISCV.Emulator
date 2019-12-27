#include <stdint.h>
#include <printk.h>
#include <unittest.h>
#include <sha1.h>
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
    uint8_t temp = 0;
    while(*ptr) {
        temp = *ptr;
        //__asm__ volatile("ebreak;");
        *(uint8_t *)0x10000000 = temp;
        ptr++;
    }
}
void
raw_puts_another(const char *);

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
    int idx0 = 0;
    for (; idx0 < 1; idx0++) {
        unit_test();
    }
#if 0
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
    raw_puts_another("Link Is The Devil\n");
    char * buffer = "Link loves Zelda and Mipha...";
    for (idx = 0; idx < 5; idx++) {
        buffer[0] ++;
        raw_puts(buffer);
        raw_puts("\n");
    }
    printk("Hello World from printk: 0x%d\n", 0x1234);
#endif
    int idx = 0;
    for (idx = 0; idx < 0; idx++) {
        __asm__ volatile("ebreak;");
        __asm__ volatile("ebreak;");
        __asm__ volatile("ebreak;");
    }
    printk("Hello World from printk: %d\n", 0xffffffff);
    //__asm__ volatile("ebreak;");
    printk("hex: 0x%d\n", -234);
    printk("hex: 0x%x\n", 0xffffffff);
    printk("char:%c\n", 'H');
    printk("string: %s\n", "Hello World");

    int tmp_index = 0;
    char string_to_hash[] = "Hello World. Hello Link";
    for (; tmp_index < 2; tmp_index++)
    {
        char * index_tbl="0123456789abcdef";
        char result[21];
        char hex_result[41];
        SHA1(result, string_to_hash, sizeof(string_to_hash) -1);
        for (idx = 0; idx < 20; idx++) {
            hex_result[idx * 2 + 1] = index_tbl[result[idx] & 0xf];
            hex_result[idx * 2] = index_tbl[(result[idx] >> 4) & 0xf];
        }
        hex_result[40] = '\0';
        printk("SHA1:%s\n", hex_result);
        ((uint32_t *)string_to_hash)[0] += 1;

    }
}

