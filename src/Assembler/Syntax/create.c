#include <stdlib.h>
#include "LBS.h"
#include "MEM.h"
#include "Assembler.h"

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
Asm_create_bytecode(char *identifier)
{
	Bytecode *ret;

	ret = ASM_malloc(sizeof(Bytecode));
	ret->name = identifier;
	ret->next = NULL;
	ret->line_number = get_current_line_number();

	return ret;
}

Bytecode *
Asm_chain_bytecode(Bytecode *list, char *identifier)
{
	Bytecode *pos;

	for (pos = list; pos->next; pos = pos->next)
		;
	pos->next = Asm_create_bytecode(identifier);

	return list;
}

Statement *
Asm_create_statement(char *label, Bytecode *code, Constant *const_opt)
{
	Statement *ret;

	ret = ASM_malloc(sizeof(Statement));
	ret->label = label;
	ret->bytecode = code;
	ret->constant = const_opt;
	ret->line_number = get_current_line_number();

	return ret;
}

StatementList *
Asm_create_statement_list(Statement *st)
{
	extern char *yytext;
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
