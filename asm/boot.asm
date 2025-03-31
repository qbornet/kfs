bits 16	                        ;Tells the assembler that its a 16 bit code
org 0x7C00                    ;Origin, tell the assembler that where the code will
				                    ;be in memory after it is been loaded

;mov si, HelloString                 ;Store string pointer to SI
;call PrintString	                ;Call print string procedure
;jmp $ 		                        ;Infinite loop, hang it here.
;
;
;PrintCharacter:	                    ;Procedure to print character on screen
;	                                ;Assume that ASCII value is in register AL
;mov ah, 0x0E	                    ;Tell BIOS that we need to print one charater on screen.
;mov bh, 0x00	                    ;Page no.
;mov bl, 0x07	                    ;Text attribute 0x07 is lightgrey font on black background
;
;int 0x10	                        ;Call video interrupt
;ret		                            ;Return to calling procedure
;
;
;
;PrintString:	                    ;Procedure to print string on screen
;	                                ;Assume that string starting pointer is in register SI
;
;next_character:	                    ;Lable to fetch next character from string
;mov al, [si]	                    ;Get a byte from string and store in AL register
;inc si		                        ;Increment SI pointer
;or al, al	                        ;Check if value in AL is zero (end of string)
;jz exit_function                    ;If end then return
;call PrintCharacter                 ;Else print the character which is in AL register
;jmp next_character	                ;Fetch next character from string
;exit_function:	                    ;End label
;ret		                            ;Return from procedure
;
;
;;Data
;HelloString db 'Hello World', 0	    ;HelloWorld string ending with 0

mov ah, 0x0a
mov al, [HelloString]
mov bh, 0x00
mov cx, 0x04
int 0x10

hang:
    jmp hang


HelloString db 'toto', 0
times 510 - ($ - $$) db 0	        ;Fill the rest of sector with 0
dw 0xAA55			                ;Add boot signature at the end of bootloader

