#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "SandBox_pri.h"
#include "MEM.h"
#include "UTL.h"
#include "DBG.h"
#include "Assembler.h"
#include "Versions.h"

ByteContainer *Gencode_compile(Asm_Compiler *compiler);
int yylex_destroy();

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	ByteContainer *container;
	ExeEnvironment *env;
	Asm_Compiler *compiler;

	setlocale(LC_ALL, "");
#if 1

	if (argc >= 2) {
		fp = fopen(argv[1], INTERPRETER_FILE_MODE);
		if (fp == NULL) {
			DBG_panic(("Cannot open file %s\n", argv[1]));
		}
	} else {
		fp = stdin;
	}

	Native_load_lib("Natives/Natives.so");
	compiler = Asm_compile_file(fp);
	fclose(fp);

	container = Gencode_compile(compiler);
	Asm_dispose_compiler(compiler, LPR_True);
	env = Coding_init_exe_env(container, LPR_ANYTHING);

	Loopr_execute(env, LPR_True);
#endif

Walle_update_alive_period();
Walle_gcollect();
Walle_dispose_environment(env);
Native_dispose_all();
Walle_dispose_byte_container(container, LPR_False);
MEM_dump_blocks(stderr);

	return 0;
}
