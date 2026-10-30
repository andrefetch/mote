#include <stdio.h>
#include <curses.h>

int main(void) {

  initscr(); // inits screen
  raw();
  keypad(stdscr, TRUE);
  noecho();

  // attron(A_BLINK | A_UNDERLINE);

  int row, col;
  getmaxyx(stdscr, row, col);

  mvprintw(row-1, 0, "NORMAL");
  move(0, 0);

  int ch = getch();
  addch(ch);

  while (ch != 'q') {
    
    ch = getch();

    if (ch == 263) {
      int x, y;
      getyx(stdscr, y, x);
      move(y, x-1);
      delch();
    } else {
      addch(ch);
    }
  }


  refresh(); 
  getch();
  endwin(); // frees the memory taken by curses
  return 0;
  
}
