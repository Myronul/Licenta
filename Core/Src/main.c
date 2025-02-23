/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "graphics.h"
#include "lcd.h"
#include "sdsys.h"
#include "audio.h"
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;

SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_spi1_rx;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

volatile uint8_t flagDmaSpiTx = 0; /*flaguri pentru transfer DMA prin SPI*/
volatile uint8_t flagDmaSpiRx = 0;

volatile bool flagDmaDAC = 0; /*Flag pentru DMA pe DAC si bufferele aferente*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM2_Init(void);
static void MX_SDIO_SD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


  void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
  {
	  /*
	   * Functie de CallBack pentru terminarea transferului SPI
	   * folosind DMA. Odata terminat transferul SPI prin DMA,
	   * aceasta functia de CallBack se va apela, setandu-ne un flag
	   * pentru a indica starea acestui transfer de date.
	   */

	  flagDmaSpiTx = 1;

  }


  void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
  {
	  /*
	   * Functie de CallBack pentru terminarea receptiei datelor
	   * prin SPI folosind DMA (analog cu functia HAL_SPI_TxCpltCallback)
	   */

	  flagDmaSpiRx = 1;

  }


  void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
  {
      /*
       * Functie de CallBack pentru finalizare 1/2 din transfer DMA pe DAC
       */

	  flagDmaDAC = 1;

  }



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_DAC_Init();
  MX_TIM2_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  init_cardSD();  /*Initializare sistem de fisiere card SD*/
  ILI9488_driver_init();  /*Initializare driver ecran LCD*/
  HAL_TIM_Base_Start(&htim2); /*Initializare timer2 pentru trigger DMA pe DAC*/


  fill_screen1(0xF100);
  HAL_Delay(1000);
  fill_screen2(0xF00F);
  HAL_Delay(1000);
  fill_screen2(0x3F51);
  HAL_Delay(1000);

  unsigned int startTick = 0;
  unsigned int endTick = 0;
  unsigned int getTime = 0;

  //Test read from LCD

  //draw_pixel(0,0,0x001F);
  //draw_pixel(1,0,0xF800);
  //draw_pixel(0,1,0x001F);
  //draw_pixel(1,1,0xFF00);

  //uint8_t *dataRec1;
  //dataRec1 = malloc(sizeof(char)*20);
  //read_pixel_frame(0,0,2,2,dataRec1);

  //free(dataRec1);

  //Test image print from SD card
  ENTITY ent;
  ent.x0 = 0;
  ent.y0 = 0;
  ent.id = 0x00;
  draw_entity(&ent, "graphic/img5.bin");
  HAL_Delay(2000);
  draw_entity(&ent, "graphic/img6.bin");
  HAL_Delay(2000);
  draw_entity(&ent, "graphic/img8.bin");
  HAL_Delay(2000);
  draw_entity(&ent, "graphic/img9.bin");
  HAL_Delay(2000);
  startTick = HAL_GetTick();
  draw_entity(&ent, "graphic/img92.bin");
  endTick = HAL_GetTick();
  getTime = endTick - startTick;

  fill_screen2(0x0000);
  draw_entity(&ent, "graphic/pixel1.bin");
  HAL_Delay(3000);
  //draw_entity(&ent, "graphic/img51.txt");
  //HAL_Delay(2000);
  //draw_entity(&ent, "graphic/img6.txt");
  //HAL_Delay(2000);
  //draw_entity(&ent, "graphic/img8.txt");
  //HAL_Delay(2000);
  //draw_entity(&ent, "graphic/img4.txt");
  //HAL_Delay(1000);

  //read_image_file("graphic/img4.txt", &ent);
  //set_adress_window(ent.x0, ent.y0, ent.x1+ent.x0-1, ent.y1+ent.y0-1, 'w');
  //LCD_send_data_multi(ent.data,sizeof(char)*(ent.x1*ent.y1)*3);

  //HAL_Delay(4000);

  //free(ent.data);

  //Test translation

  ENTITY entity;

  entity.x0 = 0;
  entity.y0 = 0;
  entity.x1 = 64;
  entity.y1 = 64;
  entity.color = 0xF100;

  //draw_entity(&entity,NULL);
  HAL_Delay(500);

  translation_test(&entity, 1, 0);

  HAL_Delay(500);
  translation_entity(&entity, entity.x0+100, entity.y0+100, 1);//, 0xF100);
  HAL_Delay(2000);
  translation_entity(&entity, entity.x0+32, entity.y0+32, 1);//, 0xF100);
  HAL_Delay(500);
  translation_entity(&entity, entity.x0+12, entity.y0+32, 1);//, 0xF100);
  HAL_Delay(500);
  translation_entity(&entity, entity.x0+12, entity.y0, 1);//, 0xF100);
  HAL_Delay(500);


  //Test_SD_Card();

  play_audio_file("audio/mine.txt"); //doremi mine songita song22

  char *fileData = NULL;
  read_file("audio/text.txt", fileData);
  HAL_Delay(1000);

  uint32_t sampleData[1024]; /*23 de ms pentru redare DMA dintre care 7 ms pentru citire din fisier*/
  	  	  	  	  	  	  	 /*Vor ramane aproximativ 16 ms pentru prelucrarea frame-ului (pentru 1024 de esantioane)*/

  startTick = HAL_GetTick();
  read_audio_file("audio/random.txt", sampleData);
  endTick = HAL_GetTick();
  getTime = endTick - startTick;

  HAL_Delay(100);

  startTick = HAL_GetTick();
  read_audio_file("audio/random.txt", sampleData);
  endTick = HAL_GetTick();
  getTime = endTick - startTick;

  HAL_Delay(100);
  read_audio_file("audio/text.txt", sampleData);
  HAL_Delay(100);
  read_audio_file("audio/text.txt", sampleData);
  HAL_Delay(100);
  read_audio_file("audio/text.txt", sampleData);
  HAL_Delay(100);

  for(uint8_t i=0;i<100;i++)
  {
	  for(uint8_t j=0; j<100;j++)
	  {

		  if(i==j)
		  {
			  draw_pixel(i,j,0xF100);
			  continue;
		  }



		  if((i == 80 && j==20) || (i == 80 && j==19) || (i == 81 && j==20) || (i == 81 && j==19))
		  {
			  draw_pixel(i,j,0x001F);
			  continue;
		  }


		  draw_pixel(i,j,0xFFFF);

	  }
  }


  //HAL_Delay(1);

  /*

  fill_screen(0x001F);

  HAL_Delay(1000);

  fill_screen(0xF100);

  HAL_Delay(1000);

  fill_screen1(0xFFFF);

  print_character(50, 50, 'a', 0x1F00, 0xFFFF);

  HAL_Delay(5000);

  fill_screen1(0x001F);

  HAL_Delay(1000);

  fill_screen2(0xFFFF);

  HAL_Delay(250);

  fill_screen2(0xF100);

  LCD_send_command(ILI9488_DISPON);

  */

  fill_screen2(0xF100);
  HAL_Delay(1000);
  fill_screen2(0xF150);
  HAL_Delay(1000);
  fill_screen2(0xF111);
  HAL_Delay(1000);
  fill_screen2(0xF10F);
  HAL_Delay(1000);
  fill_screen2(0xFFFF);
  HAL_Delay(1000);
  fill_screen2(0xFFFF);

  print_string(36, 200, "Licenta 2025", 12, 0x1F00, 0x001F);

  uint8_t dataToSend[] = {0x01, 0x02, 0x03, 0x04};
  HAL_SPI_Transmit_DMA(&hspi1, dataToSend, sizeof(dataToSend));

  draw_horizontal_line(20, 20, 80, 0xF100);
  draw_vertical_line(20, 20, 80, 0xF100);

  flagDmaSpiTx = 0;

  draw_pixel(0,0,0x001F);
  draw_pixel(1,0,0xF800);
  draw_pixel(0,1,0x001F);
  draw_pixel(1,1,0xF800);

  HAL_Delay(50);

  uint8_t *dataRec;
  dataRec = malloc(sizeof(uint8_t));
  //dataRec = LCD_read_data(2,2,0,0);
  //free(dataRec);
  read_pixel_frame(0,0,2,2,dataRec);
  //read_pixel_format();
  //HAL_UART_Transmit(&huart1, (uint8_t*)"Pixel Data: ",12,HAL_MAX_DELAY);
  free(dataRec);

  HAL_Delay(3000);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 8;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 23;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PD3 PD4 PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
