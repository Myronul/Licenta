/*
 * audio.h
 *
 *  Created on: Oct 20, 2024
 *      Author: Miron Andrei
 */

#ifndef INC_AUDIO_H_
#define INC_AUDIO_H_

//#include "main.h"
#include "sdsys.h"

void play_audio_file(char *path);
void play_audio_file_vibrato(char *path);
void play_audio_file_echo(char *path, int8_t delay, float alpha);
void play_audio_file_reverb(char *path);

#endif /* INC_AUDIO_H_ */
