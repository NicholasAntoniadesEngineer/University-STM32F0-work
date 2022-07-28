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
	libs_init_ADC_8bit();
    int slave_num;
    int potvalue;
    int potvalueprev;

	
    // hang in an infinite loop
    while(1) {


		if (libs_check_button(0)) {
			libs_write_leds(2);
            slave_num = 1;
		}
		else if (libs_check_button(1)) {
			libs_write_leds(3);
            slave_num = 2;
		}


        potvalue = libs_read_ADC(1,8);
        if(potvalueprev = potvalue){
            potvalueprev = potvalue;
            GPIOB -> ODR = potvalue;

            //Transmit
	        USART1_transmit(slave_num);
	        libs_delay(100);
	        USART1_transmit(potvalue);
	        libs_delay(100);
        }

    }
    return 0;  // keep compiler happy with a return code.

}

