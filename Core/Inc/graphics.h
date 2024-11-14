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
	 * generate. Fiecare instanta va fi incadrata intr-un frame de tip dreptunghi
	 * avand pivotul generator (x0,y0) alaturi de lungime si latime
	 * h (height) si w (width), id unic si forma asociata
	 */

	uint16_t x0;
	uint16_t y0;

	uint16_t w;
	uint16_t h;

	uint8_t form;


}ENTITY;

void print_character(uint16_t x, uint16_t y, char c, uint16_t fontColor, uint16_t backColor );
void print_string(uint16_t x, uint16_t y, char* string, uint8_t n, uint16_t fontColor, uint16_t backColor);
void draw_horizontal_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t color);
void draw_vertical_line(uint16_t x0, uint16_t y0, uint16_t y1, uint16_t color);



#endif /* INC_GRAPHICS_H_ */
