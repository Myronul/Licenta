/*
 * audio.c
 *
 *  Created on: Oct 20, 2024
 *      Author: Miron Andrei
 */

#include"audio.h"

volatile extern bool flagDmaDAC;
extern DAC_HandleTypeDef hdac;

static uint32_t *buffer; /*Buffer pentru redare audio folosit static in fisier*/


void play_audio_file(char *path)
{
	/*
	 * Functie pentru redarea unui fisier audio din cardul SD.
	 * De adaugat in viitor feature prin care printr-un semnal asincron
	 * se va iesi din while(1), resetand fisierul la 0 si oprind fluxul DMA
	 * Input: path-ul fisierului dorit de redat
	 * Output: Void
	 */

	buffer = malloc(sizeof(uint32_t)*2048);

	bool flagAudioDone = 0;

	if(buffer == NULL)
	{
		/*Alocare dinamica esuata*/
		return;
	}

	read_audio_file(path, buffer, &flagAudioDone); /*Citire in prima jumatate a bufferului -> 1024 de elemente*/
	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, buffer, 2048, DAC_ALIGN_12B_R);

	while(!flagAudioDone)
	{
		read_audio_file(path, buffer+1024, &flagAudioDone);
		while(flagDmaDAC == 0);
		flagDmaDAC = 0;
		read_audio_file(path, buffer, &flagAudioDone);
		while(flagDmaDAC == 0);
		flagDmaDAC = 0;

	}

	while(flagDmaDAC == 0);
	flagDmaDAC = 0;

	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);

	free(buffer);

}








