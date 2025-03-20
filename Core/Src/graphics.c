/*
 * graphics.c
 *
 *  Created on: Oct 20, 2024
 *      Author: Miron Andrei
 */

#include "stm32f4xx_hal.h"

#include "graphics.h"
#include "sdsys.h"


extern SPI_HandleTypeDef hspi1;
volatile extern uint8_t flagDmaSpiTx;

void convert_color_16_to_18(uint16_t color, uint8_t *const pixel)
{
	/*
	 * Functie de conversie a culorii de la 16 biti la 24 de biti
	 * Vom folosi un algoritm simplu matematic pentru a obtine
	 * valoarea asociata, in raport binar
	 * Input: Culaorea pe 16 biti, pointerul in care stocam culorile noi
	 * Output: Void
	 */

	uint8_t r,g,b;

	r = (color & 0xF800) >> 11;
	g = (color & 0x07E0) >> 5;
	b = color & 0x001F;

	//16bit->18bit extindere respectand forma de transmisie

	r = (63*r)/31 << 2;
	g = (63*g)/63 << 2;
	b = (63*b)/31 << 2;

	pixel[0] = r;
	pixel[1] = g;
	pixel[2] = b;

}


void write_color(uint16_t color)
{
	/*
	 * Functie pentru transmiterea culorii aferente,
	 * Dupa selectarea ferestrei de adresare!
	 * Functia va transforma culorile de pe 16 biti pe 24 biti
	 * Functia va fi folosita in cadrul functiilor de scriere in memorie
	 * dupa ce vor fi trimise semnalele CS si DC_DATA
	 * Parametrii: culoarea pe 16 biti (uint16_t)
	 * Output: Void
	 */


	uint8_t data[3];
	convert_color_16_to_18(color, data);

	//flagDmaSpiTx = 0;

	//HAL_SPI_Transmit_DMA(&hspi1, data, 3);

	//while(flagDmaSpiTx == 0);
	HAL_SPI_Transmit(&hspi1, data, 3, HAL_MAX_DELAY);


}


void draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
	/*
	 * Functie pentru afisarea unui pixel pe ecran.
	 * Parametrii: Coordonatele (x,y) si culoarea
	 * Output: Void
	 */

	set_adress_window(x,y,x,y, 'w');

	DC_DATA();
	CS_A();

	write_color(color);

	CS_D();

}


static inline void draw_pixel_data(uint16_t x, uint16_t y, uint8_t *data)
{
	set_adress_window(x,y,x,y, 'w');

	DC_DATA();
	CS_A();

	HAL_SPI_Transmit(&hspi1, data, 3, HAL_MAX_DELAY);

	CS_D();

}



void fill_screen1(uint16_t color)
{
	/*
	 * Functie pentru a colora ecranul intr-un mod mai eficient.
	 * Vom transmite cate o linie pe rand, un frame de date fiind
	 * practic o linie a ecranului de 320*3 octeti
	 * Input: Culoare pe 16 biti
	 * Output: Void
	 */


	uint8_t pixel[3];
	convert_color_16_to_18(color, pixel);

	uint8_t *line = malloc(320*sizeof(pixel));

	set_adress_window(0, 0, LCD_Width-1, LCD_Length-1, 'w');

	DC_DATA();
	CS_A();

	for(uint16_t x=0; x<320; x++)
	{
		memcpy(line + x*sizeof(pixel), pixel, sizeof(pixel));
	}

	for(uint16_t y=0; y<480; y++)
	{

		HAL_SPI_Transmit(&hspi1, line, 320*sizeof(pixel), HAL_MAX_DELAY);

	}

	free(line);

	CS_D();

}


