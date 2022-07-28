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
    int position;
    unsigned char message;
	
    // hang in an infinite loop
    while(1) {

    	//Receive

		GPIOA -> ODR = 0b000000;
		GPIOB -> ODR = 0b000000;
		libs_delay(100);
		if (libs_check_button(1)) {

			while(1) {
			message = USART1_receive();
			if(message == 1){      //slave number
			 position = USART1_receive();
			 GPIOB -> ODR = position;
			}
			}
		}
        GPIOA -> ODR =  0b11111;
        GPIOB -> ODR =  0b11111;
		libs_delay(100);



    }
    return 0;  // keep compiler happy with a return code.

}

