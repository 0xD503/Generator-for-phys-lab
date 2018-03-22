/*
 * Generator.c
 *
 * Created: 2/21/2018 11:56:29 AM
 *  Author: Developer
 */ 

#include "Generator.h"

volatile uint8_t Generator_TurnedOn = 1;							//	Generator state flag
volatile uint8_t Generator_Mode = 1;								//	

volatile uint32_t Generator_Output1Frequency = 0;					//	
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
	
	GENERATOR_LED_DDR |= 1 << GENERATOR_LED_PIN;								//	Initialize a generator led outputs
	GENERATOR_LED_PORT &= ~(1 << GENERATOR_LED_PIN);
}

void Generator_Init (void)
{
	Generator_EncoderInit();													//	
	Generator_OutputsInit();
}

void Generator_PulseGeneration (uint8_t width)
{
	GENERATOR_LED_PORT |= 1 << GENERATOR_LED_PIN;								//	Function for single pulse generation
	TCCR3A = 1 << COM3A1 | 1 << COM3A0 | 0 << WGM31 | 0 << WGM30;				//	Using Timer3 in CTC mode (set output on compare)
	TCCR3B = 0 << WGM33 | 1 << WGM32 | 1 << CS32 | 0 << CS31 | 0 << CS30;		//	Prescaler = 256
	OCR3A = GENERATOR_PULSE_OCR * width;										//	Setting pulse width
	TIMSK3 |= 1 << OCIE3A;														//	Enable interrupt
}

void Generator_Generation (uint16_t frequency)
{
 	if (Generator_TurnedOn == 1)
	{
		uint8_t mode = 0;
		uint32_t temp = 0;

		if (frequency == 0);													//	No pulse
		else if ((frequency < 50) && (frequency > 0))	mode = 1;				//	Level 1 - Low frequency mode
		else if ((frequency >= 50) && (frequency < 1000))	mode = 2;			//	Level 2 - 8 Hz - and more

		switch (mode)
		{
			case 0:
			{
				OCR3A = 0x00;													//	Frequency = 0
				break;
			}
			case 1:
			{
				//	PWM Frequency (in PFC mode) = F_CPU / (2 * N * TOP)
		
				TCCR3A = 1 << COM3A1 | 0 << COM3A0 | 0 << WGM30;				//	Turn on PFC PWM mode with TOP = ICR3
				TCCR3B = 1 << WGM33 | 1 << CS32 | 0 << CS31 | 0 << CS30;		//	Prescaler = 256
				ICR3 = (F_CPU / (2 * 2 * 256 * frequency));						//	Set the frequency, i.e. TOP value
				temp = (F_CPU / (2 * 2 * 256 * frequency));
				OCR3A = (temp * 26 / 10000);									//	Set the duty cycle
		
				break;
			}
			case 2:
			{
// 				TCCR3A = 1 << COM3A1 | 0 << COM3A0 | 0 << WGM30;				//	Turn on PFC PWM mode with TOP = ICR3
// 				TCCR3B = 1 << WGM33 | 1 << CS32 | 0 << CS31 | 0 << CS30;		//	Prescaler = 256
// 				ICR3 = (F_CPU / (2 * 256 * frequency));							//	Set the frequency, i.e. TOP value
// 				temp = (F_CPU / (2 * 256 * frequency));
// 				OCR3A = (temp * 1 / 10);										//	Set the duty cycle
		
				break;
			}
			case 3:
			{
				
				break;
			}
			default:
			{
				OCR3A = 0x00;													//	Frequency = 0
				break;
			}
		}
	}
	else	OCR3A = 0x00;														//	Disable generation (PWM)
}

/*
#
#	Interrupt handlers for calculating and setting the frequency
#
*/

ISR (INT0_vect)																		//	Button 1 interrupt
{
	_delay_ms(90);
	switch (Generator_Mode)
	{
		case 1:
		{
			if (Generator_Output1Frequency < 29) Generator_Output1Frequency++;		//	Increment generation frequency
			break;
		}
		
		case 2:
		{
			if (Generator_PulseWidth < 20)	Generator_PulseWidth++;					//	Increment pulse width
			break;
		}
		
		default:	break;
	}
}

ISR (INT1_vect)																		//	Button 2 interrupt
{
	_delay_ms(90);
	switch (Generator_Mode)
	{
		case 1:
		{
			if (Generator_Output1Frequency > 0) Generator_Output1Frequency--;		//	Decrement generation frequency
			break;
		}
		
		case 2:
		{
			if (Generator_PulseWidth > 1)	Generator_PulseWidth--;					//	Or decrement pulse width
			break;
		}
		
		default:	break;
	}
}

ISR (INT2_vect)
{
	_delay_ms(90);
	if (Generator_Mode == 1)					//	Set single pulse mode
	{
		TCCR3A = 0x00;
 		TCCR3B = 0x00;
		Generator_Mode = 2;
	}
	else										//	Set generation mode
	{
		TCCR3A = 0x00;
		TIMSK3 = 0x00;
		Generator_Mode = 1;
	}
}

ISR (INT3_vect)
{
	_delay_ms(90);
	switch (Generator_Mode)
	{
		case 1:															//	Switch on/off generator
		{
			if (Generator_TurnedOn == 1)	Generator_TurnedOn = 0;
			else	Generator_TurnedOn = 1;
			break;
		}
		
		case 2:															//	Generate single pulse
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
	GENERATOR_OUTPUTS_PORT &= ~(1 << GENERATOR_OUTPUT_1);				//	When the time == pulse width
	GENERATOR_LED_PORT &= ~(1 << GENERATOR_LED_PIN);					//	Disable generator and led outputs, interrupt.
 	TCCR3A = 0x00;
	TIMSK3 &= 1 << OCIE3A;
}
