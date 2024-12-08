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
	 * sistemul de fisiere prin variabila fs declarata static
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


/*

void read_audio_file(char *filePathName, uint16_t *buffer)
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
	 * 		  de 2048 de uint16_t
	 * Output: Void



	FRESULT res;
	FIL file;
	//UINT byteRead;

	res = f_open(&file, filePathName, FA_READ);

	if(res != FR_OK)
	{
		return;
	}


	/*
	 * citim prima valoarea din fisier care indica
	 * numarul total de linii din acesta


	static bool flagNewAudioFile = 1;

	static uint16_t currentFrame = 0;
	static FSIZE_t currentPosition = 0;
	static unsigned int nrLines = 0;


	f_lseek(&file, currentPosition); /*Revenim la pozitia anterioara citirii


	if(flagNewAudioFile == 1)
	{
		/*
		 * Reactualizare valorilor statice pentru deschiderea unui nou
		 * fisier audio


		flagNewAudioFile = 0;
		currentFrame = 0;

		currentPosition = 0;
		f_lseek(&file, currentPosition); /*Mutam cursorul la inceputul fisierului

		nrLines = return_int_value_first_line(filePathName, &file);
	}


	uint16_t n = 2048;
	int nrFrames = nrLines / n;
	uint8_t reminder = nrLines % n;

	char tempBuffer[10];

	if(currentFrame == nrFrames)
	{
		if(reminder != 0)
		{
			while(reminder)
			{
				f_gets(tempBuffer,sizeof(tempBuffer),&file);

				(*buffer) = (uint16_t)atoi(tempBuffer);
				buffer++;

				reminder--;

			}
		}


		flagNewAudioFile = 1;

	}

	while(n!=0)
	{
		/*
		 * Ne vom popula bufferul cu valorile esantioanelor
		 * audio ale melodiei alese.


		f_gets(tempBuffer,sizeof(tempBuffer),&file);

		(*buffer) = (uint16_t)atoi(tempBuffer);
		buffer++;

		n--;

	}

	currentFrame++;

	currentPosition = f_tell(&file);
	f_lseek (&file, currentPosition);


	f_close(&file);


}


*/


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
		flagNewAudioFile = 0;
	}

	currentFrame++;

	currentPosition = f_tell(&file);
	currentPosition++;

	f_close(&file);


}





















