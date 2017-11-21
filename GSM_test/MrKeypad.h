


#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#ifndef MrKeypad
#define Mrkeypad

#define		KEYPAD_PORT		PORTC
/*KEYPAD_PORT is the microcontroller PORT Register to which 4X4 keypad is connected. Here it is connected to PORTC*/

#define		KEYPAD_PIN		PINC
/*KEYPAD_PIN is the microcontroller PIN Register to which 4X4 keypad is connected. Here it is connected to PINC*/

#define KEYPAD_DIR	DDRC


unsigned char read_keypad(void);
void Initialize_keypad();

void Initialize_keypad()
{	 MCUCSR = (1<<JTD);//use this if u r using port C
	MCUCSR = (1<<JTD);
	KEYPAD_DIR=0x0f;
	/*PortC's upper 4 bits are declared input and lower 4 bits are declared output(4x4 Keypad is connected)*/

	KEYPAD_PORT=0xff;
	/*PortC's lower 4 bits are given high value and pull-up are enabled for upper 4 bits*/
	
}

/*4X4 Keypad Function Definitions*/
unsigned char read_keypad(void)
{
	unsigned char keypad_input=0xff,keypad_output=0xff;
	KEYPAD_PORT=0xfe;
	_delay_us(2);
	keypad_input=KEYPAD_PIN & 0xf0;

	if(keypad_input==0xe0)
	keypad_output='1';
	else if(keypad_input==0xd0)
	keypad_output='2';
	else if(keypad_input==0xb0)
	keypad_output='3';
	else if(keypad_input==0x70)
	keypad_output='C';
	else
	;

	KEYPAD_PORT=0xfd;
	_delay_us(2);
	keypad_input=KEYPAD_PIN & 0xf0;

	if(keypad_input==0xe0)
	keypad_output='4';
	else if(keypad_input==0xd0)
	keypad_output='5';
	else if(keypad_input==0xb0)
	keypad_output='6';
	else if(keypad_input==0x70)
	keypad_output='D';
	else
	;

	KEYPAD_PORT=0xfb;
	_delay_us(2);
	keypad_input=KEYPAD_PIN & 0xf0;

	if(keypad_input==0xe0)
	keypad_output='7';
	else if(keypad_input==0xd0)
	keypad_output='8';
	else if(keypad_input==0xb0)
	keypad_output='9';
	else if(keypad_input==0x70)
	keypad_output='E';
	else
	;
	
	KEYPAD_PORT=0xf7;
	_delay_us(2);
	keypad_input=KEYPAD_PIN & 0xf0;

	if(keypad_input==0xe0)
	keypad_output='A';
	else if(keypad_input==0xd0)
	keypad_output='0';
	else if(keypad_input==0xb0)
	keypad_output='B';
	else if(keypad_input==0x70)
	keypad_output='F';
	else
	;

	return keypad_output;
}


#endif