/*
 * context.s
 *
 *  Created on: Mar 17, 2025
 *      Author: Miron Andrei
 */

.syntax unified
.cpu cortex-m4
.thumb

.global SaveContext
.global RestoreContext

SaveContext:
	MRS R0, MSP
	MOV R1, R0
	MRS R0, PSP
	MOV SP, R0

	PUSH {R4-R11}

	MOV R0, SP
	MSR PSP, R0
	MSR MSP, R1

	BX LR

RestoreContext:
	POP {R4-R11}
	POP {R0-R3}
	POP {R12}
	POP {LR}
	POP {PC}
	//POP {xPSR}

	BX LR











	BX LR
