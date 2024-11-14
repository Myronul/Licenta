/*
 * font.h
 *
 *  Created on: Oct 21, 2024
 *      Author: Miron Andrei
 */


#ifndef INC_FONT_H_
#define INC_FONT_H_

#include <stdint.h>

typedef struct FONT
{
	const uint16_t *data;
	uint16_t width;
	uint16_t height;

}FONT;

extern FONT font; //declaram extern pentru a putea folosi
				  //fontul in alt fisier(incluzand headerul)


#endif /* INC_FONT_H_ */
