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
y.tab.h : edge_asm.y
	yacc -dv edge_asm.y
y.tab.c : edge_asm.y
	yacc -dv edge_asm.y
lex.yy.c : edge_asm.l edge_asm.y y.tab.h
	lex edge_asm.l
y.tab.o: y.tab.c ../../Includes/Assembly.h ../../Includes/MEM.h ../../Includes/EBS.h
	$(CC) -c -g $*.c $(INCLUDES)
lex.yy.o: lex.yy.c ../../Includes/Assembly.h ../../Includes/MEM.h ../../Includes/EBS.h
	$(CC) -c -g $*.c $(INCLUDES)
.c.o:
	$(CC) $(CFLAGS) $*.c $(INCLUDES)

create.o: create.c ../../Includes/Assembly.h ../../Includes/MEM.h ../../Includes/EBS.h 
string.o: string.c ../../Includes/Assembly.h ../../Includes/MEM.h ../../Includes/EBS.h ../../Includes/UTL.h
