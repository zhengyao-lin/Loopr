#include "EBS.h"

#define GET_2BYTE_INT(p) (((p)[0] << 8) + (p)[1])
#define GET_4BYTE_INT(p) (((p)[0] << 24) + ((p)[1] << 16) + ((p)[2] << 8) + (p)[3])

#define SET_2BYTE_INT(p, value) (((p)[0] = (value) >> 8), ((p)[1] = (value) & 0xff))
#define SET_4BYTE_INT(p, value) \
	(((p)[0] = (value) >> 24), \
	 ((p)[1] = (value) >> 16), \
	 ((p)[2] = (value) >> 8), \
	 ((p)[3] = (value) & 0xff))


#define GET_8BYTE_INT(p) \
	(((p)[0] << 56) + \
	 ((p)[1] << 48) + \
	 ((p)[2] << 40) + \
	 ((p)[3] << 32) + \
	 ((p)[4] << 24) + \
	 ((p)[5] << 16) + \
	 ((p)[6] << 8)  + \
	 (p)[7])
#define SET_8BYTE_INT(p, value) \
	(((p)[0] = (value) >> 56), \
	 ((p)[1] = (value) >> 48), \
	 ((p)[2] = (value) >> 40), \
	 ((p)[3] = (value) >> 32), \
	 ((p)[4] = (value) >> 24), \
	 ((p)[5] = (value) >> 16), \
	 ((p)[6] = (value) >> 8), \
	 ((p)[7] = (value) & 0xff))
#define NULL_VISUAL (L"(null)")
#define NULL_VALUE (0x0)
#define MEM_fill(obj, i) \
	(memset(&(obj), (i), sizeof((obj))))

typedef enum {
	EDGE_NOTHING = 1,
	EDGE_JUST_PANIC,
	EDGE_ANYTHING
} WarningFlag;

typedef struct ByteInfo_tag {
	char *assembly_name;
	Edge_Int32 need_stack;
	Edge_Int32 stack_regulator;		
} ByteInfo;

typedef struct TypeInfo_tag {
	char *short_name;
	char *assembly_name;
	Edge_Int32 size;		
} TypeInfo;

typedef struct ByteContainer_tag {
	Edge_Int32 next;
	Edge_Int32 alloc_size;
	Edge_Int32 stack_size;
	Edge_Byte *code;
} ByteContainer;

typedef struct LocalVariable_tag {
	char *identifier;
	Edge_Value *value;
} LocalVariable;

typedef struct ExeEnvironment_tag {
	WarningFlag wflag;

	Edge_Int32 entrance;
	Edge_Int32 code_length;
	Edge_Byte *code;
	Edge_Stack stack;

	Edge_Int32 local_variable_count;
	LocalVariable *local_variable;

	struct ExeEnvironment_tag *outer_env;
} ExeEnvironment;

/* execute.c */
void Edge_execute(ExeEnvironment *env);

/* value.c */
Edge_Byte *Edge_byte_serialize(const void *data, int length);
void*
Edge_byte_deserialize(void *dest, const Edge_Byte *data, int length);

Edge_InfoTable *Edge_alloc_info_table(Edge_BasicType type);
Edge_Value *Edge_alloc_value(Edge_BasicType type);
Edge_Value *Edge_create_string(Edge_Byte *data, int *offset);
Edge_Value *Edge_create_null();
Edge_Value *Edge_get_init_value(Edge_BasicType type);

/* coding.c */
Edge_Byte *Coding_alloc_byte(int length);
ByteContainer *Coding_init_coding_env(void);
void Coding_byte_cat(ByteContainer *env, Edge_Byte *src, int count);
void Coding_push_code(ByteContainer *env, Edge_Byte code, Edge_Byte *args, int args_count);
ExeEnvironment *Coding_init_exe_env(ByteContainer *env, WarningFlag wflag);

typedef void (*Walle_Marker)(void);
/* wall-e.c */
void Walle_set_header(Edge_Value *v);
Edge_Value *Walle_get_header();
void Walle_add_alloc_size(Edge_Int64 add);
Edge_Int64 Walle_get_alloc_size();
void Walle_add_threshold(Edge_Int64 add);
Edge_Int64 Walle_get_threshold();
void Walle_set_marker(Walle_Marker marker);
Walle_Marker Walle_get_marker();

void Walle_add_object(Edge_Value *v);
void Walle_reset_mark();
void Walle_dispose_value(Edge_Value **target);
void Walle_gcollect();
void Walle_check_mem();

extern ByteInfo Edge_Byte_Info[];
extern TypeInfo Edge_Type_Info[];
