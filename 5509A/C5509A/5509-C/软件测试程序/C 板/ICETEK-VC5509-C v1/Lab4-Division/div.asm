      .global start	; ����ȫ�ֱ��
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
; �����ӳ���
; ���룺NUMERA��������DENOM����
; �����QUOT�̣�ARIT����
DIV:ld 	#NUMERA,dp
	ld 	@NUMERA,T 	; ��������װ��T�Ĵ���
	mpy 	@DENOM,A 	; �����뱻������ˣ��������A�Ĵ���
	ld 	@DENOM,B 	; ������װ��B�Ĵ����ĵ�16λ
	abs 	B				; �����ֵ
	stl	B,@DENOM		; ����B�Ĵ����ĵ�16λ
	ld 	@NUMERA,B	; ��������װ��B�Ĵ�����16λ
	abs 	B 				; �����ֵ
	rpt	#15 			; �ظ�����subcָ��16��
	subc 	@DENOM,B 	; ��ɳ�������
	bcd 	done,AGT 	; ��ʱ��ת����ִ����������ָ�Ȼ���ж�A,
						; ��A>0����ת�����done,������������
	stl 	B,@QUOT 	; �����̣�B�Ĵ����ĵ�16λ��
	sth	B,@ARIT	; ����������B�Ĵ����ĸ�16λ��
	xor 	B			; ��������˵Ľ��Ϊ��������ҲӦΪ��
						; �Ƚ�B�Ĵ�����0
	sub 	@QUOT,B	; ���̷���
	stl	B,@QUOT	; ��ط��ź����ֵ
done:
	ret
