#include <stdio.h>
#include <curses.h>

int main(void) {

  initscr(); // inits screen
  raw();
  keypad(stdscr, TRUE);
  noecho();

  int row, col;
  getmaxyx(stdscr, row, col);

  mvprintw(row-1, 0, "NORMAL");
  move(0, 0);

  int ch = getch();

  while (ch != 'q') {
    ch = getch();
    addch(ch);
  }

  refresh(); 
  getch();
  endwin(); // frees the memory taken by curses
  return 0;
  
}
