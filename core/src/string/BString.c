#include "BString.h"
#include <assert.h>

#define ASSERT_VALID_STR(bstr) assert(bstr.length < bstr.capacity)

const char *bstr_to_cstr(BString bstr)
{
    ASSERT_VALID_STR(bstr);

    bstr.data[bstr.length] = '\0';
    return bstr.data;
}

size_t bstr_length(const BString bstr)
{
    ASSERT_VALID_STR(bstr);

    return bstr.length;
}

size_t bstr_capacity(const BString bstr)
{
    ASSERT_VALID_STR(bstr);

    return bstr.capacity;
}

char *bstr_raw(const BString bstr)
{
    ASSERT_VALID_STR(bstr);

    return bstr.data;
}

void bstr_cat_cstr(BString *bstr, const char *cstr)
{
    for(size_t i = 0; bstr->length < bstr->capacity-1; ++i) 
    {
        if(cstr[i] == '\0') break;
        bstr->data[bstr->length++] = cstr[i];
    }
}

void bstr_cat_view(BString *bstr, StringView view)
{
    const char *view_data = view_raw(view);
    size_t      view_len  = view_length(view);

    for(size_t i = 0; bstr->length < bstr->capacity-1; ++i) 
    {
        if(i >= view_len) break;
        bstr->data[bstr->length++] = view_data[i];
    }
}

void bstr_null_terminate(BString *bstr)
{
    bstr->data[bstr->length] = '\0';
}
