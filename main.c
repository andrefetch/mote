#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <curses.h>

#define ctrl(x) ((x) & 0x1f)
#define BACKSPACE 263

typedef enum {
  NORMAL,
  INSERT,
} Mode;

Mode mode = NORMAL;

char *stringify_mode() {
  switch(mode) {
    case NORMAL:
      return "NORMAL";
      break;
    case INSERT:
      return "INSERT";
      break;
    default:
      return "NORMAL";
      break;
  }
  return "NORMAL";
}

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
  
  mvprintw(row-1, 0, stringify_mode());
  move(0, 0);

  int ch = getch();
  addch(ch);

  int x, y = 0;
  getyx(stdscr, y, x);
  while (ch != 'r') {
    mvprintw(row-1, 0, stringify_mode());
    move(y, x);
    ch = getch();

    switch(mode) {
      case NORMAL: 
        break;
      case INSERT:
         if (ch == BACKSPACE) {
          getyx(stdscr, y, x);
          move(y, x-1);
          delch();
        } else {
          addch(ch);
        }
      }
      getyx(stdscr, y, x);
    }

  refresh(); 
  getch();
  endwin(); // frees the memory taken by curses
  return 0;
  
}
