/*
 * AMD SD186ES Demo Board 
 * Interrupt Vector Table Dumper
 * 
 * (c) 2017, 2018 Nils Stec <stecdose@gmail.com>
 * 
 * use supplied makefile for dev86/bcc. You'll also need AMD's
 * makehex utility, which is provied here:
 *   https://github.com/spacerace/am186-e86mon_utilities
 * makehex330 is working for 3.40 and 3.42 monitors.
 * 
 * I you like to compile all by hand do this: 
 *   bcc -i -Md -ansi -0 -I -I. -V -W -x dumpivt.c -o dumpivt.com
 *   ./Makehex330 leds
 */


#define uint8_t	unsigned char
#define uint16_t unsigned short

void return_to_dos();
void install_ivt_entry(uint8_t int_n, uint16_t segment, uint16_t offset);
void get_ivt_entry(uint8_t int_n, uint16_t *segment, uint16_t *offset);
void print_ivt_entry(uint8_t int_n);
void print_addr(uint16_t seg, uint16_t off);
uint16_t get_mem(uint16_t seg, uint16_t off); 
void set_mem(uint16_t seg, uint16_t off, uint16_t data);
void _am186_dos_putc(uint8_t c);
void print_str(char *s);
void put_hex8(uint8_t hex8);
void put_hex16(uint16_t hex16);
uint16_t get_cs();

int main() {
    uint16_t int_n, i;
    print_str("dump IVT:\n");

    for(int_n = 0; int_n <= 0xff; int_n+=4) {
        for(i = 0; i < 4; i++) {
            print_str("i0x");
            put_hex8((uint8_t)int_n+i);
            print_str(": ");
            print_ivt_entry((uint8_t)int_n+i);
            print_str(" | ");
        }
        
        print_str("\n");
        
    }
        
    return_to_dos();

}




/* this functions places an IVT entry, takes segment, offset and 
 * interrupt number
 */
// void install_ivt_entry(uint8_t int_n, uint16_t segment, uint16_t offset) {
// 	set_mem(0, ((int_n*4)+2), segment);
// 	set_mem(0, (int_n*4), offset);
// }

/* returns segment and offset to a given interrupt number */
void get_ivt_entry(uint8_t int_n, uint16_t *segment, uint16_t *offset) {
	*segment = get_mem(0, ((int_n*4)+2));
	*offset = get_mem(0, ((int_n*4)));
}

/* prints an entry in SEG:OFF-format */
void print_ivt_entry(uint8_t int_n) {
	uint16_t segment;
	uint16_t offset;
	get_ivt_entry(int_n, &segment, &offset);
	
	print_addr(segment, offset);
}

/* prints an address of two 16bit variables in XXXX:XXXX format */
void print_addr(uint16_t seg, uint16_t off) {
    put_hex16(seg);
    _am186_dos_putc(':');
    put_hex16(off);
}


/* get a 16bit word from an absolute location in SEG:OFF format */
uint16_t get_mem(uint16_t seg, uint16_t off) {
	#asm
		mov bx, sp
		mov es, [bx+2]	; seg
		mov	bx, [bx+4]	; off
		seg	es
		mov	ax, [bx]
	
	
	#endasm
}

/* write a 16bit word to an absolute location in SEG:OFF format */
// void set_mem(uint16_t seg, uint16_t off, uint16_t data) {
// 	#asm
// 		mov bx, sp
// 		mov es, [bx+2]	; seg
// 		mov ax, [bx+6]	; data
// 		mov bx, [bx+4]	; off
// 		seg es
// 		mov [bx], ax
// 	#endasm
// }

void _am186_dos_putc(uint8_t c) {
	#asm
/*  bios implementation: */ /* NOT WORKING ON AM186 BOARDS, DUE TO LACK OF BIOS! */
// 		mov ah, #0x0E	; int function Eh, write char, move cursor
// 		mov bx, sp
// 		mov al, [bx+2]	; arguments are on stack, 
// 						; see cdecl calling conventions
// 		mov bh, #0		; page
// 		mov cx, #1		; number of chars
// 		int 0x10

/* dos implementation: */
        mov bx, sp
        mov dl, [bx + 2]    ; c is on stack
        mov ah, #0x02       ; char out
        int 0x21
	#endasm
}

void print_str(char *s) {
	while(*s) {
		_am186_dos_putc(*s);
		s++;
	}
}

void put_hex8(uint8_t hex8) {
	uint8_t save = hex8;
	
	hex8 = hex8 >> 4;
	hex8 = hex8 & 0x0f;
	hex8 += '0';
	if(hex8 > '9') hex8 += 7;
	_am186_dos_putc(hex8);
	
	save = save & 0x0f;
	save += '0';
	if(save > '9') save += 7;
	_am186_dos_putc(save);	
}

void put_hex16(uint16_t hex16) {
	uint8_t hex8 = (hex16 >> 8) & 0xff;
	put_hex8(hex8);
	
	hex8 = hex16 & 0xff;
	put_hex8(hex8);
}

/* returns current code segment */
// uint16_t get_cs() {
// 	#asm
// 		push cs
// 		pop ax
// 	#endasm
// }

void return_to_dos() {
	#asm
        mov al, #0x55
		mov ah, #0x4c
		int 0x21
	#endasm
}


#ifdef __FIRST_ARG_IN_AX__
#error "calling conventions changed, can't compile this way."
#endif

