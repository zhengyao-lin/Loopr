
SP				[ \t\v\f]
NEWLINE			[\r\n]
PM				[+-]
DIGIT			[0-9]
HEX				[a-fA-F0-9]
OCT				[0-7]
LETTER			[a-zA-Z_]
ISUFFIX			(l|L|s|S|u|U)+
IPREFIX			(#)
FSUFFIX			(l|L)

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
	printf("Syntax Error: line %d: %s\n", Asm_get_current_compiler()->current_line_number
										, yytext);
}

static void
increment_line_number(void)
{
    Asm_get_current_compiler()->current_line_number++;
}

void
get_float_const()
{
	Constant *constant;
	char suffix = yytext[yyleng-1];

	switch (suffix) {
		case 'l':
		case 'L':
			constant = Asm_alloc_constant(CONST_DOUBLE);
			sscanf(yytext, "%lf", &constant->u.double_value);
			yylval.constant = constant;
			break;
		default:
			constant = Asm_alloc_constant(CONST_SINGLE);
			sscanf(yytext, "%f", &constant->u.single_value);
			yylval.constant = constant;
			break;
	}

	return;
}

void
get_digit_const(char *controller)
{
	Constant *constant;
	ConstantType type = 0;
	int p = yyleng - 1;
	int offset = 0;
	char suffix = yytext[p];

	if (yytext[0] == '#') {
		type = CONST_BYTE;
		offset = 1;
		goto ENDLOOP;
	}

LOOP:
	switch (suffix) {
		case 'l':
		case 'L':
			type += CONST_INT64;
			break;
		case 's':
		case 'S':
			type += CONST_INT16;
			break;
		case 'u':
		case 'U':
			type++;
			break;
		default:
			if (type <= 1) {
				type += CONST_INT32;
				goto ENDLOOP;
			} else if (type > CONST_UINT64) {
				DBG_panic(("Too much digit  affix\n"));
			} else {
				goto ENDLOOP;
			}
	}
	if (p > 0) {
		p--;
		suffix = yytext[p];
		goto LOOP;
	}

ENDLOOP:
	constant = Asm_alloc_constant(type);
	sscanf(yytext + offset, controller, &constant->u.int64_value);
	yylval.constant = constant;

	return;
}

typedef enum {
	ASM_DEC,
	ASM_HEX,
	ASM_OCT
} DIGIT_TYPE;

Loopr_Boolean st_character_literal_state;
%}
%start C_COMMENT CC_COMMENT STRING_LITERAL_STATE
%%
 /* Punctuations */
<INITIAL>"namespace"	return NAMESPACE;
<INITIAL>"default"	return DEFAULT;
<INITIAL>"import"		return IMPORT;
<INITIAL>"void" {
	Constant *constant;
	constant = Asm_alloc_constant(CONST_KEYWORD);
	constant->u.keyword_value = ASM_VOID;
	yylval.constant = constant;
	return KEYWORD_LITERAL;
}
<INITIAL>"true" {
	Constant *constant;
	constant = Asm_alloc_constant(CONST_BYTE);
	constant->u.byte_value = LPR_True;
	yylval.constant = constant;
	return DIGIT_LITERAL;
}
<INITIAL>"false" {
	Constant *constant;
	constant = Asm_alloc_constant(CONST_BYTE);
	constant->u.byte_value = LPR_False;
	yylval.constant = constant;
	return DIGIT_LITERAL;
}
<INITIAL>":"		return COLON;
<INITIAL>","		return COMMA;
<INITIAL>"{"		return LC;
<INITIAL>"}"		return RC;
<INITIAL>"("		return LP;
<INITIAL>")"		return RP;
<INITIAL>"["		return LB;
<INITIAL>"]"		return RB;
<INITIAL>"<"		return LAB;
<INITIAL>">"		return RAB;
<INITIAL>"."		return DOT;

 /* Identifiers and Constants */
<INITIAL>{LETTER}({LETTER}|{DIGIT})* {
	yylval.identifier = Asm_create_identifier(yytext);
	return IDENTIFIER;
}

<INITIAL>{IPREFIX}?{PM}?"0"{OCT}+{ISUFFIX}? {
	get_digit_const("%lo");
	return DIGIT_LITERAL;
}
<INITIAL>{IPREFIX}?{PM}?"0"[xX]{HEX}+{ISUFFIX}? {
	get_digit_const("%lx");
	return DIGIT_LITERAL;
}
<INITIAL>{IPREFIX}?{PM}?{DIGIT}+{ISUFFIX}? {
	get_digit_const("%lu");
	return DIGIT_LITERAL;
}

<INITIAL>{PM}?{DIGIT}+\.{DIGIT}+{FSUFFIX}? {
	get_float_const();
	return FLOAT_LITERAL;
}

 /* Invisible Tokens */
<INITIAL>{SP}		;
<INITIAL>{NEWLINE} {
	increment_line_number();
	return NEXT_LINE;
}

 /* Comments */
<INITIAL>"/*"     	BEGIN C_COMMENT;
<INITIAL>"//"     	BEGIN CC_COMMENT;
<INITIAL>";"     	BEGIN CC_COMMENT;

<C_COMMENT>{NEWLINE} {
	increment_line_number();
	return NEXT_LINE;
}
<C_COMMENT>"*/"		BEGIN INITIAL;
<C_COMMENT><<EOF>>	DBG_panic(("EOF in comment\n"));
<C_COMMENT>.		;

<CC_COMMENT>{NEWLINE} {
	increment_line_number();
	BEGIN INITIAL;
	return NEXT_LINE;
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
<STRING_LITERAL_STATE>\\{OCT}{1,3} {
	int letter;
	sscanf(&yytext[1], "%o", &letter);
    Asm_add_string_literal(letter);
}
<STRING_LITERAL_STATE>\\[xX]{HEX}{1,2} {
	int letter;
	sscanf(&yytext[2], "%x", &letter);
    Asm_add_string_literal(letter);
}
<STRING_LITERAL_STATE>{NEWLINE}        {
    Asm_add_string_literal(yytext[0]);
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
