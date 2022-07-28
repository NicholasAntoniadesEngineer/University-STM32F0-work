


TIMERS
-------




======================================================================================
/*Basic Timers*/
============================================================================================

The prescalar (PSC) register 
• used to divide CK_PSC by a factor to obtain CK_CNT for Timer 14.

Counter (CNT) register
• On every rising edge of CK_CNT, the value in the CNT register increments
• CNT counts from 0, in steps of 1 to the value in ARR

Events when CNT=ARR:
1. CNT resets to 0
2. Update Event (U) triggered
3. Update Event Interrupt (UI)triggered

Setup TIM14
• Step 1: Enable the TIM14 clock source
• Step 2: Set parameter values for PRESCALER and ARR
• Step 3: Enable the TIM14 update event interrupt (UIE)
• Step 4: Start TIM14
• Step 5: Clear the TIM14 update interrupt flag in the ISR
• Clear UIF (bit 0) in the TIM14 Status Register in the interrupt service routine


============================================================================================
/*General Purpose Timers*/
============================================================================================

General Purpose Timer (GPT)
• 16-bit or 32-bit
• Has functionality of basic timer and Input 
capture and Output compare.

STM32F0 has 6 General Purpose Timers
• TIM 2: 32 bit
• TIM 3, TIM 14, TIM15, TIM 16, TIM 17: 16 bit


==============
--------------
Input capture
--------------
==============

Application
• Measure the time between two button presses
• Measure the time between two rising edges

• For an input signal, a General Purpose Timer can be used 
to measure the time between events.
• Examples of an event: rising edge, falling edge, ...
•The GPT records the value of the CNT register at t1 
and t2. This can be used to calculate T, where: T = t2 -t1.
• Store the value of CCR1 into an array in the interrupt service routine 
and then calculate the difference between two successive indices.

For input signals that need debouncing (e.g. 8 clock cycles too stabilise)
• set IC1F[3:0] to ‘0011’ in TIM14_CCMR1. This generates a single clean rising edge after 8
consecutive samples.
->This is done using the input capture filter.

When input event occurs, the following steps occur
1. CC1IF flag is set in TIM_SR
2. An event update (U) is generated
3. An event update interrupt (UI) is generated, if enabled
4. The value of the CNT counter register is stored in the TIM14_CCR1 register
–>1st event: TIM14_CCR1 = 4
–>2nd event: TIM14_CCR1 = 20

Calculate the time (in seconds) between two rising edges.
• Calculate the period of the TIM14_CLK
–> Period of TIM14_CLK = 1/frequency of clock.
• Calculate the number of clock cycles between the two events
–> Number of clock cycles = (ARR - CNT1) + CNT2 + 1


------------------
Summary of C-code
------------------

/*Defining macros*/
# define GPIO_AFRL_PIN7_AF_0 = 0b00010000000000000000000000000000;
# define GPIO_AFRL_PIN7_AF_1 = 0b00100000000000000000000000000000;
# define GPIO_AFRL_PIN7_AF_2 = 0b01000000000000000000000000000000;
# define GPIO_AFRL_PIN7_AF_3 = 0b10000000000000000000000000000000;

/*Enabling GPIOA and GPIOB*/
RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;		// Enable clock PORT A.
RCC -> APB1ENR |= RCC_APB1ENR_TIM14EN;		// Enable clock for TIM14.

/*Map PA7 to TIM14_CH1 using AF4*/
GPIOA -> MODER |= GPIO_MODER_MODER7_1; 		// Set PA7 to AF mode.
GPIOA -> AFR[0] &= ~GPIO_AFRL_PIN7_AF_0; 	// Clear bit 0 of AFR7[3:0].
GPIOA -> AFR[0] &= ~GPIO_AFRL_PIN7_AF_1; 	// Clear bit 1 of AFR7[3:0].
GPIOA -> AFR[0] |= GPIO_AFRL_PIN7_AF_2; 	// Set bit 2 of AFR7[3:0].
GPIOA -> AFR[0] &= ~GPIO_AFRL_PIN7_AF_3; 	// Clear bit 3 of AFR7[3:0].

/*Setting up TIM14*/
TIM14 -> PSC = 7; 			// Setting pre scalar.
TIM14 -> ARR = 59 999; 			// Setting ARR value.
TIM14 -> CCER |= TIM_CCER_CC1E;		// Enable input capture for timer 14.
TIM14 -> DIER |= TIM_DIER_CC1IE;	// Enable interrupt for TIM 14.
NVIC_EnableIRQ(TIM14_IRQn);		// Unmask the interrupt in the NVIC.

