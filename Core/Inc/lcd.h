#ifndef __LCD_H__
#define __LCD_H__

#include "stm32f4xx_hal.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;
volatile extern uint8_t flagDmaSpiTx;
volatile extern uint8_t flagDmaSpiRx;

//Definire Pini ecran si marimi maxime mod portret

#define LCD_Width   320  //latime
#define LCD_Length  480  //lungime

#define CS_Pin GPIO_PIN_4     //CS
#define CS_GPIO_Port GPIOD
#define RST_Pin GPIO_PIN_6    //RST
#define RST_GPIO_Port GPIOD
#define DC_Pin GPIO_PIN_2     //DC
#define DC_GPIO_Port GPIOD

//Definire MACROS-uri Stari High/Low pentru semnalele RST, CS si DC

#define RST_A() HAL_GPIO_WritePin(RST_GPIO_Port,RST_Pin,GPIO_PIN_RESET)
#define RST_D() HAL_GPIO_WritePin(RST_GPIO_Port,RST_Pin,GPIO_PIN_SET)
#define CS_A() HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET)
#define CS_D() HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET)
#define DC_COMMAND() HAL_GPIO_WritePin(DC_GPIO_Port,DC_Pin,GPIO_PIN_RESET)
#define DC_DATA() HAL_GPIO_WritePin(DC_GPIO_Port,DC_Pin,GPIO_PIN_SET)

//Comenzi predefinite din DataSheet ILI9488

#define ILI9488_NOP            0x00
#define ILI9488_SWRESET        0x01
#define ILI9488_RDDID          0x04
#define ILI9488_RDDST          0x09

#define ILI9488_SLPIN          0x10
#define ILI9488_SLPOUT         0x11
#define ILI9488_PTLON          0x12
#define ILI9488_NORON          0x13

#define ILI9488_RDMODE         0x0A
#define ILI9488_RDMADCTL       0x0B
#define ILI9488_RDPIXFMT       0x0C
#define ILI9488_RDIMGFMT       0x0D
#define ILI9488_RDSELFDIAG     0x0F

#define ILI9488_INVOFF         0x20
#define ILI9488_INVON          0x21
#define ILI9488_GAMMASET       0x26
#define ILI9488_DISPOFF        0x28
#define ILI9488_DISPON         0x29

#define ILI9488_CASET          0x2A
#define ILI9488_PASET          0x2B
#define ILI9488_RAMWR          0x2C
#define ILI9488_RAMRD          0x2E

#define ILI9488_PTLAR          0x30
#define ILI9488_MADCTL         0x36
#define ILI9488_PIXFMT         0x3A

#define ILI9488_FRMCTR1        0xB1
#define ILI9488_FRMCTR2        0xB2
#define ILI9488_FRMCTR3        0xB3
#define ILI9488_INVCTR         0xB4
#define ILI9488_DFUNCTR        0xB6

#define ILI9488_PWCTR1         0xC0
#define ILI9488_PWCTR2         0xC1
#define ILI9488_PWCTR3         0xC2
#define ILI9488_PWCTR4         0xC3
#define ILI9488_PWCTR5         0xC4
#define ILI9488_VMCTR1         0xC5
#define ILI9488_VMCTR2         0xC7

#define ILI9488_RDID1          0xDA
#define ILI9488_RDID2          0xDB
#define ILI9488_RDID3          0xDC
#define ILI9488_RDID4          0xDD

#define ILI9488_GMCTRP1        0xE0
#define ILI9488_GMCTRN1        0xE1


//Functii de baza

void ILI9488_driver_init();
void LCD_send_command(uint8_t cmd);
void LCD_send_data_multi(uint8_t *data, unsigned int size);
void set_adress_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char x);
void write_color(uint16_t color);
void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void convert_color_16_to_18(uint16_t color, uint8_t *pixel);
void LCD_send_data(uint8_t data);
void read_pixel_frame(uint16_t x0, uint16_t y0, uint16_t x, uint16_t y, uint8_t*data);
void read_pixel_format();
void fill_screen(uint16_t color);
void fill_screen1(uint16_t color);
void fill_screen2(uint16_t color);

#endif /* __LCD_H__ */
