#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>

typedef uint32_t Error;

#define ERR_OK              0x0
#define ERR_OOM             0x0001
#define ERR_CONTAINER_FULL  0x0100
#define ERR_CONTAINER_EMPTY 0x0101
#define ERR_ELMT_NOT_FOUND  0x0102

#endif // ERROR_H
