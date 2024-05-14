#include "sector_operations.h"

#include <stdio.h>

void read_from_sector(int sector_num, char* buf, char* block_device_path) 
{
    int fd;
    off_t offset;
    ssize_t bytes_read;

    fd = open(block_device_path, O_RDONLY);
    if (fd == -1) {
        perror("Block device open error: ");
        exit(EXIT_FAILURE);
    }

    // Reading offset
    offset = (off_t) (sector_num - 1) * SECTOR_SIZE;

    // Set on the start of current sector
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Sector reading in buf
    bytes_read = read(fd, buf, SECTOR_SIZE);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    // Close file
    close(fd);
    //printf("SYMLBOL:> %c <", buf[17]); //2 in ASCII   (2 = 50)
    //printf("SYMLBOL:> %d <", buf[17]); //50 in int
}

int hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    return -1;
}

void write_symbol(int character_num, int sector_num, int byte_xpos, int byte_ypos, int top_order_position, char* buf, char* block_device_path)
{
    int fd;
    off_t offset;
    ssize_t bytes_write; //types.h lib for off_t(64-bit integer for offset in files) and ssize_t(counting bytes readed/writen into files, -1 in error)

    fd = open(block_device_path, O_WRONLY);
    if (fd == -1) {
        perror("Block device open error: ");
        exit(EXIT_FAILURE);
    }

    // Reading offset
    offset = (off_t) (sector_num - 1) * SECTOR_SIZE + byte_ypos * BYTES_IN_LINE + byte_xpos;

    // Set on the start of current sector
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Sector writing from buf
    int byte_to_write;
    int low_order_num;
    low_order_num = buf[byte_ypos * BYTES_IN_LINE + byte_xpos] % 16;
    if (top_order_position == 1) 
        byte_to_write = low_order_num + character_num * 16;
    else byte_to_write = buf[byte_ypos * BYTES_IN_LINE + byte_xpos] - low_order_num + character_num;
    bytes_write = write(fd, &byte_to_write, 1);
    if (bytes_write == -1) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    // Close file
    close(fd);
}