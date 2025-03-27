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
 * Se va defini o dimensiune generala a stivei oricarui proces,
 * un enum pentru posibilele stari si o structura definitorie tip TCB
 * cu un id specific, state-ul, pointer curent catre stiva si pointer catre
 * functia asociata procesului
 * Functia de contex switching se afla in context.s
 */

#define STACK_SIZE 512 /*512*4 = 2048 kB Stack*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	RUNNING, /*0*/
	READY,	 /*1*/
	BLOCKED  /*2*/

}STATE;



typedef struct TCB
{
	uint8_t pid;
	STATE state;
	uint32_t stack[STACK_SIZE];
	uint32_t *stackPointer;
	void (*processFunction)(void);

	struct TCB *pnext;


}TCB;

void rtos_init(void);
void rtos_add_process(void (*function)(void));
void rtos_delete_process(uint8_t processID);
inline void rtos_save_context(TCB *process);
inline void rtos_restore_context(TCB *process);
void rtos_scheduler(uint8_t processID);


#endif /* INC_KERNEL_H_ */
