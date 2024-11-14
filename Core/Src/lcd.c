#include "lcd.h"


void LCD_send_command(uint8_t cmd)
{
	/*
	 * Functia va transmite driverului o comanda.
	 * In mod automat se vor activate/dezactiva pinii aferenti
	 * pentru a realiza transmiterea comenzii (DC,CS).
	 * Input: 1. Comanda pe 8 biti (de obicei in hexazecimal)
	 * Output: Void
	 */

	DC_COMMAND();                                                //Modul de comanda DC LOW
	CS_A();                                                      //selectare dispozitiv CS LOW

	HAL_SPI_Transmit(&hspi1, (uint8_t*)&cmd, 1, HAL_MAX_DELAY);  //Transmitere SPI comanda

	CS_D();                                                      //deselectare ecran CS HIGH


}


void LCD_send_data_multi(uint8_t *data, unsigned int size)
{
	/*
	 * Functie de transmitere a cadrelor de date prin SPI.
	 * Vom imparti informatia mai mare decat un maxim setat in cadre
	 * pentru eficientizarea transmisiei.
	 * Input: 1.Pointer pe 8 biti ce include datele de transmis
	 * 		  2.Marimea pointerului, anume a octetilor continut de acesta
	 * Output: Void
	 */

	//flagDmaSpiTx = 1;

	unsigned int valMaxFrame = 1024; //numarul maxim de octeti pe frame
	unsigned int nrFrames = size/valMaxFrame; //numarul de frameuri de trimis
	unsigned int remainder = size%valMaxFrame; //restul de trimis daca e cazul

	DC_DATA();
	CS_A();


	if(size <= valMaxFrame)
	{
		//flagDmaSpiTx = 0;
		//HAL_SPI_Transmit_DMA(&hspi1, data, size);

		HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
	}

	else
	{
		while(nrFrames != 0)
		{
			//while(flagDmaSpiTx == 0);
			//flagDmaSpiTx = 0;
			//HAL_SPI_Transmit_DMA(&hspi1, data, valMaxFrame);
			HAL_SPI_Transmit(&hspi1, data, valMaxFrame, HAL_MAX_DELAY);
			data = data+valMaxFrame;
			nrFrames--;
		}

		if(remainder != 0)
		{
			//while(flagDmaSpiTx == 0);
			//flagDmaSpiTx = 0;
			//HAL_SPI_Transmit_DMA(&hspi1, data, remainder);
			HAL_SPI_Transmit(&hspi1, data, remainder, HAL_MAX_DELAY);

		}
	}

	//while(flagDmaSpiTx == 0);
	CS_D();

}


void LCD_send_data(uint8_t data)
{
	/*
	 * Functie de transmitere a unui singur octet prin SPI (analog cu transmiterea comenzilor).
	 * Parametrii: 1. Datele de transmis pe un octet
	 * Date returnate: Void
	 */

	DC_DATA();
	CS_A();

	HAL_SPI_Transmit(&hspi1, (uint8_t*)&data, 1, HAL_MAX_DELAY);

	CS_D();

}



