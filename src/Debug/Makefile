TARGET = dbg.o
CFLAGS =  -fPIC -c -g -Wall -DDBG_NO_DEBUG
OBJS = debug.o
INCLUDES = -I../Includes

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
clean:
	-rm -f *.o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $*.c
debug.o: debug.c ../Includes/MEM.h debug.h ../Includes/DBG.h
