## 语义分析器

### 语法制导

LR1分析过程中，每一次执行移进或归约时，均会执行对应的语义动作。



```
{
	int a;
	a = 1;
	a = a + 1 + 2 * 3;
}
```



- var
    - name 变量的名字（可以去变量表中查询）
- T_INTEGER_LITERAL
    - value int类型变量的真实值
- T_IDENTIFIER
    - name ID对应的名字
- factor
    - value 这个临时值的存在哪个临时变量里
    - type 这个临时值的类型
    - code 计算出这个临时变量的四元式
- expression
    - type 这个表达式最终的类型
    - value 这个表达式的临时值存在哪个临时变量里
    - code 计算出这个临时变量的四元式
- term
    - value 这个临时值的存在哪个临时变量里
    - type 这个临时值的类型
    - code 计算出这个
- additive_expression
    - type 这个表达式最终的类型
    - value 这个表达式的临时值存在哪个临时变量
    - code 计算出这个临时变量的四元式
- addop mulop
    - op 操作，string类型





## 优化方案



- [x] var_declaration
    - [x] expression
        - [x] simple_expression
            - [x] additive_expression
                - [x] term
                    - [x] factor
                        - [x] var
                        - [ ] fun_call
                    - [x] mulop
                - [x] addop
            - [x] relop

