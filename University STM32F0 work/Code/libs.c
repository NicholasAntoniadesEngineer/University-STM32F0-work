
#include "libs.h"
#define STM32F051
#include "stm32f0xx.h"
#include <stdint.h>
static int SW_prev[4] = {1, 1, 1, 1};
//Initialisations


void libs_init_leds(void) {
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN; //Enabling the GPIAO  clock in the RCC AHB periperal clock enableregister
    GPIOB -> MODER = 0x5555;             //Setting the LEDS as outpouts in the GPIOB mode register
}
void libs_init_switches(void) {
	// enable clock for GPIOA
	RCC->AHBENR |= (0b1 << 17);
	// configure GPIOA pull-ups
	GPIOA->PUPDR = 0b01010101;
}
void libs_init_ADC_8bit(void){
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN; //Enabling the GPIAO  clock in the RCC AHB periperal clock enableregister
    GPIOA -> MODER |= GPIO_MODER_MODER5;
    GPIOA -> MODER |= GPIO_MODER_MODER6;
    RCC -> APB2ENR |= RCC_APB2ENR_ADCEN; //Enabling the ADC clock in the RCC APB  periperal clock enableregister
    ADC1 -> CFGR1 |= 0x10;               //Setting the ADC allignment in the ADC configuration register 1
    ADC1 -> CR |= ADC_CR_ADEN;           //Enabling the ADC by setting the ADEN(ADC enable bit) in the ADC Control Register
    while((ADC1 -> ISR & 0x01) ==0 );    //Waiting for the ADRDY pin to let us know the ADC is ready.


}
void libs_init_TIM6(uint32_t arr, uint32_t psc){
    RCC -> APB1ENR |= (1 << 4);
    TIM6 -> PSC = psc;
    TIM6 -> ARR = arr;
    TIM6 -> DIER |= TIM_DIER_UIE;
    TIM6 -> CR1 |= TIM_CR1_CEN;
    TIM6 -> CR1 |= TIM_CR1_ARPE;
    *(uint32_t *)(0xE000E100) |= (1 << 17);
}


void libs_init_USART1(void){
	// must enable usart1_DE and the associated pins AF

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; //enable clock for UART1
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enable clock to port a

	GPIOA->MODER |= GPIO_MODER_MODER9_1; //Set PA9 to AF mode
	GPIOA->MODER |= GPIO_MODER_MODER10_1;//Set PA10 to AF mode

	GPIOA->AFR[1] = 0b00000000000000000000000100010000; //Map AF1 for PA9 and PA10
	USART1->CR1 &=~ USART_CR1_M; //clear M0(bit 12) of USARTx_CR1
	SystemCoreClockUpdate(); //define clock speed
	USART1->BRR = SystemCoreClock/115200;// set baud rate to 115.2kbps
	USART1->CR1 &=~ USART_CR1_PCE;//no parity
	USART1->CR2 &=~ USART_CR2_STOP;//1 stop bit
	USART1->CR1 |= USART_CR1_UE;// Enable USART1
	USART1->CR1 |= USART_CR1_TE;//Enable USART1_TX
	USART1->CR1 |= USART_CR1_RE;//Enable USART1_RX

}


void USART1_transmit(unsigned char DataToTx){
	USART1 -> TDR = DataToTx; // write character ‘c’ to the USART1_TDR
	while((USART1 -> ISR & USART_ISR_TC) == 0); // wait: transmission complete
}

unsigned char USART1_receive(void){
	unsigned char DataRx = 'z';//global
	while((USART1 -> ISR & USART_ISR_RXNE) == 0); // exits loop when data is received
	DataRx = USART1 -> RDR; // store received data into ‘DataRx’ variable
	USART1 -> ICR = 0b111111111111111111111;
	return DataRx;
}



void libs_TIM6_PSC(uint32_t psc){
        TIM6 -> PSC = psc;

}
void libs_TIM6_ARR(uint32_t arr){
        TIM6 -> ARR = arr;

}

