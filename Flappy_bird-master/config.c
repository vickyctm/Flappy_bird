/*
 All code in configscreen function was
 taken from mipslabmain.c by Axel Isaksson and F. Lundevall
 The rest was written by Victoria Torres and Manuel Rydholm.
*/
#include <stdint.h>
#include <pic32mx.h>
#include "header.h"

//sets the buttons and the switches
void configbtn() {

	TRISFSET = 0x2; //Button 1
    TRISDSET = 0xFE0; //Switches 1-4 and buttons 2-4

	PORTE = 0x0;
}

void configscreen() {
	/*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */

	//output pins for display signal
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
  	SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;

}


//timer for the enemies
void configtimer2() {
	TMR2 = 0;
	T2CON = 0x70; // 111 in hex (1:256 pre-scaling) biggest scale to avoid large number to fit in register
	T2CONSET = 0x8000; //Turning timer ON by setting bit 15 to 1
	PR2 = ((80000000/256) / ((difficulty/10)+40)); //Set the period

}

//timer for the gravity
void configtimer3() {
	TMR3 = 0;
	T3CON = 0x70; // 111 in hex (1:256 pre-scaling) biggest scale to avoid large number to fit in register
	T3CONSET = 0x8000; //Turning timer ON by setting bit 15 to 1
	PR3 = ((80000000/256) / 100); //Set the period

}

//timer for the pipes
void configtimer4() {
	TMR4 = 0;
	T4CON = 0x70; // 111 in hex (1:256 pre-scaling) biggest scale to avoid large number to fit in register
	T4CONSET = 0x8000; //Turning timer ON by setting bit 15 to 1
	PR4 = ((80000000/256) / difficulty); //Set the period

}
void user_isr() {
	return;
}
