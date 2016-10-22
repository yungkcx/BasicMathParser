#include "math_parser.h"

#define ISDIGIT(ch)    (ch <= '9' && ch >= '0')
#define ISDIGIT1TO9(ch)    (ch <= '9' && ch >= '1')

typedef struct {
    const char *start;
    const char *end;
} math_context;

void math_init(math_value *v)
{
    v->math_l = v->math_r = NULL;
    v->type   = MATH_NUMBER;
    v->math_n = 0.0;
}

void math_free(math_value *v)
{
    if (v->type == MATH_NUMBER) {
    } else if (v->type == MATH_OPERATOR) {
        if (v->math_l)
            math_free(v->math_l);
        if (v->math_r)
            math_free(v->math_r);
    }
    free(v);
}

static const char *math_parse_whitespace(const char *math)
{
    const char *p;
    p = math;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    return p;
}

static const char *math_parse_priority(math_context *c, math_context *sub, int priority)
{
    const char *p;
    char op[2];
    assert(priority == 1 || priority == 2);
    if (priority == 1){
        op[0] = '+';
        op[1] = '-';
    } else if (priority == 2) {
        op[0] = '*';
        op[1] = '/';
    }
    sub->start = sub->end = c->end;
    for (p = c->end; p != c->start - 1; --p) {
        if (*p == op[0] || *p == op[1]) {
            sub->start = p;
            sub->end = c->end;
            c->end = p - 1;
            return p;
        }
    }
    return NULL;
}

static int math_parse_number(math_context *c, math_value *v)
{
    char *end;
    const char *p = c->start;
    if (*p == '0') {
        ++p;
    } else {
        if (!ISDIGIT1TO9(*p))
            return MATH_INVALID_NUMBER;
        for (++p; ISDIGIT(*p); ++p)
            ;
    }
    if (*p == '.') {
        ++p;
        if (!ISDIGIT(*p))
            return MATH_INVALID_NUMBER;
        for (++p; ISDIGIT(*p); ++p)
            ;
    }

#if 0
    if (*p == 'e' || *p == 'E') {
        ++p;
        if (*p == '-' || *p == '+')
            ++p;
        if (!ISDIGIT(*p))
            return MATH_INVALID_NUMBER;
        for (++p; ISDIGIT(*p); ++p)
            ;
    }
#endif
    errno = 0;
    v->math_n = strtod(c->start, &end);
    if (end == c->start)
        return MATH_INVALID_NUMBER;
    if (errno == ERANGE && (v->math_n == HUGE_VAL || v->math_n == -HUGE_VAL))
        return MATH_NUMBER_TOO_LARGE;
    if (p != c->end + 1)
        return MATH_INVALID_NUMBER;
    c->start = p;
    v->type = MATH_NUMBER;
    return MATH_PARSE_OK;
}

static int math_parse_operator(math_context *c, math_value *v)
{
    v->math_op = *(c->start++);
    v->type = MATH_OPERATOR;
    return MATH_PARSE_OK;
}

static int math_parse_expression(math_context *c, math_value *v)
{
    int ret;
    math_context sub;
    if (math_parse_priority(c, &sub, 1) == NULL)
        if (math_parse_priority(c, &sub, 2) == NULL)
            return math_parse_number(c, v);
    math_parse_operator(&sub, v);
    if (sub.start != sub.end + 1) {
        v->math_r = (math_value *) malloc(sizeof(math_value));
        if ((ret = math_parse_expression(&sub, v->math_r)) != MATH_PARSE_OK)
            return ret;
    } else {
        v->math_r = NULL;
    }
    if (c->start != c->end + 1) {
        v->math_l = (math_value *) malloc(sizeof(math_value));
        if ((ret = math_parse_expression(c, v->math_l)) != MATH_PARSE_OK)
            return ret;
    } else {
        v->math_l = NULL;
    }
    return MATH_PARSE_OK;
}

int math_parse(const char *math, math_value *v)
{
    int ret;
    math_context c;

    math_init(v);
    c.end = math + strlen(math) - 1;
    c.start = math_parse_whitespace(math);
    if ((ret = math_parse_expression(&c, v)) != MATH_PARSE_OK)
        return ret;
    return MATH_PARSE_OK;
}

void math_display(const math_value *v)
{
    if (v->type == MATH_NUMBER)
        printf("%lf", math_get_number(v));
    else if (v->type == MATH_OPERATOR) {
        if (v->math_l)
            math_display(v->math_l);
        printf("%c", v->math_op);
        if (v->math_r)
            math_display(v->math_r);
    }
}

double math_get_number(const math_value *v)
{
    assert(v->type == MATH_NUMBER || v->type == MATH_OPERATOR);
    if (v->type == MATH_NUMBER)
        return v->math_n;
    else
        return v->math_res;
}

int math_calculate(math_value *v)
{
    int l, r;
    if (!v)
        return MATH_CALCULATE_OK;
    assert(v->type == MATH_OPERATOR || v->type == MATH_NUMBER);
    if (v->type == MATH_NUMBER) {
        return MATH_CALCULATE_OK;
    } else if (v->type == MATH_OPERATOR) {
        if (!v->math_r)
            return MATH_TOO_LESS_OPERAND;
        if ((l = math_calculate(v->math_l)) != MATH_CALCULATE_OK)
            return l;
        if ((r = math_calculate(v->math_r)) != MATH_CALCULATE_OK)
            return r;

        switch (v->math_op) {
        case '+':
            if (!v->math_l) {
                v->math_res = math_get_number(v->math_r);
            }else{
                v->math_res = math_get_number(v->math_l) + math_get_number(v->math_r);
                }
            break;
        case '-':
            if (!v->math_l)
                v->math_res = -math_get_number(v->math_r);
            else
                v->math_res = math_get_number(v->math_l) - math_get_number(v->math_r);
            break;
        case '*':
            if (!v->math_l)
                return MATH_TOO_LESS_OPERAND;
            v->math_res = math_get_number(v->math_l) * math_get_number(v->math_r);
            break;
        case '/':
            if (!v->math_l)
                return MATH_TOO_LESS_OPERAND;
            if (math_get_number(v->math_r) == 0)
                return MATH_DIVISION_BY_ZERO;
            v->math_res = math_get_number(v->math_l) / math_get_number(v->math_r);
            break;
        default:
            return MATH_UNSUPPORT_OPERATOR;
        }
    }
    return MATH_CALCULATE_OK;
}
