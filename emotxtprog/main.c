#include "emotxtprog.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct BarStyles {
  char *name;
  PBarStyle style;
};

struct PctStyles {
  char *name;
  PPctStyle style;
};

struct RefillBehaviors {
  char *name;
  RefillBehavior behavior;
};

struct FillPaths {
  char *name;
  FillPath path;
};

struct BarStyles barstyles[] = {
  {"dull", Dull},
  {"fire", Fire},
  {"flood", Flood},
  {"cat", Cat},
  {"robot", Robot},
  {"goat", Goat},
  {"random", Random},
  {"custom", Custom},
};

struct PctStyles pctstyles[] = {
  {"none", None},
  {"percent", Percent},
  {"count", Count},
  {"byte-count", ByteCount},
  {"time", Time},
  {"count-down", CountDown},
};

struct RefillBehaviors refill_behaviors[] = {
  {"no-refill", NoRefill},
  {"background-refill", BackgroundRefill},
};

struct FillPaths fill_paths[] = {
  {"left-to-right", FillLeftToRight},
  {"radial", FillRadial},
};

static int startswith(const char *s, const char *lead) {
  return strlen(lead) <= strlen(s) && strncmp(s,lead,strlen(lead)) == 0;
}

static void print_help() {
  fputs("emotxtprog\n"
        "    [--width=WIDTH]\n"
        "    [--height=HEIGHT]\n"
        "    [--bar-style=dull|fire|flood|cats|robot|goat|random|custom]\n"
        "    [--pct-style=none|percent|count|byte-count|time|count-down]\n"
        "    [--refill-behavior=no-refill|background-refill]\n"
        "    [--fill-path=left-to-right|radial]\n"
        "    [--fill=FILL]\n"
        "    [--background=BACKGROUND]\n"
        "    [--decorator=DENSITY.BACKGROUND]\n"
        , stdout);
}

int main(int argc, char *argv[]) {
  PBarStyle bar_style = Dull;
  PPctStyle pct_style = None;
  RefillBehavior refill_behavior = NoRefill;
  FillPath fill_path = FillLeftToRight;
  int tmp = 0;
  int width=40;
  int height=1;
  for( int argcidx = 1; argcidx < argc; ++argcidx ) {
    char *arg = argv[argcidx];
    if( startswith(arg,"--help") ) {
      print_help();
      exit(0);
    }
    if( sscanf(arg,"--width=%d",&tmp) == 1 )
      width = tmp;
    if( sscanf(arg,"--height=%d",&tmp) == 1 )
      height = tmp;
    if( startswith(arg,"--bar-style=") ) {
      char *s = arg+12;
      for( int i = 0; i < sizeof(barstyles)/sizeof(barstyles[0]); ++i ) {
        if( strcmp(barstyles[i].name,s) != 0 )
          continue;
        bar_style = barstyles[i].style;
        break;
      }
    }
    if( startswith(arg,"--pct-style=") ) {
      char *s = arg+12;
      for( int i = 0; i < sizeof(pctstyles)/sizeof(pctstyles[0]); ++i ) {
        if( strcmp(pctstyles[i].name,s) != 0 )
          continue;
        pct_style = pctstyles[i].style;
        break;
      }
    }
    if( startswith(arg,"--refill-behavior=") ) {
      char *s = arg+18;
      for( int i = 0; i < sizeof(refill_behaviors)/sizeof(refill_behaviors[0]); ++i ) {
        if( strcmp(refill_behaviors[i].name,s) != 0 )
          continue;
        refill_behavior = refill_behaviors[i].behavior;
        break;
      }
    }
    if( startswith(arg,"--fill-path=") ) {
      char *s = arg+12;
      for( int i = 0; i < sizeof(fill_paths)/sizeof(fill_paths[0]); ++i ) {
        if( strcmp(fill_paths[i].name,s) != 0 )
          continue;
        fill_path = fill_paths[i].path;
        break;
      }
    }
  }
  PBar bar;
  int n = width*height;
  int nsec = 10;
  bar_init(&bar,n,width,height,pct_style,bar_style);
  for( int i = 1; i <= n; i += 1 ) {
    usleep(nsec*1000000/n);
    bar_update(&bar, i);
  }
  bar_finish(&bar);
  return 0;
}
