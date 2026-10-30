#include <stdio.h>
#include <curses.h>

int main(void) {
  initscr(); // inits screen
  raw();
  keypad(stdscr, TRUE);
  noecho();

  int ch = getch();

  refresh(); 
  getch();
  endwin(); // frees the memory taken by curses
  return 0;

}
