TARGET = Interfaces.o
CFLAGS = -c -g -Wall -DDEBUG
OBJS = executable.o
INCLUDES = \
	-I../Includes

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
clean:
	-rm -f *.o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $*.c
executable.o: executable.c ../Includes/LBS.h
