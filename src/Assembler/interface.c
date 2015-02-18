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
	ret->default_name_space = NULL;
	ret->current_name_space_index = -1;
	ret->name_space_count = 0;
	ret->name_space = NULL;
	ret->current_line_number = 0;

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
	UsingList *pos;

	for (; env->using_list;
		 pos = env->using_list, env->using_list = env->using_list->next, ASM_free(pos));

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
Asm_dispose_name_space(NameSpace *ns)
{
	int i;

	MEM_free(ns->name);
	for (i = 0; i < ns->function_count; i++) {
		MEM_free(ns->function_definition[i].name);
	}
	MEM_free(ns->function_definition);

	return;
}

void
Asm_dispose_compiler(Asm_Compiler *compiler)
{
	int i;

	if (compiler->default_name_space) {
		MEM_free(compiler->default_name_space);
	}

	for (i = 0; i < compiler->name_space_count; i++) {
		Asm_dispose_name_space(&compiler->name_space[i]);
	}
	MEM_free(compiler->name_space);

	ASM_free(compiler);
	return;
}

void
Asm_dispose_current_compiler()
{
	Asm_dispose_compiler(Asm_get_current_compiler());
	return;
}
