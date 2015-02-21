#ifndef _UTL_H_
#define _UTL_H_

#include <stdio.h>
#include <wchar.h>

/* wchar.c */
size_t Loopr_wcslen(wchar_t *str);
wchar_t *Loopr_wcscpy(wchar_t *dest, wchar_t *src);
wchar_t *Loopr_wcsncpy(wchar_t *dest, wchar_t *src, size_t n);
int Loopr_wcscmp(wchar_t *s1, wchar_t *s2);
wchar_t *Loopr_wcscat(wchar_t *s1, wchar_t *s2);
int Loopr_mbstowcs_len(const char *src);
void Loopr_mbstowcs(const char *src, wchar_t *dest);
int Loopr_wcstombs_len(const wchar_t *src);
void Loopr_wcstombs(const wchar_t *src, char *dest);
char *Loopr_wcstombs_alloc(const wchar_t *src);
wchar_t *Loopr_wcsdup(const wchar_t *src);
char Loopr_wctochar(wchar_t src);
int Loopr_print_wcs(FILE *fp, wchar_t *str);
int Loopr_print_wcs_ln(FILE *fp, wchar_t *str);

#endif
