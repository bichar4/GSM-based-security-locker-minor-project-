/*======================================*/
/* Created By: Bichar */
/* Date : 10/17/2017 */
/*===================================*/


#define F_CPU 1000000UL						/* define Clock Frequency */
#include <avr/io.h>					
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LCD_16x2_H_file.h"				/* include 16x2 LCD Header file */
#include "USART_Interrupt.h"
#include "MrKeypad.h"				/* include USART Header file */

#define SREG   _SFR_IO8(0x3F)
#define OUTPUTPORT PORTA
#define OUTPUTDIR DDRA
#define BUZZER			5
#define MOTORP			4
#define MOTORM			3
#define MOTOREN			2

void GSM_Begin();							
void GSM_Response();
void GSM_Response_Display();
void GSM_Msg_Read(int);
bool GSM_Wait_for_Msg();
void GSM_Msg_Display();
void GSM_Msg_Delete(unsigned int);
void GSM_Send_Msg(char* , char*);
void GSM_Delete_All_Msg();

char buff[160];								/* buffer to store responses and messages */
char status_flag = 0;						/* for checking any new message */
volatile int buffer_pointer;
char Mobile_no[14];							/* store mobile no. of received message */
char message_received[60];					/* save received message */
int position = 0;							/* save location of current message */
char *myNum="009779808009249";
char mySms[60]= " ";
char warningSMS[60] = "";

int main(void)
{	
	//=========================
	OUTPUTDIR = 0xFF;
	OUTPUTPORT = 0x00;
	unsigned char count=0, keypad_value, password_status=0,mistake_count=0;
	char password_set[5], password_entered[5];
	//=======================================
	unsigned char state_input=0;
	
	buffer_pointer = 0;
	memset(message_received, 0, 60);
    LCD_Init();
	Initialize_keypad();
	USART_Init(9600);						    /* initialize USART communication */
	sei();
	LCD_String_xy(1,0,"GSM Initializing");
	_delay_ms(3000);
	LCD_Clear();
	
	LCD_String_xy(1,0,"AT");
	GSM_Begin();								/* check GSM responses and initialize GSM */
	LCD_Clear();
	//GSM_Send_Msg(myNum,mySms);
	
	  char randd[4]; char random_num[5];char random_check[4] ;
	  /*seed for generating random number*/
	  uint16_t WordOfData;
	  WordOfData = eeprom_read_word((uint16_t *) 4);
	  WordOfData = ((WordOfData+1)%255+1);
	  eeprom_update_word (( uint16_t *) 4, WordOfData );
	  srand(WordOfData);
	  
	  memset(random_num,'1',5);
	  memset(random_check,'1',5);
	  int random=1000*(rand())*(rand());
	  if(random<0)
	  {
		  random*=-1;
	  }
	  sprintf(randd,"%d",random);
	  strncpy(random_num,randd,4);
	  int r_count;
	  for(r_count=0;r_count<3;r_count++)
	  {
		  random_check[r_count]=	random_num[r_count];
	  }
	  int random_number;
	  random_number=atoi (random_check);
	  random_number=random_number/10;
	  if(random_number<0)
	  {
		  random_number*=-1;
	  }
	  sprintf( password_set,"%d",random_number);
	  
	  
//==========================================
		while (1){
		
		LCD_String_xy(1,0,"*=>send code");
		LCD_String_xy(2,0,"#=>Enter code");
		_delay_ms(500);
		_delay_ms(500);
		_delay_ms(1000);
		LCD_Clear();
		
		if(state_input !=0)
		{
			while(1)
			{
				LCD_String_xy(1,0,"Plz restart module");
			}
			
		}
		
		
		while(state_input==0)
		{	LCD_Clear();
			LCD_String_xy(1,0,"Enter Mode");
			_delay_ms(100);
			keypad_value=read_keypad();
			
		
		
		if(keypad_value == 'A')
		{	sprintf(mySms,"Your OTP for this session is %s",password_set);
			LCD_Char('*');
			_delay_ms(500);
			LCD_Clear();
			LCD_String_xy(1,0,"Sending code....");
			GSM_Send_Msg(myNum,mySms);
			//GSM_Calling("009779808009249");

			LCD_Clear();
			LCD_String_xy(1,0,"MESSAGE SENT");			
			_delay_ms(1000);
			LCD_Clear();
			//LCD_String_xy(1,0,"Clearing memory");
			memset(mySms, 0,50);
			GSM_Delete_All_Msg();
			
			
			
		}
		else if (keypad_value=='B')
		{
			
			//==================================
			LCD_Char('#');
			_delay_ms(500);

			LCD_Command(0x01);
			/*Clear screen*/

			LCD_String("Enter Password");
			/*String display in 1st row of LCD*/

			/*While loop for password entry and checking*/
			while(password_status==0x00)
			{
				LCD_Command(0xc0);
				/*Cursor moves to 2nd row 1st column of LCD*/

				/*While loop for 4 digit password entry*/
				while(count<4)
				{
					keypad_value=read_keypad();
					/*Scan's 4X4 keypad and returns pressed key value or default value*/

					/*Checking if any key is pressed or not*/
					if(keypad_value != 0xff)
					{
						password_entered[count]=keypad_value;
						/*Storing the pressed key value of 4X4 keypad in ASCII format*/

						count++;
						/*Counter increment*/

						LCD_Char(keypad_value);
						/* Star(*) is displayed in 2nd row of LCD*/
					}
					else
					{
						;
						/*Null statement*/
					}

					_delay_ms(300);
					/*300ms delay as guard time between two consecutive pressing of key*/
				}
				count=0;
				/*Counter reset to 0*/

				password_entered[4]=0;
				/*Null character at the last of array(password_entered) to convert it to a string*/

				LCD_Command(0x01);
				/*Clear screen*/

				/*Password Comparision*/
				if(!(strcmp(password_set,password_entered)))
				{
					LCD_String("Correct Password");
					/*String display in 1st row of LCD*/
					state_input = 5;
					password_status=1;
					//TODO PUT CODE OF  MOTOR
					OUTPUTPORT &= ~(1<<MOTORM);	
					OUTPUTPORT |= (1<<MOTORP)|(1<<MOTOREN);
					_delay_ms(5000);
					OUTPUTPORT &= ~(1<<MOTOREN);
					
					/*Changing the Password Status to Correct Password*/
					mistake_count=0;
					
				}
				else
				{	
					mistake_count++;
					if(mistake_count>6)//this is just fot test. it just continues the mistake count
					{
						
						mistake_count=1;
					}
					if(mistake_count==1 || mistake_count==2||mistake_count==3)
					{
						//TODO .... SENT A WARNING MESSAGE
						LCD_Command(0x01);
						LCD_String_xy(1,0,"Wrong Code: A ");
						LCD_String_xy(2,0,"================ ");
						/*String display in 1st row of LCD*/
						
						_delay_ms(500);
						_delay_ms(500);
						_delay_ms(500);
						_delay_ms(500);
						/*Display stays for 2 second*/

						LCD_Command(0x01);
						/*Clear screen*/
					}
					else if(mistake_count==4)
					{	//TODO sent notifcation to the mobile number
						sprintf(warningSMS,"Someone is trying to access your password",0);
						//GSM_Delete_All_Msg();
							GSM_Send_Msg(myNum,warningSMS);
						
						LCD_String_xy(1,0,"Wrong Code: B ");
						LCD_String_xy(2,0,"================ ");
						/*String display in 1st row of LCD*/
						
						
						_delay_ms(500);
						_delay_ms(500);
						_delay_ms(500);
						_delay_ms(500);
						/*Display stays for 2 second*/

						LCD_Command(0x01);
						/*Clear screen*/
					}
					else if(mistake_count==5)
					{	//TODO activation of siren
						LCD_String_xy(1,0,"Wrong Code: C ");
						LCD_String_xy(2,0,"================ ");
						/*String display in 1st row of LCD*/
						
						_delay_ms(500);
						_delay_ms(500);
						_delay_ms(500);
						_delay_ms(500);
						/*Display stays for 2 second*/

						LCD_Command(0x01);
						/*Clear screen*/
						OUTPUTPORT |= (1<<BUZZER);
						_delay_ms(5000);
					}
					
					OUTPUTPORT = 0x00;
					LCD_String("Reenter Password");
					/*String display in 1st row of LCD*/
				}
			}

			_delay_ms(500);
			_delay_ms(500);
			_delay_ms(500);
			_delay_ms(500);
			/*Display stays for 2 second*/
			
			LCD_Command(0x01);
			/*Clear Screen*/

			
		}
		
		}
		
		
	}
}



