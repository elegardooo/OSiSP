#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BLOCK_SIZE 512
#define BYTES_PER_LINE 16

//int main(int argc, char ** argv)
int main()
{
    int fd;
    fd = open("/dev/sda", O_RDONLY);
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
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    // create a window for out input
    //newwin(int height, int width, int starty, int startx)
    WINDOW * menuwin = newwin(3, xMax, 0, 0);
    box(menuwin, 0, 0);
    WINDOW * mainwin = newwin(yMax-3, xMax, 3, 0);
    box(mainwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    wrefresh(mainwin);

    // makes it so we can use arrow keys
    keypad(menuwin, true);

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