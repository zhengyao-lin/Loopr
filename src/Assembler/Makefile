TARGET = Assembler.o
CFLAGS = -c -g -DDEBUG -Wall -ansi -pedantic -Wswitch-enum
OBJS = \
	Syntax/Syntax.o \
	interface.o \
	label.o \
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

interface.o: interface.c ../Includes/LBS.h ../Includes/Assembler.h
gencode.o: gencode.c ../Includes/LBS.h ../Includes/MEM.h ../Includes/DBG.h ../Includes/Assembler.h ../SandBox/SandBox_pri.h
label.o: label.c ../Includes/LBS.h ../Includes/MEM.h ../Includes/DBG.h ../Includes/Assembler.h ../SandBox/SandBox_pri.h
#main.o: main.c Syntax/syntax.h global.h
