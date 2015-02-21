TARGET = SandBox.o
CFLAGS = -c -g -DDEBUG -Wall -ansi -pedantic -Wswitch-enum -std=c99
OBJS = \
	execute.o \
	value.o \
	coding.o \
	wall-e.o \
	native.o

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -I../Includes $*.c
clean:
	-rm -f *.o

execute.o: execute.c SandBox_pri.h ../Includes/LBS.h ../Includes/UTL.h ../Includes/MEM.h ../Includes/DBG.h
value.o: value.c SandBox_pri.h ../Includes/LBS.h ../Includes/MEM.h ../Includes/UTL.h ../Includes/DBG.h
coding.o: coding.c SandBox_pri.h ../Includes/LBS.h ../Includes/MEM.h ../Includes/DBG.h
wall-e.o: wall-e.c SandBox_pri.h ../Includes/LBS.h ../Includes/MEM.h
native.o: native.c SandBox_pri.h ../Includes/LBS.h ../Includes/UTL.h ../Includes/MEM.h ../Includes/DBG.h ../Includes/Versions.h
