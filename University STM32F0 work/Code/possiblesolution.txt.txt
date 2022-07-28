/*
 * main.c
 * Created on: 24 Apr 2020
 * Author: Nicholas Antoniades
 */

/* Includes */
#include "stm32f0xx.h"
#include <stdint.h>
#include "Library.h"
#include "lcd_stm32f0.h"
#include "ftoa.h"
#include <stm32f0xx_exti.h>


// Set up variables

int BPMmax = 100;
int plateudelayConstant = 1;
int motorStepsPerRev = 3200;
int gearRatio = 4;
int pauseDelayms = 40;
int plateuDelayms = 30;
#define SystemCoreClock  16000000 //8MHz crystal
#define tidalInitialSteps (motorStepsPerRev)/gearRatio
#define tidalTotalSteps motorStepsPerRev*gearRatio 			// Total steps multiplied by gear ratio

// Setting global variables
int state;
int pauseDelay;
int pauseDelayCounter;
int plateuDelay;
int plateuDelayCounter;
int tidalCounter;
int tidalInputSteps;
int tidalStepMultiplier;
int directionIndicator;
int UART_counter;
// Frequency update variables
double ARR_Val;
double ARR_bottom;
double ARR_top;
// Sensor variables
int load_cell_1;
int load_cell_2;
int flow_sensor;
int bellow_pressure;
int peep_pressure;
// Control variables
double volume_control;
double BPM_control;
// Communications variables
int uartFrequency;
int uartCounter;
// Defining bit masks
#define PA8_mask  0x0100
#define PA9_mask  0x0200
#define PA10_mask 0x0400
#define PB12_mask 0x1000
#define PB13_mask 0x2000
#define PB14_mask 0x4000
#define PB15_mask 0x8000

void initialiseValues(void){
	uartCounter = 0;
	state = 0;
	pauseDelay;
	pauseDelayCounter = 0;
	plateuDelayCounter = 0;
	tidalCounter = 0;
	tidalInputSteps = 0;
	tidalStepMultiplier;
	directionIndicator = 0;
	UART_counter = 0;
}

void sensorReadAndSend(void){
	//Sensor read and communications
//	load_cell_1 = 	  (int)ADC_input(2,8); // PA2
//	load_cell_2 =     (int)ADC_input(3,8); // PA3
//	flow_sensor = 	  (int)ADC_input(4,8); // PA4
	bellow_pressure = (int)ADC_input(5,8); // PA5
//	peep_pressure =   (int)ADC_input(6,8); // PA6
//	BPM_control =     (int)ADC_input(7,8); // PA7
//	volume_control =  (int)ADC_input(8,8); // PB0
//
//	USART1_transmit(BPM_control);
//	USART1_transmit(volume_control);
//	USART1_transmit(peep_pressure);
//	USART1_transmit(bellow_pressure);
//	USART1_transmit(flow_sensor);
//	USART1_transmit(load_cell_1);
//	USART1_transmit(load_cell_2);
	char *text = "\n";
	USART1_transmit(20);
	USART1_transmit(*text);
	USART1_transmit(30);
}

void pwmInitialization(void){
	//Initialize PWM with a read for the ARR_Val
	volume_control =(int)ADC_input(8,8);	 // Volume pot
	BPM_control =(int)ADC_input(7,8);  // Frequency pot
	ARR_top = (60*256*256);
	ARR_bottom = (volume_control*tidalTotalSteps)*(BPM_control*BPMmax);
	ARR_Val = (SystemCoreClock*ARR_top/ARR_bottom) - 1;
	init_PWM(ARR_Val);
	TIM2->ARR = ARR_Val;
	TIM2->CCR4 = ARR_Val/2;
}

void TIM6_DAC_IRQHandler(void) {

	// Reading and sending sensor information intermittently
	sensorReadAndSend();


// Gas supply faulty
//	if(flow_sensor > 150){
//		GPIOA -> ODR |= PA8_mask;
//	}else{
//		GPIOA -> ODR ^= PA8_mask;
//	}
//Electricity supply faulty
//	if(flow_sensor > 150){
//		GPIOA -> ODR |= PA9_mask;
//	}else{
//		GPIOA -> ODR ^= PA9_mask;
//	}
//Unsafe shut off alarm led on
//	if(libs_check_button_GPIOA(0)){ // Check for on off button press
//		GPIOA -> ODR &= ~PA8_mask;
//	}
//Unsafe shut off alarm led off
//	if(libs_check_button_GPIOB(1)){
//		GPIOA -> ODR |= PA8_mask;
//	}
//
//	else{
//			GPIOA -> ODR |= PA8_mask;
//
//	}
//Alarm Buzzer
//	if(flow_sensor > 150){
//		GPIOB -> ODR |= PB12_mask;
//	}else{
//		GPIOB -> ODR ^= PB12_mask;
//	}
//	//Tidal volume not achieved
//	if(flow_sensor > 150){
//		GPIOB -> ODR |= PB13_mask;
//	}else{
//		GPIOB -> ODR ^= PB13_mask;
//	}
//Insp pressure not achieved
//	if(flow_sensor > 150){
//		GPIOB -> ODR |= PB14_mask;
//	}else{
//		GPIOB -> ODR ^= PB14_mask;
//	}
// Peep pressure not achieved
//	if(flow_sensor > 150){
//		GPIOB -> ODR |= PB15_mask;
//	}else{
//		GPIOB -> ODR ^= PB15_mask;
//	}

	libs_ack_irq(); // Set interrupt flag low
}




