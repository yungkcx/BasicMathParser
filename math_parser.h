#ifndef MATH_PARSER_H__
# define MATH_PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <assert.h>

typedef enum {
    MATH_OPERATOR,
    MATH_NUMBER
} math_type;

typedef struct math_value math_value;
struct math_value {
    union {
        struct {
            char operator;
            math_value *left;
            math_value *right;
            double result;
        } op;
        double number;
    } u;
#define math_l   u.op.left
#define math_r   u.op.right
#define math_op  u.op.operator
#define math_res u.op.result
#define math_n   u.number
    math_type type;
};

enum {
    MATH_PARSE_OK,
    MATH_CALCULATE_OK,
    MATH_INVALID_NUMBER,
    MATH_DIVISION_BY_ZERO,
    MATH_TOO_LESS_OPERAND,
    MATH_NUMBER_TOO_LARGE,
    MATH_UNSUPPORT_OPERATOR,
    MATH_UNEXPECTED_BRACKET
};

void math_init(math_value *v);
void math_free(math_value *v);
int math_parse(const char *math, math_value *v);
void math_display(const math_value *v);
double math_get_number(const math_value *v);
int math_calculate(math_value *v);

#endif /* MATH_PARSER_H__ */
