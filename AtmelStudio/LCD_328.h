/*
 * LCD_4_bit.c
 *
 * Created: 10/19/2017 2:36:17 PM
 *  Author: Developer
 */ 

#ifndef LCD_328_H_
#define LCD_328_H_

#include "main.h"

// Set LCD ports and pins

#define LCD_DDR DDRB
#define LCD_PORT PORTB
#define LCD_PINS PINB

#define LCD_ENABLE_PIN 3
#define LCD_RW_PIN 2
#define LCD_RS_PIN 1

#define LCD_DATA_PIN_4 4
#define LCD_DATA_PIN_5 5
#define LCD_DATA_PIN_6 6
#define LCD_DATA_PIN_7 7

//	#########################################

//	LCD Instruction set

#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02
#define LCD_SET_DDRAM_ADRESS 0x80

#define LCD_4_BIT_2_LINES_FONT_5X8 0x28
#define LCD_SHIFT_CURSOR_TO_THE_RIGHT 0x14
#define LCD_CURSOR_ON_BLINKING_ON 0x0F
#define LCD_INCREMENT_CURSOR 0x06

//	#########################################

//	Macros for controlling LCD

#define LCD_DDR_INIT LCD_DDR = ((LCD_DDR &= ~(1 << LCD_DATA_PIN_4 | 1 << LCD_DATA_PIN_5 | 1 << LCD_DATA_PIN_6 | 1 << LCD_DATA_PIN_7)) | 1 << LCD_ENABLE_PIN | 1 << LCD_RW_PIN | 1 << LCD_RS_PIN)
#define LCD_PORT_INIT LCD_PORT |= 1 << LCD_RS_PIN | 1 << LCD_RW_PIN | 1 << LCD_ENABLE_PIN | 1 << LCD_DATA_PIN_4 | 1 << LCD_DATA_PIN_5 | 1 << LCD_DATA_PIN_6 | 1 << LCD_DATA_PIN_7

#define LCD_CLEAR_DATA_PINS LCD_PORT &= ~(1 << LCD_DATA_PIN_4 | 1 << LCD_DATA_PIN_5 | 1 << LCD_DATA_PIN_6 | 1 << LCD_DATA_PIN_7)

#define LCD_STROBE_UP LCD_PORT |= (1 << LCD_ENABLE_PIN)
#define LCD_STROBE_DOWN LCD_PORT &= ~ (1 << LCD_ENABLE_PIN)

#define LCD_DATA_MODE LCD_PORT |= (1 << LCD_RS_PIN);
#define LCD_INSTRUCTION_MODE LCD_PORT &= ~ (1 << LCD_RS_PIN)

#define LCD_WRITE_MODE LCD_PORT &= ~ (1 << LCD_RW_PIN)
#define LCD_READ_MODE LCD_PORT |= (1 << LCD_RW_PIN)

//	#########################################

#define ASCII_NULL 0x30
#define LCD_LINE_SIZE 0x40
#define LCD_FLAG_BUSY 0x80


void LCD_Init (void);

void LCD_PutByte (int8_t data);
void LCD_SendString (int8_t* string);
void LCD_SendInteger (int16_t integerValue);

void LCD_SetPosition (uint8_t x, uint8_t y);
void LCD_SetDDRAM (uint8_t address);
void LCD_ClearDisplay (void);

#endif /* LCD_328_H_ */