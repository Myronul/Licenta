/*
 * graphics.c
 *
 *  Created on: Oct 20, 2024
 *      Author: Miron Andrei
 */

#include "graphics.h"


void convert_color_16_to_18(uint16_t color, uint8_t *pixel)
{
	/*
	 * Functie de conversie a culorii de la 16 biti la 24 de biti
	 * Vom folosi un algoritm simplu matematic pentru a obtine
	 * valoarea asociata, in raport binar
	 * Input: Culaorea pe 16 biti, pointerul in care stocam culorile noi
	 * Output: Void
	 */

	uint8_t r,g,b;

	r = (color & 0xF800) >> 11;
	g = (color & 0x07E0) >> 5;
	b = color & 0x001F;

	//16bit->18bit extindere respectand forma de transmisie

	r = (63*r)/31 << 2;
	g = (63*g)/63 << 2;
	b = (63*b)/31 << 2;

	pixel[0] = r;
	pixel[1] = g;
	pixel[2] = b;

}


void write_color(uint16_t color)
{
	/*
	 * Functie pentru transmiterea culorii aferente,
	 * Dupa selectarea ferestrei de adresare!
	 * Functia va transforma culorile de pe 16 biti pe 24 biti
	 * Functia va fi folosita in cadrul functiilor de scriere in memorie
	 * dupa ce vor fi trimise semnalele CS si DC_DATA
	 * Parametrii: culoarea pe 16 biti (uint16_t)
	 * Output: Void
	 */


	uint8_t data[3];
	convert_color_16_to_18(color, data);

	//flagDmaSpiTx = 0;

	//HAL_SPI_Transmit_DMA(&hspi1, data, 3);

	//while(flagDmaSpiTx == 0);
	HAL_SPI_Transmit(&hspi1, data, 3, HAL_MAX_DELAY);


}


void draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
	/*
	 * Functie pentru afisarea unui pixel pe ecran.
	 * Parametrii: Coordonatele (x,y) si culoarea
	 * Output: Void
	 */

	set_adress_window(x,y,x,y, 'w');

	DC_DATA();
	CS_A();

	write_color(color);

	CS_D();

}




void fill_screen1(uint16_t color)
{
	/*
	 * Functie pentru a colora ecranul intr-un mod mai eficient.
	 * Vom transmite cate o linie pe rand, un frame de date fiind
	 * practic o linie a ecranului de 320*3 octeti
	 * Input: Culoare pe 16 biti
	 * Output: Void
	 */
	uint8_t r,g,b;

	r = (color & 0xF800) >> 11;
	g = (color & 0x07E0) >> 5;
	b = color & 0x001F;

	r = (255*r)/31;
	g = (255*g)/63;
	b = (255*b)/31;

	uint8_t pixel[] = {r,g,b};
	uint8_t *line = malloc(320*sizeof(pixel));

	set_adress_window(0, 0, LCD_Width-1, LCD_Length-1, 'w');

	DC_DATA();
	CS_A();

	for(uint16_t x=0; x<320; x++)
	{
		memcpy(line + x*sizeof(pixel), pixel, sizeof(pixel));
	}

	for(uint16_t y=0; y<480; y++)
	{

		HAL_SPI_Transmit(&hspi1, line, 320*sizeof(pixel), HAL_MAX_DELAY);

	}

	free(line);

	CS_D();

}


void fill_screen2(uint16_t color)
{
	set_adress_window(0, 0, LCD_Width-1, LCD_Length-1, 'w');

	uint32_t frameSize = 1200; //numarul de octeti dintr-un frame
	uint32_t nrPixel = 320*480;

	uint8_t r,g,b;

	r = (color & 0xF800) >> 11;
	g = (color & 0x07E0) >> 5;
	b = color & 0x001F;

	r = (255*r)/31;
	g = (255*g)/63;
	b = (255*b)/31;

	DC_DATA();
	CS_A();

	uint8_t frame[frameSize]; //nr de octeti de trimis intr-o tranmsisiune

	for(uint32_t j=0; j<frameSize; j=j+3)
	{
		frame[j] = r;
		frame[j+1] = g;
		frame[j+2] = b;
	}

	uint32_t data = nrPixel * 3; //nr de octeti de trimis
	uint32_t nrFrames = data / frameSize;

	flagDmaSpiTx = 1;

	for(uint32_t j = 0; j < (nrFrames); j++)
	{
		//HAL_SPI_Transmit(&hspi1, frame, frameSize, 10);

		while(flagDmaSpiTx == 0); //asteapta cat timp este 0
		flagDmaSpiTx = 0;
		HAL_SPI_Transmit_DMA(&hspi1, frame, frameSize);

	}

	//asteptam sa se finalizeze ultimul transfer -> asteptam flag1

	while(flagDmaSpiTx == 0);
	CS_D();
}



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





