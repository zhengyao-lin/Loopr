#include <stdio.h>
#include <stdlib.h>

#define ASM_malloc(size) (malloc((size)))
#define ASM_realloc(p, size) (realloc((p), (size)))
#define ASM_free(p) (free((p)))
#define NULL_VALUE (0x0)
#define ASM_fill(obj, i) \
	(memset(&(obj), (i), sizeof((obj))))
#define ASM_memset(obj, i, size) \
	(memset((obj), (i), (size)))

typedef enum {
	CONST_CHAR = 1,

	CONST_BYTE,

	CONST_INT16,
	CONST_UINT16,

	CONST_INT32,
	CONST_UINT32,

	CONST_INT64,
	CONST_UINT64,

	CONST_SINGLE,
	CONST_DOUBLE,

	CONST_STRING,
	CONST_LABEL,

	CONST_BLOCK,
	CONST_KEYWORD,

	CONST_TYPE_PLUS_1
} ConstantType;

typedef enum {
	ASM_VOID = 1,

	ASM_KEYWORD_PLUS_1
} KeyWord;

typedef struct Constant_tag {
	ConstantType type;
	union {
		char						*string_value;

		KeyWord						keyword_value;
		Loopr_Char					char_value;
		Loopr_Byte					byte_value;

		Loopr_Int16					int16_value;
		Loopr_UInt16				uint16_value;

		Loopr_Int32					int32_value;
		Loopr_UInt32				uint32_value;

		Loopr_Int64					int64_value;
		Loopr_UInt64				uint64_value;
		Loopr_Single				single_value;
		Loopr_Double				double_value;

		struct StatementList_tag	*block;
	} u;

	int line_number;
	struct Constant_tag *next;
} Constant;

typedef struct Bytecode_tag {
	char *name;
	Loopr_Boolean has_fixed;
	Loopr_Byte code;

	int line_number;
	struct Bytecode_tag *next;
} Bytecode;

typedef struct Statement_tag {
	char *label;
	Bytecode *bytecode;
	Constant *constant;
	int line_number;
} Statement;

typedef struct StatementList_tag {
	Statement *statement;
	struct StatementList_tag *next;
} StatementList;

typedef struct FunctionDefinition_tag {
	char *name;
	Loopr_Boolean is_void;
} FunctionDefinition;

typedef struct Asm_Compiler_tag {
	StatementList *top_level;

	int function_count;
	FunctionDefinition *function_definition;

	int current_line_number;
} Asm_Compiler;

typedef struct LabelContainer_tag {
	int dest;
	char *identifier;

	int ref_count;
	int *ref;
	struct LabelContainer_tag *next;
} LabelContainer;

#define GET_BIT(num, type) \
	((num) << (Loopr_Type_Info[LPR_INT64].size - Loopr_Type_Info[type].size) * 8 \
		   >> (Loopr_Type_Info[LPR_INT64].size - Loopr_Type_Info[type].size) * 8)

#define INITIALISE(p) \
	(memset((p), 0x0, sizeof(p)))

/* label.c */
void Label_init(ByteContainer *env);
LabelContainer *Label_alloc(char *identifier);
void Label_add(char *identifier, int dest);
void Label_alloc_ref(LabelContainer *dest, int pc);
void Label_ref(char *identifier, int pc);
void Label_set_all();

/* interface.c */
void Asm_set_current_compiler(Asm_Compiler *compiler);
Asm_Compiler *Asm_get_current_compiler();
Asm_Compiler *Asm_init_compiler();
Asm_Compiler *Asm_compile_file(FILE *fp);
void Asm_clean_local_env(ByteContainer *env);
void Asm_dispose_compiler(Asm_Compiler *compiler);
void Asm_dispose_current_compiler();

/* create.c */
int get_current_line_number();
Constant *Asm_alloc_constant(ConstantType type);
Constant *Asm_chain_constant(Constant *list, Constant *add);
Bytecode *Asm_create_bytecode(char *identifier, Loopr_Byte code, Loopr_Boolean has_fixed);
Bytecode *Asm_chain_bytecode(Bytecode *list, char *identifier, Loopr_Byte code, Loopr_Boolean has_fixed);
Statement *Asm_create_statement(char *label, Bytecode *code, Constant *const_opt);
StatementList *Asm_create_statement_list(Statement *st);
StatementList *Asm_cat_statement_list(StatementList *list, StatementList *addin);
StatementList *Asm_chain_statement_list(Statement *st, StatementList *list);
Constant *Asm_create_block(StatementList *list);

/* string.c */
void Asm_open_string_literal(void);
void Asm_add_string_literal(int letter);
void Asm_reset_string_literal_buffer(void);
char *Asm_close_string_literal(void);
int Asm_close_character_literal(void);
char *Asm_create_identifier(char *str);
