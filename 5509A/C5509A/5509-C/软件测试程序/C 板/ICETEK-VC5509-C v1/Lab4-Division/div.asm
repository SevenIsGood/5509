      .global start	; 定义全局标号
      .mmregs
       .data
	   .bss 	NUMERA,1
	   .bss 	DENOM,1
	   .bss 	QUOT,1
	   .bss 	ARIT,1
	   .bss 	TEMSGN,1
	   .text
start:
	nop
	nop
next:
	call 	DIV
	b 		next
; 除法子程序
; 输入：NUMERA被除数，DENOM除数
; 输出：QUOT商，ARIT余数
DIV:ld 	#NUMERA,dp
	ld 	@NUMERA,T 	; 将被除数装入T寄存器
	mpy 	@DENOM,A 	; 除数与被除数相乘，结果放入A寄存器
	ld 	@DENOM,B 	; 将除数装入B寄存器的低16位
	abs 	B				; 求绝对值
	stl	B,@DENOM		; 保存B寄存器的低16位
	ld 	@NUMERA,B	; 将被除数装入B寄存器低16位
	abs 	B 				; 求绝对值
	rpt	#15 			; 重复下条subc指令16次
	subc 	@DENOM,B 	; 完成除法运算
	bcd 	done,AGT 	; 延时条转，先执行下面两条指令，然后判断A,
						; 若A>0则跳转到标号done,结束除法运算
	stl 	B,@QUOT 	; 保存商（B寄存器的低16位）
	sth	B,@ARIT	; 保存余数（B寄存器的高16位）
	xor 	B			; 若两数相乘的结果为负，则商也应为负
						; 先将B寄存器清0
	sub 	@QUOT,B	; 将商反号
	stl	B,@QUOT	; 存回反号后的商值
done:
	ret
