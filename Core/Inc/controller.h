/*
 * controller.h
 *
 *  Created on: Apr 24, 2025
 *      Author: Miron Andrei
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#include<stdint.h>

#define DxRight   0x01   /*0b0000 0001*/
#define DxLeft    0x02   /*0b0000 0010*/
#define DxUp      0x04   /*0b0000 0100*/
#define DxDown    0x08   /*0b0000 1000*/
#define DxStart   0x10   /*0b0001 0000*/
#define DxSelect  0x20   /*0b0010 0000*/

extern uint8_t dataController;
extern uint8_t currentDx;

#endif /* INC_CONTROLLER_H_ */
