#define _XOPEN_SOURCE 600
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <math.h>
#include "emotxtprog.h"

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

static BarDecorator fire_decorators[] = {{"🏡",0.03},{"🚜",0.01},{"🐄",0.006},{"🐐",0.003},{NULL}};
static BarDecorator flood_decorators[] = {{"🏖️",0.05},{"🪣",0.01},{"🏰",0.01},{NULL}};
static BarDecorator cat_decorators[] = {{"🐟",0.05},{"🐠",0.05},{"🍣",0.01},{"🐦",0.05},{"🦎",0.05},{"🍲",0.01},{NULL}};
static BarDecorator robot_decorators[] = {{"📄",0.2},{"🔋",0.6},{NULL}};
static BarDecorator goat_decorators[] = {{"🪴",0.01},{"🧦",0.01},{"🍕",0.005},{"🌿",1.0},{NULL}};

static CustomBarStyle canned_bar_styles[] = {
 /* Dull */ {"="," ",1,NULL,NoRefill,FillLeftToRight},
 /* Fire */ {"🔥","🌾",2,fire_decorators,NoRefill,FillRadial},
 /* Flood */{"🌊","  ",2,flood_decorators,NoRefill,FillLeftToRight},
 /* Cats */ {"😺","  ",2,cat_decorators,BackgroundRefill,FillRadial},
 /* Robot */{"🤖","  ",2,robot_decorators,NoRefill,FillLeftToRight},
 /* Goat */ {"🐐","  ",2,goat_decorators,BackgroundRefill,FillRadial},
};

static void bar_init0(PBar *bar, int ntotal, int width, int height, PPctStyle pct_style, PBarStyle bar_style, CustomBarStyle *custom_bar_style) {
  bar->ntotal = ntotal;
  bar->width = width;
  bar->height = height;
  bar->n = 0;
  bar->bar_style = bar_style;
  bar->custom_bar_style = custom_bar_style;
  bar->pct_style = pct_style;
  if( height > 1 ) {
    for( int h = 0; h < height-1; ++h )
      fputc('\n',stdout);
    fflush(stdout);
  }
  get_cursor_pos(&bar->row,&bar->col);
  bar->row -= height-1;
  if( custom_bar_style->char_width > 1 )
    bar->width /= custom_bar_style->char_width;
  print_byte_size(bar->of_cnt, ntotal);
  bar->cells = malloc(sizeof(char*)*bar->width*bar->height);
  for( int i = 0; i < bar->width*bar->height; ++i ) {
    bar->cells[i] = custom_bar_style->background;
    for( BarDecorator *decorator = custom_bar_style->decorators; decorator && decorator->decorator; ++decorator ) {
      if( rand()/(float)RAND_MAX <= decorator->density ) {
        bar->cells[i] = decorator->decorator;
        break;
      }
    }
  }
  int fill_pos = ( bar->custom_bar_style->fill_path == FillLeftToRight ) ? 0 : (rand()%(bar->width*bar->height));
  bar->radial_center = fill_pos;
  bar->last_radial_pos = fill_pos;
  bar->last_radius = 1;
  bar->last_angle = 0.0;
  bar->last_pos = 0;
  bar->cells[fill_pos] = custom_bar_style->fill;
  bar->last_pct = 0.0;
  bar->last_update = 0.0;
  bar_update(bar,0);
}

void bar_init(PBar *bar, int ntotal, int width, int height, PPctStyle pct_style, PBarStyle bar_style) {
  srand(time(NULL));
  if( bar_style == Custom )
    bar_style = Random;
  if( bar_style == Random )
    bar_style = rand()%Random;
  bar_init0(bar,ntotal,width,height,pct_style,bar_style,canned_bar_styles+bar_style);
}

void bar_init_custom(PBar *bar, int ntotal, int width, int height, PPctStyle pct_style, CustomBarStyle *custom_bar_style) {
  bar_init0(bar,ntotal,width,height,pct_style,Custom,custom_bar_style);
}

static float clock_time() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC,&now);
  return now.tv_sec + now.tv_nsec / 1e9;
}

static int pos2cri(PBar *bar, int pos, int *pcol, int *prow) {
  int col = pos / bar->height;
  int row = pos % bar->height;
  if( col%2 )
    row = bar->height-row-1;
  *pcol = col;
  *prow = row;
  return row*bar->width+col;
}

