#ifndef __emotxtprog_h
#define __emotxtprog_h

enum PBarStyle {
  Dull = 0,
  Fire = 1,
  Flood = 2,
  Cat = 3,
  Robot = 4,
  Goat = 5,
  Random = 6,
  Custom = 7,
};
typedef enum PBarStyle PBarStyle;

enum PPctStyle {
  None = 0,
  Percent = 1,
  Count = 2,
  ByteCount = 3,
  Time = 4,
  CountDown = 5,
};
typedef enum PPctStyle PPctStyle;

struct BarDecorator {
  const char *decorator;   /* decorator character */
  float density;           /* 0.0 through 1.0 */
};
typedef struct BarDecorator BarDecorator;

enum RefillBehavior {
  NoRefill = 0,
  BackgroundRefill = 1
};
typedef enum RefillBehavior RefillBehavior;

enum FillPath {
  FillLeftToRight = 0,
  FillRadial = 1,
};
typedef enum FillPath FillPath;

struct CustomBarStyle {
  const char *fill;       /* Indicates progress */
  const char *background; /* Background character */
  int char_width;          /* Width of each character */
  BarDecorator *decorators;
  RefillBehavior refill_behavior;
  FillPath fill_path;
};
typedef struct CustomBarStyle CustomBarStyle;

struct PBar {
  int ntotal, width, height, n, row, col;
  PBarStyle bar_style;
  PPctStyle pct_style;
  CustomBarStyle *custom_bar_style;
  const char **cells;
  char cnt[20];
  char of_cnt[20];
  float last_pct;
  float last_update;
  float start_time;
  int last_pos;
  int radial_center, last_radial_pos;
  int last_radius;
  float last_angle;
};
typedef struct PBar PBar;

void bar_init(PBar *bar, int ntotal, int width, int height, PPctStyle pct_style, PBarStyle bar_style);
void bar_init_custom(PBar *bar, int ntotal, int width, int height, PPctStyle pct_style, CustomBarStyle *custom_bar_style);
void bar_update(PBar *bar, int n);
void bar_finish(PBar *bar);

#endif // __emotxtprog_h
