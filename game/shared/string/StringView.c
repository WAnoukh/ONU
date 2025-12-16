#include "StringView.h"
#include <string.h>

StringView view_from_cstr(const char *cstr)
{
    return (StringView){ 
        .data = cstr, 
        .length = strlen(cstr)-1 
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
