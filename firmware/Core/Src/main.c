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
#include "adc.h"
#include "dma.h"
#include "fdcan.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUF_SIZE 4
#define __VREFANALOG_VOLTAGE__ 3300
#define OUT_READY 0x8F // 1000 1111
#define OUT_READY1 0x9F // 1000 1111
#define OUT_CLOSE 0x80 // 1000 0000
#define DCR_ACTIVE 0xF5 // 1111 0101
#define DCR_SLEEP 0xFF // 1111 1111
#define DCR_CHANNEL0 0xF8 // 1111 1000
#define DCR_CHANNEL1 0xF9 // 1111 1001
#define DCR_CHANNEL2 0xFA // 1111 1010
#define DCR_CHANNEL3 0xFB // 1111 1011
// Diagnosis Registers - Read Commands
#define WRNDIAG 0x01// 0000 0001
#define STDDIAG 0x02// 0000 0010
#define ERRDIAG 0x03// 0000 0011
// Configuration Registers - Read Commands
#define OUT_READ 0x00 // 0000 0000
#define RCS_READ 0x08 // 0000 1000
#define SRC_READ 0x09 // 0000 1001
#define OCR_READ 0x04 // 0000 0100
#define RCD_READ 0x0A // 0000 1100
#define KRC_READ 0x05 // 0000 0101
#define PCS_READ 0x0B // 0000 1101
#define HWCR_READ 0x05 // 0000 0110
#define ICS_READ 0x0B // 0000 1110
#define DCR_READ 0x07 // 0000 0111
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t kek1[6] = { 0x8f, 0, 0, 0, 0, 0 };
uint8_t kek2[6];

uint16_t adc_buffer[ADC_BUF_SIZE];
uint8_t adc_ready = 0;

uint8_t tx_buffer[4];
uint8_t rx_buffer[4];

uint8_t after_first_loop = 0;

uint32_t fuse1_is_channel0=0;
uint32_t fuse2_is_channel0;
uint32_t fuse3_is_channel0;
uint32_t fuse4_is_channel0;

uint32_t fuse1_is_channel1;
uint32_t fuse2_is_channel1;
uint32_t fuse3_is_channel1;
uint32_t fuse4_is_channel1;

uint32_t fuse1_is_channel2;
uint32_t fuse2_is_channel2;
uint32_t fuse3_is_channel2;
uint32_t fuse4_is_channel2;

uint32_t fuse1_is_channel3;
uint32_t fuse2_is_channel3;
uint32_t fuse3_is_channel3;
uint32_t fuse4_is_channel3;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_ADC1_Init();
  MX_FDCAN1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
  //sleep -> ready
  tx_buffer[0] = OUT_READY;
  tx_buffer[1] = OUT_READY;
  tx_buffer[2] = OUT_READY;
  tx_buffer[3] = OUT_READY;
  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);
  //ready -> active
  tx_buffer[0] = DCR_ACTIVE;
  tx_buffer[1] = DCR_ACTIVE;
  tx_buffer[2] = DCR_ACTIVE;
  tx_buffer[3] = DCR_ACTIVE;
  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);

  HAL_ADC_Start_DMA(&hadc1, adc_buffer, ADC_BUF_SIZE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
	  // set channel 0 - 7A // value fuse = 0,2244*current + 93 // 1215 - 5A
	  tx_buffer[0] = DCR_CHANNEL0;
	  tx_buffer[1] = DCR_CHANNEL0;
	  tx_buffer[2] = DCR_CHANNEL0;
	  tx_buffer[3] = DCR_CHANNEL0;
	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
	  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);
	  // check current on channel 0 - 7A
	  fuse1_is_channel0 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[0], ADC_RESOLUTION12b);
	  fuse2_is_channel0 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[1], ADC_RESOLUTION12b);
	  fuse3_is_channel0 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[2], ADC_RESOLUTION12b);
	  fuse4_is_channel0 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[3], ADC_RESOLUTION12b);
