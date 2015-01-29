TARGET = testbed
CFLAGS = -c -g -DDEBUG -Wall -ansi -pedantic -Wswitch-enum -Wswitch
OBJS = \
	SandBox/SandBox.o \
	Memory/mem.o \
	Debug/dbg.o \
	Utils/utl.o \
	Assembler/Assembler.o \
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
	cd Assembler; $(MAKE) clean;
	-rm -f *.o $(TARGET) *~

Memory/mem.o:
	cd Memory; $(MAKE);
SandBox/SandBox.o:
	cd SandBox; $(MAKE);
Debug/dbg.o:
	cd Debug; $(MAKE);
Utils/utl.o:
	cd Utils; $(MAKE);
Assembler/Assembler.o:
	cd Assembler; $(MAKE);

main.o: main.c SandBox/SandBox_pri.h Includes/MEM.h
