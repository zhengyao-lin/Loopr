#include <stdio.h>
#include <wchar.h>

/* wchar.c */
size_t Edge_wcslen(wchar_t *str);
wchar_t *Edge_wcscpy(wchar_t *dest, wchar_t *src);
wchar_t *Edge_wcsncpy(wchar_t *dest, wchar_t *src, size_t n);
int Edge_wcscmp(wchar_t *s1, wchar_t *s2);
wchar_t *Edge_wcscat(wchar_t *s1, wchar_t *s2);
int Edge_mbstowcs_len(const char *src);
void Edge_mbstowcs(const char *src, wchar_t *dest);
int Edge_wcstombs_len(const wchar_t *src);
void Edge_wcstombs(const wchar_t *src, char *dest);
char *Edge_wcstombs_alloc(const wchar_t *src);
wchar_t *Edge_wcsdup(const wchar_t *src);
char Edge_wctochar(wchar_t src);
int Edge_print_wcs(FILE *fp, wchar_t *str);
int Edge_print_wcs_ln(FILE *fp, wchar_t *str);
