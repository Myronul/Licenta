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

uint8_t pback = 0;

static void controller_test()
{
	ENTITY entity;
	entity.x0 = 0;
	entity.y0 = 300;
	entity.x1 = 64;
	entity.y1 = 64;
	entity.id = 0x80;
	entity.ST.color = 0xF100;

	draw_entity(&entity);

	while(1)
	{

		switch(currentDx)
		{
			case DxRight:
				translation_entity(&entity, entity.x0+32, entity.y0, 1);
				currentDx = 0;
				break;
			case DxLeft:
				translation_entity(&entity, entity.x0-32, entity.y0, 1);
				currentDx = 0;
				break;
			case DxUp:
				translation_entity(&entity, entity.x0, entity.y0-32, 1);
				currentDx = 0;
				break;
			case DxDown:
				translation_entity(&entity, entity.x0, entity.y0+32, 1);
				currentDx = 0;
				break;
			default:
				currentDx = 0;
				break;
		}

		kernel_delay(50);

	}

}

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
	  fill_screen2(BackGroundColor);
	  print_string(128, 128, "os demo",0xF100, BackGroundColor);

	  kernel_add_process(Task0);
	  kernel_add_process(Task1);
	  kernel_add_process(Task2);
	  kernel_add_process(Task3);
	  //kernel_add_process(Task4);
	  //kernel_add_process(Task5);
	  kernel_start();

	  while(1)
	  {

	  }
}


static void int_to_string(uint8_t n, char *s)
{
	uint16_t divider = 1;
	uint8_t temp = n;

	uint8_t c = 0;
	uint8_t i = 0;

	if(n == 0)
	{
		s[i++] = '0';
		s[i] = '\0';
		return;
	}

	while((n/divider)>=10)
	{
		divider *= 10;
	}

	do
	{
		c = n/divider;
		s[i++] = c + '0';

		n %= divider;
		divider /= 10;

	}while(divider != 0);

	s[i] = '\0';

}

static void translation_controller_test()
{
	fill_screen2(BackGroundColor);

	ENTITY entity;
	entity.x0 = 0;
	entity.y0 = 300;
	entity.x1 = 64;
	entity.y1 = 64;
	entity.id = 0x80;
	entity.ST.color = 0xF100;

	uint8_t step = 1;
	char stepScore[4];
	currentDx = 0;

	draw_entity(&entity);
	int_to_string(step, stepScore);
	print_string(32, 460, stepScore, BLACK, BackGroundColor);

	while(1)
	{

		switch(currentDx)
		{
			case DxRight:
				translation_entity(&entity, entity.x0+step, entity.y0, 0);
				currentDx = 0;
				break;
			case DxLeft:
				translation_entity(&entity, entity.x0-step, entity.y0, 0);
				currentDx = 0;
				break;
			case DxUp:
				translation_entity(&entity, entity.x0, entity.y0-step, 0);
				currentDx = 0;
				break;
			case DxDown:
				translation_entity(&entity, entity.x0, entity.y0+step, 0);
				currentDx = 0;
				break;
			case DxSelect:
				while(currentDx == DxSelect);
					switch(currentDx)
					{
						case DxUp:
							if(step >= 32)
							{
								currentDx = 0;
								break;
							}
							else
							{
								step++;
								print_string(32, 460, "   ", BackGroundColor, BackGroundColor);
								int_to_string(step, stepScore);
								print_string(32, 460, stepScore, BLACK, BackGroundColor);
								currentDx = 0;
								break;
							}
						case DxDown:
							if(step <= 1)
							{
								currentDx = 0;
								break;
							}
							else
							{
								step--;
								print_string(32, 460, "   ", BackGroundColor, BackGroundColor);
								int_to_string(step, stepScore);
								print_string(32, 460, stepScore, BLACK, BackGroundColor);
								currentDx = 0;
								break;
							}
						case DxRight:
							pback = 1;
							currentDx = 0;
							HAL_Delay(50);
							return;
						default:
							break;
					}
			default:
				currentDx = 0;
				break;
		}

		HAL_Delay(25);

	}

}