void GSM_Begin()
{

	while(1)
	{
		LCD_Command(0xc0);
		USART_SendString("ATE0\r");			/* send ATE0 to check module is ready or not */
		_delay_ms(500);
		if(strstr(buff,"OK"))
		{
			GSM_Response();                 /* get Response */
			memset(buff,0,160);
			break;
		}
		else
		{
			LCD_String("Error");
		}
	}
	_delay_ms(1000);

	LCD_Clear();
	LCD_String_xy(1,0,"Text Mode");
	LCD_Command(0xc0);
	USART_SendString("AT+CMGF=1\r");/* select message format as text */
	GSM_Response();
	_delay_ms(1000);
}

void GSM_Msg_Delete(unsigned int position)
{
	buffer_pointer=0;
	char delete_cmd[20];
	sprintf(delete_cmd,"AT+CMGD=%d\r",position);    /* delete message at specified position */
	USART_SendString(delete_cmd);
}

void GSM_Delete_All_Msg()
{
	USART_SendString("AT+CMGDA=\"DEL ALL\"\r");		/* delete all messages of SIM */	
}

bool GSM_Wait_for_Msg()
{
	char msg_location[4];
	int i;
	_delay_ms(500);
	buffer_pointer=0;

	while(1)
	{
		if(buff[buffer_pointer]=='\r' || buff[buffer_pointer]== '\n') /*eliminate "\r \n" which is start of string */
		{
			buffer_pointer++;
		}
		else
			break;
	}
		
	if(strstr(buff,"CMTI:"))                                          /* "CMTI:" to check if any new message received */
	{
		while(buff[buffer_pointer]!= ',')
		{
			buffer_pointer++;
		}
		buffer_pointer++;
		
		i=0;
		while(buff[buffer_pointer]!= '\r')
		{
			msg_location[i]=buff[buffer_pointer];				      /* copy location of received message where it is stored */
			buffer_pointer++;
			i++;
		}

		/* convert string of position to integer value */
		position = atoi(msg_location);
		
		memset(buff,0,strlen(buff));
		buffer_pointer=0;

		return true;
	}
	else
	{
		return false;
	}
}

