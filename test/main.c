#include "emotxtprog.h"
#include <stdio.h>
#include <string.h>

struct BarStyles {
  char *name;
  PBarStyle style;
};

struct PctStyles {
  char *name;
  PPctStyle style;
};

struct BarStyles barstyles[] = {
  {"boring", Boring},
  {"triple", Triple},
  {"fire", Fire},
};

struct PctStyles pctstyles[] = {
  {"none", None},
  {"count", Count},
  {"bytecount", ByteCount},
};

static int startswith(const char *s, const char *lead) {
  return strlen(lead) <= strlen(s) && strncmp(s,lead,strlen(lead)) == 0;
}

int main(int argc, char *argv[]) {
  int styleidx = 0;
  int pctidx = 0;
  int w = 0;
  int width=40;
  for( int argcidx = 1; argcidx < argc; ++argcidx ) {
    char *arg = argv[argcidx];
    if( sscanf(arg,"--width=%d",&w) == 1 )
      width = w;
    if( startswith(arg,"--barstyle=") ) {
      char *s = arg+11;
      for( int i = 0; i < sizeof(barstyles)/sizeof(barstyles[0]); ++i ) {
        if( strcmp(barstyles[i].name,s) != 0 )
          continue;
        styleidx = i;
        break;
      }
    }
    if( startswith(arg,"--pctstyle=") ) {
      char *s = arg+11;
      for( int i = 0; i < sizeof(pctstyles)/sizeof(pctstyles[0]); ++i ) {
        if( strcmp(pctstyles[i].name,s) != 0 )
          continue;
        pctidx = i;
        break;
      }
    }
  }
  PBar bar;
  PBarFlags flags;
  int n = 1000000;
  bar_init(&bar,n,width,barstyles[styleidx].style, pctstyles[pctidx].style, flags);
  for( int i = 1; i <= n; ++i )
    bar_update(&bar, i);
  bar_finish(&bar);
  return 0;
}
