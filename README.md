简介
===
一个基本的四则运算解析库，包括加减乘除(+-*/)，括号() 和浮点数。

解析
===

主要有六个接口：
void math_init(math_value *v);
int math_parse(const char *math， math_value *v);
生成语法树
void math_calculate(math_value *v);
计算语法树的值并存储在 v->math_res 中
math_display(const math_value *v);
显示语法树，可能跟输入的不太一样，因为解析过程会进行处理
double math_get_number(math_value *v);
得到存储在该节点的值，若节点为操作符，则在计算前为 0，计算后为 v->math_res
void math_free(math_value *v);
释放节点的左右子树，但不释放节点本身

const char *math 为要解析的字符串，可以为
```
ws expression
```
ws 是任意多的空白（'\n', '\r', '\t', ' ' 四种），后面的 expression 是真正的表达式部分，不可以包含任何空白，否则解析会出错。