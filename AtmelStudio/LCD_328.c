#include "LCD_328.h"

static uint8_t LCD_Address;
static uint8_t LCD_Data;

void LCD_GetAddressAndBFStatus ()
{
	LCD_DDR_INIT;
	LCD_CLEAR_DATA_PINS;
	LCD_READ_MODE;					//	R/W = 1
	LCD_INSTRUCTION_MODE;			//	RS = 0
	
	cli();							//	Prepare to the atomic operation
	
	LCD_STROBE_UP;					//	ENABLE PIN = 1
	asm("nop");						//	Waiting for
	asm("nop");
	LCD_Address = LCD_PINS;			//	Get high nibble
	LCD_STROBE_DOWN;				//	ENABLE PIN = 0
	asm("nop");
	asm("nop");
	LCD_Address &= 0xF0;			//	Prepare an address variable to get a low nibble
	
	LCD_STROBE_UP;					//	ENABLE PIN = 1
	asm("nop");
	asm("nop");
	uint8_t lowNibble = LCD_PINS;	//	Get low nibble
	LCD_STROBE_DOWN;				//	ENABLE PIN = 0
	
	sei();							//	End of the atomic operation
	
	lowNibble >>= 4;				//	Prepare a low nibble
	LCD_Address |= lowNibble;		//	Save address
	
	LCD_DDR |= (1 << LCD_DATA_PIN_4 | 1 << LCD_DATA_PIN_5 | 1 << LCD_DATA_PIN_6 | 1 << LCD_DATA_PIN_7);
}

void LCD_CheckBF (void)
{
	loop:
	LCD_GetAddressAndBFStatus();						//	Get address and BF status
	while (LCD_FLAG_BUSY & LCD_Address)	goto loop;		//
}

void LCD_SendInstructionAndData (uint8_t byte, uint8_t mode)
{
	LCD_WRITE_MODE;										//	R/W = 0
	if (mode == 0)	LCD_INSTRUCTION_MODE;				//	RS = 0 or...
	else	LCD_DATA_MODE;								//	... RS = 1
	
	uint8_t lowNibble = byte << 4;						//	Save low nibble before sending the high nibble
	byte &= 0xF0;										//	Prepare a high nibble
	
	cli();												//	Start the atomic operation
	
	LCD_CLEAR_DATA_PINS;
	LCD_PORT |= byte;									//	Sending high nibble
	LCD_STROBE_UP;
	asm("nop");
	asm("nop");
	asm("nop");
	LCD_STROBE_DOWN;
	asm("nop");
	asm("nop");
	LCD_CLEAR_DATA_PINS;
	LCD_PORT |= lowNibble;
	LCD_STROBE_UP;
	asm("nop");
	asm("nop");
	asm("nop");
	LCD_STROBE_DOWN;
	
	sei();												//	End of the atomic operation
	
	LCD_CheckBF();										//	Wait while Busy Flag = 1
}

void LCD_PutInstruction (uint8_t instruction)
{
	LCD_SendInstructionAndData(instruction, 0);					// Send instruction byte
}

void LCD_PutByte (int8_t data)
{
	LCD_SendInstructionAndData(data, 1);						// Send data byte
}

void LCD_SendString (int8_t* string)
{
	while (* string != 0)										//	While there is no the NULL CHARACTER (or the '\0', '\n', '\r', etc...)
	{
		LCD_PutByte(* string);									//	Send the next data byte
		string++;
	}
}

