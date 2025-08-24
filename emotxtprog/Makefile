# NMAKE code here \
!MESSAGE Using NMAKE # \
!ifndef 0 # \
emotxtprog=emotxtprog.exe # \
MV=move # \
RM=del # \
CP=copy # \
CC_OUT=/Fe # \
CFLAGS=/std:c17 /I. /I../include user32.lib # \
!ifdef DEBUG # \
##When debugging using drmemory or termdbg # \
CFLAGS=$(CFLAGS) /Zi # \
!else # \
##Optimized build # \
CFLAGS=$(CFLAGS) /O2 # \
!endif # \
SRC=..\src\*.c main.c # \
HDRS=..\include\*.h # \
!else
# Make code here
$(info Using MAKE)
emotxtprog=emotxtprog
MV=mv -f
RM=rm -f
CP=cp -f
CC_OUT=-o
CFLAGS=-I ./ -I ../include
if DEBUG
##When debugging using drmemory or termdbg
CFLAGS+=-g -gdwarf-4 -fno-inline -fno-omit-frame-pointer -std=gnu99
else
##Optimized build
CFLAGS+= -O3 -std=gnu99 -lm
endif
SRC=$(wildcard ../src/*.c) main.c
HDRS=$(wildcard ../include/*.h)
# \
!endif

all: emotxtprog

# NMAKE \
emotxtprog: $(emotxtprog)

$(emotxtprog): $(SRC) $(HDRS)
	$(CC) $(SRC) $(CFLAGS) $(CC_OUT)$(emotxtprog)

clean:
	-@$(RM) $(emotxtprog)