void ILI9488_driver_init()
{
	/*
	 * Functie pentru initializarea ecranului.
	 * Se vor folosi diferite comenzi si date pentru initializare.
	 * Se va folosi formatul de 24 biti in format Portret
	 * Nu va returna nimic
	 * Input: Void
	 * Output: Void
	 */

	//Mai intai vom incepe printr-un RESET HW

	RST_A();
	HAL_Delay(10);
	RST_D();

	//Vom continua prin diferite comenzi urmate de setari specifice pentru initializare

	LCD_send_command(0x01); //Reset SW de driver
	HAL_Delay(150);
	LCD_send_command(ILI9488_SLPOUT); //scoatem sistemul din sleep
	HAL_Delay(255);

	LCD_send_command(0xE0); //Comanda pentru a seta controlul la nivel de GAMMA pozitiv
	LCD_send_data(0x00);	   //Urmat de parametrii ce vor defini curba gamma
	LCD_send_data(0x0C);
	LCD_send_data(0x05);
	LCD_send_data(0x04);
	LCD_send_data(0x0F);
	LCD_send_data(0x06);
	LCD_send_data(0x3A);
	LCD_send_data(0x56);
	LCD_send_data(0x4D);
	LCD_send_data(0x03);
	LCD_send_data(0x0A);
	LCD_send_data(0x06);
	LCD_send_data(0x30);
	LCD_send_data(0x3E);
	LCD_send_data(0x0F);

	LCD_send_command(0xE1);  //Comanda pentru a seta controlul la nivel de GAMMA negativ
	LCD_send_data(0x00);
	LCD_send_data(0x13);
	LCD_send_data(0x18);
	LCD_send_data(0x01);
	LCD_send_data(0x11);
	LCD_send_data(0x06);
	LCD_send_data(0x38);
	LCD_send_data(0x34);
	LCD_send_data(0x4D);
	LCD_send_data(0x06);
	LCD_send_data(0x0D);
	LCD_send_data(0x0B);
	LCD_send_data(0x31);
	LCD_send_data(0x37);
	LCD_send_data(0x0F);

	LCD_send_command(0xC0); //Comanda pentru PowerControl1
	LCD_send_data(0x18);
	LCD_send_data(0x16);

	LCD_send_command(0xC1); //Comanda pentru PowerControl2
	LCD_send_data(0x45);

	LCD_send_command(0xC2); //Comanda pentru PowerControl3
	LCD_send_data(0x33);

	LCD_send_command(0xC3); //Comanda pentru PowerControl3
	LCD_send_data(0x33);


	LCD_send_command(0x36); //Comanda pentru modul de setare al pixelilor (pg 192)
	LCD_send_data(0x5C);    //Comanda pentru setare mod RGB, Scriere st-dr sus-jos 5C

	LCD_send_command(0x3A); //Comanda pentru setarea numarului de biti asociat unui pixel
	LCD_send_data(0x06);    //0x66 reprezinta formatul de 18 biti

	LCD_send_command(0x00); //NOP pentru mic delay
	LCD_send_command(0x00);

	LCD_send_command(0xB0); //Comanda pentru setarea interfetei de comunicare cu display-ul
	LCD_send_data(0x00);    //Folosim pinul SDO: SDA_EN 0 0 0 VSPL HSPL DPL EPL (pg219)

	LCD_send_command(0xB1); //Comanda pentru rata de refresh pentru 24 bit culoare
	LCD_send_data(0xA0);    //Aproximaiv 60HZ
	LCD_send_data(0x11);  //Date pentru selectie zona de memorie a datelor pe ecran (nu folosim)

	LCD_send_command(0xB4); //Comanda pentru contrast
	LCD_send_data(0x02);    //Contrast 2-Dot standard

	LCD_send_command(0xB5); //Comanda pentru Blanking Porch
	LCD_send_data(0x02);
	LCD_send_data(0x02);
	LCD_send_data(0x0A);
	LCD_send_data(0x04);

	LCD_send_command(0xB6); //Display Function Control
	LCD_send_data(0x02);
	LCD_send_data(0x02);
	LCD_send_data(0x3B);

	LCD_send_command(0XE9); //Set Image Function
	LCD_send_data(0x00);    //Dezactivam modul de 24 de biti

	LCD_send_command(0xF7); //Comanda pentru DSI dar folosim SPI
	LCD_send_data(0xA9);
	LCD_send_data(0x51);
	LCD_send_data(0x2C);
	LCD_send_data(0x82);


	LCD_send_command(ILI9488_SLPOUT);
	HAL_Delay(255);
	LCD_send_command(0x51); //Luminozittea Display
	LCD_send_data(0xFF);    //maxima
	LCD_send_command(ILI9488_DISPON);


}



void convert_color_16_to_18(uint16_t color, uint8_t *pixel)
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


void set_adress_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char x)
{
	/*
	 * Functie pentru selectarea ariei de colorat. Se vor selecta pixelii
	 * pornind de la x0, y0 pana la x1-1, y1-1 (ultimul pixel nu este inclus in selectie)
	 * Input: coordonate start (x, y) coordonate finish (x1,y1)
	 * Output: Void
	 */

	uint8_t data[4];

	LCD_send_command(ILI9488_CASET); //setare coloana anume x0-x1
										//se vor tranmite mai intai MSB apoi LSB (pg 175)

	data[0] = (x0 >> 8) & 0x00FF; data[1] = x0 & 0x00FF; //coordonate de start x0
	data[2] = (x1 >> 8) & 0x00FF; data[3] = x1 & 0x00FF; //coordonate de final x1
	LCD_send_data_multi(data, sizeof(data));


	LCD_send_command(ILI9488_PASET);

	data[0] = (y0 >> 8) & 0x00FF; data[1] = y0 & 0x00FF; //coordonate de start y0
	data[2] = (y1 >> 8) & 0x00FF; data[3] = y1 & 0x00FF; //coordonate de final y1
	LCD_send_data_multi(data, sizeof(data));

	if(x == 'w')
	{
		LCD_send_command(ILI9488_RAMWR); //Comanda pentru a pregati ecranul sa primeasca culori
										//Dupa ce am setat intervalul de selectie
	}

	if(x == 'r')
	{
		LCD_send_command(ILI9488_RAMRD);
	}


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



void fill_screen(uint16_t color)
{
	/*
	 * Functie pentru a colora intreg ecranul cu o singura culoare.
	 * Input: Culoare pe 16 biti ce va fi interpolata la 24 de biti
	 * Output: Void
	 */

	unsigned int nrPixel = LCD_Width*LCD_Length;  //numarul total de pixeli
	unsigned int nrBytes = nrPixel * 3;	          //numarul de Octeti de trimis

	uint32_t i = 0;

	uint8_t r,g,b;

	r = (color & 0xF800) >> 11;
	g = (color & 0x07E0) >> 5;
	b = color & 0x001F;

	//16bit->24bit extindere

	r = (255*r)/31;
	g = (255*g)/63;
	b = (255*b)/31;

	//vom imparti informatia in cadre

	uint32_t bufSize = 65535;
	uint32_t k = 0;

	if(nrBytes <= 65535)
	{
		bufSize = nrBytes;
		k = 1;
	}

	else
	{
		k = nrBytes/65535;  //numarul de frames de trimis

		if(nrBytes % 65535 != 0)
		{
			k++;  //daca nu este divizor mai adaugam inca un transfer pentru rest
		}

		bufSize = 65535;
	}


	uint8_t frame[bufSize]; //acest singur frame il vom transmite de k ori


	for(i=0; i<bufSize/3; i++)
	{
		frame[i*3] = r;
		frame[i*3+1] = g;
		frame[i*3+2] = b;
	}

	//am populat frame-ul pe o perioada cu valorile rgb

	set_adress_window(0, 0, LCD_Width-1, LCD_Length-1, 'w');

	DC_DATA();
	CS_A();

	for(i=0; i<k; i++)
	{
		uint32_t bytesToSend;

		if(i == k-1)
		{
			//ultima transmitere
			bytesToSend = nrBytes % 65535;

			if(bytesToSend == 0)
			{
				bytesToSend = bufSize;
			}
		}

		else
		{
			bytesToSend = bufSize;
		}


		HAL_SPI_Transmit(&hspi1, frame, bytesToSend, HAL_MAX_DELAY);


	}

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
	uint8_t r,g,b;

	r = (color & 0xF800) >> 11;
	g = (color & 0x07E0) >> 5;
	b = color & 0x001F;

	r = (255*r)/31;
	g = (255*g)/63;
	b = (255*b)/31;

	uint8_t pixel[] = {r,g,b};
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
		HAL_SPI_Transmit_DMA(&hspi1, frame, frameSize);
		flagDmaSpiTx = 0;

	}

	//asteptam sa se finalizeze ultimul transfer -> asteptam flag1

	while(flagDmaSpiTx == 0);
	CS_D();
}