void fill_screen2(uint16_t color)
{
	set_adress_window(0, 0, LCD_Width-1, LCD_Length-1, 'w');

	uint32_t frameSize = 1200; //numarul de octeti dintr-un frame
	uint32_t nrPixel = 320*480;

	uint8_t r,g,b;

	r = (color & 0xF800) >> 11;
	g = (color & 0x07E0) >> 5;
	b = color & 0x001F;

	r = (255*r)/31;
	g = (255*g)/63;
	b = (255*b)/31;

	DC_DATA();
	CS_A();

	uint8_t frame[frameSize]; //nr de octeti de trimis intr-o tranmsisiune

	for(uint32_t j=0; j<frameSize; j=j+3)
	{
		frame[j] = r;
		frame[j+1] = g;
		frame[j+2] = b;
	}

	uint32_t data = nrPixel * 3; //nr de octeti de trimis
	uint32_t nrFrames = data / frameSize;

	flagDmaSpiTx = 1;

	for(uint32_t j = 0; j < (nrFrames); j++)
	{
		//HAL_SPI_Transmit(&hspi1, frame, frameSize, 10);

		while(flagDmaSpiTx == 0); //asteapta cat timp este 0
		flagDmaSpiTx = 0;
		HAL_SPI_Transmit_DMA(&hspi1, frame, frameSize);

	}

	//asteptam sa se finalizeze ultimul transfer -> asteptam flag1

	while(flagDmaSpiTx == 0);
	CS_D();
}



void print_character(uint16_t x, uint16_t y, char c, uint16_t fontColor, uint16_t backColor)
{
	set_adress_window(x, y, x+font.width-1, y+font.height-1, 'w');

	uint16_t mask = 0x8000;

	uint8_t i,j;
	uint16_t line = 0;

	uint8_t dataFont[3];
	uint8_t dataBack[3];

	convert_color_16_to_18(fontColor, dataFont);
	convert_color_16_to_18(backColor, dataBack);


	DC_DATA();
	CS_A();

	for(i=0; i<font.height; i++)
	{
		line = font.data[((c-32)*font.height)+i];

		for(j=0; j<font.width; j++)
		{
			if(((line<<j) & (mask)) != 0)
			{
				LCD_send_data_multi(dataFont,3);
			}

			else
			{
				LCD_send_data_multi(dataBack,3);
			}
		}
	}

	CS_D();

}


void print_string(uint16_t x, uint16_t y, char* string, uint8_t n, uint16_t fontColor, uint16_t backColor)
{

	char *temp = string;

	while(string<(temp+n))
	{
		if(x + font.width > 320)
		{
			x = font.width;
			y = y + font.height;
		}

		if((y + font.height) > 480)
		{
			return;
		}

		print_character(x, y, *string, fontColor, backColor);
		string++;
		x = x + font.width;
	}

}


void draw_horizontal_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t color)
{

	/*
	 * Functie pentru desenarea grafica unei linii orizontale.
	 * Input: x0 si y0 coordonatele initiale, x1 lungimea relativa liniei, si culoarea
	 * Output: Void
	 */

	x1 = x0+x1; //aflam lungimea efectiva
	set_adress_window(x0, y0, x1-1, y0, 'w');

	uint8_t pixel[3];
	convert_color_16_to_18(color, pixel);

	for(uint16_t i=0; i<x1-x0; i++)
	{
		LCD_send_data_multi(pixel,sizeof(pixel));
	}


}


void draw_vertical_line(uint16_t x0, uint16_t y0, uint16_t y1, uint16_t color)
{

	/*
	 * Functie pentru desenarea grafica unei linii verticale.
	 * Input: x0 si y0 coordonatele initiale, y1 lungimea relativa liniei, si culoarea
	 * Output: Void
	 */

	y1 = y0+y1; //aflam lungimea efectiva
	set_adress_window(x0, y0, x0, y1-1, 'w');

	uint8_t pixel[3];
	convert_color_16_to_18(color, pixel);

	for(uint16_t i=0; i<y1-y0; i++)
	{
		LCD_send_data_multi(pixel,sizeof(pixel));
	}


}


void draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	/*
	 * Functie pentru desenarea unui dreptunghi/patrat pe ecranul LCD
	 * Input: (x0,y0) coordonatele initiale
	 * 		  (x1,y1) offseturile fata de coordonatele initiale (numarul de pixeli)
	 * Output: Void
	 */


	uint8_t pixel[3];
	uint16_t pixelNr = x1*y1;
	convert_color_16_to_18(color, pixel);

	uint8_t *data = malloc(sizeof(pixel)*pixelNr);

	for(uint16_t i=0; i<pixelNr; i++)
	{
		memcpy(data + i*sizeof(pixel), pixel, sizeof(pixel));
	}

	set_adress_window(x0, y0, x0+x1-1, y0+y1-1, 'w');
	LCD_send_data_multi(data,pixelNr*sizeof(pixel));

	free(data);

}


