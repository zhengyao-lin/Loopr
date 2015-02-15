TARGET = Natives.o
CFLAGS = -c -g -Wall
OBJS = \
	just_test.o
INCLUDES = \
	-I../Includes \
	-I../SandBox

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
clean:
	-rm -f *.o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $*.c

just_test.o: just_test.c ../SandBox/SandBox_pri.h ../Includes/MEM.h ../Includes/DBG.h