void read_pixel_frame(uint16_t x0, uint16_t y0, uint16_t x, uint16_t y, uint8_t*data)
{
	/*
	* Functie pentru receptionarea datelor aferente unei portiuni din ecran.
	* Se va selecta o fereastra de adresare LCD-ului, ca mai apoi sa se trimita comanda
	* de transmitere a datelor LCD->MCU.
	*/

	hspi1.Instance->CR1 &= ~SPI_CR1_SPE;
	hspi1.Instance->CR1 &= ~SPI_CR1_BR;
	hspi1.Instance->CR1 |= SPI_BAUDRATEPRESCALER_8;


	flagDmaSpiRx = 0;

	uint16_t byteNr = 0;
	byteNr = ((abs(x-x0))*(abs(y-y0))*3);

    uint8_t dummy[1] = {0};

    free(data);
    data = malloc(byteNr);

    x--;
    y--;

    uint8_t xPos[4] = {x0 >> 8, x0 & 0xFF, x >> 8, x & 0xFF};
    uint8_t yPos[4] = {y0 >> 8, y0 & 0xFF, y >> 8, y & 0xFF};

    CS_A();

    DC_COMMAND();
    uint8_t cmdCset = 0x2A;
    HAL_SPI_Transmit(&hspi1, &cmdCset, 1, HAL_MAX_DELAY);
    DC_DATA();
    HAL_SPI_Transmit(&hspi1, xPos, 4, HAL_MAX_DELAY);

    DC_COMMAND();
    uint8_t cmdPset = 0x2B;
    HAL_SPI_Transmit(&hspi1, &cmdPset, 1, HAL_MAX_DELAY);
    DC_DATA();
    HAL_SPI_Transmit(&hspi1, yPos, 4, HAL_MAX_DELAY);

    DC_COMMAND();
    uint8_t cmdRamRead = 0x2E;
    HAL_SPI_Transmit(&hspi1, &cmdRamRead, 1, HAL_MAX_DELAY);
    DC_DATA();

    HAL_SPI_Receive(&hspi1, dummy, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive_DMA(&hspi1, data, byteNr);

    while(flagDmaSpiRx == 0);
    CS_D();

    //HAL_UART_Transmit(&huart1, dummy, 1, HAL_MAX_DELAY);
    //HAL_UART_Transmit(&huart1, data, byteNr, HAL_MAX_DELAY);


    hspi1.Instance->CR1 &= ~SPI_CR1_SPE;
    hspi1.Instance->CR1 &= ~SPI_CR1_BR;
    hspi1.Instance->CR1 |= SPI_BAUDRATEPRESCALER_2;

}



void read_pixel_format()
{
	uint8_t cmd = 0x0C;
	uint8_t data[3] = {0};

	DC_COMMAND();
	CS_A();

	HAL_SPI_Transmit(&hspi1, (uint8_t*)&cmd, 1, HAL_MAX_DELAY);
	DC_DATA();
	HAL_SPI_Receive(&hspi1, data, 3, HAL_MAX_DELAY);

	CS_D();
	HAL_UART_Transmit(&huart1, data,3,HAL_MAX_DELAY);

}




