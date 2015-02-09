#include <stdio.h>

/* executable.c */
size_t ISerialize_save_bytecode(FILE *fp, Loopr_Byte *src, size_t length);
size_t ISerialize_read_bytecode(FILE *fp, Loopr_Byte *dest, size_t length);
void ISerialize_save_byte_container(FILE *fp, ByteContainer *src);
ByteContainer *ISerialize_read_byte_container(FILE *fp);
