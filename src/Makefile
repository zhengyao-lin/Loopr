TARGET = testbed
CFLAGS = -c -g -DDEBUG -Wall -ansi -pedantic -Wswitch-enum -Wswitch
OBJS = \
	SandBox/SandBox.o \
	Memory/mem.o \
	Debug/dbg.o \
	Utils/utl.o \
	Assembly/Assembly.o \
	main.o
INCLUDES = \
	-IIncludes \
	-ISandBox

$(TARGET):$(OBJS)
	$(CC) $(OBJS) -o $@ -lm #-lefence -lpthread

.c.o:
	$(CC) $(CFLAGS) $*.c $(INCLUDES)

clean:
	cd Memory; $(MAKE) clean;
	cd SandBox; $(MAKE) clean;
	cd Debug; $(MAKE) clean;
	cd Utils; $(MAKE) clean;
	cd Assembly; $(MAKE) clean;
	-rm -f *.o $(TARGET) *~

Memory/mem.o:
	cd Memory; $(MAKE);
SandBox/SandBox.o:
	cd SandBox; $(MAKE);
Debug/dbg.o:
	cd Debug; $(MAKE);
Utils/utl.o:
	cd Utils; $(MAKE);
Assembly/Assembly.o:
	cd Assembly; $(MAKE);

main.o: main.c SandBox/SandBox_pri.h Includes/MEM.h
