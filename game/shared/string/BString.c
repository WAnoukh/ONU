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
