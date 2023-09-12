
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define RS RE0
#define EN RE1
#define LCD PORTD
#define _XTAL_FREQ 4000000

void puls();
void cmd(char,char);
void display(char);
void st(char*);
void i2c_ins();
void i2c_wait();
void i2c_st();
void i2c_restart();
void i2c_write(char);
char i2c_read(char);
void i2c_stop();

void conv(unsigned char i[] );

int main()
{
	char data[20],buf[20];
	PORTD=PORTE=PORTC=0X00;
	TRISD=TRISE=0X00;
	TRISC=0X18;
	ANSEL=ANSELH=0X00;
	
	cmd(0,0x0e);
	cmd(0,0x38);
	cmd(0,0x80);
	
	i2c_ins();
	//ACKEN=1;
	while(1)
	{
		i2c_st();
		i2c_write(0xD0);
		i2c_write(0x00);
		i2c_restart();
		i2c_write(0xD1);
		for(int i=0;i<6;i++)
		{   
			RCEN=1;
				if(i==5)
				{
				ACKDT=1;
				}
			else
				{
				ACKDT=0;
				}
			data[i]=i2c_read(i);
			
		}
		
		i2c_stop();
		conv(data);
	}

}
void puls()
{
	EN=1;
	__delay_us(10);
	EN=0;
	__delay_us(10);
}
void cmd(char command,char data)
{
	RS=command;
	LCD=data;
	puls();
}
void display(char data)
{
	cmd(1,data);
}
void st(char *string)
{
	while(*string)
	{
		cmd(1,*string++);
	}
}
void i2c_ins()
{
	SSPSTAT=0x80;
	SSPCON=0x28;
	SSPCON2=0x00;
}
void wait()
{
	while(SSPIF==0);
	SSPIF=0;
}
void i2c_st()
{
	SEN=1;
	while(SEN==1);
}

void i2c_restart()
{
	RSEN=1;
	while(RSEN==1);
}
void i2c_write(char data)
{	
	SSPBUF=data;
	while(BF==1);
	while(ACKSTAT==1);
}
char i2c_read(char i)
{
	int data;
  	 ACKEN=1;
	while(BF==0);
	data=SSPBUF;
	SSPOV=0;
	//SSPIF=0;
	//ACKEN=1;  // ENABLE FOR RECIVE DONE SEND NEXT DATA 

	return data;
}
void i2c_stop()
{
	PEN=1;
	while(PEN==1);	
}

void conv(unsigned char i[] ) //CONVERTION CALCULATIONS
{
cmd(0,0x80);
display((((i[2]) & 0x30) >> 4)+0x30);
display(((i[2]) & 0x0f)+0x30);
display(':');

display((((i[1]) & 0x70) >> 4)+0x30);
display(((i[1]) & 0x0f)+0x30);
display(':');

display((((i[0]) & 0x70) >>4 )+0x30);
display(((i[0]) & 0x0f)+0x30);

cmd(0,0xC0);
display((((i[4]) & 0xF0) >> 4)+0x30);//year
display(((i[4]) & 0x0F)+0x30);
display('/');

display((((i[5]) & 0x10) >> 4)+0x30);//month
display(((i[5]) & 0x0F)+0x30);
display('/');
/*
display((((i[6]) & 0x30) >>4 )+0x30);//date
display(((i[6]) & 0x0F)+0x30);
display('/');
*/
display((((i[3]) & 0x00) >> 4)+0x30);//day
display(((i[3]) & 0x07)+0x30);

}