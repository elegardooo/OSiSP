#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <linux/fs.h>
//#include "bdeditor.h"

#define F1          59
#define F2          60
#define F3          61
#define F4          62
#define F5          63
#define F6          64
#define F7          65
#define F8          66
#define F9          67
#define F10         68
#define BLOCK_DEVICE "/dev/loop9"
#define MAX_WINDOW_WIDTH 84
#define SECTOR_SIZE 512
#define BYTES_IN_LINE 16
#define LEFT_SPACE 4
#define WINDOW_BORDER_SPACE 1

void print_sector_data(int sector_num, char * sector_buf, WINDOW * win, int max_sectors)
{
    //Print main info
    mvwprintw(win, WINDOW_BORDER_SPACE, LEFT_SPACE, "Offset");
    for (int j = 0; j < BYTES_IN_LINE; ++j) {
        mvwprintw(win, WINDOW_BORDER_SPACE, LEFT_SPACE + 9 + j * 3, " %02X ", j);
    }
    for (int j = 0; j < BYTES_IN_LINE; ++j) {
        mvwprintw(win, WINDOW_BORDER_SPACE, LEFT_SPACE + 9 + BYTES_IN_LINE * 3 + 3 + j, "%1X", j);
    }
    mvwprintw(win, 35, LEFT_SPACE, "%d/%d sector", sector_num, max_sectors);

    //Print current sector data
    for (int i = 0; i < SECTOR_SIZE; i += BYTES_IN_LINE) {
        //Print byte position
        mvwprintw(win, i / BYTES_IN_LINE + WINDOW_BORDER_SPACE + 1, LEFT_SPACE, "%08d  ", (sector_num - 1) * SECTOR_SIZE + i);

        for (int j = 0; j < BYTES_IN_LINE; ++j) {
            if (i + j < SECTOR_SIZE) {
                mvwprintw(win, WINDOW_BORDER_SPACE + 1 + i / BYTES_IN_LINE, LEFT_SPACE + 9 + j * 3, " %02X ", (unsigned char) sector_buf[i + j]); // unsigned char for displaying an octet e.g. as hex value.
            } else {
                wprintw(win, "   ");
            }
        }

        for (int j = 0; j < BYTES_IN_LINE; ++j) {
            if (i + j < SECTOR_SIZE) {
                mvwprintw(win, WINDOW_BORDER_SPACE + 1 + i / BYTES_IN_LINE, LEFT_SPACE + 9 + BYTES_IN_LINE * 3 + 3 + j, "%c", isprint(sector_buf[i + j]) ? sector_buf[i + j] : '.');
            } else {
                break;
            }
        }

    }


}

void read_from_sector(int sector_num, char *buf) {
    int fd;
    off_t offset;
    ssize_t bytes_read;

    fd = open(BLOCK_DEVICE, O_RDONLY);
    if (fd == -1) {
        perror("open");
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
}

void print_page(WINDOW* mainwin, char* sector_buf, int sector_num, int max_sectors)
{
    read_from_sector(sector_num, sector_buf);
    print_sector_data(sector_num, sector_buf, mainwin, max_sectors);
    refresh();
    wrefresh(mainwin);
}

void remove_top_reverse(WINDOW* mainwin, int byte_xpos, int byte_ypos, char* sector_buf)
{
    wattroff(mainwin, A_REVERSE);
    mvwprintw(mainwin, WINDOW_BORDER_SPACE + 1 + byte_ypos, LEFT_SPACE + 9 + byte_xpos * 3 + 1, "%01X", (unsigned char) sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos] / 16);
    mvwprintw(mainwin, WINDOW_BORDER_SPACE + 1 + byte_ypos, LEFT_SPACE + 9 + BYTES_IN_LINE * 3 + 3 + byte_xpos, "%c", isprint(sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos]) ? sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos] : '.');
}

void print_top_reverse(WINDOW* mainwin, int byte_xpos, int byte_ypos, char* sector_buf)
{
    wattron(mainwin, A_REVERSE);
    mvwprintw(mainwin, WINDOW_BORDER_SPACE + 1 + byte_ypos, LEFT_SPACE + 9 + byte_xpos * 3 + 1, "%01X", (unsigned char) sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos] / 16);
    mvwprintw(mainwin, WINDOW_BORDER_SPACE + 1 + byte_ypos, LEFT_SPACE + 9 + BYTES_IN_LINE * 3 + 3 + byte_xpos, "%c", isprint(sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos]) ? sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos] : '.');
    wattroff(mainwin, A_REVERSE);
}

