/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "lcd.h"
   
	 

/** @addtogroup STM32F1xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
	
#define PWM_PULSE_BASE 45
#define SG90_V_MAX_ANGLE 140
#define SG90_V_MIN_ANGLE 2
#define SG90_H_MAX_ANGLE 180
#define SG90_H_MIN_ANGLE 0
#define STEP0 1
#define STEP1 5
#define STEP2 20

extern TIM_HandleTypeDef hTIM8;

int i, j, k;
int ii, jj, kk;
int pwm_cnt = 0;
int pwm_priod = 1800;
int pwm_pulse_v;
int pwm_pulse_h;

int timer_Scaler_1000ms_cnt = 0;

uint16_t LCD_display_meter[19][16] = {
{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
{16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
{32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
{48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63},
{64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79},
{80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95},
{96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111},    
{112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127},
{128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143},
{144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159},
{160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175},
{176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191},
{192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207},
{208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223},
{224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239},
{240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255},
{256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271},
{272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287},
{288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303}
};
uint16_t LCD_num[10] = {304, 305, 306, 307, 308, 309, 310, 311, 312, 313};
uint16_t LCD_display_char[4][2] = 
{
	{314, 315},//水
	{316, 317},//平
	{318, 319},//垂
	{320, 321} //直
};




/*--  水           --426~427*/
/*--  平           --428~429*/
/*--  垂           --430~431*/
/*--  直           --432~433*/
/*timer*/

void TIM8_UP_IRQHandler(void)//frequency = 2Hz
{
	read_adc_value();
	mainlogic(adc_input[0],adc_input[1],adc_input[2],adc_input[3],adc_input[4]);
	
	checkangle();
	control_sg90();
	lcd_reflash();
	
	
	timer_scaler();
	
	
	if(timer_Scaler_1000ms_cnt == 0)
	{
		count_time();
		uart_trans(1);
	}
	HAL_TIM_IRQHandler(&hTIM8);
}

	int step = 1;
void mainlogic(int h_left,int h_right,int v_up,int v_down,int center_value)
{
	if(center_value>h_left && center_value>h_right && center_value>v_up && center_value>v_down)
	{
		//暫無動作
	}
	else
	{
		int tmp = h_left-h_right;
		tmp = tmp > 0? tmp:-tmp;
		if(tmp > 120)
		{
			step = STEP2;
		}
		else if(tmp > 100)
		{
			step = STEP1;
		}
		else
		{
			step = STEP0;
		}
		
		
		if(h_left>h_right)
		{
			horizontal_angle += step;
		}
		else if(h_left<h_right)
		{
			horizontal_angle -= step;
		}
		
		
		tmp = v_up-v_down;
		tmp = tmp > 0? tmp:-tmp;
		if(tmp > 200)
		{
			step = STEP1;
		}
		else if(tmp > 120)
		{
			step = 2;
		}
		else
		{
			step = STEP0;
		}

		
		if(v_up>v_down)
		{
			vertical_angle -= step;
		}
		else if(v_up<v_down)
		{
			vertical_angle += step;
		}
	}
}

void control_sg90()
{
	pwm_pulse_v = PWM_PULSE_BASE + vertical_angle;
	if(pwm_pulse_v > (PWM_PULSE_BASE + SG90_V_MAX_ANGLE)) pwm_pulse_v = PWM_PULSE_BASE + SG90_V_MAX_ANGLE;
	if(pwm_pulse_v < (PWM_PULSE_BASE + SG90_V_MIN_ANGLE)) pwm_pulse_v = PWM_PULSE_BASE + SG90_V_MIN_ANGLE;
	__HAL_TIM_SetCompare(&TimHandle, TIM_CHANNEL_1, (uint32_t)(pwm_pulse_v));
	
	pwm_pulse_h = PWM_PULSE_BASE + horizontal_angle;
	if(pwm_pulse_h > (PWM_PULSE_BASE + SG90_H_MAX_ANGLE)) pwm_pulse_h = PWM_PULSE_BASE + SG90_H_MAX_ANGLE;
	if(pwm_pulse_h < (PWM_PULSE_BASE + SG90_H_MIN_ANGLE)) pwm_pulse_h = PWM_PULSE_BASE + SG90_H_MIN_ANGLE;
	__HAL_TIM_SetCompare(&TimHandle, TIM_CHANNEL_3, (uint32_t)(pwm_pulse_h));
}

void checkangle()
{
	if(vertical_angle > SG90_V_MAX_ANGLE) 
	{
		vertical_angle = SG90_V_MAX_ANGLE;
	}
	else if(vertical_angle < SG90_V_MIN_ANGLE) 
	{
		vertical_angle = SG90_V_MIN_ANGLE;
	}

	if(horizontal_angle > SG90_H_MAX_ANGLE) 
	{
		horizontal_angle = SG90_H_MAX_ANGLE;
	}
	else if(horizontal_angle < SG90_H_MIN_ANGLE) 
	{
		horizontal_angle = SG90_H_MIN_ANGLE;
	}
}


