/*
This file contains the main function for the program
*/
#include <stdint.h>
#include <pic32mx.h>
#include "header.h"

uint8_t screen[128*4];

int main(void) {
	configbtn();
	configscreen();
	configtimer3();
	display_init();
	render(screen);
	PORTE = 1;
    run();


}
