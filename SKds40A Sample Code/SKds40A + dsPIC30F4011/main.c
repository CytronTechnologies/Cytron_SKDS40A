/*******************************************************************************
* This is the main program for the SKds40A + dsPIC30F4011
* It should be compatible for dsPIC30F3011
*
* Author: Ober Choo @ Cytron Technologies Sdn. Bhd.
* Date: 13May11
*******************************************************************************/
// Include the neccesary header files
#include <p30F4011.h>
#include "system.h"
#include "lcd.h"
#include "delay.h"
#include "uart.h"


/*******************************************************************************
* DEVICE CONFIGURATION WORDS                                                   *
*******************************************************************************/

// Primary Oscillator Mode = XT with PLL 8x.
// Clock Switching and Monitor = Off.
_FOSC(CSW_FSCM_OFF & XT_PLL8);

// Watchdog Timer = Off.
_FWDT(WDT_OFF);

// Brown Out Reset = Off.
// POR Timer Value = 64ms.
// Master Clear = Enabled.
_FBORPOR(PBOR_OFF & PWRT_64 & MCLR_EN);

// Code Protection = On.
_FGS(CODE_PROT_ON);



/*******************************************************************************
* PRIVATE FUNCTION PROTOTYPES                                                  *
*******************************************************************************/
void beep(unsigned char count);
void lcd_1stline_msg(const char* csz_string);
void lcd_2ndline_msg(const char* csz_string);
void test_switch (void);
void test_led (void);
void test_uart1 (void);
void test_uart2 (void);
void test_uart1a (void);


/*******************************************************************************
* Global Variables                                                             *
*******************************************************************************/
unsigned char c_received_data = 0;



