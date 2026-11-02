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

typedef struct {
  size_t index;
  size_t size;
  char *contents;
} Row;

typedef struct {
  char *buf;
  Row rows[1024];
  size_t row_index;
} Buffer;

Mode mode = NORMAL;
int QUIT = 0;

char *stringify_mode() {
  switch(mode) {
    case NORMAL:
      return "-- NORMAL --";
      break;
    case INSERT:
      return "-- INSERT --";
      break;
    default:
      return "-- NORMAL --";
      break;
  }
  return "-- NORMAL --";
}

int main(void) {

  struct termios t;

  initscr(); // inits screen
  raw();
  set_escdelay(0);
  keypad(stdscr, TRUE);
  noecho();

  // Disable XON/XOFF flow control so Ctrl+Q/Ctrl+S reach the program
  if (tcgetattr(STDIN_FILENO, &t) == 0) {
    t.c_iflag &= ~(IXON | IXOFF | IXANY);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
  }

  Buffer buffer = {0};
  buffer.buf = calloc(1024, sizeof(char)); // magic numbers mothafluffa
                                          
  // attron(A_BLINK | A_UNDERLINE); (i was messing with effects)

  int row, col = 0;
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
      case NORMAL: {
        ch = getch();
        if(ch == 'i') {
          mode = INSERT;
        } else if (ch == 'h') {
          move(y, x-1);
          buffer.x -= 1;
        } else if (ch == 'l') {
          move(y, x+1);
          buffer.x += 1;
        } else if (ch == 'j') {
          move(y-1, x);
          buffer.y -= 1;
        } else if (ch == 'k') {
          move(y+1, x);
          buffer.y += 1;
        } else if(ch == ctrl('s')) {
          FILE *file = fopen("out.txt", "w");

          fwrite(buffer.buf, buffer.x*buffer.y, 1, file);
          fclose(file);
          QUIT = 1;
        }
        break;
      }
      case INSERT: {
        ch = getch();
        if (ch == ERR) {
          break;
        } else if (ch == BACKSPACE) {
          getyx(stdscr, y, x);
          move(y, x-1);
          delch();
          buffer.buf[buffer.x * x+buffer.y] = ' ';
          buffer.x -= 1;
        } else if (ch == ESCAPE) {
          mode = NORMAL;
        } else {
          buffer.buf[buffer.x * x+buffer.y] = ch;
          addch(ch);
          buffer.x += 1;
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
