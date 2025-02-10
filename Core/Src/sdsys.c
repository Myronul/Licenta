/*
 * sdsys.c
 *
 *  Created on: Nov 20, 2024
 *      Author: Miron Andrei
 */


#include<sdsys.h>

static FATFS fs; /*variabila statica pentru sistemul de fisiere card SD*/


void init_cardSD()
{
	/*
	 * Functie pentru initializarea cardului SD. Se va monta
	 * sistemul de fisiere prin variabila fs declarata static.
	 * Functia se va apela doar o singura data la initializarea
	 * intregului sistem.
	 */

	f_mount(&fs, "", 1);

}



void read_file(char *filePathName, char*fileData)
{
	/*
	 *Functie pentru a citi un anume fisier din cardul SD montat.
	 *Input: path-ul intreg al fisierului (pointer)
	 *       buffer in care se vor receptiona datele
	 *       	exemplu de path: audio/text.txt
	 *Output: void
	 */

	FRESULT res;  //stocarea starii cardului SD (util pentru depanare)
	FIL file;  //stocarea informatiilor despre fisierul deschis
	UINT byteRead;  //contor pentru numarul total de octeti cititi efectivi

	fileData = (char*)malloc(128*sizeof(char));

	res = f_open(&file, filePathName, FA_READ);

	if(res != FR_OK)
	{
		return;
	}

	f_read(&file, fileData, (sizeof(fileData)*128)-1, &byteRead);
	fileData[byteRead] = '\0';

	fileData = (char*)realloc(fileData, byteRead);

	f_close(&file);


}


unsigned int return_int_value_first_line(char *filePathName, FIL *file)
{
	/*
	 * Functie care va returna prima valoare citita din fisier
	 * in format zecimal (utila pentru citirea unui header important)
	 * Functia poate suferi ulterioare update-uri astfel incat sa se poata
	 * citi un intreg header cu valori de interes, momentan m-am limitat
	 * doar la prima valoare ce remarca numarul total de linii din fisier.
	 *
	 * Input: Calea catre fisier, obiectul fisierului deschis
	 * Output: numarul de linii ca unsigned int
	 */

	int nrLines = 0;

	UINT byteRead;

	char buffer[12];
	uint8_t index = 0;

	while(index < sizeof(buffer))
	{
		char c;

		f_read(file, &c, 1, &byteRead);

		if(c == '\n')
		{
			nrLines = (unsigned int)atoi(buffer);
			return nrLines;
		}

		buffer[index++] = c;

	}

	nrLines = (unsigned int)atoi(buffer);

	return nrLines;


}



static uint16_t string_to_int(char *string)
{
	unsigned int number = 0;

	while((*string) != '\n')
	{
		if(((*string) >= '0') && ((*string)<='9'))
		{
			number = number * 10 + ((*string) - '0');
		}

		string++;
	}

	return number;
}


static uint8_t stringHexa_to_int(char *string)
{
	unsigned int number = 0;
	uint8_t temp = 0;

	while((*string) != '\n')
	{

		if(((*string) >= '0') && ((*string)<='9'))
		{
			number = number * 16 + ((*string) - '0');
		}

		else
			if((*string) >= 'A' && ((*string)<='F') )
			{
				switch((*string))
				{
					case 'A':
						temp = 10;
						break;
					case 'B':
						temp = 11;
						break;
					case 'C':
						temp = 12;
						break;
					case 'D':
						temp = 13;
						break;
					case 'E':
						temp = 14;
						break;
					case 'F':
						temp = 15;
						break;
					default:
						temp = 0;
				}

				number = number * 16 + temp;

			}

			else
			{
				return 1;
			}

		string++;
	}

	return number;

}



void read_audio_file(char *filePathName, uint32_t *buffer)
{
	/*
	 * Functie pentru citirea fisierelor audio ce contin
	 * esantioanele .wav ale unei melodii cu rezolutia de
	 * 12 biti si frecventa de esantionare de 44.1Khz
	 * Functia este dezvoltat cu scopul de a o folosit
	 * pentru tehnica double-buffering, alaturi de DMA pe DAC.
	 * Se va apela de n ori functia, tinand de seama pointerul
	 * de referinta in fisierul cu esantioanele din carduL SD.
	 * Cand dupa cele n apeluri alternante prin double-buffering
	 * au incetat, si s-au citit toate esantioanele din fisier,
	 * se va marca printr-un flag static functiei
	 *
	 * Input: Calea catre fisier si pointerul aferent bufferului
	 * 		  de 1024 de uint16_t
	 * Output: Void
	 */


	FRESULT res;
	FIL file;
	UINT byteRead;

	res = f_open(&file, filePathName, FA_READ);

	if(res != FR_OK)
	{
		return;
	}


	static bool flagNewAudioFile = 1;

	static DWORD fileSize;
	static uint16_t currentFrame = 0;
	static FSIZE_t currentPosition = 0;

	f_lseek(&file, currentPosition); /*Revenim la pozitia anterioara citirii*/


	if(flagNewAudioFile == 1)
	{
		/*
		 * Reactualizare valorilor statice pentru deschiderea unui nou
		 * fisier audio
		*/

		flagNewAudioFile = 0;
		currentFrame = 0;

		currentPosition = 0;
		f_lseek(&file, currentPosition); /*Mutam cursorul la inceputul fisierului*/

		fileSize = f_size(&file); /*Aflam dimensiune in octeti a fisierului*/
	}

	const int n = 5120; /*1024 *5 caractere de prelucrat*/
	unsigned int nrFrames = fileSize / n;

	if(fileSize%n != 0)
	{
		nrFrames++;
	}

	uint8_t index = 0;
	char tempBuffer[n];
	char nrCharBuffer[5];

	f_read(&file, tempBuffer, (sizeof(char)*n)-1, &byteRead);
	tempBuffer[byteRead] = '\n';


	for(uint16_t i=0; i< byteRead; i++)
	{
		/*
		 * Vom parcurge tempBuffer si vom transforma caracterele citite
		 * in numere zecimale de interes
		 */

		if(tempBuffer[i] != '\n')
		{
			nrCharBuffer[index] = tempBuffer[i];
			index++;
		}

		else
		{
			/*Vom avea in nrCharBuffer esantionul in char si il vom
			 * converti la int*/

			nrCharBuffer[index] = '\n';
			*buffer = string_to_int(nrCharBuffer);
			buffer++;

			index = 0;
		}


	}


	if(currentFrame == nrFrames)
	{
		/*Resetare flag pentru reinitializare*/

		flagNewAudioFile = 1;
		f_close(&file);
		return;
	}

	currentFrame++;

	currentPosition = f_tell(&file);
	currentPosition++;

	f_close(&file);


}


