#include <stdio.h>
#include <string.h>
#include "LBS.h"
#include "DBG.h"
#include "UTL.h"
#include "MEM.h"
#include "SandBox_pri.h"

static int __function_count = 0;
static NativeFunction *Loopr_Native_Function_Info = NULL;

NativeFunction *
Native_search_function_by_name(char *name)
{
	int i;

	for (i = 0; i < __function_count; i++) {
		if (!strcmp(Loopr_Native_Function_Info[i].name, name)) {
			return &Loopr_Native_Function_Info[i];
		}
	}

	return NULL;
}

Loopr_NativeCallee
Native_search_callee_by_magic(Loopr_Int64 magic)
{
	int i;

	for (i = 0; i < __function_count; i++) {
		if (Loopr_Native_Function_Info[i].magic == magic) {
			return Loopr_Native_Function_Info[i].callee;
		}
	}

	return NULL;
}

int
Native_load_function(char *name, Loopr_Int64 magic, Loopr_NativeCallee *callee)
{
	Loopr_Native_Function_Info = MEM_realloc(Loopr_Native_Function_Info,
											 sizeof(NativeFunction) * (__function_count + 1));
	Loopr_Native_Function_Info[__function_count].name = MEM_strdup(name);
	Loopr_Native_Function_Info[__function_count].magic = magic;
	Loopr_Native_Function_Info[__function_count].callee = callee;
	__function_count++;
	return __function_count - 1;
}

void
Native_dispose_all()
{
	int i;

	for (i = 0; i < __function_count; i++) {
		MEM_free(Loopr_Native_Function_Info[i].name);
	}
	if (Loopr_Native_Function_Info) {
		MEM_free(Loopr_Native_Function_Info);
	}

	return;
}
