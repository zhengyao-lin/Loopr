/* Loopr Bytecode Standard */

#ifndef _LBS_H_
#define _LBS_H_

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
	LPR_LD_CONST,
	LPR_LD_NULL,
	LPR_LD_STRING,
	LPR_LD_LOC,
	LPR_LD_ARG,
	LPR_LD_ARRAY,

	LPR_CONVERT,
	LPR_BOXING,
	LPR_UNBOXING,

	LPR_POP,
	LPR_POP_BYTE,
	LPR_POP_FLOAT,
	LPR_POP_STRING,

	LPR_STORE_LOC,
	LPR_STORE_ARRAY,

	LPR_EQUAL_FLOAT,
	LPR_EQUAL,
	LPR_NOT_EQUAL_FLOAT,
	LPR_NOT_EQUAL,
	LPR_GREATER_THAN_FLOAT,
	LPR_GREATER_THAN,
	LPR_LESS_THAN_FLOAT,
	LPR_LESS_THAN,
	LPR_LESS_OR_EQUAL_FLOAT,
	LPR_LESS_OR_EQUAL,
	LPR_GREATER_OR_EQUAL_FLOAT,
	LPR_GREATER_OR_EQUAL,

	LPR_BRANCH,
	LPR_DUPLICATE,

	LPR_ADD_BYTE,
	LPR_ADD_FLOAT,
	LPR_ADD_STRING,
	LPR_SUB_FLOAT,
	LPR_SUB_BYTE,
	LPR_MUL_FLOAT,
	LPR_MUL_BYTE,
	LPR_DIV_FLOAT,
	LPR_DIV_BYTE,
	LPR_MOD_FLOAT,
	LPR_MOD_BYTE,
	LPR_INC_FLOAT,
	LPR_INC,
	LPR_DEC_FLOAT,
	LPR_DEC,
	LPR_MINUS_FLOAT,
	LPR_MINUS,

	LPR_INVOKE,
	LPR_NEW_ARRAY,

	LPR_GOTO,
	LPR_RETURN,

	LPR_NOP,
	LPR_CODE_PLUS_1
} LooprCodeGroup;

typedef enum {
	LCR_ENTRANCE = 1,
	LCR_MAX_STACK,
	LCR_FUNCTION,
	LCR_DEFINE,
	LCR_USING,
	LCR_CODE_PLUS_1
} LooprCodeCompilerReference;

typedef enum {
    LPR_False = 0,
    LPR_True = 1
} Loopr_Boolean;

typedef size_t			Loopr_Size;
typedef wchar_t		Loopr_Char;

typedef int8_t		 	Loopr_SByte;
typedef int16_t		Loopr_Int16;
typedef int32_t 		Loopr_Int32;
typedef int64_t	 	Loopr_Int64;

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
	LPR_ARRAY,

	LPR_BASIC_TYPE_PLUS_1
} Loopr_BasicType;

typedef struct Loopr_InfoTable_tag {
	Loopr_BasicType type;
} Loopr_InfoTable;

typedef union {
	Loopr_Int64				int_value;
	Loopr_Double			float_value;
	struct Loopr_Ref_tag	*ref_value;
	struct CallInfo_tag		*call_info;
} Loopr_Value;

typedef struct Loopr_Array_tag {
	Loopr_Size				size;
	Loopr_Value				*value;
	Loopr_Boolean			*ref_flag;
} Loopr_Array;

typedef struct Loopr_Ref_tag {
	Loopr_Int32 marked;
	Loopr_BasicType type;
	union {
		Loopr_Char				*string_value;
		struct {
			Loopr_Value		value;
			Loopr_Boolean	ref_flag;
		}						object_value;
		Loopr_Array				array_value;
	} u;

	struct Loopr_Ref_tag *prev;
	struct Loopr_Ref_tag *next;
} Loopr_Ref;

typedef struct ExeContainer_tag {
	Loopr_Size entrance;
	Loopr_Size length;
	Loopr_Byte *code;
} ExeContainer;

typedef struct Loopr_Stack_tag {
	Loopr_Size alloc_size;
	Loopr_Size stack_pointer;
	Loopr_Value *value;
	Loopr_Boolean *ref_flag;
} Loopr_Stack;

typedef struct LocalVariable_tag {
	char *identifier;
	Loopr_Value value;
	Loopr_Boolean ref_flag;
} LocalVariable;

typedef struct LocalVariableMap_tag {
	Loopr_Size count;
	LocalVariable *variable;
	struct LocalVariableMap_tag *prev;
} LocalVariableMap;

typedef struct CallInfo_tag {
	Loopr_Int32 marked;
	Loopr_BasicType filler;

	Loopr_Size pc;
	ExeContainer *caller;

	Loopr_Size base;
	Loopr_Size stack_pointer;
	LocalVariableMap *local_list;
} CallInfo;

typedef struct LabelContainer_tag {
	int dest;
	char *identifier;

	int ref_count;
	int *ref;
	struct LabelContainer_tag *next;
} LabelContainer;

typedef struct UsingList_tag {
	int name_space;
	struct UsingList_tag *next;
} UsingList;

typedef struct ByteContainer_tag {
	char							*name;
	Loopr_Boolean					is_void;

	LabelContainer					*label_header;
	UsingList						*using_list;

	Loopr_Size						next;
	Loopr_Size						alloc_size;

	Loopr_Boolean					hinted:1;
	Loopr_Size						stack_size;

	Loopr_Size						entrance;
	Loopr_Byte						*code;

	Loopr_Size						local_variable_count;
	LocalVariable					*local_variable;

	Loopr_Size						self_reflect;
	Loopr_Size						sub_name_space_count;
	struct ByteContainer_tag		**sub_name_space;

	Loopr_Size function_count;
	struct ByteContainer_tag **function;

	struct NativeFunction_tag *native_function;

	struct ByteContainer_tag *outer_env;
} ByteContainer;

typedef struct ByteInfo_tag {
	char *assembly_name;
	Loopr_Size need_stack;
	Loopr_Size stack_regulator;
} ByteInfo;

typedef struct TypeInfo_tag {
	char *short_name;
	char *assembly_name;
	char *scan_controller;
	Loopr_Size size;
} TypeInfo;

typedef enum {
	LPR_NOTHING = 1,
	LPR_JUST_PANIC,
	LPR_ANYTHING
} WarningFlag;

typedef struct ExeEnvironment_tag {
	WarningFlag wflag;

	ExeContainer *exe;
	Loopr_Stack stack;

	LocalVariableMap *local_variable_map;

	struct NativeFunction_tag *native_function;

	Loopr_Size self_reflect;
	Loopr_Size sub_name_space_count;
	struct ExeEnvironment_tag **sub_name_space;

	Loopr_Size function_count;
	struct ExeEnvironment_tag **function;
} ExeEnvironment;

typedef struct NativeFunctionInfo_tag {
	int count;
	struct NativeFunction_tag *info_list;
} NativeFunctionInfo;

typedef Loopr_Value (*Loopr_NativeCallee)(ExeEnvironment *env, int argc, Loopr_Value *argv);
typedef NativeFunctionInfo (*Loopr_NativeLoader)(void);

typedef struct NativeFunction_tag {
	char *name;
	Loopr_Int64 magic;
	Loopr_NativeCallee callee;
} NativeFunction;

#endif
