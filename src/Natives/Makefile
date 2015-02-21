TARGET = Natives.so
CFLAGS = -fPIC -c -g -Wall
OBJS = \
	just_test.o \
	../Memory/mem.o \
	../Debug/dbg.o
INCLUDES = \
	-I../Includes \
	-I../SandBox

$(TARGET):$(OBJS)
	$(CC) -shared -o $@ $(OBJS)
clean:
	-rm -f *.o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $*.c
../Memory/mem.o:
	cd ../Memory; $(MAKE);
../Debug/dbg.o:
	cd ../Debug; $(MAKE);

just_test.o: just_test.c ../SandBox/SandBox_pri.h ../Includes/MEM.h ../Includes/DBG.h ../Includes/LBS.h
