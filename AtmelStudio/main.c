/*
 * Generator.c
 *
 * Created: 2/20/2018 4:52:37 PM
 * Author : Developer
 */ 

#include "main.h"


void IO_Init (void)
{
	
}

void setup (void)
{
	IO_Init();
	LCD_Init();
	Generator_Init();
	
	sei();
}

int main(void)
{
	setup();
	
    while (1) 
    {
		switch (Generator_Mode)
		{
			case 1:
			{
				if (Generator_TurnedOn)
				{
					Generator_Generation(Generator_Output1Frequency);
					LCD_ClearDisplay();
					LCD_SetPosition(1, 1);
					LCD_SendString((int8_t* ) "Mode: Generator");
					LCD_SetPosition(1, 2);
					LCD_SendString((int8_t* ) "Frequency: ");
					LCD_SendInteger(Generator_Output1Frequency);
					
					_delay_ms(10);
				}
				else
				{
					Generator_Generation(Generator_Output1Frequency);
					LCD_ClearDisplay();
					LCD_SetPosition(1, 1);
					LCD_SendString((int8_t* ) "Mode: Generator");
					LCD_SetPosition(1, 2);
					LCD_SendString((int8_t* ) "Switched off");
					
					_delay_ms(10);
				}
				break;
			}
			
			case 2:
			{
				LCD_ClearDisplay();
				LCD_SetPosition(1, 1);
				LCD_SendString((int8_t* ) "Mode: Pulse");
				LCD_SetPosition(1, 2);
				LCD_SendString((int8_t* ) "Width: ");
				LCD_SendInteger(10 * Generator_PulseWidth);
				LCD_SendString((int8_t* ) " ms");
				
				_delay_ms(10);
				break;
			}
			default:
			{
				LCD_ClearDisplay();
				break;
			}
		}
     }
}

/*
switch (Generator_Mode)
{
	case 1:
	{
		if (Generator_TurnedOn == 1)
		{
			
		}
		else if (Generator_TurnedOn == 0)
		{
			
		}
		break;
	}
	
	case 2:
	{
		
		break;
	}
	
	default:	break;
}
*/