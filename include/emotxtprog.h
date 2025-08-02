#ifndef __emotxtprog_h
#define __emotxtprog_h

enum PBarStyle {
  Boring = 0,
  Triple = 1,
  Fire = 2,
};
typedef enum PBarStyle PBarStyle;

enum PPctStyle {
  None = 0,
  Percent = 1,
  Count = 2,
  ByteCount = 3,
};
typedef enum PPctStyle PPctStyle;

struct PBarFlags {
  int hello: 1;
};
typedef struct PBarFlags PBarFlags;

struct PBar {
  int ntotal, width, n, row, col;
  PBarStyle bar_style;
  PPctStyle pct_style;
  PBarFlags flags;
  char cnt[20];
  char of_cnt[20];
};
typedef struct PBar PBar;

void bar_init(PBar *bar, int ntotal, int width, PBarStyle bar_style, PPctStyle pct_style, PBarFlags flags);
void bar_update(PBar *bar, int n);
void bar_finish(PBar *bar);

#endif // __emotxtprog_h
