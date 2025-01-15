#ifndef __LCD_H__
#define __LCD_H__

#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Definire Pini ecran si marimi maxime mod portret

#define LCD_Width   320  //latime
#define LCD_Length  480  //lungime

#define CS_Pin GPIO_PIN_4     //CS
#define CS_GPIO_Port GPIOD
#define RST_Pin GPIO_PIN_6    //RST
#define RST_GPIO_Port GPIOD
#define DC_Pin GPIO_PIN_3     //DC
#define DC_GPIO_Port GPIOD

//Definire MACROS-uri Stari High/Low pentru semnalele RST, CS si DC

#define RST_A() HAL_GPIO_WritePin(RST_GPIO_Port,RST_Pin,GPIO_PIN_RESET)
#define RST_D() HAL_GPIO_WritePin(RST_GPIO_Port,RST_Pin,GPIO_PIN_SET)
#define CS_A() HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET)
#define CS_D() HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET)
#define DC_COMMAND() HAL_GPIO_WritePin(DC_GPIO_Port,DC_Pin,GPIO_PIN_RESET)
#define DC_DATA() HAL_GPIO_WritePin(DC_GPIO_Port,DC_Pin,GPIO_PIN_SET)

//Comenzi de baza predefinite din DataSheet ILI9488


#define ILI9488_CASET          0x2A
#define ILI9488_PASET          0x2B
#define ILI9488_RAMWR          0x2C
#define ILI9488_RAMRD          0x2E


//Functii de baza

void ILI9488_driver_init();
void LCD_send_command(uint8_t cmd);
void LCD_send_data_multi(uint8_t *data, unsigned int size);
void LCD_send_data(uint8_t data);
void set_adress_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char x);
void read_pixel_frame(uint16_t x0, uint16_t y0, uint16_t x, uint16_t y, uint8_t*data);
void read_pixel_format();


#endif /* __LCD_H__ */
