#include <stdio.h>
#include <stdlib.h>
#include "LBS.h"
#include "Assembler.h"
#include "MEM.h"

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
	ret->current_line_number = 0;
	ret->function_count = 0;
	ret->function_definition = NULL;

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
		fprintf(stderr, "Unexpected error!\n");
		exit(0);
	}

	Asm_reset_string_literal_buffer();

	return compiler;
}

void
Asm_clean_local_env(ByteContainer *env)
{
	int i;

	for (i = 0; i < env->local_variable_count; i++) {
		if (env->local_variable[i].identifier) {
			MEM_free(env->local_variable[i].identifier);
		}
	}
	if (env->local_variable) {
		MEM_free(env->local_variable);
	}

	return;
}

void
Asm_dispose_compiler(Asm_Compiler *compiler)
{
	int i;

	for (i = 0; i < compiler->function_count; i++) {
		MEM_free(compiler->function_definition[i].name);
	}
	MEM_free(compiler->function_definition);

	ASM_free(compiler);
	return;
}

void
Asm_dispose_current_compiler()
{
	Asm_dispose_compiler(Asm_get_current_compiler());
	return;
}
