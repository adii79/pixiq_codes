/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include"stdio.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

UART_HandleTypeDef huart9;

/* USER CODE BEGIN PV */
/* USER CODE BEGIN PV */
volatile uint32_t ir_data = 0;
volatile uint8_t bit_index = 0;
volatile uint32_t last_time = 0;
volatile uint8_t data_ready = 0;
volatile uint32_t data = 0;
/* USER CODE END PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART9_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE BEGIN 0 */

static inline void DWT_Init(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t micros(void)
{
  return DWT->CYCCNT / (SystemCoreClock / 1000000);
}


void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_11)
  {
    uint32_t now = micros();
    uint32_t diff = now - last_time;
    last_time = now;

    // NEC START (9ms + 4.5ms)
    if (diff > 13000 && diff < 14000)
    {
      bit_index = 0;
      ir_data = 0;
      return;
    }

    // BIT "1" (~2.25ms)
    if (diff > 2000 && diff < 2500)
    {
      ir_data |= (1UL << bit_index);
      bit_index++;
    }
    // BIT "0" (~1.12ms)
    else if (diff > 1000 && diff < 1500)
    {
      bit_index++;
    }

    // FRAME COMPLETE
    if (bit_index >= 32)
    {
      data = ir_data;      // ✅ store final decoded data
      data_ready = 1;
      bit_index = 0;
    }
  }
}

/* USER CODE END 4 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

int main(void)
{
  MPU_Config();
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_UART9_Init();

  DWT_Init();   // ✅ INIT microsecond timer

  int a = 10;
  int *ptrA = &a;


  while (1)
  {
//      HAL_UART_Transmit(&huart9, (uint8_t*)a, 100, 100);
    if (data_ready)
    {
      char msg[64];
      sprintf(msg, "IR Data: 0x%08lX\r\n", data);

      HAL_UART_Transmit(&huart9, (uint8_t*)msg, strlen(msg), 100);
      HAL_UART_Transmit(&huart9, (uint8_t*)&msg, strlen(msg), 100);
      switch(data) {
          case 0xFC03EF00:
              HAL_UART_Transmit(&huart9, (uint8_t*)"POWER ON", 8, 100);
              break;
          case 0xFD02EF00:
              HAL_UART_Transmit(&huart9, (uint8_t*)"W/WW", 4, 100);
              break;
          case 0xFE01EF00:
              HAL_UART_Transmit(&huart9, (uint8_t*)"DOWN ", 5, 100);
              break;
          case 0xFF00EF00:
              HAL_UART_Transmit(&huart9, (uint8_t*)"UP", 2, 100);
              break;
          default:
              HAL_UART_Transmit(&huart9, (uint8_t*)"Extra", 5, 100);
              break;
      }
      data_ready = 0;
    }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART9_Init(void)
{

  /* USER CODE BEGIN UART9_Init 0 */
  /* USER CODE END UART9_Init 0 */

  /* USER CODE BEGIN UART9_Init 1 */
  /* USER CODE END UART9_Init 1 */
  huart9.Instance = UART9;
  huart9.Init.BaudRate = 115200;
  huart9.Init.WordLength = UART_WORDLENGTH_8B;
  huart9.Init.StopBits = UART_STOPBITS_1;
  huart9.Init.Parity = UART_PARITY_NONE;
  huart9.Init.Mode = UART_MODE_TX_RX;
  huart9.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart9.Init.OverSampling = UART_OVERSAMPLING_16;
  huart9.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart9.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart9.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart9) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart9, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart9, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart9) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART9_Init 2 */
  /* USER CODE END UART9_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */
//  __HAL_RCC_GPIOD_CLK_ENABLE();
   __HAL_RCC_GPIOA_CLK_ENABLE();   // ADD THIS

   GPIO_InitTypeDef GPIO_InitStruct = {0};

   /* Configure PA11 as EXTI */
   GPIO_InitStruct.Pin = GPIO_PIN_11;
   GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   /* Enable EXTI interrupt */
   HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
   HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
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
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

