#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <stdio.h>
#include "LBS.h"

/* executable.c */
size_t ISerialize_save_bytecode(FILE *fp, Loopr_Byte *src, size_t length);
size_t ISerialize_read_bytecode(FILE *fp, Loopr_Byte *dest, size_t length);
void ISerialize_save_byte_container(FILE *fp, ByteContainer *src);
ByteContainer *ISerialize_read_byte_container(FILE *fp);
void ISerialize_save_exe_environment(FILE *fp, ExeEnvironment *src);
ExeEnvironment *ISerialize_read_exe_environment(FILE *fp);

#endif
