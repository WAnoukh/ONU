#ifndef STRINGVIEW_H
#define STRINGVIEW_H

#include <stddef.h>

#define VIEW_FROM_CONST_STR(cstr_const) (StringView){.data = cstr_const, .length = sizeof(cstr_const)-1}

typedef struct
{
    const char *const data;
    const size_t length;
} StringView;

StringView view_from_cstr(const char *cstr);

const char *view_raw(StringView view);

size_t view_length(StringView view);
#endif // STRINGVIEW_H
