/* Edge Bytecode Standard */
#include <wchar.h>

/* Four basic part of VM:
 * 1.Byte
 * 2.Value
 * 3.Stack
 * 4.Constant Pool (contenet the data that Byte can not content)
 */

typedef enum {
	EDGE_NULL_CODE = 0,

	EDGE_LD_BYTE = 1,
	EDGE_LD_NULL,
	EDGE_LD_STRING,
	EDGE_LD_VAR,
	EDGE_INIT,
	EDGE_POP_BYTE,
	EDGE_POP_FLOAT,
	EDGE_POP_STRING,
	EDGE_MOV_VAR,
	EDGE_ADD_BYTE,
	EDGE_ADD_FLOAT,
	EDGE_ADD_STRING,
	EDGE_SUB_BYTE,
	EDGE_MUL_BYTE,
	EDGE_DIV_BYTE,
	EDGE_JUMP,
	EDGE_EXIT,
	EDGE_CODE_PLUS_1
} EdgeCodeGroup;

typedef enum {
    False = 0,
    True = 1
} Edge_Boolean;

typedef wchar_t			Edge_Char;

typedef signed char 	Edge_SByte;
typedef short		 	Edge_Int16;
typedef int 			Edge_Int32;
typedef long	 		Edge_Int64;

typedef unsigned char 	Edge_Byte;
typedef unsigned short	Edge_UInt16;
typedef unsigned int 	Edge_UInt32;
typedef unsigned long	Edge_UInt64;

typedef float			Edge_Single;
typedef double			Edge_Double;

typedef enum {
	EDGE_DUMMY = 0,

	/* State Byte */
	EDGE_NULL = 1,

	EDGE_BOOLEAN,
	EDGE_CHAR,

	/* Numberic Byte */
	EDGE_SBYTE,
	EDGE_BYTE,

	EDGE_INT16,
	EDGE_UINT16,

	EDGE_INT32,
	EDGE_UINT32,

	EDGE_INT64,
	EDGE_UINT64,

	/* Floating Byte */
	EDGE_SINGLE,
	EDGE_DOUBLE,

	/* Multibyte */
	EDGE_STRING,

	EDGE_BASIC_TYPE_PLUS_1
} Edge_BasicType;

typedef struct Edge_InfoTable_tag {
	Edge_BasicType type;
} Edge_InfoTable;

typedef union {
	Edge_Boolean 	boolean_value;
	Edge_Char		char_value;

	Edge_SByte		sbyte_value;
	Edge_Int16		int16_value;
	Edge_Int32		int32_value;
	Edge_Int64		int64_value;

	Edge_Byte		byte_value;
	Edge_UInt16		uint16_value;
	Edge_UInt32		uint32_value;
	Edge_UInt64		uint64_value;

	Edge_Single		single_value;
	Edge_Double		double_value;

	Edge_Char		*string_value;
} Edge_ValueUnion;

typedef struct Edge_Value_tag {
	Edge_InfoTable *table;
	Edge_ValueUnion u;

	Edge_Boolean marked:1;
	struct Edge_Value_tag *prev;
	struct Edge_Value_tag *next;
} Edge_Value;

typedef struct Edge_Stack_tag {
	Edge_Int32 alloc_size;
	Edge_Int32 stack_pointer;
	Edge_Value **value;
} Edge_Stack;
