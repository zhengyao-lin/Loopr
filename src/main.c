#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "SandBox_pri.h"
#include "MEM.h"
#include "UTL.h"
#include "DBG.h"
#include "Assembly.h"

ByteContainer *Gencode_compile(Asm_Compiler *compiler);

int main(int argc, char **argv)
{
	setlocale(LC_ALL, "");
#if 1
	FILE *fp;

	if (argc >= 2) {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			DBG_panic(("Cannot open file %s\n", argv[1]));
		}
	} else {
		fp = stdin;
	}

	ByteContainer *container = Gencode_compile(Asm_compile_file(fp));
	Edge_execute(Coding_init_exe_env(container, EDGE_ANYTHING));
#endif

#if 0
	/* testbed */

	ByteContainer *container = Coding_init_coding_env();
	Edge_Int32 i1 = 123;
	Edge_Int32 i2 = 22;

	Edge_Double lf1 = 3.141592654;
	Edge_Single f2 = 1.22222;

	Edge_Char *str1 = L"这是超级无敌的字符串和i你\0";
	Edge_Char *str2 = L"niha嗖嗖嗖嗖嗖嗖嗖嗖嗖嗖嗖嗖嗖嗖嗖嗖萨\0";
	int length1 = sizeof(Edge_Char) * (Edge_wcslen(str1) + sizeof(Edge_Char));
	int length2 = sizeof(Edge_Char) * (Edge_wcslen(str2) + sizeof(Edge_Char));
	Edge_Byte *identifier1 = "this is bob";
	int address = 14;

	Coding_push_code(container, EDGE_INIT, NULL, 0);
	Coding_push_code(container, EDGE_STRING, identifier1, strlen(identifier1) + 1);

	Coding_push_code(container, EDGE_LD_STRING, str1, length1);
	Coding_push_code(container, EDGE_MOV_VAR, MEM_strdup(identifier1), strlen(identifier1) + 1);
	Coding_push_code(container, EDGE_LD_VAR, identifier1, strlen(identifier1) + 1);

	Coding_push_code(container, EDGE_LD_STRING, str2, length2);
	Coding_push_code(container, EDGE_LD_STRING, str1, length1);

	Coding_push_code(container, EDGE_ADD_STRING, NULL, 0);
	Coding_push_code(container, EDGE_ADD_STRING, NULL, 0);

	Coding_push_code(container, EDGE_POP_STRING, NULL, 0);


	Coding_push_code(container, EDGE_LD_STRING, str2, length2);
	Coding_push_code(container, EDGE_MOV_VAR, MEM_strdup(identifier1), strlen(identifier1) + 1);
	Coding_push_code(container, EDGE_LD_VAR, MEM_strdup(identifier1), strlen(identifier1) + 1);
	Coding_push_code(container, EDGE_POP_STRING, NULL, 0);

	Coding_push_code(container, EDGE_LD_BYTE, NULL, 0);
	Coding_push_code(container, EDGE_SINGLE, Edge_byte_serialize(&f2, sizeof(f2)), sizeof(f2));

	Coding_push_code(container, EDGE_LD_BYTE, NULL, 0);
	Coding_push_code(container, EDGE_DOUBLE, Edge_byte_serialize(&lf1, sizeof(lf1)), sizeof(lf1));

	Coding_push_code(container, EDGE_ADD_FLOAT, NULL, 0);

	Coding_push_code(container, EDGE_POP_FLOAT, NULL, 0);

	Coding_push_code(container, EDGE_LD_BYTE, NULL, 0);
	Coding_push_code(container, EDGE_INT32, Edge_byte_serialize(&i1, sizeof(i1)), sizeof(i1));

	Coding_push_code(container, EDGE_LD_BYTE, NULL, 0);
	Coding_push_code(container, EDGE_INT32, Edge_byte_serialize(&i2, sizeof(i2)), sizeof(i2));

	Coding_push_code(container, EDGE_ADD_BYTE, NULL, 0);

	Coding_push_code(container, EDGE_POP_BYTE, NULL, 0);

	/*Coding_push_code(&container, EDGE_JUMP, &address, 1);*/

	Coding_push_code(container, EDGE_EXIT, NULL, 0);

	Edge_execute(Coding_init_exe_env(container, EDGE_ANYTHING));

	Walle_reset_mark();
	Walle_gcollect();

	/*MEM_free(stack.value);
	MEM_free(container.code);*/
#endif
	return 0;
}
