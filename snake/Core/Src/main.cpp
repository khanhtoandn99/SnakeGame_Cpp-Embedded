/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "STM32_sh1106_oled.h"
#include "fonts.h"
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
I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN PV */
int Last_Input_Key = 14 ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */

void SH1106GotoXY(int _x, int _y){
	int x = _x*6 ;
	int y = _y*8 ;
	if(x <= 0) x = 1 ;
	if(y <= 0) y = 1 ;
	SH1106_GotoXY(x, y) ;
}

class Point {
private:
	int x, y;
public:
	Point();

	Point(int x, int y);

	void SetPoint(int x, int y);

	int GetX();

	int GetY();

	void MoveUp();

	void MoveDown();

	void MoveLeft();

	void MoveRight();

	void Draw();

	void Erase();

	void CopyPos(Point* p);

	int IsEqual(Point* p);

	void Debug();
};


class Snake {
private:
	Point* cell[MAX_SNAKE_SIZE];  // array of points
	int size;
	int dir; // current direction of snake
	Point fruit;
	int state; // bool show state of snake i.e. living or dead
	int started;
public:
	Snake();

	void AddCell(int x, int y);

	void TurnUp();

	void TurnDown();

	void TurnLeft();

	void TurnRight();

	void WelcomeScreen();

	void Move();

	int SelfCollision();

	void Debug();
};


Point::Point(){
		x = y = 10;
	}

Point::Point(int x, int y) {
		this->x = x;
		this->y = y;
	}

void Point::SetPoint(int x, int y) {
		this->x = x;
		this->y = y;
	}

int Point::GetX() {
		return x;
	}

int Point::GetY() {
		return y;
	}

void Point::MoveUp() {
		y--;
		if (y < 0) y = MAX_FRAME_Y;
	}

void Point::MoveDown() {
		y++;
		if (y > MAX_FRAME_Y) y = 0;
	}

void Point::MoveLeft() {
		x--;
		if (x < 0) x = MAX_FRAME_X;
	}

void Point::MoveRight() {
		x++;
		if (x > MAX_FRAME_X) x = 0;
	}

void Point::Draw() {
		char Shape = 'o';
		SH1106GotoXY(x, y);
		SH1106_Putc ((char)Shape, &Font_5x7, SH1106_COLOR_WHITE);
		SH1106_UpdateScreen(); //display
	}

void Point::Erase() {
		SH1106GotoXY(x, y);
		SH1106_Putc ((char)' ', &Font_5x7, SH1106_COLOR_WHITE);
		SH1106_UpdateScreen(); //display
	}

void Point::CopyPos(Point* p) {
		p->x = x;
		p->y = y;
	}

int Point::IsEqual(Point* p) {
		if (p->x == x && p->y == y) return 1;
		return 0;
	}

void Point::Debug() {
//		cout << "(" << x << "," << y << ") ";
//		SH1106_Puts ((char*)' ', &Font_5x7, 1);
	}




Snake::Snake() {
		size = 1; // default size
		cell[0] = new Point(0, 0); // default position at
		for (int i = 1; i < MAX_SNAKE_SIZE; i++) {
			cell[i] = NULL;
		}
		fruit.SetPoint(rand() % MAX_FRAME_X, rand() % MAX_FRAME_Y);
		state = 0;
		started = 0;
	}

void Snake::AddCell(int x, int y) {
		cell[size++] = new Point(x, y);
	}

void Snake::TurnUp() {
		if(dir != Key_Left_Down)
		dir = Key_Left_Up;
	}

void Snake::TurnDown() {
		if (dir != Key_Left_Up)
		dir = Key_Left_Down;
	}

void Snake::TurnLeft() {
		if (dir != Key_Left_Right)
		dir = Key_Left_Left;
	}

void Snake::TurnRight() {
		if (dir != Key_Left_Left)
		dir = Key_Left_Right;
	}

void Snake::WelcomeScreen() {
		SH1106GotoXY(4, 0) ;
		SH1106_Puts((char*)"| SNAKE GAME |", &Font_5x7, SH1106_COLOR_WHITE) ;
		SH1106GotoXY(4, 1) ;
		SH1106_Puts((char*)"-------------", &Font_5x7, SH1106_COLOR_WHITE) ;
		SH1106_UpdateScreen(); //display
	}

