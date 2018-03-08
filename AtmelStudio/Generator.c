/*
 * Generator.c
 *
 * Created: 2/21/2018 11:56:29 AM
 *  Author: Developer
 */ 

#include "Generator.h"

volatile uint8_t Generator_TurnedOn = 1;
volatile uint8_t Generator_Mode = 1;

volatile uint32_t Generator_Output1Frequency = 0;
volatile uint8_t Generator_PulseWidth = 1;

//	volatile uint8_t Generator_PulsesQuantity = 0;

//	volatile static uint8_t Generator_EncoderInputA;
//	volatile static uint8_t Generator_EncoderInputB;

void Generator_EncoderInit (void)
{
	GENERATOR_ENCODER_DDR &= ~(1 << GENERATOR_ENCODER_PIN_B | 1 << GENERATOR_ENCODER_PIN_A);		//	Initialize an encoder inputs
	GENERATOR_ENCODER_PORT |= 1 << GENERATOR_ENCODER_PIN_B | 1 << GENERATOR_ENCODER_PIN_A;			//	
	
	GENERATOR_ENCODER_DDR &= ~(1 << GENERATOR_BUTTON_PIN_B | 1 << GENERATOR_BUTTON_PIN_A);			//	Initialize a button inputs
	GENERATOR_ENCODER_PORT |= 1 << GENERATOR_BUTTON_PIN_B | 1 << GENERATOR_BUTTON_PIN_A;			//
	
	cli();																							//	Turn off the interrupt, as described in datasheet
	
	EICRA |= 1 <<ISC31 | 0 << ISC30 | 1 << ISC21 | 0 << ISC20 | 1 << ISC11 | 0 << ISC10 | 1 << ISC01 | 0 << ISC00;				//	Enable interrupts on the failing edge
	EICRA &= ~(0 << ISC31 | 1 << ISC30 | 0 << ISC21 | 1 << ISC20 | 0 << ISC11 | 1 << ISC10 | 0 << ISC01 | 1 << ISC00);			//	
	EIMSK |= 1 << INT3 | 1 << INT2 | 1 << INT1 | 1 << INT0;																		//	
}

void Generator_OutputsInit (void)
{
	GENERATOR_OUTPUTS_DDR |= 1 << GENERATOR_OUTPUT_1;							//	Initialize a generator outputs
	GENERATOR_OUTPUTS_PORT &= ~(1 << GENERATOR_OUTPUT_1);						//	
	
	GENERATOR_LED_DDR |= 1 << GENERATOR_LED_PIN;
	GENERATOR_LED_PORT &= ~(1 << GENERATOR_LED_PIN);
}

void Generator_Init (void)
{
	Generator_EncoderInit();
	Generator_OutputsInit();
}

void Generator_PulseGeneration (uint8_t width)
{
	GENERATOR_LED_PORT |= 1 << GENERATOR_LED_PIN;
	TCCR3A = 1 << COM3A1 | 1 << COM3A0 | 0 << WGM31 | 0 << WGM30;
	TCCR3B = 0 << WGM33 | 1 << WGM32 | 1 << CS32 | 0 << CS31 | 0 << CS30;
	OCR3A = GENERATOR_PULSE_OCR * width;
	TIMSK3 |= 1 << OCIE3A;
}

void Generator_Generation (uint16_t frequency)
{
 	if (Generator_TurnedOn == 1)
	{
		uint8_t mode = 0;
		uint16_t temp = 0;

		if (frequency == 0);													//	
		if ((frequency < 50) && (frequency > 0))	mode = 1;					//	Level 1 - Low frequency mode
		else if ((frequency >= 30) && (frequency < 1000))	mode = 2;			//	Level 2

		switch (mode)
		{
			case 0:
			{
				OCR3A = 0x00;													//	
				break;
			}
			case 1:
			{
				//	PWM Frequency (in PFC mode) = F_CPU / (2 * N * TOP)
		
				TCCR3A = 1 << COM3A1 | 0 << COM3A0 | 0 << WGM30;				//	Turn on PFC PWM mode with TOP = ICR3
				TCCR3B = 1 << WGM33 | 1 << CS32 | 0 << CS31 | 1 << CS30;		//	Prescaler = 1024
				ICR3 = (F_CPU / (2 * 1024 * frequency));						//	Set the frequency, i.e. TOP value
				temp = (F_CPU / (2 * 1024 * frequency));
				OCR3A = (temp * 2 / 10);										//	Set the duty cycle
		
				break;
			}
			case 2:
			{
				TCCR3A = 1 << COM3A1 | 0 << COM3A0 | 0 << WGM30;				//	Turn on PFC PWM mode with TOP = ICR3
				TCCR3B = 1 << WGM33 | 1 << CS32 | 0 << CS31 | 0 << CS30;		//	Prescaler = 256
				ICR3 = (F_CPU / (2 * 256 * frequency));							//	Set the frequency, i.e. TOP value
				temp = (F_CPU / (2 * 256 * frequency));
				OCR3A = (temp * 2 / 10);										//	Set the duty cycle
		
				break;
			}
			case 3:
			{
				
				break;
			}
			default:
			{
				OCR3A = 0x00;													//	
				break;
			}
		}
	}
	else	OCR3A = 0x00;
}

/*
#
#	Interrupt handlers for calculating and setting the frequency
#
*/

ISR (INT0_vect)
{
	switch (Generator_Mode)
	{
		case 1:
		{
			if (Generator_Output1Frequency < 29) Generator_Output1Frequency++;
			break;
		}
		
		case 2:
		{
			if (Generator_PulseWidth < 20)	Generator_PulseWidth++;
			break;
		}
		
		default:	break;
	}
}

ISR (INT1_vect)
{
	switch (Generator_Mode)
	{
		case 1:
		{
			if (Generator_Output1Frequency > 0) Generator_Output1Frequency--;
			break;
		}
		
		case 2:
		{
			if (Generator_PulseWidth > 1)	Generator_PulseWidth--;
			break;
		}
		
		default:	break;
	}
}

ISR (INT2_vect)
{
	if (Generator_Mode == 1)
	{
		TCCR3A = 0x00;
 		TCCR3B = 0x00;
		Generator_Mode = 2;
	}
	else
	{
		TCCR3A = 0x00;
		TIMSK3 = 0x00;
		Generator_Mode = 1;
	}
}

ISR (INT3_vect)
{
	switch (Generator_Mode)
	{
		case 1:
		{
			if (Generator_TurnedOn == 1)	Generator_TurnedOn = 0;
			else	Generator_TurnedOn = 1;
			break;
		}
		
		case 2:
		{
			Generator_PulseGeneration(Generator_PulseWidth);
			break;
		}
		
		default:	break;
	}
}

/*
#
#	Interrupt handler for changing the states of the generator outputs
#
*/

ISR (TIMER3_COMPA_vect)
{
	GENERATOR_OUTPUTS_PORT &= ~(1 << GENERATOR_OUTPUT_1);
	GENERATOR_LED_PORT &= ~(1 << GENERATOR_LED_PIN);
 	TCCR3A = 0x00;
	TIMSK3 &= 1 << OCIE3A;
}
