#include <stdio.h>
#include <stdlib.h>
#include "LBS.h"
#include "MEM.h"
#include "Assembler.h"
#include "Versions.h"

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
	ret->import_list = NULL;
	ret->default_name_space = NULL;
	ret->current_name_space_index = -1;
	ret->name_space_count = 0;
	ret->name_space = NULL;
	ret->current_line_number = 0;

	return ret;
}

FILE *
Asm_open_import_file(PackageName *name)
{
	FILE *ret;
	PackageName *pos;
	char *file_name = NULL;

	for (pos = name; pos; pos = pos->next) {
		if (file_name) {
			file_name = MEM_realloc(file_name, sizeof(char) * (strlen(file_name) + strlen(pos->name) + 2));
			strcat(file_name, FILE_SEPARATER);
			strcat(file_name, pos->name);
		} else {
			file_name = MEM_strdup(pos->name);
		}
	}
	file_name = MEM_realloc(file_name, sizeof(char) * (strlen(file_name) + strlen(FILE_SUFFIX) + 1));
	strcat(file_name, FILE_SUFFIX);

	ret = fopen(file_name, INTERPRETER_FILE_MODE);

	MEM_free(file_name);
	if (!ret) {
		fprintf(stderr, "cannot find import file %s\n", file_name);
		exit(0);
	}

	return ret;
}

void
Asm_compiler_name_space_cat(Asm_Compiler *dest, Asm_Compiler *src)
{
	int i;

	dest->name_space = MEM_realloc(dest->name_space,
								   sizeof(NameSpace) * (dest->name_space_count + src->name_space_count));
	for (i = dest->name_space_count;
		 i < (dest->name_space_count + src->name_space_count);
		 i++) {
		dest->name_space[i] = src->name_space[i - dest->name_space_count];
	}
	dest->name_space_count += src->name_space_count;

	return;
}

void
Asm_dispose_package_name(PackageName *pn)
{
	PackageName *pos;
	PackageName *last;

	for (pos = pn; pos; last = pos, pos = pos->next, ASM_free(last)) {
		if (pos->name) {
			MEM_free(pos->name);
		}
	}

	return;
}

Asm_Compiler *
Asm_compile_file(FILE *fp)
{
	extern FILE *yyin;
	extern int yyparse(void);

	ImportList *pos;
	ImportList *last;
	Asm_Compiler *compiler;
	Asm_Compiler *tmp;

	compiler = Asm_init_compiler();
	Asm_set_current_compiler(compiler);

	yyin = fp;
	if (yyparse()) {
		fprintf(stderr, "Unexpected error!\n");
		exit(0);
	}
	yylex_destroy();

	for (pos = compiler->import_list; pos; last = pos, pos = pos->next, ASM_free(last)) {
		fp = Asm_open_import_file(pos->name);
		Asm_dispose_package_name(pos->name);
		tmp = Asm_compile_file(fp);
		fclose(fp);
		Asm_compiler_name_space_cat(compiler, tmp);
		Asm_dispose_compiler(tmp, LPR_False);
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

	for (i = 0; i < env->sub_name_space_count; i++) {
		if (i != env->self_reflect) {
			Asm_clean_local_env(env->sub_name_space[i]);
		}
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
Asm_dispose_compiler(Asm_Compiler *compiler, Loopr_Boolean clean_name_space_flag)
{
	int i;

	if (compiler->default_name_space) {
		MEM_free(compiler->default_name_space);
	}

	if (clean_name_space_flag) {
		for (i = 0; i < compiler->name_space_count; i++) {
			Asm_dispose_name_space(&compiler->name_space[i]);
		}
	}
	MEM_free(compiler->name_space);

	ASM_free(compiler);
	return;
}

void
Asm_dispose_current_compiler()
{
	Asm_dispose_compiler(Asm_get_current_compiler(), LPR_True);
	return;
}
