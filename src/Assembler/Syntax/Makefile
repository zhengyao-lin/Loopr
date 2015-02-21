TARGET = Syntax.o
CFLAGS = -c -g -Wall -ansi -pedantic -Wswitch-enum -DDEBUG
OBJS = \
	lex.yy.o \
	y.tab.o \
	create.o \
	string.o

INCLUDES = \
	-I../../Includes \
	-I../

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)

clean:
	rm -f *.o lex.yy.c y.tab.c y.tab.h y.output *~
y.tab.h : loopr_asm.y
	yacc -dv loopr_asm.y
y.tab.c : loopr_asm.y
	yacc -dv loopr_asm.y
lex.yy.c : loopr_asm.l loopr_asm.y y.tab.h
	lex loopr_asm.l
y.tab.o: y.tab.c ../../Includes/Assembler.h ../../Includes/DBG.h ../../Includes/MEM.h ../../Includes/LBS.h
	$(CC) -c -g $*.c $(INCLUDES)
lex.yy.o: lex.yy.c ../../Includes/Assembler.h ../../Includes/DBG.h ../../Includes/LBS.h
	$(CC) -c -g  $*.c $(INCLUDES)
.c.o:
	$(CC) $(CFLAGS) $*.c $(INCLUDES)

create.o: create.c ../../Includes/Assembler.h ../../Includes/MEM.h ../../Includes/DBG.h ../../Includes/LBS.h
string.o: string.c ../../Includes/Assembler.h ../../Includes/MEM.h ../../Includes/DBG.h ../../Includes/LBS.h ../../Includes/UTL.h
