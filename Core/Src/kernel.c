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
#define STACKSIZE 250 /*stack-ul fiecarui proces va avea un maxim*/
					  /*de 250*4o=1ko*/

extern uint8_t startOS; /*variabila de start definita in main*/

int mutex = 0;

typedef struct TCB
{
  int32_t *pstack;           /*Pointer de stiva al procesului (contine adr de mem 32 de biti)*/
  struct TCB *pnext;         /*Adresa urmatorului TCB*/
  int32_t stack[STACKSIZE];  /*Zona de stiva pentru fiecare proces(inspre care pstack va pointa)*/

}TCB;


TCB tcb[MAXPR];
TCB *currentProcess;

int32_t TCB_STACK[MAXPR][STACKSIZE];


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
    __asm("MOV     SP, R4");
    __asm("POP     {R4-R7}");
    __asm("MOV     R8, R4");
    __asm("MOV     R9, R5");
    __asm("MOV     R10, R6");
    __asm("MOV     R11, R7");
    __asm("POP     {R4-R7}");

    __asm("CPSIE   I ");
    __asm("BX      LR");
}

void kernel_init()
{
	/*
	 * Functie pentru initializarea kernelului.
	 * Se vor initializa in mod static procesele din sistem
	 */

	__asm("CPSID   I");

	tcb[0].pnext = &tcb[1];
	tcb[1].pnext = &tcb[0];

	tcb[0].pstack = &tcb[0].stack[STACKSIZE-16];
	tcb[0].stack[STACKSIZE-1] = 0x01000000;
	tcb[0].stack[STACKSIZE-2] = (int32_t)(Task0);

	tcb[1].pstack = &tcb[1].stack[STACKSIZE-16];
	tcb[1].stack[STACKSIZE-1] = 0x01000000;
	tcb[1].stack[STACKSIZE-2] = (int32_t)(Task1);

    currentProcess = &tcb[0];

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



volatile void Task0()
{
    while(1)
    {


       //HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
       mutex = 1;
       HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
       fill_screen1(0x0000);
       mutex = 0;
       HAL_Delay(20);

    }
}

volatile void Task1()
{
    while(1)
    {

    	//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
    	//flagg = 1;
    	mutex = 1;
    	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
    	fill_screen1(0xF100);
    	fill_screen1(0xF1AA);
    	fill_screen1(0xFFFF);
    	mutex = 0;
    	HAL_Delay(20);
    }
}