void lcd_reflash()//V
{
	//printf("reflash\n\r");
/*meter display*/
	unsigned char meter_h_state = 0, meter_v_state = 0;//horizontal_angle
	int i;
	
	
	
	for(i = 18; i > 0; i--)
	{
		if (horizontal_angle > (i * 10 - 5)) 
		{
			meter_h_state = i; 
			break;
		}
	}
	
	LCD_DrawString(0 , 0 , &LCD_display_meter[meter_h_state][0],8);
	LCD_DrawString(2 , 0 , &LCD_display_meter[meter_h_state][8],8);
	
	
	uint16_t tmp;
	int angle;
	
	LCD_DrawString(4 , 0 , &LCD_display_char[0][0],2);//水
	LCD_DrawString(4 , 16 , &LCD_display_char[1][0],2);//平
	tmp = 322;//:
	LCD_DrawString(4 , 32 , &tmp,1);
	
	angle = horizontal_angle-90;
	//printf("%d\n\r", angle);
	if(angle < 0)
	{
		angle = - angle;
		//printf("%d\n\r", angle);
		tmp = 323;
		LCD_DrawString(4 , 40 , &tmp,1);//-
	}
	else
	{
		tmp = 324;
		LCD_DrawString(4 , 40 , &tmp,1);//
	}
	
	tmp = angle / 10;
	//printf("%d\n\r", tmp);
	LCD_DrawString(4 , 48 , &LCD_num[tmp],1);//horizontal_angle value
	tmp = angle % 10;
	LCD_DrawString(4 , 56 , &LCD_num[tmp],1);
	
	
	
	angle = 180 - vertical_angle;
	for(i = 18; i > 0; i--)
	{
		if (angle > (i * 10 - 5)) 
		{
			meter_v_state = i;
			break;
		}
	}
	
	LCD_DrawString(0 , 64 , &LCD_display_meter[meter_v_state][0],8);
	LCD_DrawString(2 , 64 , &LCD_display_meter[meter_v_state][8],8);
	
	
	angle = 90 - vertical_angle;
	if(angle < 0)
	{
		angle = - angle;
		//printf("%d\n\r", angle);
		tmp = 323;
		LCD_DrawString(4 , 104 , &tmp,1);//-
	}
	else
	{
		tmp = 324;
		LCD_DrawString(4 , 104 , &tmp,1);//
	}
	
	LCD_DrawString(4 , 64 , &LCD_display_char[2][0],2);//垂
	LCD_DrawString(4 , 80 , &LCD_display_char[3][0],2);//直
	tmp = 322;//:
	LCD_DrawString(4 , 96 , &tmp,1);
	
	tmp = angle / 10;
	LCD_DrawString(4 , 112 , &LCD_num[tmp],1);//vertical_angle value
	tmp = angle % 10;
	LCD_DrawString(4 , 120 , &LCD_num[tmp],1);

	

/*time display*/
	tmp = time[0]/ 10;//hour
	LCD_DrawString(6 , 48 , &LCD_num[tmp],1);
	tmp = time[0]% 10;
	LCD_DrawString(6 , 56 , &LCD_num[tmp],1);
	
	tmp = 322;//:
	LCD_DrawString(6 , 64 , &tmp,1);
	tmp = time[1]/ 10;//min
	LCD_DrawString(6 , 72 , &LCD_num[tmp],1);
	tmp = time[1]% 10;
	LCD_DrawString(6 , 80 , &LCD_num[tmp],1);
	
	tmp = 322;//:
	LCD_DrawString(6 , 88 , &tmp,1);
	tmp = time[2]/ 10;//sec
	LCD_DrawString(6 , 96 , &LCD_num[tmp],1);
	tmp = time[2]% 10;
	LCD_DrawString(6 , 104 , &LCD_num[tmp],1);
}

void vector2vector(uint16_t *vector1,uint16_t *vector2)
{
	for (int i = 0; i < 16; i++) 
    {
        vector2[i] = vector1[i];
    }
}

void count_time()
{
	time[2]++; // 增加秒數
	if (time[2] == 60) 
	{
		time[2] = 0; // 重置秒數
		time[1]++; // 增加分鐘
		if (time[1] == 60) 
			{
				time[1] = 0; // 重置分鐘
				time[0]++; // 增加小時
				if (time[0] == 24) 
					{
						time[0] = 0; // 重置小時
				}
		}
	}
}

void timer_scaler(void)
{
	timer_Scaler_1000ms_cnt++;
	timer_Scaler_1000ms_cnt = ((timer_Scaler_1000ms_cnt == 5)? 0:timer_Scaler_1000ms_cnt);
}



void read_adc_value()
{
	for(i=0; i<5; i++)
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,0xffff);
		adc_input[i]=HAL_ADC_GetValue(&hadc1);
	}
	HAL_ADC_Stop(&hadc1);
}

void uart_trans(int num)
{
	int h_angle = horizontal_angle - 90;
	int v_angle = 90 - vertical_angle;
	if(num == 0)
	{
		printf("\rlight chaser OPED!!\n\r");
	}
	else if(num == 1)
	{
		printf("\nat systeem time %02d:%02d:%02d\n\r", time[0], time[1], time[2]);
		printf("horizontal angle is %d\n\r", h_angle);
		printf("vertical angle is %d\n\r", v_angle);
	}
}

void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f1xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
