/*
 * sdsys.h
 *
 *  Created on: Nov 20, 2024
 *      Author: Miron Andrei
 */

#ifndef INC_SDSYS_H_
#define INC_SDSYS_H_

#include "fatfs.h"
#include "graphics.h"
#include <stdlib.h>
#include <stdbool.h>


void init_cardSD();
void read_file(char *filePathName, char*fileData);
unsigned int return_int_value_first_line(char *filePathName, FIL *file);
void read_audio_file(char *filePathName, uint32_t *buffer);
void read_image_file(char *filePathName, ENTITY *entity, uint16_t *index, bool *flagImgDone);
void read_image_file_scaling(char *filePathName, ENTITY *entity, const float factor, uint16_t *px, bool *flagTerm);
void write_image_file(char *filePathName, uint8_t *data, size_t nrBytesData, int16_t x1, int16_t y1, bool flagTerm);

#endif /* INC_SDSYS_H_ */
