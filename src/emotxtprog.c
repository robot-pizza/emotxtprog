#define _XOPEN_SOURCE 600
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif
#include <fcntl.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "emotxtprog.h"

void get_cursor_pos(int *row, int *col) {
#ifdef _MSC_VER
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  COORD cursorPos = csbi.dwCursorPosition;
  *col = cursorPos.X;
  *row = cursorPos.Y;
  CONSOLE_CURSOR_INFO info;
  info.dwSize = sizeof(info);
  GetConsoleCursorInfo(hConsole,&info);
  info.bVisible = FALSE;
  SetConsoleCursorInfo(hConsole,&info);
#else 
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
#endif
}

static void set_cursor_pos(int row, int col) {
#ifdef _MSC_VER
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD coord = {col,row};
  SetConsoleCursorPosition(hConsole,coord);
#else 
  fprintf(stdout,"\033[%d;%dH", row, col);
  fflush(stdout);
#endif
}

static const char *bsizes[] = {"b","KiB","MiB","GiB","TiB","XiB"};

const char *printable_byte_size(size_t n0, double *pd) {
  int i = 0;
  size_t n = n0;
  double d = n;
  while( n > 1024 ) {
    d = n / 1024.0;
    n >>= 10;
    ++i;
  }
  *pd = d;
  if( i >= sizeof(bsizes)/sizeof(bsizes[0]) )
    return "?";
  return bsizes[i];
}

static void print_byte_size(FILE *fp, size_t n) {
  double d = 0.0;
  const char *suffix = printable_byte_size(n,&d);
  fprintf(fp, "%.2f %s", d, suffix);
}

static BarDrop animal_drops[] = {
  {"🦴",0.1},
  {NULL}};
static BarDrop house_drops[] = {
  {"🏚️",0.05},
  {NULL}};
static BarDrop tractor_drops[] = {
  {"🚒",0.05},
  {NULL}};
static BarDecorator fire_decorators[] = {
  {"🏡",0.03,house_drops},
  {"🚜",0.01,tractor_drops},
  {"🐄",0.006,animal_drops},
  {"🐐",0.003,animal_drops},
  {NULL}};
static BarDrop umbrella_drops[] = {
  {"🛟",0.10},
  {NULL}};
static BarDrop sand_drops[] = {
  {"🐚",0.5},
  {NULL}};
static BarDecorator flood_decorators[] = {
  {"🏖️",0.05,umbrella_drops},
  {"🪣",0.01,sand_drops},
  {"🏰",0.01,sand_drops},
  {NULL}};
static BarDrop fish_drops[] = {
  {"🍣",0.05},
  {NULL}};
static BarDrop bird_drops[] = {
  {"🍗",0.05},
  {NULL}};
static BarDecorator cat_decorators[] = {
  {"🐟",0.05,fish_drops},
  {"🐠",0.05,fish_drops},
  {"🐦",0.05,bird_drops},
  {"🦎",0.05,NULL},
  {"🍲",0.01,NULL},
  {NULL}};
static BarDrop paper_drops[] = {
  {"🧻",0.01},
  {NULL}};
static BarDrop power_drops[] = {
  {"🗲 ",0.01},
  {NULL}};
static BarDecorator robot_decorators[] = {
  {"📄",0.2,paper_drops},
  {"🔋",0.6,power_drops},
  {NULL}};
static BarDrop sock_drops[] = {
  {"🧶",0.25},
  {NULL}};
static BarDrop shrub_drops[] = {
  {"💩",0.005},
  {NULL}};
static BarDecorator goat_decorators[] = {
  {"🪴",0.01,NULL},
  {"🧦",0.01,sock_drops},
  {"🍕",0.005,NULL},
  {"🌿",1.0,shrub_drops},
  {NULL}};

static CustomBarStyle canned_bar_styles[] = {
 /* Dull */ {"="," ",1,NULL,NoRefill,FillLeftToRight},
 /* Fire */ {"🔥","🌾",2,fire_decorators,NoRefill,FillRadial},
 /* Flood */{"🌊","  ",2,flood_decorators,NoRefill,FillLeftToRight},
 /* Cats */ {"😺","  ",2,cat_decorators,BackgroundRefill,FillRadial},
 /* Robot */{"🤖","  ",2,robot_decorators,NoRefill,FillLeftToRight},
 /* Goat */ {"🐐","  ",2,goat_decorators,BackgroundRefill,FillRadial},
};

