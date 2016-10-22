#include "math_parser.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define TEST_EQ_BASE(equality, expect, actual, format)\
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while (0)

#define TEST_EQ_INT(expect, actual) TEST_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define TEST_EQ_DOUBLE(expect, actual) TEST_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

#define TEST_ERROR(error, math)\
    do {\
        math_value v;\
        math_init(&v);\
        TEST_EQ_INT(error, math_parse(math, &v));\
        math_free(&v);\
    } while (0)

#define TEST_PARSE_NUMBER(expect, math)\
    do {\
        math_value v;\
        math_init(&v);\
        TEST_EQ_INT(MATH_PARSE_OK, math_parse(math, &v));\
        TEST_EQ_INT(MATH_CALCULATE_OK, math_calculate(&v));\
        TEST_EQ_DOUBLE(expect, math_get_number(&v));\
        math_free(&v);\
    } while (0)
        

static void test_number()
{
    TEST_PARSE_NUMBER(-1.0*43, "-1.0*43");
    TEST_PARSE_NUMBER(-1.0, "-1.0");
    TEST_PARSE_NUMBER(1.0, "+1.0");
    TEST_PARSE_NUMBER(123.45, "123.45");
    TEST_PARSE_NUMBER(123.45 + 12.34, "123.45+12.34");
    TEST_PARSE_NUMBER(123.45 - 12.34 * 1234, "123.45-12.34*1234");
    TEST_PARSE_NUMBER(0.0 - 0.123, "0.0-0.123");
    TEST_PARSE_NUMBER(-0.1 - 0.123, "-0.1-0.123");
    TEST_PARSE_NUMBER(0.0 - 0.123, "-0.0-0.123");
    TEST_PARSE_NUMBER(+0.1 + 0.123, "+0.1+0.123");
    TEST_PARSE_NUMBER(-4.6 / 0.123, "-4.6/0.123");
    TEST_PARSE_NUMBER(-0.239874 / 0.123, "-0.239874/0.123");
}

#define TEST_CALCULAT_ERROR(error, math)\
    do {\
        math_value v;\
        math_init(&v);\
        TEST_EQ_INT(MATH_PARSE_OK, math_parse(math, &v));\
        TEST_EQ_INT(error, math_calculate(&v));\
        math_free(&v);\
    } while (0)

void test_calculate_error()
{
    TEST_CALCULAT_ERROR(MATH_DIVISION_BY_ZERO, "11.0/0.0");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "10.0/");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "10.0-");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "0.0+");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "0.0-");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "*1.0");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "123/1.0-");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "1++3");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "1+3*/+-*4");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "-1++3");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "*1+3");
    TEST_CALCULAT_ERROR(MATH_TOO_LESS_OPERAND, "/23.43290+3");
}

void test_error()
{
    TEST_ERROR(MATH_INVALID_NUMBER, "10.0.0.0");
    TEST_ERROR(MATH_INVALID_NUMBER, "0.0.0");
    TEST_ERROR(MATH_INVALID_NUMBER, "1%2");
    TEST_ERROR(MATH_INVALID_NUMBER, "1 2");
    TEST_ERROR(MATH_INVALID_NUMBER, "1+ 2");
    TEST_ERROR(MATH_NUMBER_TOO_LARGE, "1e309");
    TEST_ERROR(MATH_NUMBER_TOO_LARGE, "-1e309");
}

static void math_test()
{
    test_number();
    test_calculate_error();
    test_error();
}

int main()
{
    math_test();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
