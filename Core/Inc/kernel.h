/*
 * kernel.h
 *
 *  Created on: Mar 17, 2025
 *      Author: Miron Andrei
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_

/*
 * Biblioteca pentru OS simplist.
 */

#include "stm32f4xx_hal.h"
#include <stdint.h>

extern uint8_t startOS; /*flag de semnalizare start so*/
extern int mutex;

__attribute__((naked))void kernel_start(void);
void kernel_add_process(void (*adrFunction)(void));


#endif /* INC_KERNEL_H_ */