void read_image_file(char *filePathName, ENTITY *entity)
{
	/*
	 * Functie pentru citirea in format binar a unui fisier.txt
	 * ce contine informatiile unei imagini.
	 * prima valoarea reprezinta latimea imaginii  -> offset x
	 * a doua valoarea reprezinta lungimea imagine -> offset y
	 * urmatoarele valori din fisier sunt datele brute (raw) ale
	 * imaginii.
	 * Se va considera ca fiecare pixel e format din 24 de biti (3 bytes)
	 * Se va folosi formatul pe 18 biti, extins pe 24
	 * ex: 0xFFFF 1100 0000 -> culoarea alb
	 * Se vor citi latimea si lungimea din fisier, urmand ca pentru fiecare
	 * cadru sa se citeasca 32x32 pixeli -> 32*32*3=3072 octeti
	 * Daca imaginea e mai mare decat 6144 octeti, se va transfera in frame-uri
	 * x si y vor fi valorile de referinta ale entitatii ce apeleaza functia
	 */


	FRESULT res;
	FIL file;
	UINT byteRead;

	res = f_open(&file, filePathName, FA_READ);

	if(res != FR_OK)
	{
		return;
	}

	static bool flagNewImageFile = 1;

	static DWORD fileSize;
	static uint16_t currentFrame = 0;
	static FSIZE_t currentPosition = 0;

	f_lseek(&file, currentPosition); /*Revenim la pozitia anterioara citirii*/


	if(flagNewImageFile == 1)
		{

		/*
		 * Reactualizare valorilor statice pentru deschiderea unui nou
		 * fisier imagine
		 * Vom afla de asemenea si dimensiune fisierului
		*/

		/*Vom citi initial headerul care contine latimea si lungimea imaginii. Primele 8 caractere*/

		char headerBuffer[8];

		f_read(&file, headerBuffer, (sizeof(headerBuffer))-1, &byteRead);
		headerBuffer[byteRead] = '\n';

		/*conversie ascii to zecimal*/

		entity->x1 = stringHexa_to_int(headerBuffer); /*latimea*/
		entity->y1 = stringHexa_to_int(headerBuffer+4); /*lungimea*/

		entity->data = malloc(3*sizeof(char)*(entity->x1)*(entity->y1));

		flagNewImageFile = 0;
		currentFrame = 0;

		currentPosition = ++byteRead;
		f_lseek(&file, currentPosition); /*Mutam cursorul dupa header*/

		/*Aflam dimensiune in octeti a imaginii de citit (scadem dimensiunea headerului)*/
		fileSize = f_size(&file) - 8;

		}


	const int n = 6144; /* 6*32*32 -> 6144 de octeti de prelucrat din fisier per frame*/
	/*care vor deveni 3072 deocteti in *data -> date reale*/

	unsigned int nrFrames = fileSize / n;


	if(fileSize%n != 0 && nrFrames!=0)
	{
		nrFrames++;
	}

	uint16_t index = 0;
	char tempBuffer[n];
	char nrCharBuffer[3];

	f_read(&file, tempBuffer, (sizeof(char)*n), &byteRead);

	for(uint16_t i=0; i<byteRead; i++)
	{
		/*Vom parcurge bufferul la intervale de 2 valori HEXA, preluand caracterele ascii
		 * pe care le vom transforma in zecimal ex: FF1200FE3000...*/

		nrCharBuffer[i%2] = tempBuffer[i];

		if(i%2 != 0)
		{
			/*Functie pentru transformare din ascii hexa in zecimal*/

			nrCharBuffer[2] = '\n';
			entity->data[index] = stringHexa_to_int(nrCharBuffer);
			index++;
		}


	}


	if(currentFrame >= nrFrames)
	{
		/*Resetare flag pentru reinitializare*/

		flagNewImageFile = 1;
		f_close(&file);
		return;
	}

	currentFrame++;

	currentPosition = f_tell(&file);
	currentPosition++;

	f_close(&file);


}




















