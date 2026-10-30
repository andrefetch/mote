#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <curses.h>

#define ctrl(x) ((x) & 0x1f)

typdef enum {
  NORMAL,
  INSERT,
} Mode;

Mode mode = NORMAL;

int main(void) {

  // Disable XON/XOFF flow control so Ctrl+Q reaches the program
  struct termios t;

  if (tcgetattr(STDIN_FILENO, &t) == 0) {
    t.c_iflag &= ~(IXON | IXOFF | IXANY);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
  }

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

  while (ch != 'r') {
    
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
