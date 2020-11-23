/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "main.h"
#include "mcu_init.h"

#include "tensorflow/lite/micro/examples/person_detection_experimental/main_functions.h"

#include "cmsis_os.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "image.h"
#include "LCD_Test.h"
#include "LCD_2inch4.h"
#include "delay.h"
#include "sys.h"
#include "ov2640.h"

#define OV2640_PIXEL_WIDTH  ((uint16_t)96)
#define OV2640_PIXEL_HEIGHT ((uint16_t)96)
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TASK1_STK_SIZE		10240
#define TASK2_STK_SIZE		256
//#define TASK1_STK_SIZE		256
//#define TASK2_STK_SIZE		256
#define LED_Pin           GPIO_PIN_14 
#define LED_GPIO_Port     GPIOB
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t camBuffer[OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT];
uint8_t modelBuffer[OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT];
uint8_t frame_flag = 0;
uint8_t tensor_flag = 0;
extern DCMI_HandleTypeDef hdcmi;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t RGB565toGRAY(uint16_t bg_color)
{
    uint8_t bg_r = 0;
    uint8_t bg_g = 0;
    uint8_t bg_b = 0;
    bg_r = ((bg_color>>11)&0xff)<<3;
    bg_g = ((bg_color>>5)&0x3f)<<2;
    bg_b = (bg_color&0x1f)<<2;
    uint8_t gray = (bg_r*299 + bg_g*587 + bg_b*114 + 500) / 1000;
    return gray;
}

void Input_Convert(uint16_t* camera_buffer , uint8_t* model_buffer)
{
	for(int i=0 ; i<OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT ; i++)
	{
		model_buffer[i] = RGB565toGRAY(camera_buffer[i]);
	}
	printf("resize over\n");
}

void task2(void *pdata);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);
void task1(void *pdata);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void task1(void *pdata)
{
	  int res = 0;
    while(1)
    {
        //printf("\r\nHello world!\r\n###This is task1 ,count is %d \r\n", count++);
        //HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
        //osDelay(2000)
		if(frame_flag == 1)
		{
			if(HAL_DCMI_Stop(&hdcmi))
				Error_Handler();
			
			Input_Convert(camBuffer , modelBuffer);
			res = loop();
			LCD_2IN4_Display(camBuffer,OV2640_PIXEL_WIDTH,OV2640_PIXEL_HEIGHT);
			if(HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS,  (uint32_t)camBuffer , (OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT)/2))
				Error_Handler();
			frame_flag = 0;
			if(res != 0)
			{
				HAL_GPIO_WritePin(GPIOB, LCD_DC_Pin|LED_Pin, GPIO_PIN_SET);
				delay_ms(50);
				HAL_GPIO_WritePin(GPIOB, LCD_DC_Pin|LED_Pin, GPIO_PIN_RESET);
			}
			//printf("display a picture\n");
//			if(tensor_flag == 1)
//			{
//				frame_flag = 0;
//				tensor_flag = 0;
//			}
		}
		osDelay(50);
		//tos_task_delay(2000);
    }
}

void task2(void *pdata)
{
    while(1)
    {
        //printf("\r\nHello TencentOS !\r\n***This is task2 ,count is %d \r\n", count++);
        //osDelay(1000);
			if(frame_flag == 1)
			{
				loop();
				tensor_flag = 1;
			}
			//HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
			osDelay(50);
			
		//tos_task_delay(1000);
		//printf("frame_flag : %d \r\n",frame_flag);
				
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */
		int i = 0;
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
	board_init();
	/* USER CODE BEGIN 2 */
	LCD_2IN4_Init();
	OV2640_Init();
	OV2640_RGB565_Mode();	
	OV2640_OutSize_Set(OV2640_PIXEL_WIDTH,OV2640_PIXEL_HEIGHT); 
	OV2640_Special_Effects(0); //特效设置,0正常
	
	__HAL_DCMI_DISABLE_IT(&hdcmi, DCMI_IT_LINE | DCMI_IT_VSYNC);
	if (HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS,  (uint32_t)camBuffer , (OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT)/2))
	{
	Error_Handler();
	}
	setup(); //tensorflow用例初始化
	
	osKernelInitialize(); //TOS Tiny kernel initialize
	osThreadCreate(osThread(task1), NULL);// Create task1
	//osThreadCreate(osThread(task2), NULL);// Create task2
	osKernelStart();//Start TOS Tiny
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

/* USER CODE BEGIN 4 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	//printf("HAL_DCMI_FrameEventCallback\r\n");
	
	if(hdcmi->State == 2 && frame_flag != 1){
		//printf("state: %d \r\n",hdcmi->State);
		frame_flag = 1;
	}
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
	printf("HAL error!!!\r\n");
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

//-------------------------------------------------------------------

// This is the default main used on systems that have the standard C entry
// point. Other devices (for example FreeRTOS or ESP32) that have different
// requirements for entry code (like an app_main function) should specialize
// this main.cc file in a target-specific subfolder.
//int main(int argc, char* argv[]) {
//  HAL_Init();
//  SystemClock_Config();
//  board_init();
//  setup();
//  while (true) {
//	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
//	delay_ms(1000);
//    loop();
//  }
//}
