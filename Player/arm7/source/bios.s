	.text
	.align 4
	.thumb
	.global swiWaitVBL
	.thumb_func
swiWaitVBL:
	swi 0x05
	bx lr
