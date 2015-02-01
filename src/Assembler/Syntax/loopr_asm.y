%{
#include <stdio.h>
#include "LBS.h"
#include "MEM.h"
#include "Assembler.h"
#define YYDEBUG 1
%}
%union {
	char			*identifier;
	Bytecode		*bytecode;
    Statement		*statement;
	Constant		*constant;
	StatementList	*statement_list;
}
%token COLON COMMA LP RP DOT NEXT_LINE
	   NULL_LITERAL
%token <identifier>		IDENTIFIER
%token <constant>		CHAR_LITERAL
%token <constant>		DIGIT_LITERAL
%token <constant>		FLOAT_LITERAL
%token <constant>		STRING_LITERAL
%token <constant>		TRUE_C FALSE_C

%type <identifier> label
%type <bytecode> dot_bytecode
%type <constant> constant constant_list constant_list_opt
%type <statement> statement
%type <statement_list> statement_list
%%
translation_unit
	: /* NULL */
	| next_line_list_opt statement_list
	{
		Asm_Compiler *current_compiler;
		current_compiler = Asm_get_current_compiler();
		current_compiler->list = $2;
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
	| DIGIT_LITERAL
	| FLOAT_LITERAL
	| STRING_LITERAL
	| TRUE_C
	| FALSE_C
	| IDENTIFIER
	{
		Constant *constant = Asm_alloc_constant(CONST_STRING);
		constant->u.string_value = $1;
		$$ = constant;
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
label
	: IDENTIFIER COLON next_line_list_opt
	{
		$$ = $1;
	}
	;
statement
	: label dot_bytecode constant_list_opt
	{
		$$ = Asm_create_statement($1, $2, $3);
	}
	| dot_bytecode constant_list_opt
	{
		$$ = Asm_create_statement(NULL, $1, $2);
	}
	;
statement_list
	: statement next_line_list
	{
		$$ = Asm_create_statement_list($1);
	}
	| statement next_line_list statement_list
	{
		$$ = Asm_chain_statement_list($1, $3);
	}
	;
%%
