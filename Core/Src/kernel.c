/*
 * kernel.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Miron Andrei
 */

#include "kernel.h"
#include <stdint.h>
#include <stdbool.h>
#include "graphics.h"

#define MAXPR 10
#define STACKSIZE 2048 /*stack-ul fiecarui proces va avea un maxim*/
					   /*de 2048*4o~8ko*/

extern TIM_HandleTypeDef htim4;

static uint8_t nrProc = 0;
uint8_t startOS = 0; /*flag de semnalizare start so*/
int mutex = 0;	/*semafor principal -> critic region*/

uint8_t flagAllSleep = 0;


typedef enum
{
	sleep, /*0*/
	awake  /*1*/

}STATE;


typedef struct TCB
{
  int32_t *pstack;           /*Pointer de stiva al procesului (contine adr de mem 32 de biti)*/
  struct TCB *pnext;         /*Adresa urmatorului TCB*/
  void (*pfunction)(void);
  int32_t stack[STACKSIZE];  /*Zona de stiva pentru fiecare proces(inspre care pstack va pointa)*/
  uint8_t pID;
  STATE state;               /*starea procesului-awake sau sleep*/
  unsigned int n;            /*nr de ms in sleep*/

}TCB;


TCB tcb[MAXPR];
TCB *currentProcess;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*
	 * ISR aferent timerului sistemului de operare.
	 * Perioada: Ts = 1ms
	 */

	kernel_count_sleep(); /*modifica flagAllSleep in 1 sau 0*/

    if (htim->Instance == TIM4)
    {

    	if(startOS == 1 && mutex==0 && flagAllSleep==0)
    	{
    		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; /*comutare de context cu ISR PendSV*/
    	}
    }


}


__attribute__((naked)) void PendSV_Handler(void)
{
	/*
	 * ISR pentru realizarea comutarii de context.
	 * Intreruperea PendSV va fi apelata in rutina pentru
	 * intreruperea generata de TImer4 (main).
	 * PendSV avand o prioritate minima setata, intotdeauna
	 * comutarea de context se va realiza dupa finalzarea
	 * alotr intreruperi critice (transfer SPI fara DMA,
	 * citirea datelor de pe cardul SD prin SDIO)
	 * Feature: De comutat in functie de o politica de planificare,
	 * momentan se comuta circular intre 2 procese de test!
	 */

    __asm("CPSID I");

    /*salvare context curent*/
    __asm("PUSH {R4-R7}");
    __asm("MOV R4, R8");
    __asm("MOV R5, R9");
    __asm("MOV R6, R10");
    __asm("MOV R7, R11");
    __asm("PUSH {R4-R7}");
    __asm("LDR R4, =currentProcess");
    __asm("LDR R5, [R4]"); /*R5->pstack*/
    __asm("MOV R6, SP");
    __asm("STR R6, [R5]"); /*salvam PS curent in pstack*/

    /*apel scheduler pentru adresa next proces*/
    __asm("PUSH {R0-R3, LR}");
    __asm("LDR R0, =kernel_scheduler");
    __asm("BLX R0");
    __asm("POP {R1-R3, R12, LR}");

    /*restaurare context al proc ales de scheduler*/
    __asm("LDR R4, =currentProcess");
    __asm("STR R0, [R4]");
    __asm("LDR R5, [R0]");
    __asm("MOV SP, R5");
    __asm("POP {R4-R7}");
    __asm("MOV R8, R4");
    __asm("MOV R9, R5");
    __asm("MOV R10, R6");
    __asm("MOV R11, R7");
    __asm("POP {R4-R7}");

    __asm("CPSIE I");
    __asm("BX LR");
}


