/*
 * sdsys.c
 *
 *  Created on: Nov 20, 2024
 *      Author: Miron Andrei
 */


#include<sdsys.h>

static FATFS fs; /*variabila statica pentru sistemul de fisiere card SD*/



void initCardSD()
{
	/*
	 * Functie pentru initializarea cardului SD. Se va monta
	 * sistemul de fisiere prin variabila fs declarata static
	 */

	f_mount(&fs, "", 1);

}



void readFile(char *filePathName, char*fileData)
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

	fileData = (char*)malloc(512*sizeof(char));

	f_open(&file, filePathName, FA_READ);

	f_read(&file, fileData, sizeof(fileData)-1, &byteRead);
	fileData[byteRead] = '\0';

	fileData = (char*)realloc(fileData, byteRead);

	f_close(&file);


}




