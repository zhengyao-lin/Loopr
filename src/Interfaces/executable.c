#include <stdio.h>
#include "LBS.h"
#include "MEM.h"
#include "DBG.h"

/* ISerialize */

size_t
ISerialize_save_bytecode(FILE *fp, Loopr_Byte *src, size_t length)
{
	return fwrite(src, sizeof(Loopr_Byte), length, fp);
}

size_t
ISerialize_read_bytecode(FILE *fp, Loopr_Byte *dest, size_t length)
{
	return fread(dest, sizeof(Loopr_Byte), length, fp);
}

void
ISerialize_save_byte_container(FILE *fp, ByteContainer *src)
{
	int i;

	fwrite(src, sizeof(ByteContainer), 1, fp);
	ISerialize_save_bytecode(fp, src->code, src->alloc_size);

	for (i = 0; i < src->function_count; i++) {
		fwrite(src->function[i], sizeof(ByteContainer), 1, fp);
		ISerialize_save_bytecode(fp, src->function[i]->code, src->function[i]->alloc_size);
	}

	return;
}

ByteContainer *
ISerialize_read_byte_container(FILE *fp)
{
	int i;
	ByteContainer *ret;
	ByteContainer *tmp;

	ret = MEM_malloc(sizeof(ByteContainer));
	fread(ret, sizeof(ByteContainer), 1, fp);
	ret->code = MEM_malloc(sizeof(Loopr_Byte) * ret->alloc_size);
	ret->name = NULL;
	ISerialize_read_bytecode(fp, ret->code, ret->alloc_size);

	ret->function = MEM_malloc(sizeof(ByteContainer) * ret->function_count);
	for (i = 0; i < ret->function_count; i++) {
		tmp = MEM_malloc(sizeof(ByteContainer));
		fread(tmp, sizeof(ByteContainer), 1, fp);
		tmp->code = MEM_malloc(sizeof(Loopr_Byte) * tmp->alloc_size);
		tmp->name = NULL;
		ISerialize_read_bytecode(fp, tmp->code, tmp->alloc_size);

		ret->function[i] = tmp;
	}

	return ret;
}

void
ISerialize_save_exe_environment(FILE *fp, ExeEnvironment *src)
{
	int i;

	fwrite(src,						sizeof(ExeEnvironment), 	1,	fp);
	fwrite(src->local_variable_map,	sizeof(LocalVariableMap), 	1,	fp);
	fwrite(src->exe,				sizeof(ExeContainer), 		1,	fp);
	if (src->native_function) {
		fwrite(src->native_function, sizeof(NativeFunction), 1, fp);
	}

	ISerialize_save_bytecode(fp, src->exe->code, src->exe->length);

	for (i = 0; i < src->sub_name_space_count; i++) {
		if (i != src->self_reflect) {
			ISerialize_save_exe_environment(fp, src->sub_name_space[i]);
		}
	}

	for (i = 0; i < src->function_count; i++) {
		ISerialize_save_exe_environment(fp, src->function[i]);
	}

	return;
}

ExeEnvironment *
ISerialize_read_exe_environment(FILE *fp)
{
	int i;
	ExeEnvironment *ret;

	ret = MEM_malloc(sizeof(ExeEnvironment));
	fread(ret, sizeof(ExeEnvironment), 1, fp);

	ret->local_variable_map = MEM_malloc(sizeof(LocalVariableMap));
	fread(ret->local_variable_map, sizeof(LocalVariableMap), 1, fp);

	ret->exe = MEM_malloc(sizeof(ExeContainer));
	fread(ret->exe, sizeof(ExeContainer), 1, fp);

	ret->exe->code = MEM_malloc(sizeof(Loopr_Byte) * ret->exe->length);
	ISerialize_read_bytecode(fp, ret->exe->code, ret->exe->length);

	if (ret->native_function) {
		ret->native_function = MEM_malloc(sizeof(NativeFunction));
		fread(ret->native_function, sizeof(NativeFunction), 1, fp);
		ret->native_function->callee = Native_search_callee_by_magic(ret->native_function->magic);

		if (!ret->native_function->callee) {
			DBG_panic(("Failed to deserialize: no native function match the magic number 0x%x\n", ret->native_function->magic));
		}
	}

	ret->sub_name_space = MEM_malloc(sizeof(ExeEnvironment) * ret->sub_name_space_count);
	for (i = 0; i < ret->sub_name_space_count; i++) {
		if (i != ret->self_reflect) {
			ret->sub_name_space[i] = ISerialize_read_exe_environment(fp);
		} else {
			ret->sub_name_space[i] = ret;
		}
	}

	ret->function = MEM_malloc(sizeof(ExeEnvironment) * ret->function_count);
	for (i = 0; i < ret->function_count; i++) {
		ret->function[i] = ISerialize_read_exe_environment(fp);
	}

	return ret;
}
