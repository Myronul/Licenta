/*
 * kernel.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Miron Andrei
 */

#include "kernel.h"
#include "stm32f4xx_hal.h"

static uint8_t currentPID = 0;
TCB *prim; /*Referinta de baza pentru LSI a proceselor*/
		   /*Procesul cu PID-ul 0 este root-ul*/

TCB *activeProcess;

void rtos_init(void)
{
	/*
	 * Functie de initializare a kernelului.
	 * Alocarea memoriei pentru primul proces
	 * Vom avea o LSI cu procesele curente
	 */

	prim = (TCB*)malloc(sizeof(TCB));

	if(prim == NULL)
	{
		return;
	}

	prim->pid = currentPID++;
	prim->pnext = NULL;

	/*Vom intra in modul user thread al procesorului privilegiat*/

	__set_CONTROL(__get_CONTROL() | 0x2);
	__ISB();




}



void rtos_add_process(void (*function)(void))
{
	/*
	 * Functie pentru adaugarea unui nou proces
	 * in LSI asociat kernelului rtos
	 * Input: adresa functiei asociate procesului
	 * Output: void
	 */

	/*Asociem parametrii noului proces*/

	TCB *q = (TCB*)malloc(sizeof(TCB));

	if(q == NULL)
	{
		return;
	}

	q->pid = currentPID++;
	q->processFunction = function;
	q->state = BLOCKED;

	uint32_t *stackTop = &(q->stack[STACK_SIZE-1]);

	*(--stackTop) = 0x01000000;			/*xPSR*/
	*(--stackTop) = (uint32_t)function; /*PC*/
	*(--stackTop) = 0xFFFFFFFD;			/*LR*/
	*(--stackTop) = 0;					/*R12*/
	*(--stackTop) = 0;					/*R3*/
	*(--stackTop) = 0;					/*R2*/
	*(--stackTop) = 0;					/*R1*/
	*(--stackTop) = 0;					/*R0*/

	*(--stackTop) = 0;					/*R11*/
	*(--stackTop) = 0;					/*R10*/
	*(--stackTop) = 0;					/*R9*/
	*(--stackTop) = 0;					/*R8*/
	*(--stackTop) = 0;					/*R7*/
	*(--stackTop) = 0;					/*R6*/
	*(--stackTop) = 0;					/*R5*/
	*(--stackTop) = 0;					/*R4*/


	q->stackPointer = stackTop;

	/*Adaugam noul proces in TCB*/

	q->pnext = prim;
	prim = q;

}


void rtos_delete_process(uint8_t processID)
{
	/*
	 * Functie pentru stergerea unui proces.
	 * Vom sterge direct la procesul din ordinea aferenta PID
	 * Input: PID-ul procesului de sters
	 * Output: Void
	 */

	if(processID == 0)
	{
		return;
	}

	TCB *temp = prim;

	while((temp!=NULL) && ((processID-1)!=0))
	{
		temp = temp->pnext;
		processID--;
	}


	/*
	 * Stergem dupa temp
	 */

	TCB *q = temp->pnext;

	temp->pnext = q->pnext;
	free(q);


}


inline void rtos_save_context(TCB *process)
{
	/*
	 * Functie inline pentru salvarea contextului a procesului
	 * curent la aparitia unei intreruperi.
	 * Functia va fi apelata doar in cadrul unui ISR.
	 * Input: Referinta catre procesul curent de salvat
	 * Output: Void
	 */

    __asm volatile(
        "MRS R0, PSP \n"
        "STMDB R0!, {R4-R11} \n"
        "STR R0, %0 \n"
        : "=m" (process->stackPointer)
        :
        : "memory"
    );



    process->state = BLOCKED;


}

inline void rtos_restore_context(TCB *process)
{
	/*
	 * Functie inline pentru restaurarea contextului procesului
	 * curent la aparitia unei intreruperi.
	 * Functia va fi apelata doar in cadrul unui ISR.
	 * Input: Referinta catre procesul curent de restaurat
	 * Output: Void
	 */

    __asm volatile(
        "LDR R0, %0 \n"
        "MSR PSP, R0 \n"
        "LDMIA R0!, {R4-R11} \n"
    	"MOV LR, 0xFFFFFFFD \n"
        "BX LR \n"
        :
        : "m" (process->stackPointer)
        : "memory"
    );


	 /*Se va face HW unstacking automat pentru:
	  * pc,lr,r12,r3-r0,xpsr */

	 process->state = RUNNING;


}


void rtos_scheduler(uint8_t processID)
{
	if(processID == 0)
	{
		return;
	}

	TCB *temp = prim;

	while((temp!=NULL))
	{
		if(processID == temp->pid)
		{
			activeProcess = temp;
			break;
		}

		else
		{
			temp = temp->pnext;
		}

	}


}
















