TARGET = SandBox.o
CFLAGS = -c -g -DDEBUG -Wall -ansi -pedantic -Wswitch-enum
OBJS = \
	execute.o \
	value.o \
	coding.o \
	wall-e.o

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -I../Includes $*.c
clean:
	-rm -f *.o

execute.o: execute.c SandBox_pri.h ../Includes/LBS.h ../Includes/UTL.h ../Includes/DBG.h
value.o: value.c SandBox_pri.h ../Includes/LBS.h ../Includes/UTL.h
coding.o: coding.c SandBox_pri.h ../Includes/LBS.h ../Includes/UTL.h
wall-e.o: wall-e.c SandBox_pri.h ../Includes/LBS.h ../Includes/UTL.h
