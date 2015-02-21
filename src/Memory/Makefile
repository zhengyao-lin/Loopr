TARGET = mem.o
CFLAGS = -fPIC -c -g -Wall
OBJS = memory.o storage.o

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
testp : $(OBJS) main.o
	$(CC) -o $@ $(OBJS) main.o
.c.o:
	$(CC) $(CFLAGS) -I../Includes $*.c
clean:
	-rm -f *.o

main.o: main.c MEM.h
memory.o: memory.c memory.h ../Includes/MEM.h
storage.o: storage.c memory.h ../Includes/MEM.h
