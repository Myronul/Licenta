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

#define MAXPR 2
#define STACKSIZE 2048 /*stack-ul fiecarui proces va avea un maxim*/
					   /*de 2048*4o~8ko*/

extern uint8_t startOS; /*variabila de start definita in main*/

int mutex = 0;

typedef struct TCB
{
  int32_t *pstack;           /*Pointer de stiva al procesului (contine adr de mem 32 de biti)*/
  struct TCB *pnext;         /*Adresa urmatorului TCB*/
  void (*pfunction)(void);
  int32_t stack[STACKSIZE];  /*Zona de stiva pentru fiecare proces(inspre care pstack va pointa)*/
  uint8_t pID;

}TCB;


TCB tcb[MAXPR];
TCB *currentProcess;



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

    __asm("CPSID   I");

    __asm("PUSH    {R4-R7}");
    __asm("MOV     R4, R8");
    __asm("MOV     R5, R9");
    __asm("MOV     R6, R10");
    __asm("MOV     R7, R11");
    __asm("PUSH    {R4-R7}");

    __asm("LDR     R0, =currentProcess");
    __asm("LDR     R1, [R0]");
    __asm("MOV     R4, SP");
    __asm("STR     R4, [R1]");
    __asm("LDR     R1, [R1,#4]"); /*diferentiere pentru pnext*/
    __asm("STR     R1, [R0]");
    __asm("LDR     R4, [R1]");
    __asm("MOV     SP, R4");      /*In R4 avem adresa stivei pnext proces OBS: De realizat un scheduler care */
    __asm("POP     {R4-R7}");		/*returneaza adresa tcb[i] a urmatorului proces de pus in executie*/
    __asm("MOV     R8, R4");
    __asm("MOV     R9, R5");
    __asm("MOV     R10, R6");
    __asm("MOV     R11, R7");
    __asm("POP     {R4-R7}");

    __asm("CPSIE   I ");
    __asm("BX      LR");
}


void kernel_add_process(void (*adrFunction)(void))
{
	__asm("CPSID   I");

	static uint8_t nrProc = 0;

	if(nrProc == 0)
	{
		tcb[0].pnext = &tcb[0];

		tcb[0].pstack = &tcb[0].stack[STACKSIZE-16];            /*Atribuire SP*/
		tcb[0].pfunction = adrFunction;                         /*Atribuire adr task in TCB*/
		tcb[0].stack[STACKSIZE-1] = 0x01000000;                 /*Atribuire thumb mod operare reg*/
		tcb[0].stack[STACKSIZE-2] = (int32_t)tcb[0].pfunction;  /*Atribuire reg PC catre adr incep Task/functie*/
		tcb[0].pID = nrProc++;

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
    __asm("ADD     SP,SP,#4"); /*simulam comportamentul stivei sarind la PC*/

    __asm("CPSIE   I ");
    __asm("BX      LR");

}




