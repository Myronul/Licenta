/*
 * graphics.h
 *
 *  Created on: Oct 20, 2024
 *      Author: Miron Andrei
 */


#ifndef INC_GRAPHICS_H_
#define INC_GRAPHICS_H_

#include "lcd.h"
#include "font.h"


typedef struct INFO
{
	uint8_t *data;
	char *filePathName;

}INFO;


typedef struct ENTITY
{
	/*
	 * Structura primitiva a oricarei forme geometrice posibil
	 * generate. Fiecare instanta va fi incadrata intr-un frame de tip patrat
	 * avand pivotul generator (x0,y0) alaturi de marimea ferestrei patratice (x1,y1).
	 * Se va folosi un Union pentru a desemna entitatea ca forma standard gemotrica (id)
	 * ori ca existenta in format raw din cardul SD
	 */

	uint8_t id; /*Daca MSB este 0 imaginea este stocata pe cardul SD */

	int16_t x0;
	int16_t y0;

	int16_t x1;
	int16_t y1;

	union
	{
		/*
		 * Union folosit pentru a defini entitati prime (forme geometrice
		 * standard precum cerc, patrat, dreptunghi) sau entitati ale caror imagini
		 * se afla in cardul SD
		 */


		uint16_t color; /*culoare pentru formele prime, pe 16 biti*/
		uint8_t *data; /*buffer de date pentru imagini card SD*/
		char *filePathName;

	};



}ENTITY;


void write_color(uint16_t color);
void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void convert_color_16_to_18(uint16_t color, uint8_t *const pixel);
void fill_screen1(uint16_t color);
void fill_screen2(uint16_t color);

void print_character(uint16_t x, uint16_t y, char c, uint16_t fontColor, uint16_t backColor );
void print_string(uint16_t x, uint16_t y, char* string, uint8_t n, uint16_t fontColor, uint16_t backColor);
void draw_horizontal_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t color);
void draw_vertical_line(uint16_t x0, uint16_t y0, uint16_t y1, uint16_t color);
void draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void draw_entity(ENTITY *entity, char *filePathName);
void translation_entity(ENTITY *const restrict entity, int16_t x, int16_t y, bool TurnOnStep);//, uint16_t color);
void translation_test(ENTITY *entity, uint8_t step, uint16_t delay);
void scaling_entity(ENTITY *entity, const float factor, char *filePathName);

#endif /* INC_GRAPHICS_H_ */