void init_PWM(void){
	/*Macros to be defined*/
	#define GPIO_AFRH_AFR10_AF2 ((uint32_t)0x00000200)
	#define GPIO_AFRH_AFR11_AF2 ((uint32_t)0x00002000)


	//Initialising clocks and pins for PWM output
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; 	  // Enable clock for GPIOB
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	  // Enable TIM2
	GPIOB->MODER |= GPIO_MODER_MODER10_1; // set PB10 to AF
	GPIOB->MODER |= GPIO_MODER_MODER11_1; // set PB11 to AF

	//Choosing AF for pins, MAPPING them to TIM2 CH3 and CH4
	GPIOB->AFR[1] |= (GPIO_AFRH_AFR10_AF2&(0b10<<8)); //Enable AF2 for PB10 in GPIOB AFR10
	GPIOB->AFR[1] |= (GPIO_AFRH_AFR11_AF2&(0b10<<12));//Enable AF2 for PB11 in GPIOB AFR11

	// specify PWM mode: OCxM bits in CCMRx. We want mode 1
	TIM2->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // PWM Mode 1
	TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // PWM Mode 1

	//Setting signal frequency
	TIM2->ARR = 48000; // f = 1 KHz
	TIM2->PSC = 0;

	//PWM Duty cycle based on fractions of ARR
	TIM2->CCR3 = 0 * 480; // Red = 20%
	TIM2->CCR4 = 20 * 480; // Green = 90%

	// Enable output compare for CH3 and CH4
	TIM2->CCER |= TIM_CCER_CC3E; //Compare 3 output enable
	TIM2->CCER |= TIM_CCER_CC4E; //Compare 4 output enable
	TIM2->CR1 |= TIM_CR1_CEN;    //Counter enable
	}

//Switch checks

int libs_check_button(int button) {
	if ((GPIOA->IDR & (0b1 << button)) == 0) {
		libs_debounce();
		return 1;
	} else { return 0; }
}

int libs_check_button_press(int button) {
	// 1 == pressed/true, 0 == not pressed/false
	int SW_current = libs_check_button(button);
	// if currently pressed and previously not pressed...
	if (SW_current == 1 && SW_prev[button] == 0) {
		libs_debounce();
		SW_prev[button] = 1;
		return 1;
	// if currently pressed but previously pressed as well...
	} else if (SW_current == 1 && SW_prev[button] == 1) {
		libs_debounce();
		return 0;
	// if not pressed....
	} else if (SW_current == 0) {
		SW_prev[button] = 0;
		return 0;
	}
	return 0;
}

int libs_check_button_release(int button) {
	// 1 == pressed/true, 0 == not pressed/false
	int SW_current = libs_check_button(button);
	// if currently not pressed and previously pressed...
	if (SW_current == 0 && SW_prev[button] == 1) {
		libs_debounce();
		SW_prev[button] = 0;
		return 1;
	// if currently not pressed but previously not pressed as well...
	} else if (SW_current == 0 && SW_prev[button] == 0) {
		libs_debounce();
		return 0;
	// if pressed....
	} else if (SW_current == 1) {
		SW_prev[button] = 1;
		return 0;
	}
	return 0;
}


//Pot values
int libs_read_ADC(int pot, int resolution){
    /* configure resolution,
    0: 12 bits
    1: 10 bits
    2: 8 bits
    3: 6 bits */
    ADC1->CFGR1 |= (resolution << 3);
    // channel select where Pot0 = PA5, Pot1 = PA6
    ADC1->CHSELR = (1 << (5 + pot));
    // start conversion
    ADC1->CR |= (0b1 << 2);
    // wait for conversion
    while ((ADC1->ISR & 0b100) == 0);
    //return value
    return ADC1->DR;
}

void libs_debounce(void) {
	int x = 36350;		// 50 milliseconds
	while (x > 0) {x--;}
}

uint8_t libs_read_leds(void){
	return GPIOB->ODR;
}
void libs_delay(int time) {
	// time given in milli seconds
	time = time * 727;
	while (time > 0) {time--;}
}

void libs_write_leds(uint8_t PATTERN){
    GPIOB -> ODR = PATTERN;

}
void libs_sort_into_another_array(uint8_t *param, uint8_t *target, int size) {




    int i, j, past_min = 2147483647, current_min = 2147483647;
    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {
            if (i == 0 || param[j] > past_min) {
                if (past_min == current_min || param[j] < current_min) {
                    current_min = param[j];
                }
            }
        }
        target[i] = current_min;
        past_min = current_min;
    }
}



void libs_ack_irq(void){
        TIM6->SR &= ~(1 << 0);
}



 /*  while(1) {
      current_state = GPIOA->IDR;
      //Button 0
      if (((current_state & 0b1)==0)&&((previous_state & 0b1)!=0)){
        patcount0++;
        if (patcount0==12) patcount0 =0;
        GPIOB->ODR = patterns0[patcount0];
        delay();
      }

      //Button 1
      if (((current_state & 0b10)==0)&&((previous_state & 0b10)!=0)){
        patcount1++;
        if (patcount1==8) patcount1 =0;
        GPIOB->ODR = patterns1[patcount1];
        delay();
      }

      //Button 2
      if ( (GPIOA->IDR & (1 << 2)) == 0){
        ADC1->CR |= (1 << 2);//Start converting
        while( (ADC1->ISR & (1 << 2)) == 0);//Wait to finish
        GPIOB->ODR = 255-(ADC1->DR);
      }
      previous_state = current_state;
    }
    return 0;  // keep compiler happy with a return code. */

