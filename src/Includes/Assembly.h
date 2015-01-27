#include <stdio.h>
#include <stdlib.h>

#define ASM_malloc(size) (malloc((size)))
#define ASM_realloc(p, size) (realloc((p), (size)))
#define ASM_free(p) (free((p)))

typedef enum {
	CONST_CHAR = 1,

	CONST_BYTE,
	CONST_INT16,
	CONST_INT32,
	CONST_INT64,

	CONST_SINGLE,
	CONST_DOUBLE,

	CONST_STRING
} ConstantType;

typedef struct Constant_tag {
	ConstantType type;
	union {
		Edge_Char		*string_value;
		Edge_Char		char_value;
		Edge_Byte		byte_value;
		Edge_Int16		int16_value;
		Edge_Int32		int32_value;
		Edge_Int64		int64_value;
		Edge_Single		single_value;
		Edge_Double		double_value;
	} u;

	int line_number;
	struct Constant_tag *next;
} Constant;

typedef struct Bytecode_tag {
	char *name;
	Edge_Boolean has_fixed;
	Edge_Byte code;

	int line_number;
	struct Bytecode_tag *next;
} Bytecode;

typedef struct Statement_tag {
	Bytecode *bytecode;
	Constant *constant;
	int line_number;
} Statement;

typedef struct StatementList_tag {
	Statement *statement;
	struct StatementList_tag *next;
} StatementList;

typedef struct Asm_Compiler_tag {
	StatementList *list;
	int current_line_number;
} Asm_Compiler;

/* interface.c */
void Asm_set_current_compiler(Asm_Compiler *compiler);
Asm_Compiler *Asm_get_current_compiler();
Asm_Compiler *Asm_init_compiler();
Asm_Compiler *Asm_compile_file(FILE *fp);

/* create.c */
int get_current_line_number();
Constant *Asm_alloc_constant(ConstantType type);
Constant *Asm_chain_constant(Constant *list, Constant *add);
Bytecode *Asm_create_bytecode(char *identifier);
Bytecode *Asm_chain_bytecode(Bytecode *list, char *identifier);
Statement *Asm_create_statement(Bytecode *code, Constant *const_opt);
StatementList *Asm_create_statement_list(Statement *st);
StatementList *Asm_chain_statement_list(StatementList *list, Statement *st);

/* string.c */
void Asm_open_string_literal(void);
void Asm_add_string_literal(int letter);
void Asm_reset_string_literal_buffer(void);
char *Asm_close_string_literal(void);
int Asm_close_character_literal(void);
char *Asm_create_identifier(char *str);
