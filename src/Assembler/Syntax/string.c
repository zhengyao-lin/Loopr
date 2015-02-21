#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LBS.h"
#include "MEM.h"
#include "DBG.h"
#include "UTL.h"
#include "Assembler.h"

#define STRING_ALLOC_SIZE       (256)

static char *st_string_literal_buffer = NULL;
static int st_string_literal_buffer_size = 0;
static int st_string_literal_buffer_alloc_size = 0;

void
Asm_open_string_literal(void)
{
    st_string_literal_buffer_size = 0;
}

void
Asm_add_string_literal(int letter)
{
    if (st_string_literal_buffer_size == st_string_literal_buffer_alloc_size) {
        st_string_literal_buffer_alloc_size += STRING_ALLOC_SIZE;
        st_string_literal_buffer
            = MEM_realloc(st_string_literal_buffer,
                          st_string_literal_buffer_alloc_size);
    }
    st_string_literal_buffer[st_string_literal_buffer_size] = letter;
    st_string_literal_buffer_size++;
}

void
Asm_reset_string_literal_buffer(void)
{
    MEM_free(st_string_literal_buffer);
    st_string_literal_buffer = NULL;
    st_string_literal_buffer_size = 0;
    st_string_literal_buffer_alloc_size = 0;
}

char *
Asm_close_string_literal(void)
{
    char *new_str;
    int new_str_len;

    Asm_add_string_literal('\0');
    new_str_len = strlen(st_string_literal_buffer);
    if (new_str_len < 0) {
		DBG_panic(("line %d: Too few character\n", get_current_line_number()));
    }
    new_str = MEM_malloc(sizeof(char) * (new_str_len+1));
    strcpy(new_str, st_string_literal_buffer);

    return new_str;
}

int
Asm_close_character_literal(void)
{
    int buf[16];
    int new_str_len;

    Asm_add_string_literal('\0');
    new_str_len = Loopr_mbstowcs_len(st_string_literal_buffer);
    if (new_str_len < 0) {
        DBG_panic(("line %d: Bad multibyte character\n", get_current_line_number()));
    } else if (new_str_len > 1) {
		DBG_panic(("line %d: Too long character literal\n", get_current_line_number()));
    }

    Loopr_mbstowcs(st_string_literal_buffer, buf);

    return buf[0];
}

char *
Asm_create_identifier(char *str)
{
    char *new_str;

    new_str = MEM_malloc(strlen(str) + 1);
    strcpy(new_str, str);

    return new_str;
}