static void scaling_test(void)
{
	/*
	 * Functie pentru testarea operatiei de scalare
	 */

	fill_screen2(BackGroundColor);

	ENTITY entity;
	init_entity_sd(&entity);

	entity.x0 = 0;
	entity.y0 = 0;
	entity.id = 0;

	assign_file_path_entity(&entity, "graphic/multi2.bin");
	draw_entity(&entity);

	while(1)
	{
		switch(currentDx)
		{
			case DxRight:
				translation_entity(&entity, entity.x0+16, entity.y0, 0);
				currentDx = 0;
				break;
			case DxLeft:
				translation_entity(&entity, entity.x0-16, entity.y0, 0);
				currentDx = 0;
				break;
			case DxUp:
				translation_entity(&entity, entity.x0, entity.y0-16, 0);
				currentDx = 0;
				break;
			case DxDown:
				translation_entity(&entity, entity.x0, entity.y0+16, 0);
				currentDx = 0;
				break;
			case DxSelect:
				while(currentDx == DxSelect);
					switch(currentDx)
					{
						case DxUp:
							erase_entity(entity);
							scaling_entity(&entity, 2);
							draw_entity(&entity);
							break;
						case DxDown:
							erase_entity(entity);
							scaling_entity(&entity, 0.5);
							draw_entity(&entity);
							break;
						case DxRight:
							free_entity_sd(&entity);
							pback = 1;
							currentDx = 0;
							HAL_Delay(50);
							return;
						default:
							break;
					}
			default:
				currentDx = 0;
				break;
		}

		HAL_Delay(25);


	}

	pback = 1;

}


static void rotation_test(void)
{
	/*
	 * Functie pentru testarea operatiei grafice
	 * de rotatie
	 */

	fill_screen2(BackGroundColor);

	ENTITY entity;
	init_entity_sd(&entity);

	entity.x0 = 80;
	entity.y0 = 80;
	entity.id = 0;

	assign_file_path_entity(&entity, "graphic/multi2.bin");
	draw_entity(&entity);
	scaling_entity(&entity, 60);
	erase_entity(entity);
	draw_entity(&entity);

	while(1)
	{
		switch(currentDx)
		{
			case DxRight:
				free_entity_sd(&entity);
				pback = 1;
				currentDx = 0;
				HAL_Delay(50);
				return;
			case DxLeft:
				rotate_entity(&entity, 270);
				break;
			case DxUp:
				rotate_entity(&entity, 90);
				break;
			case DxDown:
				rotate_entity(&entity, 180);
				break;
			default:
				currentDx = 0;
				break;
		}

		currentDx = 0;
		HAL_Delay(25);


	}


}


static void graphics_test(void)
{
	/*
	 * Aplicatie de testare grafica.
	 * Se vor testa functiile de scalare,
	 * translatie si rotatie
	 */

	fill_screen2(BackGroundColor);

	ENTITY entity;
	init_entity_sd(&entity);

	entity.x0 = 100;
	entity.y0 = 100;
	entity.id = 0;

	assign_file_path_entity(&entity, "graphic/multi2.bin");
	draw_entity(&entity);

	HAL_Delay(1000);

	erase_entity(entity);
	scaling_entity(&entity, 38);
	draw_entity(&entity);

	HAL_Delay(1000);

	erase_entity(entity);
	rotate_entity(&entity, 90);

	HAL_Delay(1000);

	erase_entity(entity);
	draw_entity(&entity);

	HAL_Delay(1000);

	erase_entity(entity);
	scaling_entity(&entity, 0.5);
	draw_entity(&entity);

	HAL_Delay(1000);

	fill_screen2(BackGroundColor);
	entity.x0 = 0;
	entity.y0 = 0;
	assign_file_path_entity(&entity, "graphic/img8.bin");
	draw_entity(&entity);

	HAL_Delay(1000);

	fill_screen2(BackGroundColor);
	scaling_entity(&entity, 0.5);
	draw_entity(&entity);

	HAL_Delay(1000);

	scaling_entity(&entity, 2);
	draw_entity(&entity);
	erase_entity(entity);

	HAL_Delay(1000);

	free_entity_sd(&entity);
	pback = 1;

}


