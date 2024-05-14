#pragma once

#include <stdlib.h>
#include <ncurses.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <linux/fs.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SECTOR_SIZE 512
#define BYTES_IN_LINE 16

void read_from_sector(int sector_num, char* buf, char* block_device_path);
int hex2int(char ch);
void write_symbol(int character_num, int sector_num, int byte_xpos, int byte_ypos, int top_order_position, char* buf, char* block_device_path);