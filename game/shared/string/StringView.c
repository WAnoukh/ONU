#include "StringView.h"
#include <string.h>

StringView view_from_cstr(const char *cstr)
{
    return (StringView){ 
        .data = cstr, 
        .length = strlen(cstr) 
    };
}

const char *view_raw(StringView view)
{
    return view.data;
}

size_t view_length(StringView view)
{
    return view.length;
}

void view_copy_to_cstr(StringView view, char *to_cstr)
{
    size_t i;
    for(i = 0; i < view.length; ++i)
    {
        to_cstr[i] = view.data[i];
    }
    to_cstr[i] = '\0';
}