static void bar_init0(PBar *bar,
                      int ntotal,
                      int width,
                      int height,
                      int npct,
                      PPctStyle pct_style[MAX_N_PCT],
                      PBarStyle bar_style,
                      CustomBarStyle *custom_bar_style) {
#ifdef _MSC_VER
  SetConsoleOutputCP(CP_UTF8);
#endif
  bar->ntotal = ntotal;
  bar->width = width;
  bar->height = height;
  bar->n = 0;
  bar->bar_style = bar_style;
  bar->custom_bar_style = custom_bar_style;
  for( int i = 0; i < MAX_N_PCT; ++i ) {
    if( i < npct || i < (height-1) )
      bar->pct[i].pct_style = pct_style[i];
    else
      bar->pct[i].pct_style = None;
    bar->pct[i].last_pct = 0.0;
    bar->pct[i].last_update = 0.0;
  }
  bar->last_n = -1;
  if( height > 1 ) {
    for( int h = 0; h < height-1; ++h )
      fputc('\n',stdout);
    fflush(stdout);
  }
  get_cursor_pos(&bar->row,&bar->col);
  bar->row -= height-1;
  if( custom_bar_style->char_width > 1 )
    bar->width /= custom_bar_style->char_width;
  bar->cells = malloc(sizeof(char*)*bar->width*bar->height);
  bar->cellflags = malloc(bar->width*bar->height);
  memset(bar->cellflags,0,bar->width*bar->height);
  bar->last_value = 0;
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
  bar_update(bar,0);
}

void bar_init(PBar *bar,
              int ntotal,
              int width, 
              int height,
              int npct,
              PPctStyle pct_style[MAX_N_PCT],
              PBarStyle bar_style) {
  srand(time(NULL));
  if( bar_style == Custom )
    bar_style = Random;
  if( bar_style == Random )
    bar_style = rand()%Random;
  bar_init0(bar,ntotal,width,height,npct,pct_style,bar_style,canned_bar_styles+bar_style);
}

void bar_init_custom(PBar *bar,
                     int ntotal,
                     int width,
                     int height,
                     int npct,
                     PPctStyle pct_style[MAX_N_PCT],
                     CustomBarStyle *custom_bar_style) {
  bar_init0(bar,ntotal,width,height,npct,pct_style,Custom,custom_bar_style);
}

#ifdef _MSC_VER
static LARGE_INTEGER perf_freq = {.QuadPart = 0};
#endif

float clock_time() {
#ifdef _MSC_VER
  LARGE_INTEGER ticks;
  QueryPerformanceCounter(&ticks);
  if( perf_freq.QuadPart == 0 )
    QueryPerformanceFrequency(&perf_freq);
  return (double)ticks.QuadPart / (double)perf_freq.QuadPart;
#else
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC,&now);
  return now.tv_sec + now.tv_nsec / 1e9;
#endif
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

static void update_cell(PBar *bar, int i, const char *value) {
  if( i < 0 || i >= bar->width*bar->height || bar->cells[i] == value )
    return;
  int row = i/bar->width;
  int col = i%bar->width;
  set_cursor_pos(row,col);
  bar->cells[i] = value;
  set_cursor_pos(bar->row+row,bar->col+col*bar->custom_bar_style->char_width);
  if( ! bar->cells[i] )
    fputs(bar->custom_bar_style->background,stdout);
  else
    fputs(bar->cells[i],stdout);
  fflush(stdout);
}

static const char *get_drop(PBar *bar, const char *last_value) {
  if( ! last_value )
    return NULL;
  CustomBarStyle *bar_style = bar->custom_bar_style;
  for( BarDecorator *decorator = bar_style->decorators; decorator && decorator->decorator; ++decorator ) {
    if( decorator->decorator != last_value )
      continue;
    for( BarDrop *drop = decorator->drops; drop && drop->drop; ++drop ) {
      if( rand()/(float)RAND_MAX <= drop->likelyhood )
        return drop->drop;
    }
    break;
  }
  return NULL;
}

