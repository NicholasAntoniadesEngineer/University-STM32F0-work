
=======================================================================================================
Initialisations
=======================================================================================================
-------------------------------------------------------------------------------------------------------

void initPorts(void)
	{
	-->LEDS
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //enable clock for GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enable clock for GPIOA
	GPIOB->MODER |= 0x505555;    //set PB0-7 as output mode as well as PB10 (red) and PB11 (green)

	-->Buttons
	GPIOA->PUPDR |=0b01010101;  //enable pull up resistors for SW0-3
	GPIOA->MODER &= 0xFFFFF0000;  //set PA0-3 to input mode.
	}


Steps:
(1)Enable the clock for the bus of the port/s which are going to be used.
->Mask the RCC Advanced High Performance Bus Enable Register.

(2)Set the port/s mode in the GPIOB Mode Register(MODER) with the correct bit mask.
->For using the LEDS set the correct ports to output.
->If port/s being used for PWM then set to alternate mode.


------------------------------------------------------------------------------------------------------

void init_ADC(void)
		{
	RCC -> APB2ENR |= RCC_APB2ENR_ADCEN; //Enabling the ADC clock in the RCC APB  periperal clock enableregister
	ADC1 -> CFGR1 |= 0x10;//0b10000               //Setting the ADC resolution in the ADC configuration register 1
	ADC1->CHSELR = 0b100000;	     // Selecting the chanel for pot-0.
	ADC1 -> CR |= ADC_CR_ADEN;           //Enabling the ADC
	while((ADC1 -> ISR & 0x01) ==0 );    //Waiting for the ADRDY pin to let us know the ADC is ready.
		}


Steps:
(1)Enable the ADC clock in the RCC APB2ENR  periperal clock enableregister
->Mask the RCC AHPB2ENR with the RCC_APB2ENR_ADCEN

(2)Configure the ADC resolution in the ADC1 CFGR1 REGISTER.
->Mask the ADC1 CFGR1 register to get the desired resolution(bits 3 and 4 determine the resolution.

(3)Select which channel the ADC must sample from in the ADC1 CHSELR
->Choose appropriate channel from the reference manual.

(4)Enable the ADC in the ADC CR.
->Mask the ADC CR with ADC_CR_ADEN to enable

(5)Wait for the ADRDY pin to go high when the ADC is ready.
->Bit wise and with the value held in the ADC ISR and 0x01 and when it returns 1 the ADC is ready.



--------------------------------------------------------------------------------------------------------

void init_NVIC(void)
	{
	NVIC_EnableIRQ(EXTI0_1_IRQn); //Enable EXTI0_1 in NVIC
	}


--------------------------------------------------------------------------------------------------------


void init_EXTI(void)
	{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;	//enables the clock for the sys config controller.
	SYSCFG->EXTICR[1] |=SYSCFG_EXTICR1_EXTI1_PA;    //Map PA1 to EXTI1.
	EXTI->IMR |=EXTI_IMR_MR1; 			//unmasks EXTI1 in interrupt mask register
	EXTI->FTSR |= EXTI_FTSR_TR1; 			//falling trigger enable for input line 1 (SW1)
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	}

Steps:
(1)Enable the clock for the SYSCFG and comparator clock in RCC

(2)Select PA1 as the source input for the EXTI1 external interupt.
->In EXTICR1 of SYSCFG, map PA1 to the EXTI1 line.

(3)unmask EXTI1 in the interrupt mask register
->Lets the interrupt through from the desired interrupt line.

(4)Enable falling trigger for input line 1 (SW1) 

(5)Enable the external interrupt in the NVIC.



--------------------------------------------------------------------------------------------------------


void initTim6(void)
	{
	RCC->APB1ENR |= (1 << 4);
	TIM6 -> PSC = 2829;
	TIM6 -> ARR = 2829;
	TIM6 -> DIER |= TIM_DIER_UIE;
	TIM6 -> CR1 |= TIM_CR1_CEN;
	TIM6-> CR1 |= TIM_CR1_ARPE;
	NVIC_EnableIRQ(TIM6_IRQn); #writing to the NVIC ISER(interrupt set enable register)
	}



(1)Enable timers clock in the correct RCC peripheral bus enable register.
(2)Set ARR and PSC values.
(3)Enable the timers in its interrupt enable register(DIER).
(4)Enable the timers counter in the timers control register
(5)Enable auto-reload preload in the timers control register.
(6)Enable the timers interrupt in the NVICs

-------------------------------------------------------------------------------------------------------
=======================================================================================================
In general:
(1)Enable the peripherals clock.
->check which peripheral bus has the enable register with the peripherals clock enable bit (e.g.) APB1ENR.
->Use the correct enable mask and |= it with the enable register.


=======================================================================================================
Convert to ASCII for the LCD.
=======================================================================================================


int count = 0;
while(count<length)
{									//puts ASCII value of number in the array
	if(count == 0)
	{
		time_min[count]= (char)(min_d % 10 +48);
	}
	else
	{
		min_d /= 10;
		time_min[count]= (char)(min_d % 10 +48);
	}
	count++;

}
for(int i=length-1; i>=0 ; i--)
{						//displays array on LDC
	lcd_putchar(time_min[i]);
}
lcd_putchar(':');
for (int counter = 0;counter< length ; counter ++)
{
	time_sec[counter]= (char)(48);
}
count = 0;





















