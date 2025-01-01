/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"
#include "lcd.h"
#include "stm32f1xx_it.h"

//-----------------------------------1------------------------------------//

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


#define  PERIOD_VALUE       (uint32_t)(1800 - 1)  /* Period Value  */
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE*0/100) /* Capture Compare 2 Value  */
#define  PULSE3_VALUE       (uint32_t)(PERIOD_VALUE*0/100) /* Capture Compare 2 Value  */



UART_HandleTypeDef huart1;
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef TimHandle;
TIM_HandleTypeDef hTIM8;
static GPIO_InitTypeDef  GPIO_InitStruct;

//--------------------------------1-------------------------------------------//

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void Error_Handler(void);
//---------------------------------2-------------------------------------------//

static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_PWM_Init(void);
static void MX_TIM8_Init(void);


TIM_OC_InitTypeDef sConfig;


uint32_t uhPrescalerValue = 0;

//--------------------------------2--------------------------------------------//
int vertical_angle = 0, horizontal_angle = 90;
int time[3];//{h, m, s}
uint32_t adc_input[5];



int main(void)
{
  HAL_Init();
	
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM8_Init();
  MX_ADC1_Init();
	
	
  LCD_Init();
	LCD_Clear();
	
	MX_TIM2_PWM_Init();
	
  HAL_TIM_Base_Start_IT(&hTIM8);
	
	uart_trans(0);
	
	
  while (1)
  {
  }
}

static void Error_Handler(void)
{
  /* Turn LED3 on */
  BSP_LED_On(LED3);
  while (1)
  {
  }
}

//------------------------4----------------------------------------------------//
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
	
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);

  return ch;
}
//------------------------4-----------------------------------------------------//

/** System Clock Configuration
*/
//void SystemClock_Config(void)
//{

//  RCC_OscInitTypeDef OscInitStruct;
//  RCC_ClkInitTypeDef ClkInitStruct;

//  OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//  OscInitStruct.HSIState = RCC_HSI_ON;
//  OscInitStruct.HSICalibrationValue = 16;
//  OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//  HAL_RCC_OscConfig(&OscInitStruct);

//  ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
//  ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
//  ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//  ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
//  HAL_RCC_ClockConfig(&ClkInitStruct, FLASH_LATENCY_0);

//  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

//  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

//}
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/* USART1 init function */

//-----------------5--------------------------------------------------------//
void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_9B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);
}

void MX_TIM2_PWM_Init(void)
{

  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
  TIM2 Configuration: generate 4 PWM signals with 4 different duty cycles.

    In this example TIM2 input clock (TIM2CLK) is set to APB1 clock (PCLK1) x2,
    since APB1 prescaler is set to 4 (0x100).
       TIM2CLK = PCLK1*2
       PCLK1   = HCLK/2
    => TIM2CLK = PCLK1*2 = (HCLK/2)*2 = HCLK = SystemCoreClock

    To get TIM2 counter clock at 2.1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM2CLK / TIM2 counter clock) - 1
       Prescaler = ((SystemCoreClock) /2.1 MHz) - 1

    To get TIM2 output clock at 3 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM2 counter clock / TIM2 output clock) - 1
           = 699

    TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM2_ARR + 1)* 100 = 50%
    TIM2 Channel2 duty cycle = (TIM2_CCR2/ TIM2_ARR + 1)* 100 = 37.5%
    TIM2 Channel3 duty cycle = (TIM2_CCR3/ TIM2_ARR + 1)* 100 = 25%
    TIM2 Channel4 duty cycle = (TIM2_CCR4/ TIM2_ARR + 1)* 100 = 12.5%

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f1xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */

  /* Initialize TIMx peripheral as follows:
       + Prescaler = (SystemCoreClock / 2000000) - 1
       + Period = (700 - 1)
       + ClockDivision = 0
       + Counter direction = Up
  */
	
	uhPrescalerValue = (uint32_t)(72000000 / (50 * PERIOD_VALUE));
	
  TimHandle.Instance = TIMx;

  TimHandle.Init.Prescaler         = uhPrescalerValue;
  TimHandle.Init.Period            = PERIOD_VALUE;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for all channels */
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  /* Set the pulse value for channel 1 */
  sConfig.Pulse = PULSE1_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }

  /* Set the pulse value for channel 2 */
  sConfig.Pulse = PULSE3_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }

  /*##-3- Start PWM signals generation #######################################*/
  /* Start channel 1 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
  /* Start channel 3 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
}


void MX_TIM8_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  hTIM8.Instance = TIM8;
  hTIM8.Init.Prescaler = 2000;
  hTIM8.Init.CounterMode = TIM_COUNTERMODE_UP;
  hTIM8.Init.Period = 7200;
  hTIM8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  hTIM8.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&hTIM8);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&hTIM8, &sClockSourceConfig);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  HAL_TIM_SlaveConfigSynchronization(&hTIM8, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&hTIM8, &sMasterConfig);

}

/* ADC1 init function */
void MX_ADC1_Init(void)
{
  __HAL_RCC_ADC1_CLK_ENABLE();


    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = ENABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 5;
	hadc1.Init.NbrOfDiscConversion = 1;
	
  HAL_ADC_Init(&hadc1);

	/**Configure Regular Channel 
	*/
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;

	// Channel 0 (PA0)
	sConfig.Channel = ADC_CHANNEL_3;//V
	sConfig.Rank = 1;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	// Channel 2 (PA2)
	sConfig.Channel = ADC_CHANNEL_5;//V
	sConfig.Rank = 2;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	// Channel 3 (PA3)
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = 3;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	// Channel 5 (PA5)
	sConfig.Channel = ADC_CHANNEL_7;//V
	sConfig.Rank = 4;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	// Channel 7 (PA7)
	sConfig.Channel = ADC_CHANNEL_12;
	sConfig.Rank = 5;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

}

//----------------5----------------------------------------------------------//

/** Pinout Configuration
*/
void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __GPIOF_CLK_ENABLE();

  /*Configure GPIO pins : PF6 PF7 PF8 PF9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*uart gpio*/
  __GPIOA_CLK_ENABLE();
  /*adc gpio*/
	__HAL_RCC_GPIOA_CLK_ENABLE();  // ?? GPIOA ???
	__HAL_RCC_GPIOC_CLK_ENABLE();  // ?? GPIOC ???

	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;  // ?? PA0, PA2, PA3, PA5, PA7
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;  // ???????
	GPIO_InitStructure.Pull = GPIO_NOPULL;  // ????
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);  // ??? GPIOA

	GPIO_InitStructure.Pin = GPIO_PIN_12;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);  // ??? GPIOA
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
