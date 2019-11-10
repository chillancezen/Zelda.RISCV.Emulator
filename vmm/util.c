/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <util.h>
#include <fcntl.h>
#include <unistd.h>

int
preload_image(void * addr, int64_t length, const char * image_path)
{
    int fd = open(image_path, O_RDONLY);
    if (fd < 0) {
        return fd;
    }
    int nr_image_length = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    int nr_left = nr_image_length;
    int nr_read = 0;
    while (nr_left > 0) {
        int nr_to_read = MIN(nr_left, length - nr_read);
        if (!nr_to_read) {
            break;
        }
        int tmp = read(fd, addr + nr_read, nr_to_read);
        if (tmp <= 0) {
            break;
        }
        nr_left -= tmp;
        nr_read += tmp;
    }
    close(fd);
    return !(nr_left == 0);
}
