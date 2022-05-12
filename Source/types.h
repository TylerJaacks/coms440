#ifndef COMS440_TYPES_H
#define COMS440_TYPES_H

typedef enum {
    UNDEFINED = 0,
    VOID = 1,
    INTEGER = 2,
    CHAR = 3,
    DOUBLE = 4,
    STRING = 5,
    FLOAT = 6,
    BOOL = 7,
    INTEGER_ARRAY = 8,
    CHAR_ARRAY = 9,
    DOUBLE_ARRAY = 10,
    STRING_ARRAY = 11,
    FLOAT_ARRAY = 12,
    BOOL_ARRAY = 13,
} type_t;
#endif