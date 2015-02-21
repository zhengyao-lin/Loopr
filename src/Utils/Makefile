TARGET = utl.o
CFLAGS = -c -g -Wall -DDEBUG
OBJS = wchar.o
INCLUDES = -I../Includes

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
clean:
	-rm -f *.o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $*.c
wchar.o: wchar.c ../Includes/LBS.h ../Includes/DBG.h ../Includes/MEM.h
