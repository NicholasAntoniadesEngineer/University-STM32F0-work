//name
//info
//stuff

#include <stdint.h>
#include "stm32f0xx.h"
#include "libs.h"

#define STM32F051


int main(void) {
    // initialisations here
	libs_init_leds();
	libs_init_switches();
	libs_init_USART1();
	
	
    // hang in an infinite loop
    while(1) {

    	/*
		if (libs_check_button(0)) {
			libs_write_leds(1);
		} else if (libs_check_button(1)) {
			libs_write_leds(2);
		} else if (libs_check_button(2)) {
			libs_write_leds(4);
		} else if (libs_check_button(3)) {
			libs_write_leds(8);
		}
		*/

    	//Receive

		libs_delay(200);
		GPIOB ->ODR = USART1_receive();
		libs_delay(200);
    		GPIOB ->ODR = 0b111;


		//Transmit
    	/*
		USART1_transmit(101);
		libs_delay(100);
		USART1_transmit(111);
		libs_delay(100);
		USART1_transmit(112);
		libs_delay(100);
		*/
    }
    return 0;  // keep compiler happy with a return code.

}