void LCD_SendInteger (int16_t integerValue)
{
	if (integerValue < 0)
	{
		LCD_PutByte('-');
		integerValue = abs(integerValue);
	}
	
	uint8_t order = 0;
	if ((integerValue < 10000) | (integerValue > 999))	order = 4;
	else if ((integerValue < 1000) | (integerValue > 99))	order = 3;
	else if ((integerValue < 100) | (integerValue > 9))	order = 2;
	else if (integerValue < 10)	order = 1;
	
	switch (order)
	{
		case 1:	LCD_PutByte((((integerValue % 1000) % 100) % 10) + ASCII_NULL);
		break;
		
		case 2:
		LCD_PutByte((((integerValue % 1000) % 100) / 10) + ASCII_NULL);
		LCD_PutByte((((integerValue % 1000) % 100) % 10) + ASCII_NULL);
		break;
		
		case 3:
		LCD_PutByte(((integerValue % 1000) / 100) + ASCII_NULL);
		LCD_PutByte((((integerValue % 1000) % 100) / 10) + ASCII_NULL);
		LCD_PutByte((((integerValue % 1000) % 100) % 10) + ASCII_NULL);
		break;
		
		case 4:
		LCD_PutByte((integerValue / 1000) + ASCII_NULL);
		LCD_PutByte(((integerValue % 1000) / 100) + ASCII_NULL);
		LCD_PutByte((((integerValue % 1000) % 100) / 10) + ASCII_NULL);
		LCD_PutByte((((integerValue % 1000) % 100) % 10) + ASCII_NULL);
		break;
		
		default: LCD_SendString((int8_t* ) "Invalid number");
		break;
	}
}

void LCD_SetPosition (uint8_t x, uint8_t y)
{
	uint8_t position = ((x - 1) + (y - 1) * LCD_LINE_SIZE) | LCD_SET_DDRAM_ADRESS;		//	First position - (1; 1)
	LCD_PutInstruction(position);
}

void LCD_SetDDRAM (uint8_t address)
{
	address |= LCD_SET_DDRAM_ADRESS;
	LCD_PutInstruction(address);
}

void LCD_ClearDisplay (void)
{
	LCD_PutInstruction(LCD_CLEAR_DISPLAY);
}

void LCD_GetData ()
{
	LCD_DDR_INIT;									//	Set LCD pins as inputs
	LCD_READ_MODE;									//	R/W = 1
	LCD_DATA_MODE;									//	RS = 1
	
	cli();											//	Start the atomic operation
	
	LCD_STROBE_UP;
	asm("nop");
	asm("nop");
	LCD_Data = LCD_PINS;
	LCD_STROBE_DOWN;
	asm("nop");
	asm("nop");
	LCD_Data &= 0xF0;
	LCD_STROBE_UP;
	asm("nop");
	asm("nop");
	uint8_t lowNibble = LCD_PINS;
	LCD_STROBE_DOWN;
	
	sei();											//	End of the atomic operation
	
	lowNibble >>= 4;
	LCD_Data |= lowNibble;
	
	LCD_DDR |= (1 << LCD_DATA_PIN_4 | 1 << LCD_DATA_PIN_5 | 1 << LCD_DATA_PIN_6 | 1 << LCD_DATA_PIN_7);				//	Set LCD pins as outputs
	LCD_CheckBF();
}

void sendInstructionLCD8 (uint8_t byte)
{
	LCD_PORT = byte;
	LCD_STROBE_UP;
	asm ("nop");
	asm ("nop");
	asm ("nop");
	LCD_STROBE_DOWN;
}

void LCD_Init (void)
{
	cli();												//	Start the atomic operation
	
	_delay_ms(16);
	sendInstructionLCD8(0x30);							//	8-bit mode
	_delay_us(4500);
	sendInstructionLCD8(0x30);							//	8-bit mode
	_delay_us(115);
	sendInstructionLCD8(0x30);							//	8-bit mode
	LCD_CheckBF();										//	Busy Flag waiting
	sendInstructionLCD8(0x20);							//	4-bit mode
	LCD_CheckBF();
	LCD_PutInstruction(LCD_4_BIT_2_LINES_FONT_5X8);		//	Character font 5x8
	
	sei();												//	End of the atomic operation
	
	LCD_PutInstruction(LCD_SHIFT_CURSOR_TO_THE_RIGHT);	// 
	LCD_PutInstruction(LCD_CURSOR_ON_BLINKING_ON);		//	
	LCD_PutInstruction(LCD_INCREMENT_CURSOR);			// 
	LCD_PutInstruction(LCD_RETURN_HOME);				//	Entry mode set
	LCD_PutInstruction(LCD_CLEAR_DISPLAY);
}