/*******************************************************************************
* MAIN FUNCTION                                                                *
*******************************************************************************/
int main(void)
{	
	unsigned char test_no = 0;
	// Clear all ports.	
	LATB = 0;
	LATC = 0;
	LATD = 0;
	LATE = 0;
	LATF = 0;
	
	// Initialize I/O directions.
	TRISB = 0;
	TRISC = 0;
	TRISD = 0;
	TRISDbits.TRISD0 = 1;	// SW2 is connected at RD0 of dsPIC30F4011/3011
	TRISE = 0;				// dsPIC30F3011 & 4011 have PORTE
	TRISEbits.TRISE8 = 1;	// SW1 is connected at RE8 of dsPIC30F4011/3011
	TRISF = 0;	

	
	// Wait 1ms before we start the loop.
	beep(2);
	delay_ms(200);	
	lcd_initialize();	// initialize LCD
	
	lcd_clear();
	lcd_putstr("     Cytron\n  Technologies");	//Buzzer and LED share same output pin	
	delay_ms(1500);
	
	lcd_clear();
	lcd_putstr("    SKds40A\n  dsPIC30F4011");	
	delay_ms(1500);	
	LED1 = 0;
	LED2 = 0;
	
	test_switch();	//need to ensure switches is working before other test
	
	test_no = 1;
// Testing LEDs, UARTs (1, 2 and Alternative)
	while(1){
		lcd_2ndline_msg("1-> +,  2->Test");	//display the message for test menu navigation
				
		switch (test_no) //start the switch case to check which test mode to execute
			{
			case 1:	
				lcd_1stline_msg("1. Test All");	//mode 1 is to run all test sequentially			
				if (SW2 == 0) 			// if SW2 is press, to select the mode and run
				{	while (SW2 == 0);	// waiting for SW3 to be let go
					test_led();			// Test LEDs					
					test_uart1();		// Test UART1
					test_uart2();		// Test UART2
					test_uart1a();		// Test UART1 Alternative
				}	
				break;
				
			case 2:		// mode 2, test LEDs on SKds40A												
				lcd_1stline_msg("2: Test LEDs");	
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_led();			// Test LEDs
				}	
				break;
				
			case 3:		// mode 3, test UART1, ensure jumper at Tx and RX Selector is connected to TX1 and RX1 respectively
				lcd_1stline_msg("3: Test UART1");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_uart1();		// Test UART1
				}	
				break;
				
			case 4:		// mode 4, test UART2, ensure jumper at Tx and RX Selector is connected to TX2 and RX2 respectively
				lcd_1stline_msg("4: Test UART2");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_uart2();		// Test UART2
				}	
				break;	
				
			case 5:		// mode 5, test UART1A, ensure jumper at Tx and RX Selector is connected to TX1A and RX1A respectively
				lcd_1stline_msg("5: Test UART1A");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_uart1a();		// Test UART1A
				}	
				break;			
		}//switch (test_no)	
			
		
		// If SW1 is pressed...		
		if (SW1 == 0) 
		{
			if (++test_no > 5) // if SW1 is press increase the test number until it is max and loop back
			{
				test_no = 1;
			}				
			while (SW1 == 0);	//wait for SW1 to be released
			beep(1);
		}		
	} // while (1)
}
// =========================================== PRIVATE Function Defination =================================
/*******************************************************************************
* PRIVATE FUNCTION: beep
*
* PARAMETERS:
* ~ count	- The value of buzzer beep.
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* activate and deactivate buzzer according to count value 
*
*******************************************************************************/
void beep(unsigned char count)
{
	while (count-- > 0) {
		BUZZER = 1;	
		delay_ms(50);
		BUZZER = 0;	
		delay_ms(50);
	}
}	
/*******************************************************************************
* PRIVATE FUNCTION: lcd_1stline_msg
*
* PARAMETERS:
* ~ csz_string	- The null terminated string to display on 1st line
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* clear LCD on 1st line (upper row) and display message from home
*
*******************************************************************************/
void lcd_1stline_msg(const char* csz_string)
{
	unsigned char i = 0;
	lcd_home();
	lcd_putstr(csz_string);
	
	while (*csz_string != '\0') {		
	i++;		
	csz_string++;
	}
	
	if(i < 16)
	{
		for(; i<16;i++)
		{
		lcd_putchar(' ');	
		}
	}
		
}	
/*******************************************************************************
* PRIVATE FUNCTION: lcd_2ndline_msg
*
* PARAMETERS:
* ~ csz_string	- The null terminated string to display on 2nd line
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* clear LCD on 2nd line (lower row) and display message from 1st column of 2nd row
*
*******************************************************************************/
void lcd_2ndline_msg(const char* csz_string)
{
	unsigned char i = 0;
	lcd_2ndline();
	lcd_putstr(csz_string);
	
	while (*csz_string != '\0') {		
	i++;		
	csz_string++;
	}
	
	if(i < 16)
	{
		for(; i<16;i++)
		{
		lcd_putchar(' ');	
		}
	}	
}

/*******************************************************************************
* PRIVATE FUNCTION: test_switch
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the push buttons.
*
*******************************************************************************/
void test_switch(void)
{
	// Display message
	lcd_clear();
	lcd_putstr("Test Switches");
	delay_ms(1000);		
	
	lcd_1stline_msg("Press SW1");
	
	// Waiting for user to press SW1
	while (SW1 == 1);
	
	// If SW1 is pressed but other switches also become low, trap the error.
	if (SW2 == 0) {			
	lcd_clear();
	lcd_putstr("SW2 detected, Error!");
	while (1);		// trap
	}		
	// Waiting for user to release SW1.
	while (SW1 == 0);	
	beep(1);
	
	// Display message
	lcd_clear();
	lcd_putstr("Press SW2");
	
	// Waiting for user to press SW2.
	while (SW2 == 1);
	
	// If SW2 is pressed but other switches also become low, trap the error.
	if (SW1 == 0) 
	{
	lcd_clear();
	lcd_putstr("SW1 detected, Error!");
	while (1);		// trap
	}	
	
	// Waiting for user to release SW2.
	while (SW2 == 0);	
	beep(2);	
	lcd_clear();
	lcd_putstr("Switches Pass!");	
	delay_ms(500);	
}	

