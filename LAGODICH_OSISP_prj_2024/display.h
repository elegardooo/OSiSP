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

#define KEY_ESC 27
#define MAX_WINDOW_WIDTH 84
#define SECTOR_SIZE 512
#define BYTES_IN_LINE 16
#define LEFT_SPACE 4
#define WINDOW_BORDER_SPACE 1

void print_sector_data(int sector_num, char * sector_buf, WINDOW * win, int max_sectors);
void print_page(WINDOW* mainwin, char* sector_buf, int sector_num, int max_sectors, char* block_device_path);
void remove_top_reverse(WINDOW* mainwin, int byte_xpos, int byte_ypos, char* sector_buf);
void print_top_reverse(WINDOW* mainwin, int byte_xpos, int byte_ypos, char* sector_buf);
void remove_low_reverse(WINDOW* mainwin, int byte_xpos, int byte_ypos, char* sector_buf);
void print_low_reverse(WINDOW* mainwin, int byte_xpos, int byte_ypos, char* sector_buf);