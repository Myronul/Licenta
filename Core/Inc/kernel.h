/*
 * kernel.h
 *
 *  Created on: Mar 17, 2025
 *      Author: Miron Andrei
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_

/*
 * Biblioteca pentru kernelul minimalist.
 * Se va defini o dimensiune generală a stivei oricărui proces,
 * un enum pentru posibilele stări și o structură definitorie tip TCB
 * cu un id specific, state-ul, pointer curent către stivă și pointer către
 * funcția asociată procesului
 */

#include "stm32f4xx_hal.h"
#include <stdint.h>

__attribute__((naked))void LaunchScheduler(void);
void OsInitThreadStack();
volatile void Task0();
volatile void Task1();

#endif /* INC_KERNEL_H_ */
