////
////#include "main.h"
////#include <stdio.h>
////
/////* ================= VARIABLES ================= */
////
////UART_HandleTypeDef huart3;
////
////volatile uint32_t last_time = 0;
////volatile uint32_t pulse_width = 0;
////
////volatile uint8_t bit_index = 0;
////volatile uint32_t ir_data = 0;
////volatile uint8_t receiving = 0;
////
/////* ================= PROTOTYPES ================= */
////
////void SystemClock_Config(void);
////static void MX_GPIO_Init(void);
////static void MX_USART3_UART_Init(void);
////void Error_Handler(void);
////
/////* ================= PRINTF ================= */
////
////int _write(int file, char *ptr, int len)
////{
////    HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, HAL_MAX_DELAY);
////    return len;
////}
////
/////* ================= IR INTERRUPT ================= */
////
////void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
////{
////    if (GPIO_Pin == GPIO_PIN_11)
////    {
////        uint32_t now = HAL_GetTick();   // millisecond timing
////        pulse_width = now - last_time;
////        last_time = now;
////
////        // Detect start frame (~13ms)
////        if (pulse_width >= 12 && pulse_width <= 20)
////        {
////            receiving = 1;
////            bit_index = 0;
////            ir_data = 0;
////            return;
////        }
////
////        if (receiving)
////        {
////            // Rough decoding (ms resolution)
////            if (pulse_width >= 2)   // treat as logic 1
////            {
////                ir_data |= (1UL << bit_index);
////            }
////            // else logic 0
////
////            bit_index++;
////
////            if (bit_index >= 32)
////            {
////                receiving = 0;
////
////                printf("IR Code: 0x%08lX\r\n", ir_data);
////
////                bit_index = 0;
////            }
////        }
////    }
////}
////
/////* ================= MAIN ================= */
////
////int main(void)
////{
////    HAL_Init();
////    SystemClock_Config();
////
////    MX_GPIO_Init();
////    MX_USART3_UART_Init();
////
////    printf("IR Decoder Started (No Timer Mode)\r\n");
////
////    while (1)
////    {
////        HAL_Delay(1000);
////    }
////}
////
/////* ================= GPIO INIT ================= */
////
////static void MX_GPIO_Init(void)
////{
////    GPIO_InitTypeDef GPIO_InitStruct = {0};
////
////    __HAL_RCC_GPIOA_CLK_ENABLE();
////    __HAL_RCC_GPIOD_CLK_ENABLE();
////
////    /* IR input PA11 */
////    GPIO_InitStruct.Pin = GPIO_PIN_11;
////    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
////    GPIO_InitStruct.Pull = GPIO_PULLUP;
////    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
////
////    /* USART3 TX + RX */
////    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
////    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
////    GPIO_InitStruct.Pull = GPIO_NOPULL;
////    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
////    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
////    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
////
////    /* Enable EXTI */
////    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
////    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
////}
////
/////* ================= UART INIT ================= */
////
////static void MX_USART3_UART_Init(void)
////{
////    huart3.Instance = USART3;
////    huart3.Init.BaudRate = 115200;
////    huart3.Init.WordLength = UART_WORDLENGTH_8B;
////    huart3.Init.StopBits = UART_STOPBITS_1;
////    huart3.Init.Parity = UART_PARITY_NONE;
////    huart3.Init.Mode = UART_MODE_TX_RX;
////    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
////    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
////
////    if (HAL_UART_Init(&huart3) != HAL_OK)
////    {
////        Error_Handler();
////    }
////}
////
/////* ================= CLOCK ================= */
////
////void SystemClock_Config(void)
////{
////    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
////    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
////
////    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
////    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
////
////    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
////
////    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
////    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
////    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
////    HAL_RCC_OscConfig(&RCC_OscInitStruct);
////
////    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK;
////    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
////
////    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
////}
////
/////* ================= ERROR HANDLER ================= */
////
////void Error_Handler(void)
////{
////    __disable_irq();
////    while (1)
////    {
////    }
////}
//
//
//
//
//#include "main.h"
//#include <stdio.h>
//
///* ================= VARIABLES ================= */
//
//UART_HandleTypeDef huart3;
//uint8_t data[] = "Hello world\n";
//volatile uint32_t last_time = 0;
//volatile uint32_t pulse_width = 0;
//
//volatile uint8_t bit_index = 0;
//volatile uint32_t ir_data = 0;
//volatile uint8_t receiving = 0;
//
//volatile uint8_t data_ready = 0;
//volatile uint32_t final_data = 0;
//
//// ===== FILTER VARIABLES =====
//uint32_t last_printed = 0;
//uint32_t last_print_time = 0;
//
///* ================= PROTOTYPES ================= */
//
//void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
//static void MX_USART3_UART_Init(void);
//void Error_Handler(void);
//
///* ================= PRINTF ================= */
//
//int _write(int file, char *ptr, int len)
//{
//    HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, HAL_MAX_DELAY);
//    return len;
//}
//
///* ================= IR INTERRUPT ================= */
//
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//    if (GPIO_Pin == GPIO_PIN_11)
//    {
//        uint32_t now = HAL_GetTick();
//        pulse_width = now - last_time;
//        last_time = now;
//
//        // ===== START FRAME (~13ms) =====
//        if (pulse_width >= 12 && pulse_width <= 20)
//        {
//            receiving = 1;
//            bit_index = 0;
//            ir_data = 0;
//            return;
//        }
//
//        if (receiving)
//        {
//            if (pulse_width == 0)
//                return;
//
//            // logic 1 (~1.7ms)
//            if (pulse_width >= 2)
//            {
//                ir_data |= (1UL << bit_index);
//            }
//
//            bit_index++;
//
//            // ===== FRAME COMPLETE =====
//            if (bit_index >= 32)
//            {
//                receiving = 0;
//
//                final_data = ir_data;
//                data_ready = 1;
//
//                bit_index = 0;
//                ir_data = 0;
//            }
//        }
//    }
//}
//
///* ================= MAIN ================= */
//
//int main(void)
//{
//    HAL_Init();
//    SystemClock_Config();
//
//    MX_GPIO_Init();
//    MX_USART3_UART_Init();
//
//    printf("IR Decoder Started (No Repeat Mode)\r\n");
//
//    while (1)
//    {
//    	 HAL_UART_Transmit(&huart3, data, 12, 1000); HAL_Delay(1000);
//        if (data_ready)
//        {
//            uint32_t now = HAL_GetTick();
//
//            // ===== FILTER REPEAT SIGNAL =====
//            if (final_data != last_printed || (now - last_print_time) > 500)
//            {
//                printf("IR Code: 0x%08lX\r\n", final_data);
//
//                last_printed = final_data;
//                last_print_time = now;
//            }
//
//            data_ready = 0;
//        }
//
//        HAL_Delay(10); // small delay to reduce CPU load
//    }
//}
//
///* ================= GPIO INIT ================= */
//
//static void MX_GPIO_Init(void)
//{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//    __HAL_RCC_GPIOD_CLK_ENABLE();
//
//    /* IR input PA11 */
//    GPIO_InitStruct.Pin = GPIO_PIN_11;
//    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//    /* USART3 TX (PD8), RX (PD9) */
//    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
//    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
//
//    /* EXTI Interrupt Enable */
//    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
//}
//
///* ================= UART INIT ================= */
//
//static void MX_USART3_UART_Init(void)
//{
//    huart3.Instance = USART3;
//    huart3.Init.BaudRate = 115200;
//    huart3.Init.WordLength = UART_WORDLENGTH_8B;
//    huart3.Init.StopBits = UART_STOPBITS_1;
//    huart3.Init.Parity = UART_PARITY_NONE;
//    huart3.Init.Mode = UART_MODE_TX_RX;
//    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
//
//    if (HAL_UART_Init(&huart3) != HAL_OK)
//    {
//        Error_Handler();
//    }
//}
//
///* ================= CLOCK ================= */
//
//void SystemClock_Config(void)
//{
//    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
//    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
//
//    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
//
//    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
//    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//    HAL_RCC_OscConfig(&RCC_OscInitStruct);
//
//    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK;
//    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
//
//    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
//}
//
///* ================= ERROR HANDLER ================= */
//
//void Error_Handler(void)
//{
//    __disable_irq();
//    while (1)
//    {
//    }
//}


