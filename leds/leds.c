/*
 * AMD SD186ES Demo Board LEDs / Am186ES PIO Example with init
 * Am186ES Timer2 OneShot Example + delay_ms function in C
 * 
 * (c) 2017, 2018 Nils Stec <stecdose@gmail.com>
 * 
 * use supplied makefile for dev86/bcc. You'll also need AMD's
 * makehex utility, which is provied here:
 *   https://github.com/spacerace/am186-e86mon_utilities
 * makehex330 is working for 3.40 and 3.42 monitors.
 * 
 * I you like to compile all by hand do this: 
 *   bcc -i -Md -ansi -0 -I -I. -V -W -x leds.c -o leds.com
 *   ./Makehex330 leds
 */

// PIO Control Registers
#define PIO_DATA1     0xff7a         // PIO data 1 register
#define PIO_DIR1      0xff78         // PIO direction 1 register
#define PIO_MODE1     0xff76         // PIO mode 1 register
#define PIO_DATA0     0xff74         // PIO data registers
#define PIO_DIR0      0xff72         // PIO direction register
#define PIO_MODE0     0xff70         // PIO mode 0 register

// Timer 2 control registers
#define TMR2_CON      0xff66         // Timer 2 mode/control register
#define TMR2_CMPA     0xff62         // Timer 2 maxcount compare A register
#define TMR2_CNT      0xff60         // Timer 2 count register

#define TMR2_BIT_CONT   0x0001          // continouus (=1) operation or one shot (=0)
#define TMR2_BIT_MC     0x0020          // MaxCount bit (CNT reached CMPA)
#define TMR2_BIT_INT    0x2000          // enable interrupt generation
#define TMR2_BIT_nINH   0x4000          // needs to be 1 if EN should be set
#define TMR2_BIT_EN     0x8000          // enable timer 2

#define uint8_t	 unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned long

void print_str(char *s);                    // print string, ends with \0, not dos $
// void put_hex8(uint8_t hex8);                // print 8bit hex "FF"
// void put_hex16(uint16_t hex16);             // print 16bit hex "FFFF"
void _am186_dos_putc(uint8_t c);            // console output single char, DOS function, works on DOSBOX, real DOS and e86mon dosemu
void _outw(uint16_t port, uint16_t data);   // output 16bit to port
uint16_t _inw(uint16_t port);               // input 16bit from port
void return_to_dos(uint8_t ret);            // return to DOS with return value, that e86mon displays

void init_leds();
void set_leds(uint8_t val);

void t2_delay_ms(uint16_t ms);
void t2_oneshot_delay_1mS();


int main() {
    uint8_t leds;
    
    print_str("LED Test SD186ES/EM\n");
   
    init_leds();
    
    leds = 0;
    for(;;) {
        set_leds(leds);
        leds++;
        t2_delay_ms(250);
    }
        
    return_to_dos(0xAA);

}

void t2_delay_ms(uint16_t ms) {
    while(ms > 0) {
        t2_oneshot_delay_1mS();
        ms--;
    }
    return;
}

void t2_oneshot_delay_1mS() {
    _outw(TMR2_CMPA, 10000);
    _outw(TMR2_CON, 0xC000);
    while(!(_inw(TMR2_CON) & TMR2_BIT_MC));
    return;
}
/*
 * From AMD's 'CK000901/samples/romdemo/Leds.c':
 *    PostLEDS() -- light the LEDs with the supplied values
 *                  This routine assumes that the E86MON monitor has
 *                  already configured the respective bits in the 
 *                  PIO Mode 0 and PIO Data 0 registers with a value
 *                  of 1.  The demo board led is lit by writting a 
 *                  0 bit value to the respective bit in the PIO 
 *                  Direction 0 register (changing the PIO from an  
 *                  input to an output).
 *
 */
void init_leds() {
    uint16_t temp;
    
    temp = _inw(PIO_MODE0);
    temp |= 0xC03F;
    _outw(PIO_MODE0, temp);
    
    temp = _inw(PIO_DATA0);
    temp |= 0xC03F;
    _outw(PIO_MODE0, temp);
}

void set_leds(uint8_t val) {
    uint16_t mask;
    uint16_t temp;
    
    val = ~val;     // LEDs low active, turn around
    
    mask = ((val << 8) & 0xC000) | (val & 0x003F);  // leds are not in order, rearrange them to their corresponding PIOs
    temp = _inw(PIO_DIR0);                          // we don't want to overwrite PIO settings, we read them first
    
    temp &= ~(0xC03F);      // turn off all LED bits
    temp |= mask;           // turn on desired LED bits
    _outw(PIO_DIR0, temp);  // write PIO register, update LEDs
    
    return;
}

/* this putc function works on dosbox, real dos and e86mon dos extensions */
void _am186_dos_putc(uint8_t c) {
	#asm
        mov bx, sp
        mov dl, [bx + 2]    ; c is on stack
        mov ah, #0x02       ; char out
        int 0x21
	#endasm
}

void _outw(uint16_t port, uint16_t data) {
	#asm
		mov bx, sp
		mov dx, [bx+2]	; get port
		mov ax, [bx+4]	; get data
		out dx, ax		; 16bit output
	#endasm
}

uint16_t _inw(uint16_t port) {
	#asm
		mov bx, sp
		mov dx, [bx+2]	; get port
		in ax, dx
	#endasm
}

void print_str(char *s) {
	while(*s) {
		_am186_dos_putc(*s);
		s++;
	}
}
/*
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
}*/

void return_to_dos(uint8_t ret) {
	#asm
        mov bx, sp
        mov al, [bx + 2]    ; return value
		mov ah, #0x4c
		int 0x21
	#endasm
}

#ifdef __FIRST_ARG_IN_AX__
#error "calling conventions changed, can't compile this way."
#endif

