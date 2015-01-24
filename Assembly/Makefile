TARGET = Assembly.o
CFLAGS = -c -g -DDEBUG -Wall -ansi -pedantic -Wswitch-enum
OBJS = \
	Syntax/Syntax.o \
	interface.o \
	gencode.o

INCLUDES = \
	-I../Includes \
	-ISyntax \
	-I../SandBox

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $*.c $(INCLUDES)
clean:
	cd Syntax; $(MAKE) clean;
	-rm -f *.o *~ test
Syntax/Syntax.o:
	cd Syntax; $(MAKE);
#test:
#	$(CC) -o test $(OBJS)

interface.o: interface.c ../Includes/EBS.h ../Includes/Assembly.h
gencode.o: gencode.c ../Includes/EBS.h ../Includes/MEM.h ../Includes/DBG.h ../Includes/Assembly.h ../SandBox/SandBox_pri.h
#main.o: main.c Syntax/syntax.h global.h