void bar_update(PBar *bar, int n) {
  float now = clock_time();
  if( n == 0 ) {
    bar->start_time = now;
    bar->last_update = now;
  }
  float pct = (float)n/bar->ntotal;
  float pos_pct = 1.0/bar->width/bar->height;
  CustomBarStyle *bar_style = bar->custom_bar_style;
  int last_pos = bar->last_pos;
  int cur_pos = last_pos;
  int redraw_cells = (n==0);
  int do_flush = 0;

  if( bar_style->fill_path == FillLeftToRight ) { 
    cur_pos = (int)floor(pct/pos_pct);
    while( last_pos < cur_pos ) {
      int last_col, last_row, last_i;
      if( bar_style->refill_behavior == BackgroundRefill) {
        last_i = pos2cri(bar, last_pos, &last_col, &last_row);
        if( bar->cells[last_i] != bar_style->background ) {
          bar->cells[last_i] = NULL;
          redraw_cells = 1;
        }
      }
      ++last_pos;
      last_i = pos2cri(bar, last_pos, &last_col, &last_row);
      if( bar->cells[last_i] != bar_style->fill ) {
        bar->cells[last_i] = bar_style->fill;
        redraw_cells = 1;
      }
    }
    bar->last_pos = cur_pos;
  } else if( bar_style->fill_path == FillRadial ) {
    cur_pos = (int)floor(pct/pos_pct);
    if( cur_pos > last_pos ) {
      int fill_pos = -1;
      int col = bar->radial_center % bar->width;
      int row = bar->radial_center / bar->width;
      float max_radius = 1+ceil(sqrt(pow(bar->width,2.0)+pow(bar->height,2.0)));
      int radius = bar->last_radius;
      float angle = bar->last_angle;
      while( 1 ) {
        if( radius > max_radius )
          break;
        if( angle >= 2.0*M_PI || angle <= -2.0*M_PI ) {
          ++radius;
          angle = 0.0;
          continue;
        }
        angle += 1.0/radius/M_PI;
        int test_col = floor(col+radius*cos(angle));
        if( test_col < 0 || test_col >= bar->width )
          continue;
        int test_row = floor(row+radius*sin(angle));
        if( test_row < 0 || test_row >= bar->height )
          continue;
        fill_pos = test_row*bar->width+test_col;
        if( fill_pos == bar->last_radial_pos )
          continue;
        if( bar->cells[fill_pos] && bar->cells[fill_pos] != bar_style->fill ) {
          if( bar_style->refill_behavior == BackgroundRefill)
            bar->cells[bar->last_radial_pos] = NULL;
          bar->last_radius = radius;
          bar->last_angle = angle;
          bar->cells[fill_pos] = bar_style->fill;
          redraw_cells = 1;
          bar->last_pos = cur_pos;
          bar->last_radial_pos = fill_pos;
          break;
        }
      }
    }
  }

  if( redraw_cells ) {
    do_flush = 1;
    set_cursor_pos(bar->row,bar->col);
    for( int i = 0, n = bar->width*bar->height; i < n; ++i ) {
      if( i > 0 && i % bar->width == 0 )
        fputc('\n',stdout);
      if( ! bar->cells[i] )
        fputs(bar_style->background,stdout);
      else
        fputs(bar->cells[i],stdout);
    }
  }

  if( n == 0 || now - bar->last_update > 0.1 ) {
    do_flush = 1;
    set_cursor_pos(bar->row+bar->height,bar->col+bar->width*bar_style->char_width);
    float pct = (float)n/bar->ntotal;
    if( bar->pct_style == Percent )
      fprintf(stdout, " %.2f%%  ", pct*100.0);
    else if( bar->pct_style == Count )
      fprintf(stdout, " %d / %d  ", n, bar->ntotal);
    else if( bar->pct_style == ByteCount ) {
      print_byte_size(bar->cnt, n);
      fprintf(stdout, " %s / %s  ", bar->cnt, bar->of_cnt);
    } else if( bar->pct_style == Time ) {
      int elapsed = (int)floor(now - bar->start_time);
      int hours = (int)floor(elapsed / 3600);
      int minutes = (elapsed % 3600) / 60;
      int seconds = elapsed % 60;
      fprintf(stdout, " %02d:%02d:%02d  ", hours, minutes, seconds);
    } else if( bar->pct_style == CountDown ) {
      int elapsed = (int)floor(now - bar->start_time);
      int hours = elapsed / 3600;
      int minutes = (elapsed % 3600) / 60;
      int seconds = elapsed % 60;
      fprintf(stdout, " %02d:%02d:%02d ", hours, minutes, seconds);
      if( pct > 0 ) {
        int estimated = (int)((now - bar->start_time) / pct);
        hours = estimated / 3600;
        minutes = (estimated % 3600) / 60;
        seconds = estimated % 60;
        fprintf(stdout, " / %02d:%02d:%02d ", hours, minutes, seconds);
      }
    }
    bar->last_update = now;
    bar->last_pct = pct;
  }

  if( do_flush )
    fflush(stdout);
}

void bar_finish(PBar *bar) {
  bar_update(bar,bar->ntotal);
  fputc('\n',stdout);
  // show the cursor
  printf("\033[?25h");
  fflush(stdout);
}

