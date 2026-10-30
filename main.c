#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <curses.h>

#define ctrl(x) ((x) & 0x1f)
#define BACKSPACE 263
#define ESCAPE 27

typedef enum {
  NORMAL,
  INSERT,
} Mode;

Mode mode = NORMAL;
int QUIT = 0;

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

  struct termios t;

  initscr(); // inits screen
  raw();
  set_escdelay(0);
  keypad(stdscr, TRUE);
  noecho();

  // Disable XON/XOFF flow control so Ctrl+Q/Ctrl+S reach the program
  // Must be done AFTER raw() which re-enables it
  if (tcgetattr(STDIN_FILENO, &t) == 0) {
    t.c_iflag &= ~(IXON | IXOFF | IXANY);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
  }

  char *buf = malloc(sizeof(char) * 1024);
  size_t buf_s = 0;

  // attron(A_BLINK | A_UNDERLINE);

  int row, col;
  getmaxyx(stdscr, row, col);
  
  mvprintw(row-1, 0, stringify_mode());
  move(0, 0);

  int ch = 0;

  int x, y = 0;
  while (ch != 'q' && QUIT != 1) {
    refresh();
    mvprintw(row-1, 0, stringify_mode());
    move(y, x);

    switch(mode) {
      case NORMAL:
        ch = getch();
        if(ch == 'i'){
          mode = INSERT;
        } else if(ch == ctrl('s')) {

          FILE *file = fopen("out.txt", "w");
          
          fwrite(buf, buf_s, 1, file);
          fclose(file);
          QUIT = 1;
        }
        break;
      case INSERT: {
        ch = getch();
        if (ch == ERR) {
          break;
        } else if (ch == BACKSPACE) {
          getyx(stdscr, y, x);
          move(y, x-1);
          delch();
          buf[buf_s--] = ' ';
        } else if (ch == ESCAPE) {
          mode = NORMAL;
        } else {
          buf[buf_s++] = ch;
          addch(ch);
        }
        break;
    }
    }
    getyx(stdscr, y, x);
  }

  refresh(); 
  getch();
  endwin(); // frees the memory taken by curses
  return 0;
  
}
