#include <avr/io.h>

.global bin_search
bin_search:
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
	movw r14,r24
	movw r16,r24
	add r22,r22
	adc r23,r23
loop:
	asr r23
	ror r22
	add r24, r22
	adc r25, r23
	movw r30,r24
	andi r30, 254
	ld r18,Z
	ldd r19,Z+1

	or r22,r23
	breq 99f

	cp r18,r20
	cpc r19,r21
	breq 99f
	brcs 80f
	movw r24, r16 
	rjmp loop 
80:	
	movw r16, r24
	rjmp loop
99: 
	movw r24,r30
	sub r24,r14
	sbc r25,r15
	asr r25
	ror r24
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	ret
	
