#include <string.h>
#include "LBS.h"
#include "MEM.h"
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
	Asm_Compiler *compiler;
	Statement *ret;

	compiler = Asm_get_current_compiler();
	ret = ASM_malloc(sizeof(Statement));
	ret->label = label;
	ret->bytecode = code;
	ret->constant = const_opt;
	ret->line_number = get_current_line_number();

	if (code && code->name == NULL && code->next
		&& !strcmp(code->next->name, Loopr_CR_Info[LCR_FUNCTION].assembly_name)) {
		compiler->function_definition = MEM_realloc(compiler->function_definition,
													sizeof(FunctionDefinition) * (compiler->function_count + 1));
		compiler->function_definition[compiler->function_count].name = MEM_strdup(const_opt->u.string_value);
		compiler->function_count++;
	}

	return ret;
}

StatementList *
Asm_create_statement_list(Statement *st)
{
	StatementList *ret = NULL;

	ret = ASM_malloc(sizeof(StatementList));
	ret->statement = st;

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
