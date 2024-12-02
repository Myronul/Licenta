/*
 * sdsys.h
 *
 *  Created on: Nov 20, 2024
 *      Author: Miron Andrei
 */

#ifndef INC_SDSYS_H_
#define INC_SDSYS_H_

#include "fatfs.h"
#include <stdlib.h>
#include <stdbool.h>

void init_cardSD();
void read_file(char *filePathName, char*fileData);
unsigned int return_int_value_first_line(char *filePathName, FIL *file);
void read_audio_file(char *filePathName, uint16_t *buffer);
uint16_t string_to_int(char *string);


#endif /* INC_SDSYS_H_ */
