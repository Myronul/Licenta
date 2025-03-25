/*
 * kernel.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Miron Andrei
 */

#include "kernel.h"


static uint8_t currentPID = 0;
TCB *prim; /*Referinta de baza pentru LSI a proceselor*/
		   /*Procesul cu PID-ul 0 este root-ul*/

void rtos_init()
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
	q->stackPointer = &(q->stack[STACK_SIZE-1]);
	q->state = BLOCKED;

	/*Adaugam noul proces in lista dinamica*/

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