void remove_low_reverse(WINDOW* mainwin, int byte_xpos, int byte_ypos, char* sector_buf)
{
    wattroff(mainwin, A_REVERSE);
    mvwprintw(mainwin, WINDOW_BORDER_SPACE + 1 + byte_ypos, LEFT_SPACE + 9 + byte_xpos * 3 + 2, "%01X", (unsigned char) sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos] % 16);
    mvwprintw(mainwin, WINDOW_BORDER_SPACE + 1 + byte_ypos, LEFT_SPACE + 9 + BYTES_IN_LINE * 3 + 3 + byte_xpos, "%c", isprint(sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos]) ? sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos] : '.');
}

void print_low_reverse(WINDOW* mainwin, int byte_xpos, int byte_ypos, char* sector_buf)
{
    wattron(mainwin, A_REVERSE);
    mvwprintw(mainwin, WINDOW_BORDER_SPACE + 1 + byte_ypos, LEFT_SPACE + 9 + byte_xpos * 3 + 2, "%01X", (unsigned char) sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos] % 16);
    mvwprintw(mainwin, WINDOW_BORDER_SPACE + 1 + byte_ypos, LEFT_SPACE + 9 + BYTES_IN_LINE * 3 + 3 + byte_xpos, "%c", isprint(sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos]) ? sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos] : '.');
    wattroff(mainwin, A_REVERSE);
}


//int main(int argc, char ** argv)
int main()
{
    int fd;
    fd = open(BLOCK_DEVICE, O_RDONLY);
    if (fd == -1) 
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    int max_sectors;
    ioctl(fd, BLKGETSIZE, &max_sectors);
    printf("%d\n", max_sectors);
     /* NCURSES START */
    initscr();
    cbreak();
    noecho();

    // get screen size
    // int yMax, xMax;
    // getmaxyx(stdscr, yMax, xMax);
    int sector_num = 1;

    // create a window for out input
    //newwin(int height, int width, int starty, int startx)
    
    WINDOW * mainwin = newwin(SECTOR_SIZE / BYTES_IN_LINE + LEFT_SPACE + WINDOW_BORDER_SPACE, MAX_WINDOW_WIDTH, WINDOW_BORDER_SPACE, WINDOW_BORDER_SPACE);
    box(mainwin, 0, 0);
    char sector_buf[SECTOR_SIZE];

    print_page(mainwin, sector_buf, sector_num, max_sectors);

    // makes it so we can use arrow keys
    keypad(mainwin, true);

    //char * choices[1] = {"Walk"};
    int choice;
    int byte_xpos = 0;
    int byte_ypos = 0;
    int top_order_position = 1;
    noecho();
    curs_set(0);
    choice = wgetch(mainwin);
    while(1)
    {
        // for(int i =0; i < 1; i++)
        // {
            // if(i == highlight)
            // wattron(mainwin, A_REVERSE);
            // mvwprintw(mainwin, WINDOW_BORDER_SPACE + 1 + byte_ypos, LEFT_SPACE + 9 + byte_xpos * 3 + 1, "%02X", (unsigned char) sector_buf[byte_ypos * BYTES_IN_LINE + byte_xpos]);
            // wattroff(mainwin, A_REVERSE);
        // }
        if (top_order_position == 1)
            print_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
        else print_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);

        choice = wgetch(mainwin);

        switch(choice)
        {
            case KEY_F(1):
            if (sector_num > 1)
            {               
                sector_num--;
                print_page(mainwin, sector_buf, sector_num, max_sectors);
                break;
            }
            break;

            case KEY_F(2):
            if (sector_num < max_sectors)
            {               
                sector_num++;
                print_page(mainwin, sector_buf, sector_num, max_sectors);
                break;
            }
            break;

            case KEY_UP:
            if (byte_ypos > 0)
            {
                if (top_order_position == 1)
                    remove_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                else remove_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                byte_ypos--;
                break;
            }
            break;

            case KEY_DOWN:
            if (byte_ypos < 31)
            {
                if (top_order_position == 1)
                    remove_top_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                else remove_low_reverse(mainwin, byte_xpos, byte_ypos, sector_buf);
                byte_ypos++;
                break;
            }
            break;

            case KEY_LEFT:
            if (byte_xpos > 0 || top_order_position != 1)
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
            }
            break;

            case KEY_RIGHT:
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
            }
            break;

            default:
                break;
        }
        if(choice == 27)
            break;
    }

    // make sure program waits before exiting...
    getch();
    endwin();
    /* NCURSES END */
    printf("Hello world!\n");
    close(fd);
    return 0;
}