TIM14 -> CCMR1 |= TIM_CCMR1_CC1S_0; 			// Configure TIM14 for IC		 
TIM14 -> CCMR1 |= (TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1);// Debouncing to 8 clock cycles with the IC filter.
TIM14 -> CCMR1 &= ~TIM_CCMR1_IC1PSC; 	// Choose event capture frequency,set PSC = 0 to capture every event.
TIM14 -> CCER &= ~TIM_CCER_CC1NP;       // Setting channel as output.
TIM14 -> CCER &= ~TIM_CCER_CC1P;        // Set channel as active high.

TIM14 -> CR1 |= TIM_CR1_CEN;		// enable counter for TIM14


=================
-----------------
Output compare
-----------------
=================

• On a specific output pin (example PB1), the GPT can be used 
to generate a square waveform with a known period T.

• Period T
->Tov = (1/ f CLK )*(PSC+1)*(ARR+1)
->T = 2*T ov 

• When an event occurs, three processes are triggered
->(An event occurs when CCR1 == CNT)
1. The output pin changes state, depending on OC1M bits in the CCMR1 register.
   ->Keep same, Take high, Take low, Toggle (ie. change state)
2. The CC1IF flag is set in TIM14_SR
3. An interrupt is generated, if it was enabled


------------------
Summary of C-code
------------------

/*Defining macros*/
# define GPIO_AFRL_PIN1_AF0 = 0b00000000000000000000000011110000;

/*Setting up ports*/
RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;		// Enable clock GPIOB.
GPIOB -> MODER |= GPIO_MODER_MODER1_1; 		// Set PB1 to AF mode.
GPIOB -> AFR[0] &= ~GPIO_AFRL_PIN1_AF0;		// Set PB1 to AF0.

/*Setting up TIM14*/
RCC -> APB1ENR |= RCC_APB1ENR_TIM14EN; 	 // Enable clock for TIM14
TIM14 -> CCER &= ~TIM_CCER_CC1NP;	 // Setting channel as output.
TIM14 -> CCER &= ~TIM_CCER_CC1P; 	 // Set channel as active high.
TIM14 -> CCER |= TIM_CCER_CC1E; 	 // Enable output compare.
TIM14 -> CCMR1 &= ~TIM_CCMR1_CC1S; 	 // Configure TIM14 for Output Control.
TIM14 -> CCMR1 |= (TIM_CCMR1_OC1M_0 |TIM_CCMR1_OC1M_1); Setting output compare mode to toggle.
TIM14 -> CCMR1 &= ~TIM_CCMR1_OC1M_2; 	 // Select output state to toggle.

/*Choosing event frequency*/
TIM14 -> PSC = 7;	// Setting PSC.
TIM14 -> ARR = 59 999; // Setting ARR.
TIM14 -> CCR1 = 59 999; // Setting CCR1 PSC.
->Assuming f CLK = 48MHz, T ov = 10ms, T = 20ms.

/*Enable Interrupts*/
TIM14 -> DIER |= TIM_DIER_CC1IE; 	// Enable interrupt for TIM 14
NVIC_EnableIRQ(TIM14_IRQn); 		// Unmask the interrupt in the NVIC.

TIM14 -> CR1 |= TIM_CR1_CEN; 		// Enable counter for TIM14.


=========================
-------------------------
Output Compare Mode: PWM
-------------------------
=========================
/**/

• List of GPT that can be used.
->TIM2, TIM3, TIM14, TIM15, TIM16, TIM17

• Timers in STM32F0 that can generate edge-aligned PWM.
->TIM1, TIM2, TIM3, TIM14, TIM16, TIM17

• Timers in STM32F0 that can generate both edge-aligned and centre-aligned PWM.
->TIM1, TIM2, TIM3

• Timer 14 is only able to generate a single PWM output.
• TIM2 can generate 4 independent PWM outputs.

• Calculate the T, Ton, Duty cycle.
->T = 1/f CLK x (PSC+1) x (ARR +1)
->Duty cycle = CCR1/(ARR+1)
->Ton = Duty cycle x T

----------
RC servos
----------

• The PWM duty cycle controls the angular position of the shaft
->Duty cycle = 5%,   Position 0°
->Duty cycle = 7.5%, Position 90°
->Duty cycle = 10%,  Position 180°

• A RC servos has 3 input wires:
->Signal – +3V to 5V, PWM waveform with a frequency of 50Hz [T = 20ms]
->Power – +4.8V to +6V
->Ground – 0V




------------------
Summary of C-code
------------------

/*Defining macros*/
# define GPIO_AFRH_PIN10_AF2 = 0b00000000000000000000001000000000;