void Snake::Move() {
//		SH1106_Clear();
		if (state == 0) {
			if (!started) {
				SH1106_Clear();
				WelcomeScreen();
				SH1106GotoXY(3, 3) ;
				SH1106_Puts((char*)"PRESS OK TO START ", &Font_5x7, SH1106_COLOR_WHITE) ;
				SH1106_UpdateScreen(); //display
				while(1){
					if(HAL_GPIO_ReadPin(BUTTON_RIGHT_LEFT_GPIO_Port, BUTTON_RIGHT_LEFT_Pin) == 0){
						HAL_Delay(200) ;
						SH1106_Clear();
						break;
					}
				}
				started = 1;
				state = 1;
				size = 1;
			}
			else {
				SH1106_Clear();
				SH1106GotoXY(6, 2) ;
				SH1106_Puts((char*)" GAME OVER", &Font_5x7, SH1106_COLOR_WHITE) ;
				SH1106GotoXY(2, 4) ;
				SH1106_Puts((char*)"PRESS OK TO RESTART ", &Font_5x7, SH1106_COLOR_WHITE) ;
				SH1106_UpdateScreen(); //display
				while(1){
					if(HAL_GPIO_ReadPin(BUTTON_RIGHT_LEFT_GPIO_Port, BUTTON_RIGHT_LEFT_Pin) == 0){
						HAL_Delay(200) ;
						SH1106_Clear();
						break;
					}
				}
				state = 1;
				size = 1;
			}
		}

		// making snake body folow its head
		for (int i = size - 1; i > 0; i--) {
			cell[i-1]->CopyPos(cell[i]);
 		}

		// turning snake's head
		switch (dir){
		case Key_Left_Up :
			cell[0] -> MoveUp();
			break;
		case Key_Left_Down :
			cell[0]->MoveDown();
			break;
		case Key_Left_Left:
			cell[0]-> MoveLeft();
			break;
		case Key_Left_Right:
			cell[0]->MoveRight();
			break;
		}

		if (SelfCollision()) state = 0;

		if (fruit.GetX() == cell[0]->GetX() && fruit.GetY() == cell[0]->GetY()) {
			AddCell(0, 0);
			fruit.SetPoint(rand() % MAX_FRAME_X, rand() % MAX_FRAME_Y);
		}

		// Toan da chinh sua doan code nay : ( muc dich trach oled in lai toan bo than ran -> mat thoi gian hien thi)
//		for (int i = 0; i < size; i++) {
//			cell[i]->Draw();
//		}
		cell[0] -> Draw() ;    // Toan giai quyet
		cell[size-1] ->Erase() ;
		// ---- xong -----

		fruit.Draw();
		//Debug();
		SH1106_UpdateScreen(); //display
		HAL_Delay((int)(10/size)) ;
	}

int Snake::SelfCollision() {
		for (int i = 1; i < size; i++)
			if (cell[0]->IsEqual(cell[i])) return 1;
		return 0;

	}

void Snake::Debug() {
		for (int i = 0; i < size; i++) {
			cell[i]->Debug();
		}
	}


int InputKey(){
	int Input_Key ;
	if ( Last_Input_Key == Key_Right_Up){
		Input_Key = Key_Left_Right ;
		Last_Input_Key = Key_Left_Right ;
	}
	Input_Key = Last_Input_Key;
	if(HAL_GPIO_ReadPin(BUTTON_LEFT_UP_GPIO_Port, BUTTON_LEFT_UP_Pin) == 0){
	  Input_Key = Key_Left_Up ;
	  Last_Input_Key = Key_Left_Up ;
	}else if (HAL_GPIO_ReadPin(BUTTON_LEFT_DOWN_GPIO_Port, BUTTON_LEFT_DOWN_Pin) == 0){
	  Input_Key = Key_Left_Down ;
	  Last_Input_Key = Key_Left_Down ;
	}else if (HAL_GPIO_ReadPin(BUTTON_LEFT_LEFT_GPIO_Port, BUTTON_LEFT_LEFT_Pin) == 0){
	  Input_Key = Key_Left_Left ;
	  Last_Input_Key = Key_Left_Left ;
	}else if (HAL_GPIO_ReadPin(BUTTON_LEFT_RIGHT_GPIO_Port, BUTTON_LEFT_RIGHT_Pin) == 0){
	  Input_Key = Key_Left_Right ;
	  Last_Input_Key = Key_Left_Right ;
	}else if (HAL_GPIO_ReadPin(BUTTON_RIGHT_UP_GPIO_Port, BUTTON_RIGHT_UP_Pin) == 0){
	  Input_Key = Key_Right_Up ;
	  Last_Input_Key = Key_Right_Up ;
	}else if (HAL_GPIO_ReadPin(BUTTON_RIGHT_DOWN_GPIO_Port, BUTTON_RIGHT_DOWN_Pin) == 0){
	  Input_Key = Key_Right_Down ;
	  Last_Input_Key = Key_Right_Down ;
	}else if (HAL_GPIO_ReadPin(BUTTON_RIGHT_LEFT_GPIO_Port, BUTTON_RIGHT_LEFT_Pin) == 0){
	  Input_Key = Key_Right_Left ;
	  Last_Input_Key = Key_Right_Left ;
	}else if (HAL_GPIO_ReadPin(BUTTON_RIGHT_RIGHT_GPIO_Port, BUTTON_RIGHT_RIGHT_Pin) == 0){
	  Input_Key = Key_Right_Right ;
	  Last_Input_Key = Key_Right_Right ;
	}
	return Input_Key ;
}


