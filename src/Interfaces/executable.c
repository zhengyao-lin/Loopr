#include <stdio.h>
#include "LBS.h"
#include "MEM.h"

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
