#Optimized build
#CFLAGS=-I ./ -I ../include -O3 -std=gnu99
#When debugging using drmemory or termdbg
CFLAGS=-g -I ./ -I ../include -gdwarf-4 -fno-inline -fno-omit-frame-pointer -std=gnu99
SRC=$(wildcard ../src/*.c) main.c
HDRS=$(wildcard ../include/*.h)

all: tinyai

tinyai: $(SRC) $(HDRS)
	gcc $(SRC) $(CFLAGS) -lm -o test

clean:
	-@rm -f test
