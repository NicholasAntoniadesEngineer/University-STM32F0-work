// STDNUM001
// Prac Exam 1 Part 2 Solution 2015-10-16

#include <stdint.h>
#define STM32F051
#include "stm32f0xx.h"  // you can remove this if you're not using it
#include "libs.h"
// DO NOT CHANGE THIS ARRAY. The marker will change the values *AND LENGTH* at compile time.
uint8_t myArray[] = {0x42, 0x69, 0x12, 0xCC, 0xBB, 0x55, 0xA1, 0x33, 0x1A, 0xDF, 0x56};
uint8_t newArray[1000];
int size_of_myArray = sizeof(myArray)/sizeof(myArray[0]);
void sort_another_array(uint8_t *param, uint8_t *target, int size);


int main(void) {
        // initialisations here
    libs_init_ADC_8bit();
    libs_init_leds();
    libs_init_switches();
    libs_sort_into_another_array(myArray, newArray, size_of_myArray);

    // suggestion: find and store the largest value and smallest value in the array here.
    // hang in an infinite loop
    while(1){
        libs_write_leds(newArray[size_of_myArray-1]);
    }

    /*while(1) {
        // if SW0 not held, display largest value from array.
        // if SW0 is held, display smallest value from array.
        int i = 0;
        while(i < size_of_myArray){
            libs_write_leds(newArray[i]);
            i = i +1;
            libs_delay(200);
        }
        libs_write_leds(newArray[size_of_myArray-1]);
        libs_delay(1000);
        libs_write_leds(newArray[0]);
        libs_delay(1000);
    }*/
    return 0;  // keep compiler happy with a return code.

}
void TIM6_DAC_IRQHandler(void) {
    libs_ACK_IRQ();
    libs_write_leds(newArray[0]);
    libs_delay(200);


}


