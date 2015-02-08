#include <stdio.h>
#include <string.h>
#include "MEM.h"
#include "DBG.h"
#include "SandBox_pri.h"
#include "Assembler.h"

static LabelContainer *backup = NULL;
static LabelContainer *header = NULL;
static ByteContainer *env_backup = NULL;
static ByteContainer *current_env = NULL;

void
Label_init(ByteContainer *env)
{
	if (header) {
		backup = header;
	}
	if (current_env) {
		env_backup = current_env;
	}
	header = NULL;
	current_env = env;
	return;
}

LabelContainer *
Label_alloc(char *identifier)
{
	LabelContainer *ret;

	ret = ASM_malloc(sizeof(LabelContainer));
	ret->dest = -1;
	ret->identifier = MEM_strdup(identifier);
	ret->ref_count = 0;
	ret->ref = NULL;
	ret->next = NULL;

	return ret;
}

void
Label_add(char *identifier, int dest)
{
	LabelContainer *new_l;
	LabelContainer *pos;

	if (header) {
		for (pos = header; pos; pos = pos->next) {
			if (!strcmp(pos->identifier, identifier) && pos->dest == -1) {
				pos->dest = dest;
				return;
			} else if (!pos->next) {
				new_l = Label_alloc(identifier);
				new_l->dest = dest;
				pos->next = new_l;
				return;
			}
		}
	} else {
		header = Label_alloc(identifier);
		header->dest = dest;
	}

	return;
}

void
Label_alloc_ref(LabelContainer *dest, int pc)
{
	dest->ref = ASM_realloc(dest->ref, sizeof(int) * (dest->ref_count + 1));
	dest->ref[dest->ref_count] = pc;
	dest->ref_count++;
}

void
Label_ref(char *identifier, int pc)
{
	LabelContainer *pos;
	LabelContainer *last = NULL;

	for (pos = header; pos; pos = pos->next) {
		if (!strcmp(pos->identifier, identifier)) {
			Label_alloc_ref(pos, pc);
			return;
		}
		last = pos;
	}

	Label_add(identifier, -1);
	Label_alloc_ref(last && last->next ?
					last->next : header, pc);
	return;
}

void
Label_set_all()
{
	int i;
	LabelContainer *pos;
	LabelContainer *last = NULL;

	for (pos = header; pos; last = pos, pos = pos->next, ASM_free(last)) {
		for (i = 0; i < pos->ref_count; i++) {
			if (pos->dest > 0) {
				memcpy(&current_env->code[pos->ref[i]],
					   &pos->dest, sizeof(Loopr_Int32));
			} else {
				DBG_panic(("Cannot find label by name \"%s\"\n", pos->identifier));
			}
		}
		MEM_free(pos->identifier);
		ASM_free(pos->ref);
	}
	header = (backup ? backup : NULL);
	current_env = (env_backup ? env_backup : NULL);
}
