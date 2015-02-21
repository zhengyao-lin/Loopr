#include <stdio.h>
#include "LBS.h"
#include "MEM.h"
#include "DBG.h"
#include "SandBox_pri.h"

static int __function_count = 0;
static NativeFunction *Loopr_Native_Function_Info = NULL;

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

Loopr_Value
proc_hello_world(ExeEnvironment *env, int argc, Loopr_Value *argv)
{
	printf("hello, world!\n");
	return NULL_REF;
}

Loopr_Value
proc_print(ExeEnvironment *env, int argc, Loopr_Value *argv)
{
	if (argc < 1) {
		DBG_panic(("Native: print: Arguments less than one"));
	}

	printf("%ls", get_visual_string(argv[0].ref_value));

	return NULL_REF;
}

Loopr_Value
proc_getc(ExeEnvironment *env, int argc, Loopr_Value *argv)
{
	Loopr_Value ret;

	ret.int_value = getc(stdin);

	return ret;
}

Loopr_Value
proc_gets(ExeEnvironment *env, int argc, Loopr_Value *argv)
{
	Loopr_Value ret;
	char buffer[CONV_STRING_BUFFER_SIZE];

	ret.ref_value = Loopr_alloc_ref(LPR_STRING);
	fscanf(stdin, "%s", buffer);
	ret.ref_value->u.string_value = Loopr_conv_string(buffer);

	return ret;
}

NativeFunctionInfo
_LPR_NATIVE_LOAD_()
{
	NativeFunctionInfo ret;

	Native_load_function("hello", 0x01010101, proc_hello_world);
	Native_load_function("print", 0x01010102, proc_print);
	Native_load_function("getc", 0x01010103, proc_getc);
	Native_load_function("gets", 0x01010104, proc_gets);

	ret.count = __function_count;
	ret.info_list = Loopr_Native_Function_Info;

	return ret;
}
