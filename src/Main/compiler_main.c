#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "SandBox_pri.h"
#include "MEM.h"
#include "UTL.h"
#include "DBG.h"
#include "Assembler.h"
#include "Interfaces.h"
#define DEFAULT_EXE "a.lexe"

ByteContainer *Gencode_compile(Asm_Compiler *compiler);
int yylex_destroy();

int main(int argc, char **argv)
{
	FILE *src = NULL;
	FILE *dest = NULL;
	Asm_Compiler *compiler;
	ByteContainer *container;
	setlocale(LC_ALL, "");
#if 1

	if (argc >= 2) {
		src = fopen(argv[1], "r");
		if (src == NULL) {
			DBG_panic(("Cannot open source file %s\n", argv[1]));
		}
		if (argc >= 3) {
			dest = fopen(argv[2], "w+b");
		} else {
			dest = fopen(DEFAULT_EXE, "w+b");
		}
		if (dest == NULL) {
			DBG_panic(("Cannot open dest file %s\n", argv[2]));
		}
	} else {
		src = stdin;
	}

	Native_load_lib("Natives/Natives.so");
	compiler = Asm_compile_file(src);
	container = Gencode_compile(compiler);
	ISerialize_save_exe_environment(dest, Coding_init_exe_env(container, LPR_ANYTHING));
	fclose(src);
	fclose(dest);
	Asm_dispose_compiler(compiler, LPR_True);

#endif
Native_dispose_all();
Walle_dispose_byte_container(container, LPR_True);
MEM_dump_blocks(stderr);

	return 0;
}
