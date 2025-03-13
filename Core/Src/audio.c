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
static bool flagBuffer = 0; /*Flag pentru a indica bufferul curent de redare*/



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


static void vibrato_filter(int8_t delay)
{
	/*
	 * Functie pentru prelucrarea esantioanelor pentru a obtine un efect vibrato.
	 * Se va folosi un delay variat in timp. Vom folosi o perioada de 5 HZ a variatiei
	 * delay-ului intre valorile de amplitudine +2ms si -2ms (efect sinusoidal)
	 * Input: Delay care indica valorile de delay
	 * ce se schimba la fiecare 8192 de esantioane citite 0 +2ms -2ms
	 * FlagBuffer ne spune in ce buffer ne aflam in momentul prelucrarii 0 sau 1
	 * (prima jumatate sau a doua)
	 * Output: Void
	 */

	int index = (int)((delay*1000)/22.67);

	if(flagBuffer == 0)
	{
		/*Prima jumatate a bufferului*/

		for(uint16_t i=0; i<1024; i++)
		{

			if(i+index < 0)
			{
				/*Pentru cazul intarzierii*/

				buffer[i] = buffer[2047+(index+i)+1];
				continue;
			}


			buffer[i] = buffer[i+index];

		}

	}

	else
	{
		/*A doua jumatate a bufferului*/

		for(uint16_t i=1024; i<2048; i++)
		{

			if(i+index > 2048)
			{
				/*Pentru cazul anticiparii*/

				buffer[i] = buffer[((i+index)-2047)-1];
				continue;
			}


			buffer[i] = buffer[i+index];

		}

	}


	flagBuffer = !flagBuffer; /*schimbam jumatatea bufferului*/


}



void play_audio_file_vibrato(char *path)
{
	/*
	 * Functie pentru redarea unui fisier audio din cardul cu efect vibrato SD.
	 * Input: path-ul fisierului dorit de redat
	 * Output: Void
	 */

	buffer = malloc(sizeof(uint32_t)*2048);

	bool flagAudioDone = 0;
	flagBuffer = 1; /*Incepem din a doua jumatate*/

	int8_t delay = 0;
	uint8_t k = 0; /*determinam intervalul de timp cand schimbam delayul -> la fiecare 8*1024 esantioane*/
	int8_t delayArray[3] = {0,2,4};
	uint8_t delayIndex = 1;
	bool flagDx = 0;

	if(buffer == NULL)
	{
		/*Alocare dinamica esuata*/
		return;
	}

	read_audio_file(path, buffer, &flagAudioDone); /*Citire in prima jumatate a bufferului -> 1024 de elemente*/
	k = (k+1) % 8;

	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, buffer, 2048, DAC_ALIGN_12B_R);

	while(!flagAudioDone)
	{
		read_audio_file(path, buffer+1024, &flagAudioDone);
		vibrato_filter(delay);
		k = (k+1) % 8;

		if(k == 0)
		{
			/*
			 * Schimbam valaorea delay-ului variational
			 */

			if(flagDx == 0)
			{
				delay = delayArray[++delayIndex];
			}

			else
			{
				delay = delayArray[--delayIndex];
			}

			if(delayIndex == 2)
			{
				flagDx = 1;
			}

			if(delayIndex == 0)
			{
				flagDx = 0;
			}

		}

		while(flagDmaDAC == 0);
		flagDmaDAC = 0;

		read_audio_file(path, buffer, &flagAudioDone);
		vibrato_filter(delay);
		k = (k+1) % 8;

		if(k == 0)
		{
			/*
			 * Schimbam valaorea delay-ului variational
			 */

			if(flagDx == 0)
			{
				delay = delayArray[++delayIndex];
			}

			else
			{
				delay = delayArray[--delayIndex];
			}

			if(delayIndex == 2)
			{
				flagDx = 1;
			}

			if(delayIndex == 0)
			{
				flagDx = 0;
			}

		}

		while(flagDmaDAC == 0);
		flagDmaDAC = 0;


	}

	while(flagDmaDAC == 0);
	flagDmaDAC = 0;

	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);

	free(buffer);

}


static void echo_filter(int8_t delay)
{
	/*
	 * Functie pentru prelucrarea bufferului audio in vederea
	 * obtinerii unui efect de ecou. Se prelucreaza sub forma
	 * unui filtru FIR exprimat prin:
	 * y[n] = x[n] + a*x[n-delay]
	 */

	uint16_t index = (uint16_t) delay/0.022; /*Impartim delay-ul la timpul necesar redarii unui esantion*/




}


void play_audio_file_echo(char *path)
{
	/*
	 * Functie pentru redarea unui fisier audio din cardul cu efect ecou SD.
	 * Input: path-ul fisierului dorit de redat
	 * Output: Void
	 */

	buffer = malloc(sizeof(uint32_t)*2048);

	bool flagAudioDone = 0;
	flagBuffer = 1; /*Incepem din a doua jumatate*/

	int8_t delay = 0;

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
		echo_filter(delay);

		while(flagDmaDAC == 0);
		flagDmaDAC = 0;

		read_audio_file(path, buffer, &flagAudioDone);
		echo_filter(delay);

		while(flagDmaDAC == 0);
		flagDmaDAC = 0;
	}

	while(flagDmaDAC == 0);
	flagDmaDAC = 0;

	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);

	free(buffer);

}