static void set_graphics_gui(void)
{
	init_cursor();

	fill_screen1(BackGroundColor);
	cursor.y0 = 16;
	cursor.x0 = 32;

	print_string(cursor.x0, cursor.y0, "Graphics Player", BLACK, BackGroundColor);
	cursor.y0 += 32;
	print_string(cursor.x0, cursor.y0, "Test Demo", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Scaling", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Translation", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Rotation", BLACK, BackGroundColor);

	init_cursor();

}


static void list_graphics(void)
{
	/*
	 * Interfata pentru selectarea si redarea unor
	 * melodii alese de utilizator
	 */

	set_graphics_gui();

	cursor.x0 = 0;
	cursor.y0 = 48;

	int16_t x = 0;
	int16_t y = 48;

	uint8_t Q = 0;

	uint8_t nrOptions = 4;
	nrOptions--; /*0 inclus*/

	draw_entity(&cursor);

	currentDx = 0;

	while(1)
	{

		if(pback == 1)
		{
			/*
			 * Initializare gui si pt cazul in care ne intaorcem
			 * dintr-o interfata selectat
			 */

			set_graphics_gui();

			cursor.x0 = 0;
			cursor.y0 = 48;

			x = 0;
			y = 48;

			Q = 0;

			draw_entity(&cursor);

			currentDx = 0;
			pback = 0;
		}

		if(currentDx == DxDown)
		{
			if(Q == nrOptions)
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
				Q = nrOptions;
				y = 48+16*nrOptions;
			}
			else
			{
				Q--;
				y -= 16;
			}

			translation_entity(&cursor, x, y, 0);
			currentDx = 0;
		}

		if(currentDx == DxSelect)
		{
			currentDx = 0;

			switch(Q)
			{
				case 0:
					graphics_test();
					break;
				case 1:
					scaling_test();
					break;
				case 2:
					translation_controller_test();
					break;
				case 3:
					rotation_test();
					break;
				default:
					break;

			}

			currentDx = 0;

		}

		if(currentDx == DxRight)
		{
			pback = 1;
			currentDx = 0;
			return;
		}


		HAL_Delay(25);


	}


}


static void set_audio_effects_gui(void)
{
	init_cursor();

	fill_screen1(BackGroundColor);
	cursor.y0 = 16;
	cursor.x0 = 32;

	print_string(cursor.x0, cursor.y0, "Choose Filter:", BLACK, BackGroundColor);
	cursor.y0 += 32;
	print_string(cursor.x0, cursor.y0, "Reverb", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Vibrato", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Echo", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "No Filters", BLACK, BackGroundColor);

	init_cursor();
}


static void list_audio_effects(void)
{

	pback = 1;

	cursor.x0 = 0;
	cursor.y0 = 48;

	int16_t x = 0;
	int16_t y = 48;

	uint8_t Q = 0;

	uint8_t nrOptions = 4;
	nrOptions--; /*0 inclus*/

	currentDx = 0;

	while(1)
	{

		if(pback == 1)
		{
			/*
			 * Initializare gui si pt cazul in care ne intaorcem
			 * dintr-o interfata selectat
			 */

			set_audio_effects_gui();

			cursor.x0 = 0;
			cursor.y0 = 48;

			x = 0;
			y = 48;

			Q = 0;

			draw_entity(&cursor);

			pback = 0;
		}

		if(currentDx == DxDown)
		{
			if(Q == nrOptions)
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
				Q = nrOptions;
				y = 48+16*nrOptions;
			}
			else
			{
				Q--;
				y -= 16;
			}

			translation_entity(&cursor, x, y, 0);
			currentDx = 0;
		}

		if(currentDx == DxSelect)
		{
			currentDx = 0;

			switch(Q)
			{
				case 0:
					play_audio_file_reverb("Audio/acoustic.txt");
					break;
				case 1:
					play_audio_file_vibrato("Audio/acoustic.txt");
					break;
				case 2:
					play_audio_file_echo("Audio/acoustic.txt", 20, 0.8);
					break;
				case 3:
					play_audio_file("Audio/acoustic.txt");
					break;
				default:
					break;

			}

			currentDx = 0;

		}

		if(currentDx == DxRight)
		{
			pback = 1;
			currentDx = 0;
			return;
		}


	}
}


