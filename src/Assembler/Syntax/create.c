#include <string.h>
#include "LBS.h"
#include "MEM.h"
#include "DBG.h"
#include "Assembler.h"

extern ByteInfo Loopr_CR_Info[];

int
get_current_line_number()
{
	return Asm_get_current_compiler()->current_line_number;
}

Constant *
Asm_alloc_constant(ConstantType type)
{
	Constant *ret;

	ret = ASM_malloc(sizeof(Constant));
	ASM_fill(ret->u, NULL_VALUE);
	ret->type = type;
	ret->next = NULL;
	ret->line_number = get_current_line_number();

	return ret;
}

Constant *
Asm_chain_constant(Constant *list, Constant *add)
{
	Constant *pos;

	for (pos = list; pos->next; pos = pos->next)
		;
	pos->next = add;

	return list;
}

Bytecode *
Asm_create_bytecode(char *identifier, Loopr_Byte code, Loopr_Boolean has_fixed)
{
	Bytecode *ret;

	ret = ASM_malloc(sizeof(Bytecode));

	ret->name = identifier;
	ret->has_fixed = has_fixed;
	ret->code = code;
	ret->next = NULL;
	ret->line_number = get_current_line_number();

	return ret;
}

Bytecode *
Asm_chain_bytecode(Bytecode *list, char *identifier, Loopr_Byte code, Loopr_Boolean has_fixed)
{
	Bytecode *pos;

	for (pos = list; pos->next; pos = pos->next)
		;
	pos->next = Asm_create_bytecode(identifier, code, has_fixed);

	return list;
}

Statement *
Asm_create_statement(char *label, Bytecode *code, Constant *const_opt)
{
	NameSpace *name_space;
	Statement *ret;

	name_space = &Asm_get_current_compiler()->name_space[Asm_get_current_compiler()->current_name_space_index];
	ret = ASM_malloc(sizeof(Statement));
	ret->label = label;
	ret->bytecode = code;
	ret->constant = const_opt;
	ret->line_number = get_current_line_number();

	if (code && code->name == NULL && code->next
		&& !strcmp(code->next->name, Loopr_CR_Info[LCR_FUNCTION].assembly_name)) {
		if (name_space->function_definition) {
			name_space->function_definition = MEM_realloc(name_space->function_definition,
														sizeof(FunctionDefinition) * (name_space->function_count + 1));
		} else {
			name_space->function_definition = MEM_malloc(sizeof(FunctionDefinition) * (name_space->function_count + 1));
		}

		name_space->function_definition[name_space->function_count].is_void = LPR_False;
		while (const_opt && const_opt->type == CONST_KEYWORD) {
			switch (const_opt->u.keyword_value) {
				case ASM_VOID:
					name_space->function_definition[name_space->function_count].is_void = LPR_True;
					const_opt = const_opt->next;
					break;
				default:
					DBG_panic(("line %d: Unknown keyword %d\n", const_opt->line_number, const_opt->u.keyword_value));
					break;
			}
		}
		name_space->function_definition[name_space->function_count].name = MEM_strdup(const_opt->u.string_value);
		name_space->function_count++;
	}

	return ret;
}

StatementList *
Asm_create_statement_list(Statement *st)
{
	StatementList *ret = NULL;

	ret = ASM_malloc(sizeof(StatementList));
	ret->statement = st;
	ret->next = NULL;

	return ret;
}

StatementList *
Asm_cat_statement_list(StatementList *list, StatementList *addin)
{
	StatementList *pos;

	for (pos = list; pos && pos->next; pos = pos->next)
		;
	pos->next = addin;

	return pos;
}

StatementList *
Asm_chain_statement_list(Statement *st, StatementList *list)
{
	StatementList *pos;

	pos = Asm_create_statement_list(st);
	pos->next = list;

	return pos;
}

Constant *
Asm_create_block(StatementList *list)
{
	Constant *ret;

	ret = Asm_alloc_constant(CONST_BLOCK);
	ret->u.block = list;

	return ret;
}

void
Asm_begin_namespace(char *name)
{
	Asm_Compiler *compiler;

	compiler = Asm_get_current_compiler();
	compiler->name_space = MEM_realloc(compiler->name_space,
									   sizeof(NameSpace) * (compiler->name_space_count + 1));
	compiler->name_space[compiler->name_space_count].name = name;
	compiler->name_space[compiler->name_space_count].top_level = NULL;
	compiler->name_space[compiler->name_space_count].function_count = 0;
	compiler->name_space[compiler->name_space_count].function_definition = NULL;
	compiler->current_name_space_index = compiler->name_space_count;
	compiler->name_space_count++;

	return;
}

PackageName *
Asm_create_package_name(char *name)
{
	PackageName *ret;

	ret = ASM_malloc(sizeof(PackageName));
	ret->name = name;
	ret->next = NULL;

	return ret;
}

PackageName *
Asm_chain_package_name(char *name, PackageName *list)
{
	PackageName *ret;

	ret = Asm_create_package_name(name);
	ret->next = list;

	return ret;
}

void
Asm_chain_import_list(PackageName *name)
{
	Asm_Compiler *compiler;
	ImportList *pos;

	compiler = Asm_get_current_compiler();
	if (pos = compiler->import_list) {
		for (; pos->next; pos = pos->next);
		pos->next = ASM_malloc(sizeof(ImportList));
		pos->next->name = name;
		pos->next->next = NULL;
	} else {
		compiler->import_list = ASM_malloc(sizeof(ImportList));
		compiler->import_list->name = name;
		compiler->import_list->next = NULL;
	}

	return;
}
