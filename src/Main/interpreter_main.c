#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "SandBox_pri.h"
#include "MEM.h"
#include "UTL.h"
#include "DBG.h"
#include "Assembler.h"

ByteContainer *Gencode_compile(Asm_Compiler *compiler);

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	ByteContainer *container;
	setlocale(LC_ALL, "");
#if 1

	if (argc >= 2) {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			DBG_panic(("Cannot open file %s\n", argv[1]));
		}
	} else {
		fp = stdin;
	}

	container = Gencode_compile(Asm_compile_file(fp));
	fclose(fp);
	ExeEnvironment *env = Coding_init_exe_env(container, LPR_ANYTHING);

	Loopr_execute(env);
#endif

Walle_reset_mark();
Walle_gcollect();
Walle_dispose_environment(env);
MEM_free(container);
MEM_dump_blocks(stderr);

	return 0;
}
