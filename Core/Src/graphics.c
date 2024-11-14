/*
 * graphics.c
 *
 *  Created on: Oct 20, 2024
 *      Author: Miron Andrei
 */

#include "graphics.h"

ENTITY entity;

void print_character(uint16_t x, uint16_t y, char c, uint16_t fontColor, uint16_t backColor)
{
	set_adress_window(x, y, x+font.width-1, y+font.height-1, 'w');

	uint16_t mask = 0x8000;

	uint8_t i,j;
	uint16_t line = 0;

	uint8_t dataFont[3];
	uint8_t dataBack[3];

	convert_color_16_to_18(fontColor, dataFont);
	convert_color_16_to_18(backColor, dataBack);


	DC_DATA();
	CS_A();

	for(i=0; i<font.height; i++)
	{
		line = font.data[((c-32)*font.height)+i];

		for(j=0; j<font.width; j++)
		{
			if(((line<<j) & (mask)) != 0)
			{
				LCD_send_data_multi(dataFont,3);
			}

			else
			{
				LCD_send_data_multi(dataBack,3);
			}
		}
	}

	CS_D();

}


void print_string(uint16_t x, uint16_t y, char* string, uint8_t n, uint16_t fontColor, uint16_t backColor)
{

	char *temp = string;

	while(string<(temp+n))
	{
		if(x + font.width > 320)
		{
			x = font.width;
			y = y + font.height;
		}

		if((y + font.height) > 480)
		{
			return;
		}

		print_character(x, y, *string, fontColor, backColor);
		string++;
		x = x + font.width;
	}

}


void draw_horizontal_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t color)
{

	/*
	 * Functie pentru desenarea grafica unei linii orizontale.
	 * Input: x0 si y0 coordonatele initiale, x1 lungimea relativa liniei, si culoarea
	 * Output: Void
	 */

	x1 = x0+x1; //aflam lungimea efectiva
	set_adress_window(x0, y0, x1-1, y0, 'w');

	uint8_t pixel[3];
	convert_color_16_to_18(color, pixel);

	for(uint16_t i=0; i<x1-x0; i++)
	{
		LCD_send_data_multi(pixel,sizeof(pixel));
	}


}


void draw_vertical_line(uint16_t x0, uint16_t y0, uint16_t y1, uint16_t color)
{

	/*
	 * Functie pentru desenarea grafica unei linii verticale.
	 * Input: x0 si y0 coordonatele initiale, y1 lungimea relativa liniei, si culoarea
	 * Output: Void
	 */

	y1 = y0+y1; //aflam lungimea efectiva
	set_adress_window(x0, y0, x0, y1-1, 'w');

	uint8_t pixel[3];
	convert_color_16_to_18(color, pixel);

	for(uint16_t i=0; i<y1-y0; i++)
	{
		LCD_send_data_multi(pixel,sizeof(pixel));
	}


}



void define_entity(uint8_t form)
{
	/*
	 * Fnctie pentru crearea unei forme geometrice pe ecran
	 * definita ca o entitate de o anumita forma specifica.
	 * Fiecare entitate va fi initial incadrata intr-un frame de 8x8 pixeli
	 */

	entity.x0 = 152;
	entity.y0 = 232;

	entity.w = entity.x0 + 8;
	entity.h = entity.y0 + 8;

	entity.form = form;

}


void create_square()
{

}








