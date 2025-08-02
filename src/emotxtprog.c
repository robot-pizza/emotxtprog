#include "emotxtprog.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>

void get_cursor_pos(int *row, int *col) {
    // Make sure the terminal is in raw mode
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Send the escape sequence to request cursor position
    printf("\033[6n");
    fflush(stdout);

    // Read the response
    char buf[32];
    int bytesRead = read(STDIN_FILENO, buf, sizeof(buf) - 1);
    if (bytesRead > 0) {
        buf[bytesRead] = '\0'; // Null-terminate the string
        // Parse the response
        if (sscanf(buf, "\033[%d;%dR", row, col) != 2) {
            fprintf(stderr, "Failed to get cursor position\n");
        }
    } else {
        fprintf(stderr, "Failed to read cursor position\n");
    }

    // Restore the terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    // hide the cursor
    printf("\033[?25l");
    fflush(stdout);
}

static void set_cursor_pos(int row, int col) {
  fprintf(stdout,"\033[%d;%dH", row, col);
  fflush(stdout);
}

static const char *bsizes[] = {"b","KiB","MiB","GiB","TiB","XiB"};

static void print_byte_size(char *s, int n) {
  int i = 0;
  float f = (float)n;
  while( f > 1024.0 ) {
    f /= 1024.0;
    ++i;
  }
  sprintf(s, "%.2f %s", f, bsizes[i]);
}

void bar_init(PBar *bar, int ntotal, int width, int height, PBarStyle bar_style, PPctStyle pct_style, PBarFlags flags) {
  bar->ntotal = ntotal;
  bar->width = width;
  bar->height = height;
  bar->n = 0;
  bar->bar_style = bar_style;
  bar->pct_style = pct_style;
  bar->flags = flags;
  if( height > 1 ) {
    for( int h = 0; h < height-1; ++h )
      fputc('\n',stdout);
    fflush(stdout);
  }
  get_cursor_pos(&bar->row,&bar->col);
  bar->row -= height-1;
  if( bar_style != Dull )
    bar->width /= 2;
  print_byte_size(bar->of_cnt, ntotal);
  bar_update(bar,0);
}

static void write_pct(PBar *bar, int n) {
  float pct = (float)n/bar->ntotal;
  if( bar->pct_style == Percent )
    fprintf(stdout, " %.2f  ", pct);
  else if( bar->pct_style == Count )
    fprintf(stdout, " %d / %d  ", n, bar->ntotal);
  else if( bar->pct_style == ByteCount ) {
    print_byte_size(bar->cnt, n);
    fprintf(stdout, " %s / %s  ", bar->cnt, bar->of_cnt);
  }
}

struct BarChars {
  PBarStyle style;
  const char *y;
  const char *n;
  const char *d;
  int density;
  int w;
};

static struct BarChars bar_chars[] = {
 { Dull,"="," ","",0,1},
 { Fire,"🔥","🌾","🏡",13,2},
 { Flood,"🌊","  ","🏖️",5,2},
 { Cats,"😺","  ","🐟",13,2},
 { Robot,"🤖","📄","",0,2}
};

void bar_update(PBar *bar, int n) {
  float pct = (float)n/bar->ntotal;
  float barpct = 1.0/bar->width;
  set_cursor_pos(bar->row,bar->col);
  struct BarChars *barchars = bar_chars+bar->bar_style;
  float tickpct = barpct/bar->height;
  for( int j = 0; j < bar->height; ++j ) {
    for( int i = 0; i < bar->width; ++i ) {
      if( i*barpct+tickpct*(i%2?bar->height-1-j:j) < pct )
        fputs(barchars->y,stdout);
      else if( barchars->density && (i+j*barchars->density/2) % barchars->density == 0 )
        fputs(barchars->d,stdout);
      else
        fputs(barchars->n,stdout);
    }
    if( j+1 != bar->height )
      fputc('\n',stdout);
    else
      write_pct(bar,n);
  }
  fflush(stdout);
}

void bar_finish(PBar *bar) {
  bar_update(bar,bar->ntotal);
  fputc('\n',stdout);
  // show the cursor
  printf("\033[?25h");
  fflush(stdout);
}