//	  if (fuse1_is_channel0 > 110 && after_first_loop) {
//
//	  tx_buffer[0] = OUT_CLOSE;
//	  tx_buffer[1] = OUT_CLOSE;
//	  tx_buffer[2] = OUT_CLOSE;
//	  tx_buffer[3] = OUT_CLOSE;
//	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
//	  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
//	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);
//	}
//	  after_first_loop = 1;

	  // set channel 1 - 4A // value fuse = 0,2244*current + 93
	  tx_buffer[0] = DCR_CHANNEL1;
	  tx_buffer[1] = DCR_CHANNEL1;
	  tx_buffer[2] = DCR_CHANNEL1;
	  tx_buffer[3] = DCR_CHANNEL1;
	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
	  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);
	  // check current on channel 1 - 4A
	  fuse1_is_channel1 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[1], ADC_RESOLUTION12b);
	  fuse2_is_channel1 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[1], ADC_RESOLUTION12b);
	  fuse3_is_channel1 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[2], ADC_RESOLUTION12b);
	  fuse4_is_channel1 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[3], ADC_RESOLUTION12b);


	  // set channel 2 - 4A // value fuse = 0,2244*current + 93
	  tx_buffer[0] = DCR_CHANNEL2;
	  tx_buffer[1] = DCR_CHANNEL2;
	  tx_buffer[2] = DCR_CHANNEL2;
	  tx_buffer[3] = DCR_CHANNEL2;
	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
	  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);
	  // check current on channel 2 - 4A
	  fuse1_is_channel2 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[2], ADC_RESOLUTION12b);
	  fuse2_is_channel2 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[1], ADC_RESOLUTION12b);
	  fuse3_is_channel2 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[2], ADC_RESOLUTION12b);
	  fuse4_is_channel2 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[3], ADC_RESOLUTION12b);


	  // set channel 3 - 7A // value fuse = 0,2244*current + 93
	  tx_buffer[0] = DCR_CHANNEL3;
	  tx_buffer[1] = DCR_CHANNEL3;
	  tx_buffer[2] = DCR_CHANNEL3;
	  tx_buffer[3] = DCR_CHANNEL3;
	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
	  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);
	  // check current on channel 3 - 7A // value fuse = 0,2244*current + 93
	  fuse1_is_channel3 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[2], ADC_RESOLUTION12b);
	  fuse2_is_channel3 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[1], ADC_RESOLUTION12b);
	  fuse3_is_channel3 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[2], ADC_RESOLUTION12b);
	  fuse4_is_channel3 = __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,adc_buffer[3], ADC_RESOLUTION12b);
	  HAL_Delay(100);
//	  if (fuse2_is_channel3 > 150  && after_first_loop) {
//
//	 	  tx_buffer[0] = OUT_CLOSE;
//	 	  tx_buffer[1] = OUT_CLOSE;
//	 	  tx_buffer[2] = OUT_CLOSE;
//	 	  tx_buffer[3] = OUT_CLOSE;
//	 	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
//	 	  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
//	 	  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);
//	 	}

	  if (fuse1_is_channel0 > 1180 || fuse2_is_channel0 > 1180 || fuse3_is_channel0 > 1180|| fuse4_is_channel0 > 1180 ||
		  fuse1_is_channel3 > 1180 || fuse2_is_channel3 > 1180 || fuse3_is_channel3 > 1180|| fuse4_is_channel3 > 1180  && after_first_loop)
	  {

		  tx_buffer[0] = OUT_CLOSE;
		  tx_buffer[1] = OUT_CLOSE;
		  tx_buffer[2] = OUT_CLOSE;
		  tx_buffer[3] = OUT_CLOSE;
		  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
		  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
		  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);
		  HAL_Delay(1000);

	  }
	  if (fuse1_is_channel1 > 990 || fuse2_is_channel1 > 990 || fuse3_is_channel1 > 990|| fuse4_is_channel1 > 990 ||
		  fuse1_is_channel2 > 990 || fuse2_is_channel2 > 990 || fuse3_is_channel2 > 990|| fuse4_is_channel2 > 990  && after_first_loop)
	  	  {

	  		  tx_buffer[0] = OUT_CLOSE;
	  		  tx_buffer[1] = OUT_CLOSE;
	  		  tx_buffer[2] = OUT_CLOSE;
	  		  tx_buffer[3] = OUT_CLOSE;
	  		  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);
	  		  HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, 4, 100);
	  		  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET);
	  		HAL_Delay(1000);

	  	  }
	 	  after_first_loop = 1;

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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
