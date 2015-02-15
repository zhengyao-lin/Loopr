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
%token COLON COMMA LB RB LP RP DOT
	   NEXT_LINE NULL_LITERAL
%token <identifier>		IDENTIFIER
%token <constant>		CHAR_LITERAL
%token <constant>		DIGIT_LITERAL
%token <constant>		FLOAT_LITERAL
%token <constant>		STRING_LITERAL
%token <constant>		KEYWORD_LITERAL

%type <bytecode> dot_bytecode compiler_ref
%type <constant> constant constant_list constant_list_opt block
%type <statement> statement label_statement
%type <statement_list> statement_list
%%
/*************** Frame ***************/
translation_unit
	: /* NULL */
	| translation_unit top_level_unit translation_unit
	;
top_level_unit
	: next_line_list_opt statement_list next_line_list_opt
	{
		Asm_Compiler *current_compiler;
		current_compiler = Asm_get_current_compiler();
		if (current_compiler->top_level) {
			Asm_cat_statement_list(current_compiler->top_level,
								   $2);
		} else {
			current_compiler->top_level = $2;
		}
	}
	;

/*************** Detail Syntax ***************/
dot_bytecode
	: IDENTIFIER
	{
		$$ = Asm_create_bytecode($1, 0, LPR_False);
	}
	| DIGIT_LITERAL
	{
		$$ = Asm_create_bytecode(NULL, $1->u.byte_value, LPR_True);
		ASM_free($1);
	}
	| dot_bytecode DOT IDENTIFIER
	{
		$$ = Asm_chain_bytecode($1, $3, 0, LPR_False);
	}
	| dot_bytecode DOT DIGIT_LITERAL
	{
		$$ = Asm_chain_bytecode($1, NULL, $3->u.byte_value, LPR_True);
		ASM_free($3);
	}
	;
compiler_ref
	: DOT IDENTIFIER
	{
		$$ = Asm_chain_bytecode(Asm_create_bytecode(NULL, 0, LPR_False),
								$2, 0, LPR_False);
	}
	| DOT DIGIT_LITERAL
	{
		$$ = Asm_chain_bytecode(Asm_create_bytecode(NULL, 0, LPR_False),
								NULL, $2->u.byte_value, LPR_True);
		ASM_free($2);
	}
	;
block
	: LB next_line_list_opt
	  statement_list
	  next_line_list_opt RB
	{
		$$ = Asm_create_block($3);
	}
	;
constant
	: CHAR_LITERAL
	| KEYWORD_LITERAL
	| DIGIT_LITERAL
	| FLOAT_LITERAL
	| STRING_LITERAL
	| IDENTIFIER
	{
		Constant *constant = Asm_alloc_constant(CONST_LABEL);
		constant->u.string_value = $1;
		$$ = constant;
	}
	| block
	;
constant_list
	: constant
	| constant_list COMMA constant
	{
		$$ = Asm_chain_constant($1, $3);
	}
	| constant_list constant
	{
		$$ = Asm_chain_constant($1, $2);
	}
	| constant_list LP constant_list_opt RP
	{
		Constant *pos;
		for (pos = $1; pos->next; pos = pos->next);
		pos->next = $3;

		$$ = $1;
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
label_statement
	: IDENTIFIER COLON next_line_list_opt
	{
		$$ = Asm_create_statement($1, NULL, NULL);
	}
	| IDENTIFIER COLON next_line_list_opt block
	{
		$$ = Asm_create_statement($1, NULL, $4);
	}
	;
statement
	: label_statement
	| dot_bytecode constant_list_opt NEXT_LINE
	{
		$$ = Asm_create_statement(NULL, $1, $2);
	}
	| compiler_ref constant_list_opt NEXT_LINE
	{	
		$$ = Asm_create_statement(NULL, $1, $2);
	}
	;
statement_list
	: statement next_line_list_opt
	{
		$$ = Asm_create_statement_list($1);
	}
	| statement next_line_list_opt statement_list
	{
		$$ = Asm_chain_statement_list($1, $3);
	}
	;
%%
