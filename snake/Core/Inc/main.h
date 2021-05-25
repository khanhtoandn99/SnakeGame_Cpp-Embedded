/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <time.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define MAX_SNAKE_SIZE 100
#define MAX_FRAME_X 20
#define MAX_FRAME_Y 7

#define Key_Left_Up 	11
#define Key_Left_Down 	12
#define Key_Left_Left 	13
#define Key_Left_Right  14

#define Key_Right_Up 	21
#define Key_Right_Down  22
#define Key_Right_Left 	23
#define Key_Right_Right 24
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUTTON_LEFT_UP_Pin GPIO_PIN_0
#define BUTTON_LEFT_UP_GPIO_Port GPIOA
#define BUTTON_LEFT_LEFT_Pin GPIO_PIN_1
#define BUTTON_LEFT_LEFT_GPIO_Port GPIOA
#define BUTTON_LEFT_RIGHT_Pin GPIO_PIN_2
#define BUTTON_LEFT_RIGHT_GPIO_Port GPIOA
#define BUTTON_LEFT_DOWN_Pin GPIO_PIN_3
#define BUTTON_LEFT_DOWN_GPIO_Port GPIOA
#define BUTTON_RIGHT_DOWN_Pin GPIO_PIN_4
#define BUTTON_RIGHT_DOWN_GPIO_Port GPIOA
#define BUTTON_RIGHT_LEFT_Pin GPIO_PIN_5
#define BUTTON_RIGHT_LEFT_GPIO_Port GPIOA
#define BUTTON_RIGHT_RIGHT_Pin GPIO_PIN_6
#define BUTTON_RIGHT_RIGHT_GPIO_Port GPIOA
#define BUTTON_RIGHT_UP_Pin GPIO_PIN_7
#define BUTTON_RIGHT_UP_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
