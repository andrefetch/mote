#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <curses.h>

#define ctrl(x) ((x) & 0x1f)
#define BACKSPACE 263
#define ESCAPE 27
#define ENTER 10

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
  size_t cur_pos;
  size_t row_s; 
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
  buffer.row_s = 1;
  buffer.cur_pos = 1;

  for(size_t i = 0; i < 1024; i++) {
    buffer.rows[i].contents = calloc(1024, sizeof(char));
    buffer.rows[i].contents[0] = ' ';
    // magic numbers mothafluffa
  }
                                          
  // attron(A_BLINK | A_UNDERLINE); (i was messing with effects)

  int row, col;
  getmaxyx(stdscr, row, col);
  
  mvprintw(row-1, 0, stringify_mode());
  move(0, 0);

  int ch = 0;

  int x, y = 0;
  while (ch != 'q' && QUIT != 1) {
    refresh();
    mvprintw(row-1, 0, stringify_mode());
   
    for(size_t i = 0; i < buffer.row_s; i++) {
      mvprintw(0, i+1, buffer.rows[i].contents);
    }

    move(y, x);
    ch = getch();
    switch(mode) {
      case NORMAL: {
        if(ch == 'i') {
          mode = INSERT;
        } else if (ch == 'h' || ch == KEY_LEFT) {
          move(y, x-1);
          buffer.cur_pos -= 1;
        } else if (ch == 'l' || ch == KEY_RIGHT) {
          move(y, x+1);
          buffer.cur_pos += 1;
        } else if (ch == 'j' || ch == KEY_DOWN) {
          move(y+1, x);
          buffer.row_index += 1;
        } else if (ch == 'k' || ch == KEY_UP) {
          move(y-1, x);
          buffer.row_index -= 1;
        } else if(ch == ctrl('s')) {
          FILE *file = fopen("out.txt", "w");

          // fwrite(buffer.buf, buffer.row_index*buffer.cur_pos, 1, file);
          fclose(file);
          QUIT = 1;
        }
        break;
      }
      case INSERT: {
        if (ch == ERR) {
          break;
        } else if (ch == BACKSPACE) {
          getyx(stdscr, y, x);
          Row *cur = &buffer.rows[buffer.row_index];
          if (buffer.cur_pos > 0) {
            buffer.cur_pos--;
            cur->contents[buffer.cur_pos] = ' ';
            move(y, buffer.cur_pos + 1);
          }
        } else if (ch == ESCAPE) {
          mode = NORMAL;
        } else if (ch == ENTER) {
          buffer.row_index += 1;
          buffer.row_s++;
        } else {
          Row *cur = &buffer.rows[buffer.row_index];
          cur->contents[buffer.cur_pos] = ch;
          buffer.cur_pos += 1;
          move(y, buffer.cur_pos + 1);
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
