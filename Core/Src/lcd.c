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

	flagDmaSpiTx = 1;

	unsigned int valMaxFrame = 1024; //numarul maxim de octeti pe frame
	unsigned int nrFrames = size/valMaxFrame; //numarul de frameuri de trimis
	unsigned int remainder = size%valMaxFrame; //restul de trimis daca e cazul

	DC_DATA();
	CS_A();


	if(size <= valMaxFrame)
	{
		flagDmaSpiTx = 0;
		HAL_SPI_Transmit_DMA(&hspi1, data, size);
		//HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
	}

	else
	{
		while(nrFrames != 0)
		{
			while(flagDmaSpiTx == 0);
			flagDmaSpiTx = 0;
			HAL_SPI_Transmit_DMA(&hspi1, data, valMaxFrame);
			//HAL_SPI_Transmit(&hspi1, data, valMaxFrame, HAL_MAX_DELAY);
			data = data+valMaxFrame;
			nrFrames--;
		}

		if(remainder != 0)
		{
			while(flagDmaSpiTx == 0);
			flagDmaSpiTx = 0;
			HAL_SPI_Transmit_DMA(&hspi1, data, remainder);
			//HAL_SPI_Transmit(&hspi1, data, remainder, HAL_MAX_DELAY);

		}
	}

	while(flagDmaSpiTx == 0);
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
	LCD_send_command(0x11); //scoatem sistemul din sleep
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


	LCD_send_command(0x11);
	HAL_Delay(255);
	LCD_send_command(0x51); //Luminozittea Display
	LCD_send_data(0xFF);    //maxima
	LCD_send_command(0x29); //Display on


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



void read_pixel_frame(uint16_t x0, uint16_t y0, uint16_t x, uint16_t y, uint8_t*data)
{
	/*
	* Functie pentru receptionarea datelor aferente unei portiuni din ecran.
	* Se va selecta o fereastra de adresare LCD-ului, ca mai apoi sa se trimita comanda
	* de transmitere a datelor LCD->MCU.
	*/

	hspi1.Instance->CR1 &= ~SPI_CR1_SPE; //dezactivare temporara SPI
	hspi1.Instance->CR1 &= ~SPI_CR1_BR;  //resetare valoare BD
	hspi1.Instance->CR1 |= SPI_BAUDRATEPRESCALER_8; //setare valoare BD prescaler de 8

	hspi1.Instance->CR1 |= SPI_CR1_SPE; //reactivare SPI


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

    free(data);

    //HAL_UART_Transmit(&huart1, dummy, 1, HAL_MAX_DELAY);
    //HAL_UART_Transmit(&huart1, data, byteNr, HAL_MAX_DELAY);


    hspi1.Instance->CR1 &= ~SPI_CR1_SPE;
    hspi1.Instance->CR1 &= ~SPI_CR1_BR;
    hspi1.Instance->CR1 |= SPI_BAUDRATEPRESCALER_2;

    hspi1.Instance->CR1 |= SPI_CR1_SPE; //reactivare SPI

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







