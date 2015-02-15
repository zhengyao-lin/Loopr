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
Natives_load_all();

int main(int argc, char **argv)
{
	FILE *src = NULL;
	FILE *dest = NULL;
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

	Natives_load_all();
	container = Gencode_compile(Asm_compile_file(src));
	ISerialize_save_exe_environment(dest, Coding_init_exe_env(container, LPR_ANYTHING));
	fclose(src);
	fclose(dest);
	yylex_destroy();
	Asm_dispose_current_compiler();

#endif
Native_dispose_all();
Walle_dispose_byte_container(container, LPR_True);
MEM_dump_blocks(stderr);

	return 0;
}
