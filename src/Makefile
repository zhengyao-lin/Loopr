TARGET = testbed
TARGET_SANDBOX = sandbox
TARGET_COMPILER = compiler
CFLAGS = -c -g -DDEBUG -Wall -ansi -pedantic -Wswitch-enum -Wswitch
OBJS = \
	SandBox/SandBox.o \
	Memory/mem.o \
	Debug/dbg.o \
	Utils/utl.o \
	Interfaces/Interfaces.o
INCLUDES = \
	-IIncludes \
	-ISandBox

$(TARGET):$(OBJS) Assembler/Assembler.o Main/interpreter_main.o Natives/Natives.so $(TARGET_SANDBOX) $(TARGET_COMPILER)
	$(CC) $(OBJS) Assembler/Assembler.o interpreter_main.o -o $@ -lm -ldl

$(TARGET_SANDBOX):$(OBJS) Main/sandbox_main.o
	$(CC) $(OBJS) sandbox_main.o -o $@ -lm -ldl

$(TARGET_COMPILER):$(OBJS) Assembler/Assembler.o Main/compiler_main.o
	$(CC) $(OBJS) Assembler/Assembler.o compiler_main.o -o $@ -lm -ldl

.c.o:
	$(CC) $(CFLAGS) $*.c $(INCLUDES)

clean:
	cd Memory; $(MAKE) clean;
	cd SandBox; $(MAKE) clean;
	cd Debug; $(MAKE) clean;
	cd Utils; $(MAKE) clean;
	cd Assembler; $(MAKE) clean;
	cd Interfaces; $(MAKE) clean;
	cd Natives; $(MAKE) clean;
	-rm -f *.o *.lexe $(TARGET) $(TARGET_SANDBOX) $(TARGET_COMPILER) *~

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
Interfaces/Interfaces.o:
	cd Interfaces; $(MAKE);
Natives/Natives.so:
	cd Natives; $(MAKE);

Main/interpreter_main.o: Main/interpreter_main.c SandBox/SandBox_pri.h Includes/MEM.h Includes/DBG.h Includes/UTL.h Includes/Assembler.h Includes/Versions.h
Main/sandbox_main.o: Main/sandbox_main.c SandBox/SandBox_pri.h Includes/MEM.h Includes/DBG.h Includes/UTL.h Includes/Interfaces.h Includes/Versions.h
Main/compiler_main.o: Main/compiler_main.c SandBox/SandBox_pri.h Includes/MEM.h Includes/DBG.h Includes/UTL.h Includes/Assembler.h
