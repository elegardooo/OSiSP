#include "display.h"
#include "sector_operations.h"

#include <stdio.h>

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

void print_page(WINDOW* mainwin, char* sector_buf, int sector_num, int max_sectors, char* block_device_path)
{
    endwin();
    refresh();
    read_from_sector(sector_num, sector_buf, block_device_path);
    print_sector_data(sector_num, sector_buf, mainwin, max_sectors);
    mainwin = newwin(SECTOR_SIZE / BYTES_IN_LINE + LEFT_SPACE + WINDOW_BORDER_SPACE, MAX_WINDOW_WIDTH, WINDOW_BORDER_SPACE, WINDOW_BORDER_SPACE);
    box(mainwin, 0, 0);
    wbkgd(mainwin, COLOR_PAIR(1));
    bkgd(COLOR_PAIR(1));
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
