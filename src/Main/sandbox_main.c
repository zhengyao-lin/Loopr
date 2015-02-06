#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "MEM.h"
#include "UTL.h"
#include "DBG.h"
#include "Interfaces.h"

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	ByteContainer *container;
	setlocale(LC_ALL, "");
#if 1

	if (argc >= 2) {
		fp = fopen(argv[1], "r+b");
		if (fp == NULL) {
			DBG_panic(("Cannot open file %s\n", argv[1]));
		}
	} else {
		fprintf(stdout, "Loopr SandBox [test version]\n"
						"Bug report: https://github.com/Ivory-Next/Loopr\n");
		exit(0);
	}

	container = ISerialize_read_byte_container(fp);
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
