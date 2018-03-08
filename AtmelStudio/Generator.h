/*
 * Generator.h
 *
 * Created: 2/21/2018 11:56:48 AM
 *  Author: Developer
 *	
 *	OCR = (F_CPU / (2 * Generator_Prescaler * Generator_Frequency)) - 1
 *	
 *	#####################################################
 *	#	ATmega32U4	--------------	Arduino Leonardo	#
 *	#													#
 *	#	Generator output, PC6	--	DP - 5				#
 *	#													#
 *	#	Encoder input A, PD0	--	DP - 3				#
 *	#	Encoder input B, PD1	--	DP - 2				#
 *	#	Mode button, PD2		--	DP - 1				#
 *	#	Pulse button, PD3		--	DP - 0				#
 *	#													#
 *	#	LCD pin RS, PB1	----------	DP - SCK			#
 *	#	LCD pin RW, PB2	----------	DP - MOSI			#
 *	#	LCD pin E, PB3	----------	DP - MISO			#
 *	#	LCD pin D4, PB4	----------	DP - 8				#
 *	#	LCD pin D5, PB5	----------	DP - 9				#
 *	#	LCD pin D6, PB6	----------	DP - 10				#
 *	#	LCD pin D7, PB7	----------	DP - 11				#
 *	#													#
 *	#####################################################
 */ 


#ifndef GENERATOR_H_
#define GENERATOR_H_

#include "main.h"

#define GENERATOR_OUTPUTS_DDR DDRC
#define GENERATOR_OUTPUTS_PORT PORTC
#define GENERATOR_OUTPUTS_PINS PINC
#define GENERATOR_OUTPUT_1 6

#define GENERATOR_ENCODER_DDR DDRD
#define GENERATOR_ENCODER_PORT PORTD
#define GENERATOR_ENCODER_PINS PIND

#define GENERATOR_ENCODER_PIN_A 0
#define GENERATOR_ENCODER_PIN_B 1

#define GENERATOR_BUTTON_PIN_A 2
#define GENERATOR_BUTTON_PIN_B 3

#define GENERATOR_LED_DDR DDRD
#define GENERATOR_LED_PORT PORTD
#define GENERATOR_LED_PINS PIND
#define GENERATOR_LED_PIN 6

#define GENERATOR_PULSE_OCR 624

volatile uint8_t Generator_TurnedOn;
volatile uint8_t Generator_Mode;

volatile uint32_t Generator_Output1Frequency;
volatile uint8_t Generator_PulseWidth;

void Generator_Init (void);
void Generator_Generation (uint16_t frequency);

#endif /* GENERATOR_H_ */