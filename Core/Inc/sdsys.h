/*
 * sdsys.h
 *
 *  Created on: Nov 20, 2024
 *      Author: Miron Andrei
 */

#ifndef INC_SDSYS_H_
#define INC_SDSYS_H_

#include "fatfs.h"

void initCardSD();
void readFile(char *filePathName, char*fileData);


#endif /* INC_SDSYS_H_ */
