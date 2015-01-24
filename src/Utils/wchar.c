#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <limits.h>
#include "DBG.h"
#include "MEM.h"
#include "EBS.h"

size_t
Edge_wcslen(wchar_t *str)
{
    return (str ? wcslen(str) : -1);
}

wchar_t *
Edge_wcscpy(wchar_t *dest, wchar_t *src)
{
    return wcscpy(dest, src);
}

wchar_t *
Edge_wcsncpy(wchar_t *dest, wchar_t *src, size_t n)
{
    return wcsncpy(dest, src, n);
}

int
Edge_wcscmp(wchar_t *s1, wchar_t *s2)
{
    return wcscmp(s1, s2);
}

wchar_t *
Edge_wcscat(wchar_t *s1, wchar_t *s2)
{
    return wcscat(s1, s2);
}

int
Edge_mbstowcs_len(const char *src)
{
    int src_idx, dest_idx;
    int status;
    mbstate_t ps;

    memset(&ps, 0, sizeof(mbstate_t));
    for (src_idx = dest_idx = 0; src[src_idx] != '\0'; ) {
        status = mbrtowc(NULL, &src[src_idx], MB_LEN_MAX, &ps);
        if (status < 0) {
            return status;
        }
        dest_idx++;
        src_idx += status;
    }

    return dest_idx;
}

void
Edge_mbstowcs(const char *src, wchar_t *dest)
{
    int src_idx, dest_idx;
    int status;
    mbstate_t ps;

    memset(&ps, 0, sizeof(mbstate_t));
    for (src_idx = dest_idx = 0; src[src_idx] != '\0'; ) {
        status = mbrtowc(&dest[dest_idx], &src[src_idx],
                         MB_LEN_MAX, &ps);
        dest_idx++;
        src_idx += status;
    }
    dest[dest_idx] = L'\0';
}

int
Edge_wcstombs_len(const wchar_t *src)
{
    int src_idx, dest_idx;
    int status;
    char dummy[MB_LEN_MAX];
    mbstate_t ps;

    memset(&ps, 0, sizeof(mbstate_t));
    for (src_idx = dest_idx = 0; src[src_idx] != L'\0'; ) {
        status = wcrtomb(dummy, src[src_idx], &ps);
        src_idx++;
        dest_idx += status;
    }

    return dest_idx;
}

void
Edge_wcstombs(const wchar_t *src, char *dest)
{
    int src_idx, dest_idx;
    int status;
    mbstate_t ps;

    memset(&ps, 0, sizeof(mbstate_t));
    for (src_idx = dest_idx = 0; src[src_idx] != '\0'; ) {
        status = wcrtomb(&dest[dest_idx], src[src_idx], &ps);
        src_idx++;
        dest_idx += status;
    }
    dest[dest_idx] = '\0';
}

char *
Edge_wcstombs_alloc(const wchar_t *src)
{
    int len;
    char *ret;

    len = Edge_wcstombs_len(src);
    ret = MEM_malloc(len + 1);
    Edge_wcstombs(src, ret);

    return ret;
}

wchar_t *
Edge_wcsdup(const wchar_t *src)
{
	int len;
	wchar_t *ret;

	len = Edge_wcslen(src);
	ret = MEM_malloc(sizeof(wchar_t) * (len + 1));

	return Edge_wcscpy(ret, src);
}

char
Edge_wctochar(wchar_t src)
{
    mbstate_t ps;
    int status;
    char dest;

    memset(&ps, 0, sizeof(mbstate_t));
    status = wcrtomb(&dest, src, &ps);
    DBG_assert(status == 1, ("wcrtomb status..%d\n", status));

    return dest;
}

int
Edge_print_wcs(FILE *fp, wchar_t *str)
{
    return fprintf(fp, "%ls", str);
}


int
Edge_print_wcs_ln(FILE *fp, wchar_t *str)
{
    int result;

    result = Edge_print_wcs(fp, str);
    fprintf(fp, "\n");

    return result;
}
