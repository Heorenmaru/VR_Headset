/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define L13_Pin GPIO_PIN_13
#define L13_GPIO_Port GPIOC
#define INT_Pin GPIO_PIN_0
#define INT_GPIO_Port GPIOA
#define VDD3_3_Pin GPIO_PIN_1
#define VDD3_3_GPIO_Port GPIOA
#define USB___Pin GPIO_PIN_2
#define USB___GPIO_Port GPIOA
#define VDD3v3d_Pin GPIO_PIN_3
#define VDD3v3d_GPIO_Port GPIOA
#define VDD1v2_Pin GPIO_PIN_4
#define VDD1v2_GPIO_Port GPIOA
#define L2_Pin GPIO_PIN_5
#define L2_GPIO_Port GPIOA
#define L1_Pin GPIO_PIN_6
#define L1_GPIO_Port GPIOA
#define VN5_Pin GPIO_PIN_10
#define VN5_GPIO_Port GPIOB
#define D_IO3v3_Pin GPIO_PIN_11
#define D_IO3v3_GPIO_Port GPIOB
#define C_IO3v3_Pin GPIO_PIN_12
#define C_IO3v3_GPIO_Port GPIOB
#define BTN1_Pin GPIO_PIN_14
#define BTN1_GPIO_Port GPIOB
#define BTN2_Pin GPIO_PIN_15
#define BTN2_GPIO_Port GPIOB
#define VDD1v8d_Pin GPIO_PIN_9
#define VDD1v8d_GPIO_Port GPIOA
#define VDD1v1_Pin GPIO_PIN_10
#define VDD1v1_GPIO_Port GPIOA
#define RST_Pin GPIO_PIN_15
#define RST_GPIO_Port GPIOA
#define BL_Pin GPIO_PIN_6
#define BL_GPIO_Port GPIOB
#define B_IO1v8_Pin GPIO_PIN_7
#define B_IO1v8_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