/*******************************************************************************
* PRIVATE FUNCTION: test_led
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the LEDs.
*
*******************************************************************************/
void test_led(void)
{
	unsigned char i= 0;
	// Display message
	lcd_clear();
	lcd_putstr("Test LEDs");
	delay_ms(1000);		
	
	lcd_1stline_msg("LED1 ON");	
	LED1 = 1;				// activate LED1
	delay_ms(1000);			// delay for a while
	lcd_1stline_msg("LED1 OFF");		
	LED1 = 0;				// deactivate LED1
	delay_ms(1000);			// delay for a while
	
	lcd_1stline_msg("LED2 ON");	
	LED2 = 1;				// activate LED1
	delay_ms(1000);			// delay for a while
	lcd_1stline_msg("LED2 OFF");	
	LED2 = 0;				// deactivate LED1
	delay_ms(1000);			// delay for a while
	
	lcd_1stline_msg("LED1 BLINK");
	for(i=0; i < 20; i++)	// Blink LED1
	{
	LED1 = ~LED1;
	delay_ms(70);	
	}
	LED1 = 0;
	delay_ms(500);
	
	lcd_1stline_msg("LED2 BLINK");
	for(i=0; i < 20; i++)	// Blink LED2
	{
	LED2 = ~LED2;
	delay_ms(70);	
	}
	LED2 = 0;
	delay_ms(500);
	
	beep(2);	
	lcd_1stline_msg("LEDs Pass!");	
	delay_ms(500);	
}	
/*******************************************************************************
* PRIVATE FUNCTION: test_uart1
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the UART1.
*
*******************************************************************************/
void test_uart1(void)
{	
	unsigned char i = 0;
	while (SW1==1) // Loop until SW1 is press
	{
	lcd_clear();
	lcd_putstr("SW1 test UART1\nConnect UC00A");	
	for (i = 0; i < 50; i++)
		{
		if(SW1 == 0) break;			
		delay_ms(30);		
		}
	
	lcd_clear();
	lcd_putstr("TX Selector=TX1\nRX Selector=RX1");	
	for (i = 0; i < 50; i++) 
		{	
		if(SW1 == 0) break;			
		delay_ms(30);		
		}
	lcd_clear();
	lcd_putstr("HyperTerminal\n9600, 8, N, 1");	
	for (i = 0; i < 50; i++) 
		{	
		if(SW1 == 0) break;			
		delay_ms(30);		
		}
	}//while (SW1 == 1)		
	while(SW1==0); //wait for SW1 to be release
	
	uart1_initialize();	// initialize UART1
	lcd_clear();
	lcd_putstr("Press enter on\nkeyboard to exit");	
	
	// Sending message to the PC through UART1
	uart1_transmit(0x0C);			//clear HyperTerminal
	uart1_putstr("\r\n\nThanks for using Cytron Technologies product\r\n");
	uart1_putstr("\r\n\nDemo UART1 on SKds40A...\r\n");
	uart1_putstr("Press any key on keyboard to demo.\r\n");
	uart1_putstr("It will be echo back to HyperTerminal.\r\n");
	uart1_putstr("Press enter to exit.\r\n\n");
	
	do {
		c_received_data = uc_uart1_receive();	//echo back what is being received
		LED1 = ~LED1;							//toggle LED1
		uart1_transmit(c_received_data);
	}
	while (c_received_data != '\r' && c_received_data != '\n');	
	
	// Sending message to the PC.
	uart1_putstr("\r\n\nUART1 Demo Completed.\r\n");
	LED1 = 0;	
	beep(2);		// buzzer indicator of pass
	lcd_clear();
	lcd_putstr("UART1 Pass!");	
	delay_ms(500);	
}

