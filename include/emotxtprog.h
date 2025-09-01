#ifndef __emotxtprog_h
#define __emotxtprog_h

#define MAX_N_PCT (4)
#define PCT_UPDATE_TOL (0.1)

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
  Text = 6,
};
typedef enum PPctStyle PPctStyle;

struct BarDrop {
  const char *drop;   /* drop character */
  float likelyhood;   /* 0.0 through 1.0 */
};
typedef struct BarDrop BarDrop;

struct BarDecorator {
  const char *decorator;   /* decorator character */
  float density;           /* 0.0 through 1.0 */
  BarDrop *drops;
};
typedef struct BarDecorator BarDecorator;

enum RefillBehavior {
  NoRefill = 0,
  BackgroundRefill = 1
};
typedef enum RefillBehavior RefillBehavior;

enum FillRoute {
  FillLeftToRight = 0,
  FillRadial = 1,
};
typedef enum FillRoute FillRoute;

struct CustomBarStyle {
  const char *fill;       /* Indicates progress */
  const char *background; /* Background character */
  int char_width;          /* Width of each character */
  BarDecorator *decorators;
  RefillBehavior refill_behavior;
  FillRoute fill_path;
};
typedef struct CustomBarStyle CustomBarStyle;

struct PPct {
  float last_pct;
  float last_update;
  PPctStyle pct_style;
};
typedef struct PPct PPct;

struct PBar {
  int ntotal, width, height, n, row, col;
  PBarStyle bar_style;
  CustomBarStyle *custom_bar_style;
  const char **cells;
  unsigned char *cellflags;
  const char *last_value;
  float start_time;
  int last_pos;
  int radial_center, last_radial_pos;
  int last_radius;
  float last_angle;
  float last_n;
  PPct pct[MAX_N_PCT];
};
typedef struct PBar PBar;

void bar_init(PBar *bar,
              int ntotal,
              int width,
              int height,
              int npct,
              PPctStyle pct_style[MAX_N_PCT],
              PBarStyle bar_style);
void bar_init_custom(PBar *bar,
                     int ntotal,
                     int width,
                     int height,
                     int npct,
                     PPctStyle pct_style[MAX_N_PCT],
                     CustomBarStyle *custom_bar_style);
void bar_update(PBar *bar, float n);
void bar_finish(PBar *bar);

float clock_time();

#endif // __emotxtprog_h
