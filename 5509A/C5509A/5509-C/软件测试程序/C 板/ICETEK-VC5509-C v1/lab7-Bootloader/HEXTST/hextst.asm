	.def start
	.text
start	
	.loop 400h
	.word $-0x100
	.endloop
