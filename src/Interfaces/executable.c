#include <stdio.h>
#include "SandBox_pri.h"
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
	fwrite(src, sizeof(ByteContainer), 1, fp);
	ISerialize_save_bytecode(fp, src->code, src->alloc_size);
	return;
}

ByteContainer *
ISerialize_read_byte_container(FILE *fp)
{
	ByteContainer *ret;

	ret = MEM_malloc(sizeof(ByteContainer));
	fread(ret, sizeof(ByteContainer), 1, fp);
	ret->code = MEM_malloc(sizeof(Loopr_Byte) * ret->alloc_size);
	ISerialize_read_bytecode(fp, ret->code, ret->alloc_size);

	return ret;
}