void init_entity_sd(ENTITY *entity)
{
	entity->x0 = 0;
	entity->y0 = 0;
	entity->y1 = 0;
	entity->x1 = 0;

	entity->ST.SD.filePathName = malloc(sizeof(char)*20);
}


void free_entity_sd(ENTITY *entity)
{
	free(entity->ST.SD.filePathName);
	free(entity->ST.SD.data);
}


void draw_entity(ENTITY *entity)
{
	/*
	 * Functie care va desena entitatea sub forma ei prima, anume
	 * fereastra patratica de referinta
	 *
	 * Feature: De dat ca parametru numele unui fisier ce contine datele imaginii
	 * de afisat si nu culoarea, parametrul color e doar de test
	 */

	uint8_t *data;
	bool flagImgDone = 0;

	if(((entity->id & 1<<7) != 0))
	{
		/*Avem imagine monocolor <= 32x32 -> imagine nu se afla pe cardul SD!*/

		if((entity->id & (0xC0)) == 0xC0)
		{
			/*
			 * Imagine stocata local in bufferul *data din afara structurii
			 */

			LCD_send_data_multi(entity->ST.LC.data, entity->ST.LC.size);
			return;

		}

		uint16_t pixelNr = (entity->x1)*(entity->y1);

		uint8_t pixel[3];
		convert_color_16_to_18(entity->ST.color, pixel);

		data = malloc(sizeof(pixel)*pixelNr);

		for(uint16_t i=0;i<pixelNr;i++ )
		{
			memcpy(data + i*sizeof(pixel), pixel, sizeof(pixel));
		}

		set_adress_window(entity->x0, entity->y0, (entity->x1)+(entity->x0)-1, (entity->y1)+(entity->y0)-1, 'w');
		LCD_send_data_multi(data,pixelNr*sizeof(pixel));

		free(data);
	}


	else
	{
		/*Imagine stocata in bufferul *data din cardul SD*/

		uint16_t byteNr = 0; /*index*/

		read_image_file(entity, &byteNr, &flagImgDone);

		set_adress_window(entity->x0, entity->y0, (entity->x1)+(entity->x0)-1, (entity->y1)+(entity->y0)-1, 'w');

		DC_DATA();
		CS_A();

		if(byteNr < 3072)
		{
			/*
			 * Pentru cazul unui singur frame de transmis
			 */

			LCD_send_data_multi(entity->ST.SD.data, byteNr);
			free(entity->ST.SD.data);
			return;
		}

		else
		{
			/*
			 * Pentru cazul mai multor frameuri
			 */
			flagDmaSpiTx = 0;

			//LCD_send_data_multi(entity->data, byteNr);
			HAL_SPI_Transmit_DMA(&hspi1, entity->ST.SD.data, byteNr);

			do
			{
				if(flagImgDone == 1)
				{
					break;
				}

				read_image_file(entity, &byteNr, &flagImgDone);

				while(flagDmaSpiTx == 0);
				flagDmaSpiTx = 0;
				HAL_SPI_Transmit_DMA(&hspi1, entity->ST.SD.data, byteNr);

			}while(byteNr >= 3072);

		}

		while(flagDmaSpiTx == 0);
		free(entity->ST.SD.data);
		CS_D();

	}


}



