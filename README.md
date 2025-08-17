It's a text progress bar.  It uses emojis when you ask it to.  Written in C.

Easy to use.

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

Different Options

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

Here's what it looks like.

TODO: Put gif here


