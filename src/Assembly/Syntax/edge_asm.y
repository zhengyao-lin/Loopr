%{
#include <stdio.h>
#include "EBS.h"
#include "Assembly.h"
#define YYDEBUG 1
%}
%union {
	char			*identifier;
	Bytecode		*bytecode;
    Statement		*statement;
	Constant		*constant;
	StatementList	*statement_list;
}
%token COMMA LP RP SEMICOLON DOT NEXT_LINE
%token <identifier>		IDENTIFIER
%token <constant>		CHAR_LITERAL
%token <constant>		INT32_LITERAL
%token <constant>		INT64_LITERAL
%token <constant>		DOUBLE_LITERAL
%token <constant>		SINGLE_LITERAL
%token <constant>		STRING_LITERAL
%token <constant>		WSTRING_LITERAL

%type <bytecode> dot_bytecode
%type <constant> constant constant_list constant_list_opt
%type <statement> statement
%type <statement_list> statement_list
%%
translation_unit
	: /* NULL */
	| statement_list
	{
		Asm_Compiler *current_compiler;
		current_compiler = Asm_get_current_compiler();
		current_compiler->list = $1;
	}
	;
dot_bytecode
	: IDENTIFIER
	{
		$$ = Asm_create_bytecode($1);
	}
	| dot_bytecode DOT IDENTIFIER
	{
		$$ = Asm_chain_bytecode($1, $3);
	}
	;

constant
	: CHAR_LITERAL
	| INT32_LITERAL
	| INT64_LITERAL
	| DOUBLE_LITERAL
	| SINGLE_LITERAL
	| STRING_LITERAL
	| WSTRING_LITERAL
	| LP INT32_LITERAL RP
	{
		($2)->type = CONST_BYTE;
		$$ = $2;
	}
	;

constant_list
	: constant
	| constant_list COMMA constant
	{
		$$ = Asm_chain_constant($1, $3);
	}
	;

constant_list_opt
	: /* NULL */
	{
		$$ = NULL;
	}
	| constant_list
	;

next_line_list
	: NEXT_LINE
	| next_line_list NEXT_LINE
	;
next_line_list_opt
	: /* NULL */
	| next_line_list
	;

statement
	: dot_bytecode constant_list_opt
	{
		$$ = Asm_create_statement($1, $2);
	}
	| dot_bytecode constant_list_opt SEMICOLON
	{
		$$ = Asm_create_statement($1, $2);
	}
	;

statement_list
	: statement next_line_list_opt
	{
		$$ = Asm_create_statement_list($1);
	}
	| statement_list statement next_line_list_opt
	{
		$$ = Asm_chain_statement_list($1, $2);
	}
	;
%%
