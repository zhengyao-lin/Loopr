#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "SandBox_pri.h"
#include "MEM.h"
#include "UTL.h"
#include "DBG.h"
#include "Interfaces.h"
#include "Versions.h"

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	ExeEnvironment *env;

	setlocale(LC_ALL, "");
#if 1

	if (argc >= 2) {
		fp = fopen(argv[1], "r+b");
		if (fp == NULL) {
			DBG_panic(("Cannot open file %s\n", argv[1]));
		}
	} else {
		fprintf(stdout, "Loopr SandBox [ " VERSION " ]\n"
						"Bug report: https://github.com/Ivory-Next/Loopr\n");
		exit(0);
	}

	Native_load_lib("Natives/Natives.so");
	env = ISerialize_read_exe_environment(fp);
	fclose(fp);

	Loopr_execute(env, LPR_True);
#endif

Walle_update_alive_period();
Walle_gcollect();
Native_dispose_all();
Walle_dispose_environment(env);
MEM_dump_blocks(stderr);

	return 0;
}
