#ifndef BSTRING_H
#define BSTRING_H

// BString is for bound string
// Invariants : 
//      (length <= capacity - 1) for null-terminated c-string interopability

#include "string/StringView.h"
#include <stddef.h>

typedef struct
{
    char        *data;
    size_t       length;
    const size_t capacity;
}BString;

size_t bstr_length(BString bstr);

size_t bstr_capacity(BString bstr);

char *bstr_raw(BString bstr);

void bstr_cat_cstr(BString *bstr, const char *cstr);

void bstr_cat_view(BString *bstr, StringView view);

void bstr_null_terminate(BString *bstr);

#endif // BSTRING_H
