
#pragma used+
sfrb PINB=3;
sfrb DDRB=4;
sfrb PORTB=5;
sfrb PINC=6;
sfrb DDRC=7;
sfrb PORTC=8;
sfrb PIND=9;
sfrb DDRD=0xa;
sfrb PORTD=0xb;
sfrb TIFR0=0x15;
sfrb TIFR1=0x16;
sfrb TIFR2=0x17;
sfrb PCIFR=0x1b;
sfrb EIFR=0x1c;
sfrb EIMSK=0x1d;
sfrb GPIOR0=0x1e;
sfrb EECR=0x1f;
sfrb EEDR=0x20;
sfrb EEARL=0x21;
sfrb EEARH=0x22;
sfrw EEAR=0x21;   
sfrb GTCCR=0x23;
sfrb TCCR0A=0x24;
sfrb TCCR0B=0x25;
sfrb TCNT0=0x26;
sfrb OCR0A=0x27;
sfrb OCR0B=0x28;
sfrb GPIOR1=0x2a;
sfrb GPIOR2=0x2b;
sfrb SPCR=0x2c;
sfrb SPSR=0x2d;
sfrb SPDR=0x2e;
sfrb ACSR=0x30;
sfrb MONDR=0x31;
sfrb SMCR=0x33;
sfrb MCUSR=0x34;
sfrb MCUCR=0x35;
sfrb SPMCSR=0x37;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-

#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x01
	.EQU __sm_mask=0x0E
	.EQU __sm_adc_noise_red=0x02
	.EQU __sm_powerdown=0x04
	.EQU __sm_powersave=0x06
	.EQU __sm_standby=0x0C
	.EQU __sm_ext_standby=0x0E
	.SET power_ctrl_reg=smcr
	#endif
#endasm

unsigned short int SensorLineData = 0x0000;
float AngleOfRotation = 0.0;

void MoveForward (void) {

if (1.0 > 1.0) {
(*(unsigned char *) 0xb3) = 255 / 1.0;
(*(unsigned char *) 0xb4) = 0;
OCR0A = 255;
OCR0B = 0;
}
else if (1.0 < 1.0) {
(*(unsigned char *) 0xb3) = 255;
(*(unsigned char *) 0xb4) = 0;
OCR0A = 255 / 1.0;
OCR0B = 0;
}
else {
(*(unsigned char *) 0xb3) = 255;
(*(unsigned char *) 0xb4) = 0;
OCR0A = 255;
OCR0B = 0; 
}
}

void MoveStop (void) {

(*(unsigned char *) 0xb3) = 255;
(*(unsigned char *) 0xb4) = 0;
OCR0A = 255;
OCR0B = 0;

}

void MoveLeft (void) {

(*(unsigned char *) 0xb3) = 0;
(*(unsigned char *) 0xb4) = 0;
OCR0A = 255;
OCR0B = 0;

}

void MoveRight (void) {

(*(unsigned char *) 0xb3) = 255;
(*(unsigned char *) 0xb4) = 0;
OCR0A = 0;
OCR0B = 0;

}