/*******************************************************************************
* PRIVATE FUNCTION: test_uart2
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the UART2.
*
*******************************************************************************/
void test_uart2(void)
{
	unsigned char i = 0;
	while (SW1==1) // Loop until SW2 is press
	{
	lcd_clear();
	lcd_putstr("SW1 test UART2\nConnect UC00A");		
	for (i = 0; i < 50; i++)
		{
		if(SW1 == 0) break;			
		delay_ms(30);		
		}
	
	lcd_clear();
	lcd_putstr("TX Selector=TX2\nRX Selector=RX2");	
	for (i = 0; i < 50; i++) 
		{	
		if(SW1 == 0) break;			
		delay_ms(30);		
		}
	lcd_clear();
	lcd_putstr("HyperTerminal\n9600, 8, N, 1");	
	for (i = 0; i < 50; i++) 
		{	
		if(SW1 == 0) break;			
		delay_ms(30);		
		}
	}//while (SW1 == 1)		
	while(SW1==0); //wait for SW1 to be release
	
	uart2_initialize();	// initialize UART2
	lcd_clear();
	lcd_putstr("Press enter on\nkeyboard to exit");	
	
	// Sending message to the PC through UART2
	uart2_transmit(0x0C);			//clear HyperTerminal
	uart2_putstr("\r\n\nThanks for using Cytron Technologies product\r\n");
	uart2_putstr("\r\n\nDemo UART2 on SKds40A...\r\n");
	uart2_putstr("Press any key on keyboard to demo.\r\n");
	uart2_putstr("It will be echo back to HyperTerminal.\r\n");
	uart2_putstr("Press enter to exit.\r\n\n");
	
	do {
		c_received_data = uc_uart2_receive();	//echo back what is received
		LED2 = ~LED2;							//toggle LED 2
		uart2_transmit(c_received_data);
	}
	while (c_received_data != '\r' && c_received_data != '\n');	//check for "ENTER" from HyperTerminal

	// Sending message to the PC.
	uart2_putstr("\r\n\nUART2 Demo Completed.\r\n");
	LED2 = 0;
	beep(2);	
	lcd_clear();
	lcd_putstr("UART2 Pass!");	
	delay_ms(500);	
}

/*******************************************************************************
* PRIVATE FUNCTION: test_uart1a
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the UART1 Alternative.
*
*******************************************************************************/
void test_uart1a(void)
{	
	unsigned char i = 0;
	while (SW1==1) // Loop until SW1 is press
	{
	lcd_clear();
	lcd_putstr("SW1 test UART1A\nConnect UC00A");	
	for (i = 0; i < 50; i++)
		{
		if(SW1 == 0) break;			
		delay_ms(30);		
		}
	
	lcd_clear();
	lcd_putstr("TX Selector=TX1A\nRX Selector=RX1A");	
	for (i = 0; i < 50; i++) 
		{	
		if(SW1 == 0) break;			
		delay_ms(30);		
		}
	lcd_clear();
	lcd_putstr("HyperTerminal\n9600, 8, N, 1");	
	for (i = 0; i < 50; i++) 
		{	
		if(SW1 == 0) break;			
		delay_ms(30);		
		}
	}//while (SW1 == 1)	
		
	while(SW1==0); //wait for SW1 to be release
	
	uart1a_initialize();	// initialize UART1A
	lcd_clear();
	lcd_putstr("Press enter on\nkeyboard to exit");	
	
	// Sending message to the PC through UART1
	uart1_transmit(0x0C);	//clear HyperTerminal
	uart1_putstr("\r\n\nThanks for using Cytron Technologies product\r\n");
	uart1_putstr("\r\n\nDemo UART1 Alternative on SKds40A...\r\n");
	uart1_putstr("Press any key on keyboard to demo.\r\n");
	uart1_putstr("It will be echo back to HyperTerminal.\r\n");
	uart1_putstr("Press enter to exit.\r\n\n");
	
	do {
		c_received_data = uc_uart1_receive();	//echo what is received
		LED1 = ~LED1;		// toggle LED 1
		uart1_transmit(c_received_data);
	}
	while (c_received_data != '\r' && c_received_data != '\n');	
	
	// Sending message to the PC.
	uart1_putstr("\r\n\nUART1 Alternative Demo Completed.\r\n");
	LED1 = 0;	
	beep(2);	
	lcd_clear();
	lcd_putstr("UART1 Alternative\nPass!");	
	delay_ms(500);	
}
