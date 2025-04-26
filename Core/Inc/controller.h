/*
 * controller.h
 *
 *  Created on: Apr 24, 2025
 *      Author: Miron Andrei
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#include<stdint.h>

#define DxRight   0x11   /*0b0001 0001*/
#define DxLeft    0x33   /*0b0011 0011*/
#define DxUp      0x55   /*0b0101 0101*/
#define DxDown    0x77   /*0b0111 0111*/
#define DxStart   0x99   /*0b1001 1001*/
#define DxSelect  0xBB   /*0b1011 1011*/

extern uint8_t dataController;
extern uint8_t currentDx;

#endif /* INC_CONTROLLER_H_ */
