/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
bool chenillard_actif = false;
uint8_t current_pattern = 1;
uint8_t current_frequency = 1; // 1 = TIM1, 2 = TIM2, 3 = TIM3

const uint16_t leds[3] = {GPIO_PIN_0, GPIO_PIN_7, GPIO_PIN_14};

char rx_buffer[256];
char *ptr = rx_buffer;
uint8_t ch;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, &ch, 1);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
        if (!chenillard_actif) return;
        switch (current_pattern){
        	case 1:
        		chenillard_classique();
        		break;
        	case 2:
        		chenillard_int_ext();
        		break;
        	case 3:
        		chenillard_on_off();
        		break;
        }
}

void chenillard_classique(){
	static int i = 0;
	static int direction = 1;

	reset_led();
	HAL_GPIO_WritePin(GPIOB, leds[i], GPIO_PIN_SET);
	if (direction) {
		i++;
		if (i >= 2) {
			direction = false;
		}
	} else {
	    i--;
	    if (i <= 0) {
	    	direction = true;
	    }
	}
}

void chenillard_int_ext(){
	static uint8_t state = 1;
	reset_led();
	if (state){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	} else {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    }
	state = !state;
}

void chenillard_on_off(){
	for (uint8_t i = 0; i < 3; i++){
		HAL_GPIO_TogglePin(GPIOB, leds[i]);
	}
}

void set_chenillard_frequency(uint8_t freq) {
    HAL_TIM_Base_Stop_IT(&htim1);
    HAL_TIM_Base_Stop_IT(&htim3);
    HAL_TIM_Base_Stop_IT(&htim4);

    current_frequency = freq;

    switch(freq) {
        case 1:
        	HAL_TIM_Base_Start_IT(&htim1);
        	break;
        case 2:
        	HAL_TIM_Base_Start_IT(&htim3);
        	break;
        case 3:
        	HAL_TIM_Base_Start_IT(&htim4);
        	break;
    }
}


void reset_led(){
	for (uint8_t i = 0; i < 3; i++){
		HAL_GPIO_WritePin(GPIOB, leds[i], GPIO_PIN_RESET);
	}
}

void led_on_off(){
	char last = rx_buffer[strlen(rx_buffer)-1];
	if (last == 'F') { //LED OFF
		HAL_GPIO_WritePin(GPIOB, leds[(int)(rx_buffer[3] - '0')-1], GPIO_PIN_RESET);
	} else if (last == 'N'){//LED ON
		HAL_GPIO_WritePin(GPIOB, leds[(int)(rx_buffer[3] - '0')-1], GPIO_PIN_SET);
	}
}

void manage_chenillard(){
	char last = rx_buffer[strlen(rx_buffer) - 1];
	if ( last == 'N'){
		reset_led();
		current_pattern = rx_buffer[10] - '0';
		chenillard_actif = true;
		set_chenillard_frequency(current_frequency);
	} else if(last == 'F'){
		if(rx_buffer[10] - '0' == current_pattern){
			chenillard_actif = false;
			reset_led();
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	 if (huart->Instance == USART3) {
		 if (ch == '\r' || ch == '\n') {
			 *ptr = '\0';

			 if (rx_buffer[0] == 'L'){ //Controle des LED
				 if (chenillard_actif){
					 printf("LED Control OFF, disable chenillard first\r\n");
				 }
				 led_on_off();
			 } else if (strncmp(rx_buffer, "CHENILLARD FREQUENCE", 20) == 0){ //Frequence
				 set_chenillard_frequency((uint8_t)(rx_buffer[strlen(rx_buffer)-1] - '0'));
			 } else if (strncmp(rx_buffer, "CHENILLARD", 10) == 0) { //Chenillard
				 manage_chenillard();
			 }
			 memset(rx_buffer, 0, strlen(rx_buffer));
			 ptr = rx_buffer;
		 } else {
	         *ptr = ch;
	         ptr++;
		 }
	     HAL_UART_Receive_IT(&huart3, (uint8_t *)&ch, 1);
	 }
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

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
