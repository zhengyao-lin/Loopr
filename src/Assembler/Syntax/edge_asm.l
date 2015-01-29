
DIGIT			[0-9]
LETTER			[a-zA-Z_]

%{
#include <stdio.h>
#include <stdlib.h>
#include "LBS.h"
#include "DBG.h"
#include "Assembler.h"
#include "y.tab.h"

int
yywrap(void)
{
    return 1;
}

int yyerror(char* msg)
{
	printf("Syntax Error: at line %d\n", Asm_get_current_compiler()->current_line_number);
}

static void
increment_line_number(void)
{
    Asm_get_current_compiler()->current_line_number++;
}

Loopr_Boolean st_character_literal_state;
%}
%start C_COMMENT CC_COMMENT STRING_LITERAL_STATE
%%
 /* Punctuations */
<INITIAL>","		return COMMA;
<INITIAL>"("		return LP;
<INITIAL>")"		return RP;
<INITIAL>";"		return SEMICOLON;
<INITIAL>"."		return DOT;

 /* Identifiers and Constants */
<INITIAL>{LETTER}({LETTER}|{DIGIT})* {
	yylval.identifier = Asm_create_identifier(yytext);
	return IDENTIFIER;
}
<INITIAL>[1-9][0-9]* {
	Constant *constant = Asm_alloc_constant(CONST_INT32);
	sscanf(yytext, "%d", &constant->u.int32_value);
	yylval.constant = constant;
	return INT32_LITERAL;
}
<INITIAL>"0"[xX][0-9a-fA-F]+ {
	Constant *constant = Asm_alloc_constant(CONST_INT32);
	sscanf(yytext, "%x", &constant->u.int32_value);
	yylval.constant = constant;
	return INT32_LITERAL;
}
<INITIAL>"0"[0-7]+ {
	Constant *constant = Asm_alloc_constant(CONST_INT32);
	sscanf(yytext, "%o", &constant->u.int32_value);
	yylval.constant = constant;
	return INT32_LITERAL;
}
<INITIAL>"0" {
	Constant *constant = Asm_alloc_constant(CONST_INT32);
	constant->u.int32_value = 0;
	yylval.constant = constant;
	return INT32_LITERAL;
}
<INITIAL>[1-9][0-9]*[lL] {
	Constant *constant = Asm_alloc_constant(CONST_INT64);
	sscanf(yytext, "%ld", &constant->u.int64_value);
	yylval.constant = constant;
	return INT64_LITERAL;
}
<INITIAL>"0"[xX][0-9a-fA-F]+[lL] {
	Constant *constant = Asm_alloc_constant(CONST_INT64);
	sscanf(yytext, "%lx", &constant->u.int64_value);
	yylval.constant = constant;
	return INT64_LITERAL;
}
<INITIAL>"0"[lL] {
	Constant *constant = Asm_alloc_constant(CONST_INT64);
	constant->u.int64_value = 0;
	yylval.constant = constant;
	return INT64_LITERAL;
}
<INITIAL>[0-9]+\.[0-9]+[lL] {
	Constant *constant = Asm_alloc_constant(CONST_DOUBLE);
	sscanf(yytext, "%lf", &constant->u.double_value);
	yylval.constant = constant;
	return DOUBLE_LITERAL;
}
<INITIAL>[0-9]+\.[0-9]{6,}+ {
	Constant *constant = Asm_alloc_constant(CONST_DOUBLE);
	sscanf(yytext, "%lf", &constant->u.double_value);
	yylval.constant = constant;
	return DOUBLE_LITERAL;
}
<INITIAL>[0-9]+\.[0-9]{1,5}+ {
	Constant *constant = Asm_alloc_constant(CONST_SINGLE);
	sscanf(yytext, "%f", &constant->u.single_value);
	yylval.constant = constant;
	return SINGLE_LITERAL;
}

 /* Invisible Tokens */
<INITIAL>[ \t]		;
<INITIAL>[\r\n] {
	increment_line_number();
	return NEXT_LINE;
}

 /* Comments */
<INITIAL>"/*"     	BEGIN C_COMMENT;
<INITIAL>"//"     	BEGIN CC_COMMENT;

<C_COMMENT>[\r\n]	increment_line_number();
<C_COMMENT>"*/"		BEGIN INITIAL;
<C_COMMENT><<EOF>>	abort();
<C_COMMENT>.		;

<CC_COMMENT>\n {
	increment_line_number();
	BEGIN INITIAL;
}
<CC_COMMENT><<EOF>>	BEGIN INITIAL;
<CC_COMMENT>.		;

 /* String Constants */
<INITIAL>\' {
    Asm_open_string_literal();
    st_character_literal_state = LPR_True;
    BEGIN STRING_LITERAL_STATE;
}
<INITIAL>\" {
    Asm_open_string_literal();
    st_character_literal_state = LPR_False;
    BEGIN STRING_LITERAL_STATE;
}

<STRING_LITERAL_STATE>\" {
    if (st_character_literal_state) {
        Asm_add_string_literal('\"');
    } else {
		Constant *constant = Asm_alloc_constant(CONST_STRING);
		constant->u.string_value = Asm_close_string_literal();
		yylval.constant = constant;
		BEGIN INITIAL;
		return STRING_LITERAL;
    }
}
<STRING_LITERAL_STATE>\' {
    if (st_character_literal_state) {
        Constant *constant = Asm_alloc_constant(CONST_CHAR);
        constant->u.char_value = Asm_close_character_literal();
		yylval.constant = constant;
        BEGIN INITIAL;
        return CHAR_LITERAL;
    } else {
        Asm_add_string_literal('\'');
    }
}
<STRING_LITERAL_STATE>\\[0-7]{1,3} {
	int letter;
	sscanf(&yytext[1], "%o", &letter);
    Asm_add_string_literal(letter);
}
<STRING_LITERAL_STATE>\\[xX][0-9a-fA-F]{1,2} {
	int letter;
	sscanf(&yytext[2], "%x", &letter);
    Asm_add_string_literal(letter);
}
<STRING_LITERAL_STATE>\n        {
    Asm_add_string_literal('\n');
    increment_line_number();
}
<STRING_LITERAL_STATE>\\\"      Asm_add_string_literal('"');
<STRING_LITERAL_STATE>\\n       Asm_add_string_literal('\n');
<STRING_LITERAL_STATE>\\t       Asm_add_string_literal('\t');
<STRING_LITERAL_STATE>\\\\      Asm_add_string_literal('\\');
<STRING_LITERAL_STATE><<EOF>>   {
	DBG_panic(("EOF in string literal\n"));
}
<STRING_LITERAL_STATE>.         {
    Asm_add_string_literal(yytext[0]);
}
%%
