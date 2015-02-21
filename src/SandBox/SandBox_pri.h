#ifndef _SANDBOX_PRI_H_
#define _SANDBOX_PRI_H_

#include "LBS.h"

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
#define CONV_STRING_BUFFER_SIZE (1024)
#define MEM_fill(obj, i) \
	(memset(&(obj), (i), sizeof((obj))))
#define Loopr_is_ref_type(value) \
	((value)->type == LPR_STRING \
  || (value)->type == LPR_OBJECT \
  || (value)->type == LPR_ARRAY)
#define get_visual_string(src) ((src) ? (src)->u.string_value : NULL_VISUAL)

static Loopr_Value NULL_REF = { NULL };

/* execute.c */
Loopr_Value *Loopr_execute(ExeEnvironment *env, Loopr_Boolean top_level);

/* value.c */
Loopr_Byte *Loopr_byte_serialize(const void *data, int length);
#define Loopr_byte_deserialize(dest, data, length) \
	(memcpy((dest), (data), (length)))

Loopr_Ref *Loopr_alloc_ref(Loopr_BasicType type);
Loopr_Ref *Loopr_create_string(Loopr_Byte *data, int *offset);
Loopr_Char *Loopr_conv_string(Loopr_Byte *data);
#define Loopr_create_null() (NULL)
Loopr_Ref *Loopr_create_object(Loopr_Value orig, Loopr_Boolean ref_flag);
Loopr_Value Loopr_get_init_value(Loopr_BasicType type);

/* coding.c */
Loopr_Byte *Coding_alloc_byte(int length);
ByteContainer *Coding_init_coding_env(void);
int Coding_init_local_variable(ByteContainer *env, char *identifier);
int Coding_get_local_variable_index(ByteContainer *env, char *name);
void Coding_byte_cat(ByteContainer *env, Loopr_Byte *src, int count);
void Coding_push_code(ByteContainer *env, Loopr_Byte code, Loopr_Byte *args, int args_count);
void Coding_push_one_byte(ByteContainer *env, Loopr_Byte code);
ExeContainer *Coding_alloc_exe_container(ByteContainer *env);
ExeEnvironment *Coding_init_exe_env(ByteContainer *env, WarningFlag wflag);

/* wall-e.c */
typedef void (*Walle_Marker)(void);
void Walle_update_alive_period();
int Walle_get_alive_period();
void Walle_set_header(Loopr_Ref *v);
Loopr_Ref *Walle_get_header();
void Walle_add_alloc_size(Loopr_Int64 add);
Loopr_Int64 Walle_get_alloc_size();
void Walle_add_threshold(Loopr_Int64 add);
Loopr_Int64 Walle_get_threshold();
void Walle_set_marker(Walle_Marker marker);
Walle_Marker Walle_get_marker();

void Walle_add_object(Loopr_Ref *v);
void Walle_dispose_value(Loopr_Ref **target);
void Walle_gcollect();
void Walle_check_mem();

void Walle_dispose_environment(ExeEnvironment *env);
void Walle_dispose_byte_container(ByteContainer *env, Loopr_Boolean flag_clean_code);

/* native.c */
NativeFunction *Native_search_function_by_name(char *name);
Loopr_NativeCallee Native_search_callee_by_magic(Loopr_Int64 magic);
int Native_load_function(char *name, Loopr_Int64 magic, Loopr_NativeCallee *callee);
void Native_dispose_all();
void Native_load_lib(char *file_path);

extern ByteInfo Loopr_Byte_Info[];
extern TypeInfo Loopr_Type_Info[];

#endif