void translation_entity(ENTITY *const restrict entity, int16_t x, int16_t y, bool TurnOnStep)
{
	/*
	 * Functie pentru realizarea translatiei unei imagini (entitati)
	 * Input: Un tip de data ENTITY alaturi de noile coordonate si culoarea temporara
	 * a entiatii (de adaugat culoarea fundalului)
	 * Output: Void
	 *
	 * Feature: De dat ca parametru numele unui fisier ce contine datele imaginii
	 * de afisat
	 * Momentan de rezumam la un exemplu simplu pentru a demonstra functionalitatea
	 */


	if(x<0 || x>LCD_Width || y<0 || y> LCD_Length)
	{
		/*Pentru cazul depasirii limitelor ecranului*/

		return;
	}


	ENTITY temp = *entity;

	entity->x0 = x;
	entity->y0 = y;


	if((temp.y0 == y) && (TurnOnStep==1))
	{
		/*Pentru cazul unui Step cuprins in cadrul anterior pe axa x*/

		if((x < (temp.x0+temp.x1)) && (x > (temp.x0)))
		{
			/*Pentru cazul deplasarii pe +x*/
			draw_entity(entity);
			draw_rectangle(temp.x0, temp.y0, x-temp.x0, temp.y1, 0xFFFF); /*Culoare background*/
		}

		if((x+temp.x1 < (temp.x0+temp.x1)) && (x+temp.x1 > temp.x0))
		{
			/*Pentru cazul deplasarii pe -x*/
			draw_entity(entity);
			draw_rectangle(temp.x0+temp.x1-(temp.x0-x), temp.y0, temp.x0-x, temp.y1, 0xFFFF); /*Culoare background*/
		}

	}

	else

		if((temp.x0==x) && (TurnOnStep==1))
		{
			if((y < (temp.y0+temp.y1)) && (y > (temp.y0)))
			{
				/*Pentru cazul deplasarii pe +y*/
				draw_entity(entity);
				draw_rectangle(temp.x0, temp.y0, temp.x1, y-temp.y0, 0xFFFF);
			}

			if((y+temp.y1 < (temp.y0+temp.y1)) && (y+temp.y1 > temp.y0))
			{
				/*Pentru cazul deplasarii pe -y*/
				draw_entity(entity);
				draw_rectangle(temp.x0, temp.y0+temp.y1-(temp.y0-y), temp.x1, temp.y0-y, 0xFFFF);
			}

		}

		else
		{
			/*Pentru orice alt caz (deplasare pe diagonala sau aleatoriu)*/

			draw_entity(entity);
			draw_entity(&temp);
		}


}


void translation_test(ENTITY *entity, uint8_t step, uint16_t delay)
{


	while(1)
	{
		while((entity->x0 + entity->x1) < LCD_Width)
		{
			translation_entity(entity, entity->x0+step, entity->y0, 1);//, color);
			HAL_Delay(delay);
		}

		draw_entity(entity);
		entity->x0 = LCD_Width - entity->x1;

		while((entity->y0 + entity->y1) < LCD_Length)
		{

			translation_entity(entity, entity->x0, entity->y0+step, 1);//, color);
			HAL_Delay(delay);
		}

		draw_entity(entity);
		entity->y0 = LCD_Length - entity->y1;

		while((entity->x0 - step) > 0)
		{
			translation_entity(entity, entity->x0-step, entity->y0, 1);//, color);
			HAL_Delay(delay);
		}

		entity->ST.color = 0xFFFF;
		draw_entity(entity);
		entity->ST.color = 0xF100;
		entity->x0 = 0;

		while((entity->y0 - step) > 0)
		{
			translation_entity(entity, entity->x0, entity->y0-step, 1);//, color);
			HAL_Delay(delay);
		}
		entity->ST.color = 0xFFFF;
		draw_entity(entity);
		entity->ST.color = 0xF100;
		entity->y0 = 0;

	}
}


