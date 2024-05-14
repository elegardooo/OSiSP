#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <linux/fs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "display.h"
#include "sector_operations.h"

#define KEY_ESC 27
#define MAX_WINDOW_WIDTH 84
#define SECTOR_SIZE 512
#define BYTES_IN_LINE 16
#define LEFT_SPACE 4
#define WINDOW_BORDER_SPACE 1


//int main(int argc, char ** argv)
int main(int argc, char ** argv)
{
    if(argc == 2 ) 
    {
        printf("%s\n", argv[1]);
    }
    else if(argc > 2 ) {
        printf("Too many arguments supplied.\nPrint only one path to the block device after %s\n", argv[0]);
        exit(1);
    }
    else {
        printf("One argument expected.\nPrint path to the block device after %s\n", argv[0]);
        exit(1);
    }
    char* block_device_path;
    block_device_path = argv[1]; // /dev/loop9
    //printf("%s\n", block_device_path);
    //return 1;

    int fd;
    fd = open(block_device_path, O_RDONLY);
    if (fd == -1) 
    {
        perror("Block device open error: ");
        exit(EXIT_FAILURE);
    }
    struct stat stat_buf;
    if (stat(block_device_path, &stat_buf) == -1)
    {
        perror("BLock device stat error: ");
        exit(EXIT_FAILURE);
    }
    if (!S_ISBLK(stat_buf.st_mode))
    {
        printf("Supplied path is not a block device.\n");
        exit(1);
    }
    int max_sectors;
    ioctl(fd, BLKGETSIZE, &max_sectors);
    //printf("%d\n", max_sectors);
     /* NCURSES START */   
    initscr();
    refresh();
    WINDOW * mainwin = newwin(SECTOR_SIZE / BYTES_IN_LINE + LEFT_SPACE + WINDOW_BORDER_SPACE, MAX_WINDOW_WIDTH, WINDOW_BORDER_SPACE, WINDOW_BORDER_SPACE);
    int sector_num = 1;
    // create a window for out input
    //newwin(int height, int width, int starty, int startx)
    box(mainwin, 0, 0);
    char sector_buf[SECTOR_SIZE];
    print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
    cbreak();
    noecho();
    if(!has_colors)
    {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    wbkgd(mainwin, COLOR_PAIR(1));
    bkgd(COLOR_PAIR(1));
    refresh();
    wrefresh(mainwin);

    keypad(mainwin, true);
    int choice;
    int byte_xpos = 0;
    int byte_ypos = 0;
    int top_order_position = 1;
    noecho();
    curs_set(0);
    while(1)
    {
        if (top_order_position == 1)
            print_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
        else print_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);

        choice = wgetch(mainwin);

        switch(choice)
        {
            //Page switching
            case KEY_F(1):
                if (sector_num > 1) //Previous page
                {               
                    sector_num--;
                    print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
                    break;
                }
                break;

            case KEY_F(2):
                if (sector_num < max_sectors) //Next page
                {               
                    sector_num++;
                    print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
                    break;
                }
                break;

            //Symbol pointer switching
            case KEY_UP:
                if (byte_ypos > 0) //Climb up by 1 row
                {
                    if (top_order_position == 1)
                        remove_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                    else remove_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                    byte_ypos--;
                    break;
                } else if (sector_num > 1) { //Climb up by 1 row(last row of previous page)         
                    sector_num--;
                    print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
                    byte_ypos = 31;
                    break;
                }
                break;

            case KEY_DOWN:
                if (byte_ypos < 31) //Go down by 1 row
                {
                    if (top_order_position == 1)
                        remove_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                    else remove_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                    byte_ypos++;
                    break;
                } else if (sector_num < max_sectors) { //Go down by 1 row(first row of previous page)            
                   sector_num++;
                    print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
                    byte_ypos = 0;
                    break;
                }
                break;

            case KEY_LEFT:
                if (byte_xpos > 0 || top_order_position != 1) //Go to _previous symbol_ / _top order of current symbol_
                {
                    if (top_order_position == 1) {
                        remove_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                        top_order_position--;
                        byte_xpos--;
                    }
                    else {
                        remove_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                        top_order_position++;
                    }
                    break;
                } else if (byte_ypos > 0) { //Go to end of previous row
                    remove_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                    byte_xpos = 15;
                    byte_ypos--;
                    top_order_position--;
                    break;
                } else if (sector_num > 1) { //Go to end of previous sector    
                    sector_num--;
                    print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
                    byte_xpos = 15;
                    byte_ypos = 31;
                    top_order_position--;
                    break;
                }
                break;

            case KEY_RIGHT:
                if (byte_xpos < 15 || top_order_position == 1) //Go to _next symbol_ / _low order of current symbol_
                {
                    if (top_order_position == 1) {
                        remove_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                        top_order_position--;
                    }
                    else {
                        remove_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                        top_order_position++;
                        byte_xpos++;
                    } 
                    break;
                } else if (byte_ypos < 31) { //Go to start of next row
                    remove_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                    byte_xpos = 0;
                    byte_ypos++;
                    top_order_position++;
                    break;
                } else if (sector_num < max_sectors) { //Go to start of next sector    
                    sector_num++;
                    print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
                    byte_xpos = 0;
                    byte_ypos = 0;
                    top_order_position++;
                    break;
                }
                break;

            //Write symbol at current position in block device
            case '0' ... '9':
            case 'A' ... 'F':
            case 'a' ... 'f':
                int character_num;
                character_num = hex2int(choice);
                //printf("%d ", character_num);
                write_symbol(character_num, sector_num, byte_xpos, byte_ypos, top_order_position, sector_buf, block_device_path);
                print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
                //Moving to the next symbol after writing
                if (byte_xpos < 15 || top_order_position == 1)
                {
                    if (top_order_position == 1) {
                        remove_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                        top_order_position--;
                    }
                    else {
                        remove_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                        top_order_position++;
                        byte_xpos++;
                    }
                    break;
                } else if (byte_ypos < 31) {
                    remove_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                    byte_xpos = 0;
                    byte_ypos++;
                    top_order_position++;
                    break;
                } else if (sector_num < max_sectors) {               
                    sector_num++;
                    print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
                    byte_xpos = 0;
                    byte_ypos = 0;
                    top_order_position++;
                    break;
                }
                break;


            //Optimise programm for terminal resizing
            case KEY_RESIZE:
                wclear(mainwin);
                wrefresh(mainwin);
                print_page(mainwin, sector_buf, sector_num, max_sectors, block_device_path);
                break;

            default:
                break;
        }
        if(choice == KEY_ESC)
            break;
    }

    // make sure program waits before exiting...
    getch();
    endwin();
    /* NCURSES END */
    close(fd);
    return 0;
}