void kernel_add_process(void (*adrFunction)(void))
{

	__asm("CPSID   I");

	if(nrProc==MAXPR)
	{
	    __asm("CPSIE   I ");
		return;
	}

	if(nrProc == 0)
	{
		tcb[0].pnext = &tcb[0];

		tcb[0].pstack = &tcb[0].stack[STACKSIZE-16];            /*Atribuire SP*/
		tcb[0].pfunction = adrFunction;                         /*Atribuire adr task in TCB*/
		tcb[0].stack[STACKSIZE-1] = 0x01000000;                 /*Atribuire thumb mod operare reg*/
		tcb[0].stack[STACKSIZE-2] = (int32_t)tcb[0].pfunction;  /*Atribuire reg PC catre adr incep Task/functie*/
		tcb[0].pID = nrProc++;
		tcb[0].state = awake;

	}

	else
	{
		tcb[nrProc].pnext = &tcb[nrProc-1];
		tcb[0].pnext = &tcb[nrProc];

		tcb[nrProc].pstack = &tcb[nrProc].stack[STACKSIZE-16];           /*Atribuire SP*/
		tcb[nrProc].pfunction = adrFunction;                             /*Atribuire adr task in TCB*/
		tcb[nrProc].stack[STACKSIZE-1] = 0x01000000;                     /*Atribuire thumb mod operare reg*/
		tcb[nrProc].stack[STACKSIZE-2] = (int32_t)tcb[nrProc].pfunction; /*Atribuire reg PC catre adr incep Task/functie*/
		tcb[nrProc].pID = nrProc;
		tcb[nrProc].state = awake;

	    currentProcess = &tcb[nrProc];
	    nrProc++;
	}

    __asm("CPSIE   I ");

}



__attribute__((naked)) void kernel_start(void)
{
	/*
	 * Functie de start a kernelului.
	 */

	__asm("CPSID   I");

	__asm("LDR     R0, =startOS");
	__asm("MOV     R1, 1");
	__asm("STR     R1, [R0]");
    __asm("LDR     R0, =currentProcess");
    __asm("LDR     R2, [R0]");
    __asm("LDR     R4, [R2]");
    __asm("MOV     SP, R4"); /*punem in SP procesorului pstack al &curentProc*/
    /*restauram contextul initial, emuland o intr*/
    __asm("POP     {R4-R7}");
    __asm("MOV     R8, R4");
    __asm("MOV     R9, R5");
    __asm("MOV     R10, R6");
    __asm("MOV     R11, R7");
    __asm("POP     {R4-R7}");
    __asm("POP     {R0-R3}");
    __asm("POP     {R4}");
    __asm("MOV     R12, R4");
    __asm("ADD     SP,SP,#4"); /*restauram registrul LR sarind peste R12*/
    __asm("POP     {R4}");		/*deaorece nu se poate face pop pentru R12*/

    __asm("MOV     LR, R4");
    __asm("ADD     SP,SP,#4"); /*simulam comportamentul procesorului sarind la PC*/

    __asm("CPSIE   I ");
    __asm("BX      LR");

}


void kernel_delay(unsigned int delay)
{
	/*
	 * Functie pentru a pune procesul pe sleep (delay = ms).
	 * Se va modifica starea procesului curent din variabila
	 * currentProcess.
	 * Input: void
	 * Output: void
	 */

	currentProcess->state = sleep;
	currentProcess->n = delay;

	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; /*comutare de context cu ISR PendSV*/

}


static TCB* kernel_scheduler(void)
{
	/*
	 * Scheduler de timp round robin pe timp.
	 * Pornind de la procesul curent se va cauta urmatorul
	 * proces ce poate fi rulat (adica eset awake).
	 * Functia va fi apelata in ISR PendSV
	 * Input: void
	 * Output: TCB*(adresa TCB al procesului)
	 */


	TCB *temp = currentProcess->pnext;

	while(temp!=currentProcess)
	{
		if(temp->state == awake)
		{
			return temp;
		}

		else
		{
			temp = temp->pnext;
		}
	}

	/*In cazul in care toate procesele sunt in sleep
	 * Se va returna adresa procesului curent*/

	return currentProcess;

}



void kernel_count_sleep(void)
{
	/*
	 * Functie inline pentru a cronometra sleep-ul procesului.
	 * In timerul so se va apela in isr aceasta functie.
	 * Ttimer = 1ms
	 * Input: void
	 * Output: void
	 */

	static uint8_t nrProcSleep = 0;

	nrProcSleep = 0;
	flagAllSleep = 0;

	for(uint8_t i=0; i<nrProc; i++)
	{
		if(tcb[i].state == sleep)
		{
			nrProcSleep++;

			tcb[i].n--;

			if(tcb[i].n == 0)
			{
				tcb[i].state = awake;
			}
		}
	}

	if(nrProcSleep == nrProc)
	{
		flagAllSleep = 1;
	}


}