void SnakeMainControl(){
	// random no generation
	srand((unsigned)time(NULL));


	Snake snake;
	int op = Key_Left_Right;
	do {
		op = InputKey();
		switch (op) {
			case Key_Left_Up:
				snake.TurnUp();
				break;
			case Key_Left_Down:
				snake.TurnDown();
				break;
			case Key_Left_Left:
				snake.TurnLeft();
				break;
			case Key_Left_Right:
				snake.TurnRight();
				break;
		}
		snake.Move();
	} while (op != Key_Right_Up );

}


void SH1106TestButton(){
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0){
	  SH1106GotoXY (0,0);
	  SH1106_Puts ((char*)"A4", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0){
	  SH1106GotoXY (0,0);
	  SH1106_Puts ((char*)"A5", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0){
	  SH1106GotoXY (0,0);
	  SH1106_Puts ((char*)"A6", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 0){
	  SH1106GotoXY (0,0);
	  SH1106_Puts ((char*)"A7", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0){
	  SH1106GotoXY (0,0);
	  SH1106_Puts ((char*)"A0", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 0){
	  SH1106GotoXY (0,0);
	  SH1106_Puts ((char*)"A1", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == 0){
	  SH1106GotoXY (0,0);
	  SH1106_Puts ((char*)"A2", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == 0){
	  SH1106GotoXY (0,0);
	  SH1106_Puts ((char*)"A3", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	}
}
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

  /* MCU Configuration------------------------------------`--------------------*/

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
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  SH1106_Init();
  HAL_Delay(1000) ;
  SH1106_Clear() ;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	  SH1106_Init();  // initialise

	  SH1106_Clear() ;
	  SH1106GotoXY (0,0);
	  SH1106_Puts ((char*)"        Hi world !", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106GotoXY (0,1);
	  SH1106_Puts ((char*)" ", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106GotoXY (0,2);
	  SH1106_Puts ((char*)"This game project was", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106GotoXY (0,3);
	  SH1106_Puts ((char*)"developed by Toan Dep", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106GotoXY (0,4);
	  SH1106_Puts ((char*)"Chai, using STM32 MCU ", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106GotoXY (0,5);
	  SH1106_Puts ((char*)"and Oled Display.", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106GotoXY (0,6);
	  SH1106_Puts ((char*)"----------------------", &Font_5x7, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen(); //display
	  HAL_Delay(3000) ;

	  char C = 0 ;
	  while(1){
		  if(HAL_GPIO_ReadPin(BUTTON_RIGHT_LEFT_GPIO_Port, BUTTON_RIGHT_LEFT_Pin) == 0){
			  HAL_Delay(200) ;
			  SH1106_Clear();
			  break;
		  }
	  }
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13) ;
	  HAL_Delay(100) ;
	  SnakeMainControl();
//	  if(HAL_GPIO_ReadPin(BUTTON_RIGHT_DOWN_GPIO_Port, BUTTON_RIGHT_DOWN_Pin) == 0){
//		  HAL_Delay(100);
//		  SH1106GotoXY (5,5);
//		  SH1106_Putc ((char)C, &Font_5x7, SH1106_COLOR_WHITE);
//		  C = C + 1 ;
//		  if( C >= 255) C = 0 ;
//		  SH1106_UpdateScreen(); //display
//	  }

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

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_LEFT_UP_Pin BUTTON_LEFT_LEFT_Pin BUTTON_LEFT_RIGHT_Pin BUTTON_LEFT_DOWN_Pin
                           BUTTON_RIGHT_DOWN_Pin BUTTON_RIGHT_LEFT_Pin BUTTON_RIGHT_RIGHT_Pin BUTTON_RIGHT_UP_Pin */
  GPIO_InitStruct.Pin = BUTTON_LEFT_UP_Pin|BUTTON_LEFT_LEFT_Pin|BUTTON_LEFT_RIGHT_Pin|BUTTON_LEFT_DOWN_Pin
                          |BUTTON_RIGHT_DOWN_Pin|BUTTON_RIGHT_LEFT_Pin|BUTTON_RIGHT_RIGHT_Pin|BUTTON_RIGHT_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
