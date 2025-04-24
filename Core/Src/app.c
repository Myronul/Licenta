/*
 * app.c
 *
 *  Created on: Apr 24, 2025
 *      Author: Miron Andrei
 */

#include "app.h"

/*
 * Biblioteca ce include functii pentru aplicatii
 * de nivel inalt. Meniu de utilizator, teste ale
 * functiilor dezvoltate etc.
 */

static volatile void Task0()
{

	ENTITY entity;
	entity.x0 = 0;
	entity.y0 = 0;
	entity.x1 = 64;
	entity.y1 = 64;
	entity.id = 0x80;
	entity.ST.color = 0xF100;

    while(1)
    {
        //HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
        //mutex = 1;
        //HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
        //HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
        translation_test(&entity, 1, 0);
    	//mutex = 0;
    }
}

static volatile void Task1()
{

    while(1)
    {
    	play_audio_file("Audio/acoustic.txt");
    	kernel_delay(20);
    }
}

/*

volatile void Task1()
{
	ENTITY entity;
	entity.x0 = 0;
	entity.y0 = 0;
	entity.x1 = 64;
	entity.y1 = 64;
	entity.id = 0x80;
	entity.ST.color = 0xF100;

    while(1)
    {

    	//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
    	//flagg = 1;
    	//mutex = 1;
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
        translation_entity(&entity, entity.x0+1, entity.y0, 1);
    	//mutex = 0;
    	HAL_Delay(40);
    }
}
*/

static volatile void Task2()
{
	ENTITY entity;
	entity.x0 = 0;
	entity.y0 = 300;
	entity.x1 = 64;
	entity.y1 = 64;
	entity.id = 0x80;
	entity.ST.color = 0xF100;

    while(1)
    {

    	//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
    	//flagg = 1;
    	//mutex = 1;
        //HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
        translation_test(&entity, 1, 0);
        //translation_entity(&entity, entity.x0+1, entity.y0, 1);
    	//mutex = 0;

    }
}


static volatile void Task3()
{
	while(1)
	{
		controller_test();
	}
}

static volatile void Task4()
{
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
		kernel_delay(2000);
	}
}

static volatile void Task5()
{
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
		kernel_delay(2000);
	}
}


static void demo_os_1()
{
	  BackGroundColor = 0xFFFF;
	  fill_screen2(0xFFFF);
	  print_string(128, 128, "os demo",0xF100, BackGroundColor);

	  kernel_add_process(Task0);
	  kernel_add_process(Task1);
	  kernel_add_process(Task2);
	  //kernel_add_process(Task3);
	  //kernel_add_process(Task4);
	  //kernel_add_process(Task5);
	  kernel_start();

	  while(1)
	  {

	  }
}

static void set_main_gui(void)
{
	/*
	 * Functie statica pentru setarea interfetei
	 * user.
	 * Input: void
	 * Output: void
	 */

	init_cursor();

	BackGroundColor = WHITE;
	fill_screen1(BackGroundColor);
	cursor.y0 = 16;
	cursor.x0 = 32;

	print_string(cursor.x0, cursor.y0, "My Menu", BLACK, BackGroundColor);
	cursor.y0 += 32;
	print_string(cursor.x0, cursor.y0, "Operating Systems", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Graphics", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Audio", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "App1", BLACK, BackGroundColor);

	init_cursor();

}


void main_app()
{
	/*
	 * Meniul prinicpal al aplicatiei.
	 * Input: void
	 * Output: void
	 */

	set_main_gui();

	cursor.x0 = 0;
	cursor.y0 = 48;

	int16_t x = 0;
	int16_t y = 48;

	uint8_t Q = 0;

	draw_entity(&cursor);

	while(1)
	{
		if(currentDx == DxDown)
		{
			if(Q == 3)
			{
				Q = 0;
				y = 48;
			}

			else
			{
				Q++;
				y += 16;
			}

			translation_entity(&cursor, x, y, 0);
			currentDx = 0;
		}

		if(currentDx == DxUp)
		{
			if(Q == 0)
			{
				Q = 3;
				y = 48+16*3;
			}
			else
			{
				Q--;
				y -= 16;
			}

			translation_entity(&cursor, x, y, 0);
			currentDx = 0;
		}

		if(currentDx == DxRight)
		{
			if(Q == 0)
			{
				currentDx = 0;
				demo_os_1();
			}
		}


	}


}

















