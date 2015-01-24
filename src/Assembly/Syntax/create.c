#include <stdlib.h>
#include "EBS.h"
#include "MEM.h"
#include "Assembly.h"

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
Asm_create_statement(Bytecode *code, Constant *const_opt)
{
	Statement *ret;

	ret = ASM_malloc(sizeof(Statement));
	ret->bytecode = code;
	ret->constant = const_opt;
	ret->line_number = get_current_line_number();

	return ret;
}

StatementList *
Asm_create_statement_list(Statement *st)
{
	StatementList *ret;

	ret = ASM_malloc(sizeof(StatementList));
	ret->statement = st;

	return ret;
}

StatementList *
Asm_chain_statement_list(StatementList *list, Statement *st)
{
	StatementList *pos;

	for (pos = list; pos->next; pos = pos->next)
		;
	pos->next = Asm_create_statement_list(st);

	return list;
}