/* ISR routine to save responses/new message */
ISR(USART_RXC_vect)
{
	buff[buffer_pointer] = UDR;							/* copy UDR(received value) to buffer */
	buffer_pointer++;
	status_flag = 1;						            /* flag for new message arrival */
}


void GSM_Send_Msg(char *num,char *sms)
{
	char sms_buffer[35];
	buffer_pointer=0;
	sprintf(sms_buffer,"AT+CMGS=\"%s\"\r",num);
	USART_SendString(sms_buffer);                       /*send command AT+CMGS="Mobile No."\r */
	_delay_ms(200);
	while(1)
	{
		if(buff[buffer_pointer]==0x3e)                  /* wait for '>' character*/
		{
			buffer_pointer = 0;
			memset(buff,0,strlen(buff));
			USART_SendString(sms);                      /* send msg to given no. */
			USART_TxChar(0x1a);                         /* send Ctrl+Z then only message will transmit*/
			break;
		}
		buffer_pointer++;
	}
	_delay_ms(300);
	buffer_pointer = 0;
	memset(buff,0,strlen(buff));
	memset(sms_buffer,0,strlen(sms_buffer));
}


void GSM_Response()
{
	unsigned int timeout=0;
	int CRLF_Found=0;
	char CRLF_buff[2];
	int Response_Length=0;
	while(1)
	{
		if(timeout>=60000)								/*if timeout occur then return */
		return;
		Response_Length = strlen(buff);
		if(Response_Length)
		{
			_delay_ms(2);
			timeout++;
			if(Response_Length==strlen(buff))
			{
				for(int i=0;i<Response_Length;i++)
				{
					memmove(CRLF_buff,CRLF_buff+1,1);
					CRLF_buff[1]=buff[i];
					if(strncmp(CRLF_buff,"\r\n",2))
					{
						if(CRLF_Found++==2)				/* search for \r\n in string */
						{
							GSM_Response_Display();		/* display response */
							return;
						}
					}

				}
				CRLF_Found = 0;

			}
			
		}
		_delay_ms(1);
		timeout++;
	}
	status_flag=0;
}

void GSM_Response_Display()
{
	buffer_pointer = 0;
	int lcd_pointer = 0;
	while(1)
	{
		if(buff[buffer_pointer]== '\r' || buff[buffer_pointer]== '\n')  /* search for \r\n in string */
		{
			buffer_pointer++;
		}
		else
			break;
	}
	

	LCD_Command(0xc0);
	while(buff[buffer_pointer]!='\r')								   /* display response till "\r" */
	{
		LCD_Char(buff[buffer_pointer]);								
		buffer_pointer++;
		lcd_pointer++;
		if(lcd_pointer==15)								               /* check for end of LCD line */
		LCD_Command(0x80);
	}
	buffer_pointer=0;
	memset(buff,0,strlen(buff));
}

void GSM_Msg_Read(int position)
{
	char read_cmd[10];
	sprintf(read_cmd,"AT+CMGR=%d\r",position);
	USART_SendString(read_cmd);							/* read message at specified location/position */
	GSM_Msg_Display();									/* display message */
}

void GSM_Msg_Display()
{
	_delay_ms(500);
	if(!(strstr(buff,"+CMGR")))                         /*check for +CMGR response */
	{
		LCD_String_xy(1,0,"No message");
	}
	else
	{
		buffer_pointer = 0;
		
		while(1)
		{
			if(buff[buffer_pointer]=='\r' || buff[buffer_pointer]== 'n')  /*wait till \r\n not over*/
			{
				buffer_pointer++;
			}
			else
			break;
		}
		
		/* search for 1st ',' to get mobile no.*/
		while(buff[buffer_pointer]!=',')
		{
			buffer_pointer++;
		}
		buffer_pointer = buffer_pointer+2;

		/* extract mobile no. of message sender */
		for(int i=0;i<=12;i++)
		{
			Mobile_no[i] = buff[buffer_pointer];
			buffer_pointer++;
		}
		
		do
		{
			buffer_pointer++;
		}while(buff[buffer_pointer-1]!= '\n');
		
		LCD_Command(0xC0);
		int i=0;

		/* display and save message */
		while(buff[buffer_pointer]!= '\r' && i<31)
		{
				LCD_Char(buff[buffer_pointer]);
				message_received[i]=buff[buffer_pointer];
				
				buffer_pointer++;
				i++;
				if(i==16)
					LCD_Command(0x80);						/* display on 1st line */
		}
		
		buffer_pointer = 0;
		memset(buff,0,strlen(buff));
	}
	status_flag = 0;
}