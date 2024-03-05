## 语义分析器

### 语法制导

LR1分析过程中，每一次执行移进或归约时，均会执行对应的语义动作。



### 目标代码生成

这一阶段，面临要将四元式中间代码转换为目标代码的问题。

四元式 `(op, arg1, arg2, result)` ，根据 `op` 选择不同的策略进行目标代码生成。

`arg1` `arg2` 可能的类型有：1. 局部变量；2. 全局变量；3. 临时变量；4. 立即数；5. 空值。
`result` 可能的类型有：1. 局部变量；2. 全局变量；3. 临时变量。

对于变量的访问，在 `arg1` `arg2` 的位置称为“读取”，在 `result` 的位置称为“写入”。

其中，局部变量和全局变量是用户在输入的代码中定义的变量，在语义分析阶段已经分配了相应的堆栈空间；
临时变量是在生成中间代码时额外产生的变量，通常生命周期较短。

变量必须先写入，后读取。

变量的生命周期指变量第一次被写入的时间到最后一次被访问的时间。

要访问变量，必须先将变量加载到寄存器中。变量如果是第一次被访问，直接为其分配一个寄存器；否则，使用已经分配给它的寄存器。

由于寄存器的数量有限，当存活的变量数量超过寄存器数量时，需要将一部分暂时不活跃的变量保存到内存中，再之后再次使用该变量时，将其从内存中读回寄存器。



#### 临时寄存器保存和恢复

- **初始化**

    `$fp` 用于记录栈帧的底部，类似于x86架构中的的 `bp` 基址指针，在函数一开始执行。

    ```assembly
    move 	$fp, $sp
    ```

- **保存(溢出)**

    ```assembly
    sw 		$t0, 0($sp)
    addi 	$sp, $sp, -4
    
    sw 		$t1, 0($sp)
    addi 	$sp, $sp, -4
    ```

    | addr | register | value |
    | :--: | :------: | :---: |
    |  92  |    sp    |       |
    |  96  |          |  $t1  |
    | 100  |    fp    |  $t0  |

- **访问**

    ```assembly
    lw 	$t0, 0($fp)
    lw	$t1, -4($fp)
    ```



#### 临时寄存器分配

下面讨论如何分配临时寄存器。

MIPS中，有 `$t0~$t9` 十个临时寄存器。最多同时占用 9 个寄存器，保留一个寄存器供接收运算结果。当活跃变量数量等于临时寄存器数量时，要选择一个寄存器溢出到内存中。



**需要管理两个表：**

1. 每一个临时寄存器是否被占用，被哪一个变量占用
2. 每一个变量现在被存储在哪里，寄存器或栈。如果是寄存器，哪一个寄存器；如果是栈，偏移是多少。



**如何选择寄存器（线性扫描算法）：**

1. **初始化活跃集合：** 算法开始时，没有任何寄存器被分配，所以活跃集合（存储当前被使用的寄存器信息）是空的。
2. **遍历生存期：** 对于每个变量的生存期（即程序中变量从被定义到最后一次使用的区间），按照它们开始时间的顺序进行排序和处理。
3. **移除过期变量：** 在尝试为新的变量分配寄存器之前，检查活跃集合中是否有变量的生存期已经结束。如果有，就把它们对应的寄存器释放回寄存器池，这样就可以重新被分配了。
4. **寄存器分配决策：**
    - 如果所有寄存器都已经被占用（即活跃集合的长度等于寄存器总数），那么就需要选择一个变量进行溢出处理，将它存储到内存中，而不是寄存器。
    - 如果还有空闲寄存器，就从寄存器池中移除一个寄存器，分配给当前变量，并更新活跃集合，保持按结束时间排序。
5. **处理寄存器溢出：** 如果没有空闲寄存器，需要选择一个变量进行溢出。这里选择的是活跃集合中结束时间最晚的那个变量（因为它占用寄存器的时间最长）。如果这个变量的结束时间比当前变量的结束时间晚，就把寄存器分配给当前变量，并把那个长期变量溢出到内存。否则，当前变量被溢出到内存。

```python
LinearScanRegisterAllocation:
	active := {}
	for i in live interval in order of increasing start point
		ExpireOldIntervals(i)
		if length(avtive) == R
			SpillAtInterval(i)
		else
      register[i] := a regsiter removed from pool of free registers
			add i to active, sorted by increasing end point
ExpireOldInterval(i)
	for interval j in active, in order of increaing end point
		if endpoint[j] <= startpoint[i]
			return
		remove j from active
		add register[j] to pool of free registers
SpillAtInterval(i)
	spill := last interval in active
	if endpoint[spill] > endpoint[i]
		register[i] := register[spill]
		location[spill] := new stack location
		remove spill from active
		add i to active, sorted by increasing end point
	else
		location[i] := new stack location
```



### 函数调用

函数调用要考虑几个问题：

#### 现场保存与恢复

1. 将调用前的所有寄存器中的变量溢出到栈中。

2. 保存 `$fp` 指针（OLD EBP）到栈中。

3. 保存 `$ra` （返回地址）到栈中

    ```assembly
    addi $sp, $sp, -4
    sw $fp, 0($sp)
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    
    jal read
    
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    lw $fp, 0($sp)
    addi $sp, $sp, 4
    ```

只保存这几个指针可能不够，封装成函数 `save_context` 和 `load_context`

保存 `a0 a1 a2 a3 fp`



```assembly
addi $sp, $sp, -8
sw $ra, 0($sp)
sw $fp, 4($sp)

lw $ra, 0($sp)
sw $fp, 4($sp)
addi $sp, $sp, 8
```



#### 参数传递

mips 预留了4 个寄存器 `$a0` `$a1` `$a2` `$a3` 供变量传递。如果参数数量大于 4 个，可以通过栈空间来传递。

​	

```assembly
addi $sp, $sp, -4
sw $t0, 0($sp)
li $t0, 5
addi $sp, $sp, -4
sw $t0, 0($sp)
```



#### 返回值获取

mips 预留了`$v0` 用作参数保存。

```assembly
move $v0, $t0
```



递归实现：修改文法，区分函数声明和定义，声明的时候就把函数加入函数表里。



## 优化方案

删除未使用变量

记录错误位置（行数列数）



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



