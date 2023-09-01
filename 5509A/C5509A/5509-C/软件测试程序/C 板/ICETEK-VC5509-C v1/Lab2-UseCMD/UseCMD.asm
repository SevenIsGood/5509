	.global start	; 定义全局标号
    .mmregs
	.data
	.bss	x,1		; 开设全局变量（非初始化段）
	.bss	y,1     ; 三个变量各为一个字（16位）
	.bss	z,1
	.text
start:
	LD		#x,DP	; 因下面诸变量使用直接寻址，装载DP值
	ST		#1,@x	; 变量赋初值，x=1
	ST		#2,@y   ;             y=2
	LD		@x,A    ; 将x的值装载至累加器
	add		@y,A    ; 累加器加上y的值
	stl		A,@z    ; 将累加器结果（32位）的低16位存放到变量z
xh:	
	b		xh      ; 空循环
	.end
	
