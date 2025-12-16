#ifndef BSTRING_H
#define BSTRING_H

// BString is for bound string
// Invariants : 
//      (length <= capacity - 1) for null-terminated c-string interopability

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

#endif // BSTRING_H
