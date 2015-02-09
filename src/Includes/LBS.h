/* Loopr Bytecode Standard */
#include <wchar.h>
#include <inttypes.h>

/* Four basic part of VM:
 * 1.Byte
 * 2.Value
 * 3.Stack
 */

typedef enum {
	LPR_NULL_CODE = 0,

	LPR_LD_BYTE = 1,
	LPR_LD_NULL,
	LPR_LD_STRING,
	LPR_LD_LOC,

	LPR_INIT_LOC,
	LPR_CONVERT, /* MLES */
	LPR_BOXING,
	LPR_UNBOXING,

	LPR_POP_BYTE,
	LPR_POP_FLOAT,
	LPR_POP_STRING,

	LPR_STORE_LOC,

	LPR_BRANCH,
	LPR_DUPLICATE,

	LPR_ADD_BYTE,
	LPR_ADD_FLOAT,
	LPR_ADD_STRING,
	LPR_SUB_BYTE,
	LPR_MUL_BYTE,
	LPR_DIV_BYTE,

	LPR_CALL,
	LPR_LOAD_ARG,

	LPR_GOTO,
	LPR_RETURN,

	LPR_NOP,
	LPR_CODE_PLUS_1
} LooprCodeGroup;

typedef enum {
	LCR_ENTRANCE = 1,
	LCR_MAX_STACK,
	LCR_FUNCTION,
	LCR_CODE_PLUS_1
} LooprCodeCompilerReference;

typedef enum {
    LPR_False = 0,
    LPR_True = 1
} Loopr_Boolean;

typedef wchar_t			Loopr_Char;

typedef int8_t		 	Loopr_SByte;
typedef int16_t		 	Loopr_Int16;
typedef int32_t 		Loopr_Int32;
typedef int64_t	 		Loopr_Int64;

typedef uint8_t 		Loopr_Byte;
typedef uint16_t		Loopr_UInt16;
typedef uint32_t 		Loopr_UInt32;
typedef uint64_t		Loopr_UInt64;

typedef float			Loopr_Single;
typedef double			Loopr_Double;

typedef enum {
	/* State Byte */
	LPR_NULL = 1,

	LPR_BOOLEAN,
	LPR_CHAR,

	/* Numberic Byte */
	LPR_SBYTE,
	LPR_BYTE,

	LPR_INT16,
	LPR_UINT16,

	LPR_INT32,
	LPR_UINT32,

	LPR_INT64,
	LPR_UINT64,

	/* Floating Byte */
	LPR_SINGLE,
	LPR_DOUBLE,

	/* Multibyte */
	LPR_STRING,

	/* Pointer Byte */
	LPR_OBJECT,

	LPR_BASIC_TYPE_PLUS_1
} Loopr_BasicType;

typedef struct Loopr_InfoTable_tag {
	Loopr_BasicType type;
} Loopr_InfoTable;

typedef struct Loopr_Value_tag {
	Loopr_Boolean marked:1;

	Loopr_InfoTable *table;
	union {
		Loopr_Boolean 			boolean_value;
		Loopr_Char				char_value;

		Loopr_SByte				sbyte_value;
		Loopr_Int16				int16_value;
		Loopr_Int32				int32_value;
		Loopr_Int64				int64_value;

		Loopr_Byte				byte_value;
		Loopr_UInt16			uint16_value;
		Loopr_UInt32			uint32_value;
		Loopr_UInt64			uint64_value;

		Loopr_Single			single_value;
		Loopr_Double			double_value;

		Loopr_Char				*string_value;

		struct Loopr_Value_tag	*object_value;
	} u;

	struct Loopr_Value_tag *prev;
	struct Loopr_Value_tag *next;
} Loopr_Value;

typedef struct Loopr_Stack_tag {
	Loopr_Int32 alloc_size;
	Loopr_Int32 stack_pointer;
	Loopr_Value **value;
} Loopr_Stack;

typedef struct LocalVariable_tag {
	char *identifier;
	Loopr_InfoTable *table;
	Loopr_Value *value;
} LocalVariable;

typedef struct ByteContainer_tag {
	char *name;

	Loopr_Int32 next;
	Loopr_Int32 alloc_size;

	Loopr_Boolean hinted:1;
	Loopr_Int32 stack_size;

	Loopr_Int32 entrance;
	Loopr_Byte *code;

	Loopr_Int32 local_variable_count;
	LocalVariable *local_variable;

	Loopr_Int32 function_count;
	struct ByteContainer_tag **function;

	struct ByteContainer_tag *outer_env;
} ByteContainer;

typedef struct ByteInfo_tag {
	char *assembly_name;
	Loopr_Int32 need_stack;
	Loopr_Int32 stack_regulator;		
} ByteInfo;

typedef struct TypeInfo_tag {
	char *short_name;
	char *assembly_name;
	Loopr_Int32 size;		
} TypeInfo;
