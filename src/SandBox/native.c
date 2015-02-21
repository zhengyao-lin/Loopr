#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "LBS.h"
#include "DBG.h"
#include "UTL.h"
#include "MEM.h"
#include "Versions.h"
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

void
Native_native_info_cat(NativeFunctionInfo src)
{
	int i;

	Loopr_Native_Function_Info = MEM_realloc(Loopr_Native_Function_Info,
											 sizeof(NativeFunction) * (__function_count + src.count));
	for (i = 0; i < __function_count + src.count; i++) {
		Loopr_Native_Function_Info[i + __function_count] = src.info_list[i];
	}
	__function_count += src.count;

	return;
}

void
Native_load_lib(char *file_path)
{
	void *dp = NULL;
	Loopr_NativeLoader loader;
	NativeFunctionInfo ret_info;

	if (!(dp = dlopen(file_path, RTLD_LAZY | RTLD_GLOBAL))) {
		fprintf(stderr, "Native: Failed to load library %s: %s\n", file_path, dlerror());
		exit(0);
	}

	loader = dlsym(dp, NATIVE_LOAD_FUNCTION_NAME);
	ret_info = loader();
	Native_native_info_cat(ret_info);
	MEM_free(ret_info.info_list);

	return;
}