static void set_audio_player_gui(void)
{
	init_cursor();

	fill_screen1(BackGroundColor);
	cursor.y0 = 16;
	cursor.x0 = 32;

	print_string(cursor.x0, cursor.y0, "Music PLayer", BLACK, BackGroundColor);
	cursor.y0 += 32;
	print_string(cursor.x0, cursor.y0, "Song1", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Song2", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Song3", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Song4", BLACK, BackGroundColor);

	init_cursor();
}


static void list_audio_player(void)
{
	/*
	 * Interfata pentru selectarea si redarea unor
	 * melodii alese de utilizator
	 */

	pback = 1;

	cursor.x0 = 0;
	cursor.y0 = 48;

	int16_t x = 0;
	int16_t y = 48;

	uint8_t Q = 0;

	uint8_t nrOptions = 4;
	nrOptions--; /*0 inclus*/

	currentDx = 0;

	while(1)
	{

		if(pback == 1)
		{
			/*
			 * Initializare gui si pt cazul in care ne intaorcem
			 * dintr-o interfata selectat
			 */

			set_audio_player_gui();

			cursor.x0 = 0;
			cursor.y0 = 48;

			x = 0;
			y = 48;

			Q = 0;

			draw_entity(&cursor);

			pback = 0;
		}

		if(currentDx == DxDown)
		{
			if(Q == nrOptions)
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
				Q = nrOptions;
				y = 48+16*nrOptions;
			}
			else
			{
				Q--;
				y -= 16;
			}

			translation_entity(&cursor, x, y, 0);
			currentDx = 0;
		}

		if(currentDx == DxSelect)
		{
			currentDx = 0;

			switch(Q)
			{
				case 0:
					play_audio_file("Audio/acoustic.txt");
					break;
				case 1:
					play_audio_file("Audio/king.txt");
					break;
				case 2:
					play_audio_file("Audio/acoustic.txt");
					break;
				case 3:
					play_audio_file("Audio/acoustic.txt");
					break;
				default:
					break;

			}

			currentDx = 0;

		}

		if(currentDx == DxRight)
		{
			pback = 1;
			currentDx = 0;
			return;
		}


	}


}


static void set_audio_gui(void)
{
	init_cursor();

	fill_screen1(BackGroundColor);
	cursor.y0 = 16;
	cursor.x0 = 32;

	print_string(cursor.x0, cursor.y0, "Choose Mode", BLACK, BackGroundColor);
	cursor.y0 += 32;
	print_string(cursor.x0, cursor.y0, "Music PLayer", BLACK, BackGroundColor);
	cursor.y0 += 16;
	print_string(cursor.x0, cursor.y0, "Audio Effects", BLACK, BackGroundColor);

	init_cursor();
}


static void list_audio(void)
{

	pback = 1;

	cursor.x0 = 0;
	cursor.y0 = 48;

	int16_t x = 0;
	int16_t y = 48;

	uint8_t Q = 0;

	uint8_t nrOptions = 2;
	nrOptions--; /*0 inclus*/

	currentDx = 0;

	while(1)
	{
		if(pback == 1)
		{
			/*
			 * Initializare gui si pt cazul in care ne intaorcem
			 * dintr-o interfata selectat
			 */

			set_audio_gui();

			cursor.x0 = 0;
			cursor.y0 = 48;

			x = 0;
			y = 48;

			Q = 0;

			draw_entity(&cursor);

			pback = 0;
		}

		if(currentDx == DxDown)
		{
			if(Q == nrOptions)
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
				Q = nrOptions;
				y = 48+16*nrOptions;
			}
			else
			{
				Q--;
				y -= 16;
			}

			translation_entity(&cursor, x, y, 0);
			currentDx = 0;
		}

		if(currentDx == DxSelect)
		{
			currentDx = 0;

			switch(Q)
			{
				case 0:
					list_audio_player();
					break;
				case 1:
					list_audio_effects();
					break;
				default:
					break;

			}

			currentDx = 0;

		}

		if(currentDx == DxRight)
		{
			pback = 1;
			currentDx = 0;
			return;
		}


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
	print_string(cursor.x0, cursor.y0, "Operating System(boot)", BLACK, BackGroundColor);
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

	pback = 1;

	cursor.x0 = 0;
	cursor.y0 = 48;

	int16_t x = 0;
	int16_t y = 48;

	uint8_t Q = 0;

	uint8_t nrOptions = 4;
	nrOptions--; /*0 inclus*/

	while(1)
	{
		if(pback == 1)
		{
			/*
			 * Initializare gui si pt cazul in care ne intaorcem
			 * dintr-o interfata selectat
			 */

			set_main_gui();

			cursor.x0 = 0;
			cursor.y0 = 48;

			x = 0;
			y = 48;

			Q = 0;

			draw_entity(&cursor);

			pback = 0;
		}


		if(currentDx == DxDown)
		{
			if(Q == nrOptions)
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
				Q = nrOptions;
				y = 48+16*nrOptions;
			}
			else
			{
				Q--;
				y -= 16;
			}

			translation_entity(&cursor, x, y, 0);
			currentDx = 0;
		}

		if(currentDx == DxSelect)
		{
			currentDx = 0;

			switch(Q)
			{
				case 0:
					demo_os_1();
					break;
				case 1:
					list_graphics();
					break;
				case 2:
					list_audio();
					break;
				case 3:
					break;
				default:
					break;
			}

			currentDx = 0;

		}


	}


}

