void EXTI_state_1(void){
	GPIOB -> ODR = 0x4;							//Toggle direction pin PB2
	directionIndicator = 0;
	tidalInputSteps = tidalInitialSteps; 		// Set this value to set amount to move away from limit switch
	if (tidalCounter >=tidalInputSteps){
	   tidalCounter = 0;
	   state = 0;
	   RCC->APB1ENR = ~RCC_APB1ENR_TIM2EN;		// TIM2 disable after moving set steps
	}else{
	   tidalCounter ++;
	}
}

void EXTI_state_2(void){

	//Enable frequency

    TIM2->CR1 = ~TIM_CR1_CEN;


    if (tidalCounter  /*+ tidalInitialSteps*/ >=tidalInputSteps){ // Set counter overflow value to that relatable to pwm signal


		// Updating frequency keeping BPM constant for change in volume
    	/* Divide the read values by 256*/

		volume_control =(int)ADC_input(8,8);	 // Volume pot
		BPM_control =(int)ADC_input(7,8);  // Frequency pot
		ARR_top = (60*256*256);
		ARR_bottom = (volume_control*tidalTotalSteps)*(BPM_control*BPMmax);
		ARR_Val = SystemCoreClock*ARR_top/ARR_bottom - 1;


		// Zero value limiter
//		if(ARR_Val <= 100){
//			ARR_Val = 100;
//		}

		// Down
		if (directionIndicator == 1){
			GPIOB -> ODR = 0x4;		//Toggle direction pin PB2
			directionIndicator = 0;

			// Update change in volume
			tidalStepMultiplier = tidalTotalSteps/256;
			tidalInputSteps = volume_control*tidalStepMultiplier;

			//Setting breathing ratio
			TIM2->ARR = ARR_Val;
			TIM2->CCR4 = ARR_Val/2;

			pauseDelay = pauseDelayms * 727;
			while (pauseDelay > 0) {
				if (pauseDelayCounter > pauseDelay/20){
					sensorReadAndSend(); // Reading and sending sensor information intermittently
					pauseDelayCounter = 0;
				}else{
					pauseDelayCounter++;
				}
				pauseDelay--;
			}

		}
		// Up
		else{
			// Milliseconds plateau breath delay with communications inside
			plateuDelay = plateuDelayms * 727;
			while (plateuDelay > 0) {
				if (plateuDelayCounter > plateuDelay/20){
					sensorReadAndSend(); // Reading and sending sensor information intermittently
					plateuDelayCounter = 0;
				}else{
					plateuDelayCounter++;
				}
				plateuDelay--;
			}

			GPIOB -> ODR = 0;		 //Toggle direction pin PB2
			directionIndicator=1; 	 //Toggle direction indicator

			//Setting breathing ration
			TIM2->ARR = ARR_Val/4;
			TIM2->CCR4 = ARR_Val/8;

			//USART1_transmit(BPM_control);
		}
		tidalCounter = 0;
	}else{
		tidalCounter ++;
	}


	TIM2->CR1 = TIM_CR1_CEN; // Start PWM counter
}


void EXTI0_1_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line1) != RESET){

		// Initial, blank interrupt state
		if(state == 0){

		}
		// Set distance from limit switch
		if(state == 1){
			EXTI_state_1();
		}
		if(state == 2){
			EXTI_state_2();
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line1); //CLear interrupt pending bit
}

void state_0(void){	
	SystemCoreClockUpdate();
	init_ADC();					 // Initialize Analog to digital converters
	init_Ports();				 // Initialize ports
	init_EXTI();				 // Initializing External Interrupts from pins
	libs_init_TIM6(50,8000);     // 1ms interrupt
	pwmInitialization();   		 // Initialize Pulse Width Modulation
	libs_init_USART1();
}
void state_1(void){
	
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN; //Disable PWM signal
	initialiseValues(); // Initialization functions
	// GPIOA -> ODR |= PA8_mask;
	
	// Wait for start button push
	while(1){
		if(libs_check_button_GPIOA(0)){		// PA0 switch
			TIM2->CR1 |= TIM_CR1_CEN;    	// PWM Counter enable
			break;
		}
	}
	// Wait for limit switch trigger
	// Set to state == 1
	while(1){
		if(libs_check_button_GPIOA(11)){	    // PA11 connected to limit switch
			RCC->APB1ENR = RCC_APB1ENR_TIM2EN;	// TIM2 Enable
			state = 1;							// Change state in EXTI handler
			break;
		}
	}
}

void state_2(void){
	while(1){
		if(libs_check_button_GPIOA(0)){	      	// PA0 switch
			tidalStepMultiplier = tidalTotalSteps/256;
			tidalInputSteps = ADC_input(8,8)*tidalStepMultiplier;
			state = 2;							// Change state in EXTI handler
			RCC->APB1ENR = RCC_APB1ENR_TIM2EN;	// Enable PWM
			break;
		}
	}
}


int main(void){


	state_1();
	state_2();

	// Main loop


	while (1){


		// This is not lekker
		if(uartCounter>uartFrequency){
			sensorReadAndSend();
			uartCounter = 0;
		}else{
			uartCounter++;
		}

		 Start button release
		while(1){
			if(!libs_check_button_GPIOA(0)){
				break;
			}
		}
		// Start button press
		while(1){
			if(libs_check_button_GPIOA(0)){	      	// PA0 switch
				state_1();
				state_2();
				// wait for start button press

				//Ventilate


			}
		}
	}

	return 0; //keep compiler happy
}
