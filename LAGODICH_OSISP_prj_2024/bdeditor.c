#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
//#include "bdeditor.h"

#define BLOCK_DEVICE "/dev/loop9"
#define MAX_WINDOW_WIDTH 84
#define SECTOR_SIZE 512
#define BYTES_IN_LINE 16
#define LEFT_SPACE 4
#define WINDOW_BORDER_SPACE 1

void print_sector_data(int current_address, char * sector_buf, WINDOW * win, int sector_num)
{
    mvwprintw(win, WINDOW_BORDER_SPACE, LEFT_SPACE, "Offset");
    for (int j = 0; j < BYTES_IN_LINE; ++j) {
        mvwprintw(win, WINDOW_BORDER_SPACE, LEFT_SPACE + 9 + j * 3, " %02X ", j);
    }

    for (int j = 0; j < BYTES_IN_LINE; ++j) {
        mvwprintw(win, WINDOW_BORDER_SPACE, LEFT_SPACE + 9 + BYTES_IN_LINE * 3 + 3 + j, "%1X", j);
    }

    for (int i = 0; i < SECTOR_SIZE; i += BYTES_IN_LINE) {
        
        mvwprintw(win, i / BYTES_IN_LINE + WINDOW_BORDER_SPACE + 1, LEFT_SPACE, "%08d  ", current_address + i);

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
    offset = (off_t) sector_num * SECTOR_SIZE;

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

     /* NCURSES START */
    initscr();
    cbreak();
    noecho();

    // get screen size
    // int yMax, xMax;
    // getmaxyx(stdscr, yMax, xMax);
    int sector_num = 0;


    // create a window for out input
    //newwin(int height, int width, int starty, int startx)
    // WINDOW * menuwin = newwin(3, xMax, 0, 0);
    // box(menuwin, 0, 0);

    WINDOW * mainwin = newwin(SECTOR_SIZE / BYTES_IN_LINE + LEFT_SPACE + WINDOW_BORDER_SPACE, MAX_WINDOW_WIDTH, WINDOW_BORDER_SPACE, WINDOW_BORDER_SPACE);
    box(mainwin, 0, 0);
    char sector_buf[SECTOR_SIZE];
    read_from_sector(sector_num, sector_buf);

    print_sector_data(sector_num * SECTOR_SIZE, sector_buf, mainwin, sector_num);




    refresh();
    wrefresh(mainwin);

    // makes it so we can use arrow keys
    // keypad(menuwin, true);

    // char * choices[1] = {"Walk"};
    // int choice;
    // int highlight = 0;

    // while(1)
    // {
    //     for(int i =0; i < 1; i++)
    //     {
    //         if(i == highlight)
    //             wattron(menuwin, A_REVERSE);
    //         mvwprintw(menuwin, i+1, 1, choices[i]);
    //         wattroff(menuwin, A_REVERSE);
    //     }
    //     choice = wgetch(menuwin);

    //     switch(choice)
    //     {
    //         case KEY_UP:
    //             highlight--;
    //             break;
    //         case KEY_DOWN:
    //             highlight++;
    //             break;
    //         default:
    //             break;
    //     }
    //     if(choice == 10)
    //         break;
    // }

    // make sure program waits before exiting...
    getch();
    endwin();
    /* NCURSES END */
    printf("Hello world!\n");
    close(fd);
    return 0;
}