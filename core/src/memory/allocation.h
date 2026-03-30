#ifndef ALLOCATION_H
#define ALLOCATION_H

#define TRY_ALLOC(dst, expr, err, fail_tag)         \
    do {                                            \
        (dst) = (expr);                             \
        if(!(dst)) {error = (err); goto fail_tag;}  \
    } while(0)                                      \

#define TRY(expr, fail_tag)                 \
    do {                                    \
        error = (expr);                     \
        if(error != ERR_OK) goto fail_tag;  \
    } while(0)                              \


#endif // ALLOCATION_H
