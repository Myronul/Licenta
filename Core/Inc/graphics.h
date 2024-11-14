/*
 * graphics.h
 *
 *  Created on: Oct 20, 2024
 *      Author: Miron Andrei
 */

#include "lcd.h"
#include "font.h"

#ifndef INC_GRAPHICS_H_
#define INC_GRAPHICS_H_

typedef struct ENTITY
{
	/*
	 * Structura primitiva a oricarei forme geometrice posibil
	 * generate. Fiecare instanta va fi incadrata intr-un frame de tip patrat
	 * avand pivotul generator (x0,y0) alaturi de marimea ferestrei patratice
	 */

	uint16_t x0;
	uint16_t y0;

	uint8_t size;


}ENTITY;

void write_color(uint16_t color);
void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void convert_color_16_to_18(uint16_t color, uint8_t *pixel);
void fill_screen1(uint16_t color);
void fill_screen2(uint16_t color);

void print_character(uint16_t x, uint16_t y, char c, uint16_t fontColor, uint16_t backColor );
void print_string(uint16_t x, uint16_t y, char* string, uint8_t n, uint16_t fontColor, uint16_t backColor);
void draw_horizontal_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t color);
void draw_vertical_line(uint16_t x0, uint16_t y0, uint16_t y1, uint16_t color);



#endif /* INC_GRAPHICS_H_ */
