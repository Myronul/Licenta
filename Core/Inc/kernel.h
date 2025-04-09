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

__attribute__((naked))void kernel_start(void);
void kernel_init(void);
volatile void Task0(void);
volatile void Task1(void);

#endif /* INC_KERNEL_H_ */
