#include <stdio.h>
#include <stdlib.h>
#include "LBS.h"
#include "Assembler.h"

Asm_Compiler *__current_compiler;

void
Asm_set_current_compiler(Asm_Compiler *compiler)
{
	__current_compiler = compiler;
	return;
}

Asm_Compiler *
Asm_get_current_compiler()
{
	return __current_compiler;
}

Asm_Compiler *
Asm_init_compiler()
{
	Asm_Compiler *ret;

	ret = ASM_malloc(sizeof(Asm_Compiler));
	ret->top_level = NULL;
	ret->current_line_number = 1;

	return ret;
}

Asm_Compiler *
Asm_compile_file(FILE *fp)
{
	extern FILE *yyin;
	extern int yyparse(void);

	Asm_Compiler *compiler;

	compiler = Asm_init_compiler();
	Asm_set_current_compiler(compiler);

	yyin = fp;
	if (yyparse()) {
		fprintf(stderr, "UNEXPECTED ERROR!\n");
		exit(0);
	}

	Asm_reset_string_literal_buffer();

	return compiler;
}
