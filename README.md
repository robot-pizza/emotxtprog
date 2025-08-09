It's a text progress bar.  It uses emoticons when you ask it to.  Written in C.

Easy to use.

```C
#include "emotxtprog.h"

int main() {
  PBar bar;
  PBarFlags flags;
  int width = 80, height = 3;
  int n = 1000000;
  bar_init(&bar,n,width,height,Fire,ByteCount,flags);
  for( int i = 1; i <= n; i += n/100000 )
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
  Cats = 3,
  Robot = 4,
};

enum PPctStyle {
  None = 0,
  Percent = 1,
  Count = 2,
  ByteCount = 3,
};
```

Here's what it looks like.

<pre>
./test --width=80 --height=4 --barstyle=dull --pctstyle=bytecount
========================================
========================================
========================================
========================================                                         488.27 KiB / 976.56 KiB

./test --width=80 --height=4 --barstyle=fire --pctstyle=bytecount
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🌾🌾🌾🌾🌾🌾🏡🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🏡
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🏡🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🏡🌾🌾🌾🌾🌾🌾
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🌾🌾🌾🌾🌾🌾🏡🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🏡
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🏡🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🌾🏡🌾🌾🌾🌾🌾🌾 488.27 KiB / 976.56 KiB

./test --width=80 --height=4 --barstyle=flood --pctstyle=bytecount
🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🏖️        🏖️        🏖️        🏖️
🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊      🏖️        🏖️        🏖️        🏖️
🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🏖️        🏖️        🏖️        🏖️
🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊🌊      🏖️        🏖️        🏖️        🏖️   488.27 KiB / 976.56 KiB
  
 ./test --width=80 --height=4 --barstyle=cats --pctstyle=bytecount
😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺            🐟                        🐟
😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺🐟                        🐟
😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺            🐟                        🐟
😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺😺🐟                        🐟             488.27 KiB / 976.56 KiB

./test --width=80 --height=4 --barstyle=robot --pctstyle=bytecount
🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄
🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄
🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄
🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖🤖📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄📄 488.27 KiB / 976.56 KiB
</pre>


