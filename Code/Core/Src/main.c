/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "scheduler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define PERIOD_500MS  50
//#define PERIOD_1S    100
//#define PERIOD_1_5S  150
//#define PERIOD_2S    200
//#define PERIOD_2_5S  250
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_TIM2_Init(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

//void Task_500ms(void);
//void Task_1s(void);
//void Task_1_5s(void);
//void Task_2s(void);
//void Task_2_5s(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

void Task_500ms(){
	HAL_GPIO_TogglePin(GPIOA, LED_RED_Pin);
}

void Task_1s(void){
	HAL_GPIO_TogglePin(GPIOA, RED_1_Pin);
}

void Task_1_5s(void){
	HAL_GPIO_TogglePin(GPIOA, RED_2_Pin);
}

void Task_2s(void){
//	HAL_GPIO_TogglePin(GPIOA, RED_3_Pin);
}

void Task_2_5s(void){
//	HAL_GPIO_TogglePin(GPIOA, RED_4_Pin);
}

void OneShot_TestTask(void){
	// Task test One-shot: Chỉ chạy 1 lần sau 3s
	// Đảo trạng thái LED_RED_Pin (Toggle)
	HAL_GPIO_TogglePin(LED_RED_GPIO_Port, RED_4_Pin);
	// Sau đó task sẽ bị xóa
}

// Test tính Non-preemtive
void TaskA(void){
	HAL_GPIO_TogglePin(GPIOA, RED_1_Pin);
	HAL_Delay(1000);
}

void TaskB(void){
	HAL_GPIO_TogglePin(GPIOA, RED_2_Pin);
}

void Error_Test_Task(void){
    // Test Xóa Task không hợp lệ
    // Cố gắng xóa vị trí task 30 (SCH_MAX_TASKS=40)
    SCH_Delete_Task(30);
}

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
  MX_TIM2_Init();
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);

    // Khởi tạo Scheduler
    SCH_Init();
//    HAL_GPIO_WritePin(GPIOA, LED_RED_Pin | RED_1_Pin | RED_2_Pin | RED_3_Pin | RED_4_Pin, GPIO_PIN_SET);
    // Thêm 5 task chạy định kỳ
    // Sử dụng DELAY khác nhau để tránh Task Overlap (theo 2.3.11 trong PDF)
//    SCH_Add_Task(Task_500ms, 0, 50); // Chạy ngay (delay 0), chu kỳ 500ms (50 ticks)
//    SCH_Add_Task(Task_1s, 50, 100); // Chạy sau 0.5s, chu kỳ 1s (100 ticks)
//    SCH_Add_Task(Task_1_5s, 100, 150); // Chạy sau 1s, chu kỳ 1.5s (150 ticks)
//    SCH_Add_Task(Task_2s, 150, 200); // Chạy sau 1.5s, chu kỳ 2s (200 ticks)
//    SCH_Add_Task(Task_2_5s, 200, 250); // Chạy sau 2s, chu kỳ 2.5s (250 ticks)
    SCH_Add_Task(OneShot_TestTask, 300, 0);

//    SCH_Add_Task(TaskA, 100, 100);
//    SCH_Add_Task(TaskB, 100, 100);

    // Test quá tải tasks
//    for(int i = 0; i < 50; i++){
//    	  // Task không hoạt động (chỉ để chiếm chỗ)
//    	  SCH_Add_Task(Error_Test_Task, 0, 10);
//      }

//    SCH_Add_Task(Error_Test_Task, 100, 0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  SCH_Add_Task(led1test, 100, 200);
  while (1)
  {
    /* USER CODE END WHILE */
	  SCH_Dispatch_Tasks();

	  // --- Dùng để DEBUG/Kiểm tra ---
	   if (Error_code_G != 0) {
	       // Báo hiệu bằng LED nếu phát hiện lỗi
	       HAL_GPIO_WritePin(GPIOA, TEST_PIN_Pin, GPIO_PIN_RESET);
	   } else {
	       HAL_GPIO_WritePin(GPIOA, TEST_PIN_Pin, GPIO_PIN_SET);
	   }
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
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_RED_Pin|RED_1_Pin|RED_2_Pin|RED_3_Pin
                          |RED_4_Pin|TEST_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_RED_Pin RED_1_Pin RED_2_Pin RED_3_Pin
                           RED_4_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin|RED_1_Pin|RED_2_Pin|RED_3_Pin
                          |RED_4_Pin|TEST_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim) {
	SCH_Update();
}
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
