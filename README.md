It's the worlds greatest text progress bar. Written in C. Windows or Linux.

![pbar-opt](https://github.com/user-attachments/assets/c59d8887-1b39-4cfb-9ab2-8c835802bd21)

By command line

```bash
emotxtprog --width=80 --pct-style=count-down --count=10 --increment=time

./myprog.py | emotxtprog --width=80 --height=3 --pct-style=percent --bar-style=goat --count=1000 --increment=byte
```

By Code

```C
#include "emotxtprog.h"

int main() {
  PBar bar;
  int width = 80, height = 3;
  int n = 100;
  bar_init(&bar,n,width,height,ByteCount,Random);
  for( int i = 1; i <= n; ++i )
    bar_update(&bar, i);
  bar_finish(&bar);
  return 0;
}
```

Various Options

```C
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

enum PPctStyle {
  None = 0,
  Percent = 1,
  Count = 2,
  ByteCount = 3,
  Time = 4,
  CountDown = 5,
};
```