void bar_update(PBar *bar, float n) {
  float now = clock_time();
  float pct = (float)n/bar->ntotal;
  float pos_pct = 1.0/bar->width/bar->height;
  CustomBarStyle *bar_style = bar->custom_bar_style;
  int last_pos = bar->last_pos;
  int cur_pos = last_pos;

  if( bar_style->fill_path == FillLeftToRight ) { 
    cur_pos = (int)floor(pct/pos_pct);
    while( last_pos < cur_pos ) {
      int last_col, last_row, last_i;
      last_i = pos2cri(bar, last_pos, &last_col, &last_row);
      const char *drop = get_drop(bar,bar->last_value);
      if( drop )
        update_cell(bar,last_i,drop);
      else if( bar_style->refill_behavior == BackgroundRefill)
        update_cell(bar,last_i,NULL);
      ++last_pos;
      last_i = pos2cri(bar, last_pos, &last_col, &last_row);
      bar->last_value = bar->cells[last_i];
      update_cell(bar,last_i,bar_style->fill);
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
        if( bar->cellflags[fill_pos] == 0 ) {
          bar->cellflags[fill_pos] = 1;
          const char *drop = get_drop(bar,bar->last_value);
          if( drop )
            update_cell(bar,bar->last_radial_pos,drop);
          else if( bar_style->refill_behavior == BackgroundRefill)
            update_cell(bar,bar->last_radial_pos,NULL);
          bar->last_radius = radius;
          bar->last_angle = angle;
          bar->last_value = bar->cells[fill_pos];
          update_cell(bar,fill_pos,bar_style->fill);
          bar->last_pos = cur_pos;
          bar->last_radial_pos = fill_pos;
          break;
        }
      }
    }
  }

  if( bar->last_n == -1 ) {
    bar->start_time = now;
    set_cursor_pos(bar->row,bar->col);
    for( int i = 0, n = bar->width*bar->height; i < n; ++i ) {
      if( i > 0 && i % bar->width == 0 )
        fputc('\n',stdout);
      if( ! bar->cells[i] )
        fputs(bar_style->background,stdout);
      else
        fputs(bar->cells[i],stdout);
    }
    fflush(stdout);
  }

  for( int i = 0; i < MAX_N_PCT; ++i ) {
    PPct *pctind = bar->pct+i;
    if( pctind->pct_style == None || pctind->pct_style == Text )
      continue;
    if( bar->last_n == -1 )
      pctind->last_update = now;
    if( bar->last_n != 0 && n != bar->ntotal && now - pctind->last_update <= PCT_UPDATE_TOL )
      continue;
    set_cursor_pos(bar->row+bar->height-1-i,bar->col+bar->width*bar_style->char_width);
    float pct = (float)n/bar->ntotal;
    if( pctind->pct_style == Percent )
      fprintf(stdout, " %.2f%%  ", pct*100.0);
    else if( pctind->pct_style == Count )
      fprintf(stdout, " %d / %d  ", (int)n, bar->ntotal);
    else if( pctind->pct_style == ByteCount ) {
      fputs(" ",stdout);
      print_byte_size(stdout, n);
      fputs(" / ",stdout);
      print_byte_size(stdout, bar->ntotal);
      fputs("  ",stdout);
    } else if( pctind->pct_style == Time ) {
      int elapsed = (int)floor(now - bar->start_time);
      int hours = (int)floor(elapsed / 3600);
      int minutes = (elapsed % 3600) / 60;
      int seconds = elapsed % 60;
      fprintf(stdout, " %02d:%02d:%02d  ", hours, minutes, seconds);
    } else if( pctind->pct_style == CountDown ) {
      int elapsed = (int)floor(now - bar->start_time);
      int hours = elapsed / 3600;
      int minutes = (elapsed % 3600) / 60;
      int seconds = elapsed % 60;
      fprintf(stdout, " %02d:%02d:%02d", hours, minutes, seconds);
      if( pct > 0 ) {
        int estimated = (int)((now - bar->start_time) / pct);
        hours = estimated / 3600;
        minutes = (estimated % 3600) / 60;
        seconds = estimated % 60;
        fprintf(stdout, " / %02d:%02d:%02d  ", hours, minutes, seconds);
      } else {
        fputs("  ",stdout);
      }
    }
    pctind->last_update = now;
    pctind->last_pct = pct;
    fflush(stdout);
  }
  bar->last_n = n >= 0 ? n : 0;
}

void bar_update_text(PBar *bar, int idx, const char *text) {
  if( idx < 0 || idx >= MAX_N_PCT || bar->pct[idx].pct_style != Text )
    return;
  set_cursor_pos(bar->row+bar->height-1-idx,bar->col+bar->width*bar->custom_bar_style->char_width+1);
  fputs(text,stdout);
  fflush(stdout);
}

void bar_finish(PBar *bar) {
  for( int i = 0; i < MAX_N_PCT; ++i )
    bar->pct[i].last_update = 0;
  bar_update(bar, bar->last_n);
  set_cursor_pos(bar->row+bar->height-1,0);
#ifdef MSC_VER
  CONSOLE_CURSOR_INFO info;
  info.dwSize = sizeof(info);
  GetConsoleCursorInfo(hConsole,&info);
  info.bVisible = FALSE;
  SetConsoleCursorInfo(hConsole,&info);
#else
  fputs("\n",stdout);
  fflush(stdout);
  // show the cursor
  printf("\033[?25h");
#endif
  fflush(stdout);
}