void scaling_entity(ENTITY *entity, const float factor)
{
	/*
	 * Functie pentru scalarea unei imagini. Se vor da ca parametrii
	 * o referinta catre entitatea de scalat si factorul asociat
	 */


	/*
	 * Initial vom afla noile marimi pentru imaginea
	 * de scalat
	 */

	FRESULT res;
	char *scalFilePath;
	char *tempFile = "graphic/scalare/temp.bin";
	char *fileName = return_file_name_current_path(entity->ST.SD.filePathName);

	scalFilePath = assign_filePath("graphic/scalare/");
	scalFilePath = realloc(scalFilePath, strlen(scalFilePath)+ strlen(fileName) +1 );
	strcat(scalFilePath, fileName);

	uint16_t x = 0; /*nr d elinii de prelucrat din M1 ai sa avem 32x32 pixeli de prelucrat in M2*/
	bool flagTerm = 0;

	int i = 0; /*indexi de referinta in M2*/
	int j = 0;
	int ik = 0; /*indexi de referinta in M1*/
	int jk = 0;

	bool flagPixel = 0;


	uint8_t *data = malloc(sizeof(uint8_t)*3072);
	int16_t x1 = 0;
	int16_t y1 = 0;
	int16_t index = 0;

	x1 = (int16_t)((entity->x1)*factor);
	y1 = (int16_t)((entity->y1)*factor);



	while(!flagTerm)
	{
		read_image_file_scaling(entity->ST.SD.filePathName, entity, factor, &x, &flagTerm);

		i = 0;
		j = 0;

		ik = 0;
		jk = 0;

		for(int k=0; k<((int)(factor*x*x1*3)) ;k++)
		{
			/*
			 * Parcurgem frameul asociat matricei scalate M2
			 */

			if((k%(x1*3)==0) && (k!=0))
			{
				/*
				 * new line
				 */

				i++;
				j = 0;
			}

			if(k%3 == 0)
			{
				flagPixel = 1;
			}

			if(flagPixel == 1)
			{
				ik = (int)i/factor;
				jk = (int)j/factor;

				index = ik*(entity->x1)*3 + jk*3; /*index normat la M1*/

				data[k] = entity->ST.SD.data[index];
				data[k+1] = entity->ST.SD.data[index + 1];
				data[k+2] = entity->ST.SD.data[index + 2];

				j++;
				flagPixel = 0;
			}

		}

		/*
		 * Scriem in fisier datele obitnute din frame-ul curent in fisiserul aferent.
		 * Fisiserul va fi salvat in folderul de scalare
		 */

		write_image_file(tempFile, data, (int)(x1*factor)*x*3, x1, y1, flagTerm);

	}


	/*
	 * Vom sterge vechiul fisier rezultatul unei prelucrari anterioare,
	 * renumind fisiserul tempFile in care ne-am scris datele din temp.bin in
	 * numele aferent acestuia stocat in scalFilePath
	 */


	res = f_unlink(scalFilePath);

	if((res != FR_OK) && (res != FR_NO_FILE))
	{
		return;
	}

	res = f_rename(tempFile, scalFilePath);

	if(res != FR_OK)
	{
		return;
	}

	/*
	 * Atribuim noile valori entitatii prelucrate
	 */

	//free(entity->filePathName);

	entity->x1=x1;
	entity->y1=y1;
	assign_file_path_entity(entity, scalFilePath);

	free(data);
	free(scalFilePath);
	//free(entity->data);

}


void rotate_entity(ENTITY *entity, int theta)
{
	/*
	 * Functie pentru rotatia unei imagini.
	 * Rotatia se va realiza in jurul unui pivot(punct de referinta),
	 * ales ca mijlocul ferestrei de referinta a imaginii.
	 * Pixelii vor fi compusi si transmisi pe rand.
	 * Input: referinta catre entitate, unghiul de rotatie
	 * Output: Void
	 */

	/*Aflam initial coordonatele pivotului de referinta*/

	const int16_t pivotX = entity->x0 + (int16_t)(entity->x1/2);
	const int16_t pivotY = entity->y0 + (int16_t)(entity->y1/2);

	int16_t i = (int16_t)(-(entity->y1/2));
	int16_t j = (int16_t)(-(entity->x1/2));

	int16_t rotPosX = 0;
	int16_t rotPosY = 0;

	bool flagImgDone = 0;
	bool flagPixel = 0;
	uint16_t byteNr = 0;

	uint8_t pixel[3];


	while(!flagImgDone)
	{
		read_image_file(entity, &byteNr, &flagImgDone);


		for(int16_t k = 0; k<byteNr; k++)
		{
			if(j == (entity->x1/2))
			{
				i++;
				j = (int16_t)(-(entity->x1/2));

			}

			if(k%3 == 0)
			{
				flagPixel = 1;
			}

			if(flagPixel == 1)
			{
				pixel[0] = entity->ST.SD.data[k];
				pixel[1] = entity->ST.SD.data[k+1];
				pixel[2] = entity->ST.SD.data[k+2];

				/*Test pentru 90 de grade*/

				rotPosX = -i + pivotX;
				rotPosY =  j + pivotY;

				draw_pixel_data(rotPosX, rotPosY, pixel);

				j++;
				flagPixel = 0;

			}


		}
	}


	free(entity->ST.SD.data);

}

















