/*Setting up ports*/
RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;	 // Enable clock PORT B
RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;	 // Enable clock for TIM2
GPIOA -> MODER |= GPIO_MODER_MODER10_1;	 // Set PB10 to AF mode
GPIOB -> AFR[1] |= GPIO_AFRH_PIN10_AF2;  // Set PB10 to AF2


/*Setting up TIM2*/
TIM2 -> CCER |= TIM_CCER_CC1E; 	  	// Enable output compare
TIM2 -> CCER &= ~TIM_CCER_CC3NP; 	// Clear CC3NP bit for OC
TIM2 -> CCMR1 &= ~TIM_CCMR1_CC1S;	// Configure TIM2 for OC
TIM2 -> CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); // PWM mode 1
TIM2 -> CCMR1 &= ~TIM_CCMR1_OC1M_0;	// PWM mode 1

/*Choosing event frequency*/
TIM2 -> PSC = 0;	          // Write ‘0’ to TIM2_PSC
TIM2 -> ARR = 47999;	  	  // Write ‘47999’ to TIM2_ARR
TIM2 -> CCR3 = 38400; 	  	  // 0.8 x 48000 = 38400	
TIM2 -> CR1 &= ~TIM_CR1_CMS;	  // Set edge-aligned PWM
TIM2 -> CCER &= ~TIM_CCER_CC3P;   // Set channel 3 as active high

TIM2 -> CR1 |= TIM_CR1_CEN; // enable counter for TIM2




============================================================================================
/*Notes*/
============================================================================================

--------------------------------
For registers with 2-bit options
--------------------------------	



--------------------------------
For registers with 3-bit options
--------------------------------

(e.g.)

#define  TIM_CCMR1_OC1M_0      ((uint16_t)0x0010)    /*!<Bit 0 */  // 001
#define  TIM_CCMR1_OC1M_1      ((uint16_t)0x0020)    /*!<Bit 1 */  // 010
#define  TIM_CCMR1_OC1M_2      ((uint16_t)0x0040)    /*!<Bit 2 */  // 100
#define  TIM_CCMR1_OC1M        ((uint16_t)0x0070)    /*!<All 3 */  // 111	

============================================================================================


Interrupts
----------

======================================================================================
/*EXTI*/
============================================================================================

An interrupt can be triggered on the following change in a GPIO pin
• Rising edge
• Falling edge
• Both rising and falling edge


1. The EXTI controller maps GPIO lines to interrupt lines of the NVIC

2. One GPIO pin, selected from PA[x] – PF[x], is linked to external line EXTI[x]
   ->EXTI[x], the external line being mapped to using the MUX seletor.
   ->EXTI0_1, the interupt line 

3. Two external lines map to one interrupt line
• EXTI0 and EXTI1 are both mapped to the interrupt line EXTI0_1
• How do we set one external line to an interrupt line?
• By masking EXTI0 and unmasking EXTI1, EXTI1 is mapped to EXTI0_1
• Masking of an external line is used to make the external line inactive
• Unmasking of an external line is used to make the external line active


------------------
Summary of C-code
------------------

/*Initialisations*/
RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN; // SYSCFG and comparator clock enable.

/*Setting up External line*/
SYSCFG -> EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;  //Map PA[x] to external line EXTI[x].
EXTI -> IMR |= EXTI_IMR_MR0; 			 // Unmask EXTI[x] using mask MR[x].
EXTI -> FTSR |= EXTI_FTSR_TR0;	       		 // Trigger on falling edge.

/*Enabling Interrupt*/
NVIC_EnableIRQ(EXTI0_[x]_IRQn);         // Enable interrupt EXTI[x]_[y] in the NVIC.	


/*ISR for the External Interrupt*/
EXTI -> PR |= EXTI_PR_PR0;	 // PR bit cleared by writing a 1 to it.	


============================================================================================
/*Notes*/
============================================================================================


------------------------     --------------
External interrupt line: <-> External line:
------------------------     -------------- 
 SYSCFG_EXTICR1  	     EXTI0, EXTI1, EXTI2, EXTI3 
 SYSCFG_EXTICR2 	     EXTI4, EXTI5, EXTI6, EXTI7
 SYSCFG_EXTICR3		     EXTI8, EXTI9, EXTI10, EXTI11
 SYSCFG_EXTICR4 	     EXTI12, EXTI13, EXTI4, EXTI5

--------------- --------------------    --------------
Interrupt:      NVIC Interrupt line:	External line:
--------------- --------------------    -------------- 
EXTI0_1_IRQn    	5     	   	EXTI Line 0 and 1 
EXTI2_3_IRQn    	6          	EXTI Line 2 and 3  
EXTI4_15_IRQn   	7          	EXTI Line 4 to 15





















