/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************

  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpu.h"
#include "GameEngine.h"
#include "gamepad.h"
#include "touchpad.h"
#include "gyroscope.h"
#include "auxiliary.h"
#include "Scheduler.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "background.h"
#include "angryBird.h"
#include "bricks_bitmap.h"
#include "paddle.h"
#include "paddle_rot_90.h"
#include "ball.h"

#include "printf.h"

#define SCREEN_W         240
#define SCREEN_H         320
#define SCREEN_W_RO      320
#define SCREEN_H_RO      240
#define BIRD_WIDTH       50
#define BIRD_HEIGHT      50
#define ONEBRICK_WIDHT   40
#define ONEBRICK_HEIGHT  16
#define BRICKS_WIDTH     6
#define BRICKS_HEIGHT    5
#define PADDLE_WIDTH     60
#define PADDLE_HEIGHT    16
#define PADDLE_WIDTH_RO  80
#define PADDLE_HEIGHT_RO 12
#define BALL_WIDTH       12
#define BALL_HEIGHT      12
#define DT               (0.001f)
#define PADDLE_CENTRE     SCREEN_W/2 - PADDLE_WIDTH/2
#define RAND()    (HAL_RNG_GetRandomNumber(&hrng))

typedef enum
{
	GAME_STATE_INIT ,
	GAME_STATE_LEVEL ,
	GAME_STATE_RUN ,
	GAME_STATE_END
}GameState;

RNG_HandleTypeDef hrng;
GameState state = GAME_STATE_INIT  ;
Sprite BackGround ;
Sprite bricks[BRICKS_WIDTH*BRICKS_HEIGHT];
Sprite Paddle;
Sprite Ball;

GAMEPAD_BUTTON GamePad;
TC_Panel tc ;

float gyro_rd[3] ;
float angle_x , angle_y,angle_z;
float x , y , z ;
int hits ;
int score;
char strbuf[131];

float X0 , Y0 , Z0 ;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_RNG_Init(void);

void Init_backGround(Sprite *spr);
void Init_bird(Sprite *spr);
void InitBricks(Sprite *bricks);
void Init_Paddle(Sprite *paddle);
void Init_Ball(Sprite *ball);
void Control_Ball_Move(Sprite * ball, GAMEPAD_BUTTON *GamePad);
void Control_Paddle_Move(Sprite * paddle ,Sprite * ball ,  GAMEPAD_BUTTON *GamePad);
void DrawBricks(Sprite *bricks);
void Control_collsion(void);
void game_main(void);
void Read_inputs(void) ;
void Calculate_Angle(void);

void Gyro_claibrated_read(void);
void toggle_led(void)
{
	GPIOG->ODR ^=1<<13 ;
}
 /* @brief  The application entry point.
  * @retval int
  */
void gyro_calibaration(void)
{

	for(int i= 0 ; i < 500 ; i++)
	{
		HAL_Delay(1);
		GYRO_GetXYZ(gyro_rd);
		X0 += gyro_rd[0] ;
		Y0 += gyro_rd[1] ;
		Z0 += gyro_rd[2] ;
	}
	X0 /= 500.0  ;
	Y0 /= 500.0 ;
	Z0 /= 500.0 ;
}
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_RNG_Init() ;

  InitDisplay((uint32_t)image_background);

  GamePad_Init();
  touchpad_init();
  GYRO_Init();
  GYRO_Reset();
  gyro_calibaration();

  //Assign_smallFun(toggle_led) ; // callback function to GPU core.

  Add_Task(Calculate_Angle , INTERVAL_10MS) ;
  Add_Task(Read_inputs , INTERVAL_50MS);
  Add_Task(game_main, MAIN_INTERVAL);
  while (1)
  {
	  Run_Scheduler() ;
  }//while
}//main

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
   RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 180;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 3;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
    /** Activate the Over-Drive mode
    */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
      Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
      Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 60;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
}



static void MX_RNG_Init(void)
{
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }

}

void game_main(void)
{

	switch(state)
	{
	case GAME_STATE_INIT :

		/*
		 * At this state init background , Paddle , ball.
		 */
		Init_backGround(&BackGround);
		Init_Paddle(&Paddle);
		Init_Ball(&Ball);

		//Transition to level selection state
		state = GAME_STATE_LEVEL ;
		break ;

	case GAME_STATE_LEVEL:

		/*
		 * At this state we go for init the game level
		 */
		InitBricks(bricks);

		//zeroing hits
		hits = 0 ;
		//Transition to level selection state
		state = GAME_STATE_RUN ;
		break ;

	case GAME_STATE_RUN :

		/*
		 * The core of the game logic goes here.
		 */
	if(Is_Render_cplt())
	{
		Render_flush();
		DrawSprite(&BackGround , ROT_ZERO);
		Control_Paddle_Move(&Paddle,&Ball, &GamePad);
		Control_Ball_Move(&Ball, &GamePad);
		Control_collsion();
		DrawSprite(&Paddle,ROT_90);
		DrawSprite(&Ball,ROT_90);
		DrawBricks(bricks);
		if(GamePad.state == GP_NOT_CONNECTED)
			Draw_String_fast(20, 160, "GamePad not connected!!");

		snprintf(strbuf,10,"Score %d" , score);
		Draw_String_fast(80, 260, strbuf);
		FlipBuffers();
	}//if
	else
	{
			//CPU can do anything here until Rendering is completed
	}
	if(  (GamePad.state == GP_CONNECTED &&GamePad.button.select == PRESSED)
				                        || hits== BRICKS_WIDTH * BRICKS_HEIGHT )

		{
				//shutdown the game
				state = GAME_STATE_END ;
		}
		break;
	case GAME_STATE_END :

		state = GAME_STATE_INIT ;

		break ;
	}//switch state

}

void Init_backGround(Sprite *spr)
{
   spr->x = 0 ;
   spr->y = 0 ;
   spr->Width =  SCREEN_W ;
   spr->Height = SCREEN_H;
   spr->ImageData = image_background ;
   spr->LayerId   = BACKGROUND ;
   spr->active =1 ;
}


void Init_Paddle(Sprite *paddle)
{

	paddle->x = SCREEN_W/2 - PADDLE_WIDTH/2 ;
	paddle->y = SCREEN_H-PADDLE_HEIGHT ;
	paddle->Width =  PADDLE_WIDTH ;
	paddle->Height = PADDLE_HEIGHT;
	paddle->ImageData = image_paddle ;
	paddle->xSpeed = 4;
	paddle->ySpeed = 0;
	paddle->LayerId   = SPRITE ;
	paddle->active = 1 ;
#if 0
	    paddle->x =SCREEN_W_RO/2 - PADDLE_WIDTH_RO/2; //SCREEN_H/2 - 80/2 ;
		paddle->y =SCREEN_H_RO-PADDLE_HEIGHT_RO; //SCREEN_W - 12 ;
		paddle->Width =  PADDLE_WIDTH_RO ;
		paddle->Height = PADDLE_HEIGHT_RO;
		paddle->ImageData = image_paddle_rot_90 ;
		paddle->xSpeed = 4;
		paddle->ySpeed = 0;
		paddle->LayerId   = SPRITE ;
		paddle->active = 1 ;
#endif
}

void Control_Paddle_Move(Sprite * paddle ,Sprite * ball ,  GAMEPAD_BUTTON *GamePad)
{
	if(GamePad->state == GP_CONNECTED &&
			GamePad->button.right == PRESSED)
	{
		paddle->x += paddle->xSpeed ;

		if(ball->ySpeed == 0)
			ball->x += paddle->xSpeed ;
	}
	else if(GamePad->state == GP_CONNECTED &&
			      GamePad->button.left == PRESSED)
	{
		  paddle->x -= paddle->xSpeed ;
		  if(ball->ySpeed == 0)
		  			ball->x -= paddle->xSpeed ;
	}


	if(ball->ySpeed)
	    paddle->x = PADDLE_CENTRE+ y*3 ;


	if(tc.state == TC_PRESSED)
	{
		paddle->x = tc.x ;
	}

	//Keep paddle in the screen X values
	if(paddle->x > (SCREEN_W - PADDLE_WIDTH))
	{
		paddle->x = SCREEN_W - PADDLE_WIDTH ;
	}

	if(paddle->x < 0)
	{
		paddle->x = 0 ;
	}
}

void Control_Ball_Move(Sprite * ball, GAMEPAD_BUTTON *GamePad)
{
	if(ball->ySpeed == 0 &&
			(( GamePad->button.start  == PRESSED && GamePad->state == GP_CONNECTED ) || tc.state == TC_PRESSED) )
	{
		// start to move the ball todo : slope to get xspeed , yspeed
		ball->xSpeed = 3 ;
		ball->ySpeed = -2 ;
	}
	ball->x += ball->xSpeed ;
	ball->y += ball->ySpeed ;

	//Keep the ball in the screen boundaries.

	if(ball->x > (SCREEN_W - ball->Width) || ball->x < 0)
	{
		ball->xSpeed = 0- ball->xSpeed ;
		ball->x += ball->xSpeed ;
	}

	if(ball ->y > (SCREEN_H - ball->Height) || ball->y < 0)
	{
		if(ball ->y > (SCREEN_H - ball->Height)) score -= 20 ;
		ball->ySpeed = 0- ball->ySpeed ;
		ball->y += ball->ySpeed ;


		//you loose Todo: score--

	}
}


void Control_collsion(void)
{
   // Check ball collision with bricks

	for(int i = 0 ; i <BRICKS_WIDTH * BRICKS_HEIGHT ; i++)
		{
		  if(collision(&Ball , &bricks[i]) &&bricks[i].active)
		  {
			  //destroy this brick
			  bricks[i].active = 0;

			  //ball action
			  Ball.ySpeed = -Ball.ySpeed ; //bounce the ball
			  Ball.xSpeed += (-1 +RAND()%3 ) ;

			  //increase score
			  hits++; ;
			  score += 10;
		  }//if
		} //for

	if(collision(&Ball, &Paddle) && Ball.ySpeed)
			{
		 //ball action
			     Ball.ySpeed = -Ball.ySpeed ; //bounce the ball
				 Ball.xSpeed += (-1 +RAND()%3 ) ;
			}
}

void Init_Ball(Sprite *ball)
{

	ball->x = SCREEN_W/2 - BALL_WIDTH/2 ;
    ball->y = SCREEN_H-(PADDLE_HEIGHT + BALL_HEIGHT) ;
	ball->Width =  BALL_WIDTH ;
	ball->Height = BALL_HEIGHT;
	ball->xSpeed  = 0 ;
	ball->ySpeed  = 0 ;
	ball->ImageData = image_ball ;
	ball->LayerId   = SPRITE ;
	ball->active =1;

#if 0
	    ball->x = 0;//SCREEN_H/2 - BALL_WIDTH/2 ;
	    ball->y =0; //SCREEN_W-(12 + BALL_HEIGHT) ;
		ball->Width =  BALL_WIDTH ;
		ball->Height = BALL_HEIGHT;
		ball->xSpeed  = 0 ;
		ball->ySpeed  = 0 ;
		ball->ImageData = image_ball ;
		ball->LayerId   = SPRITE ;
		ball->active =1;
#endif
}

void InitBricks(Sprite *bricks)
{
	for(int i = 0 ; i <BRICKS_WIDTH * BRICKS_HEIGHT ; i++)
	{
		   bricks[i].x = ONEBRICK_WIDHT * (i%BRICKS_WIDTH) ;
		   bricks[i].y = ONEBRICK_HEIGHT *(i%BRICKS_HEIGHT) ;
		   bricks[i].ImageData = image_Bricks[RAND()%6] ;
		   bricks[i].Width = ONEBRICK_WIDHT ;
		   bricks[i].Height = ONEBRICK_HEIGHT ;
		   bricks[i].LayerId = SPRITE ;
		   bricks[i].active = 1 ;
	}
}

void DrawBricks(Sprite *bricks)
{
	for(int i = 0 ; i <BRICKS_WIDTH * BRICKS_HEIGHT ; i++)
		{
		  DrawSprite(&bricks[i],ROT_ZERO) ;
		}
}

void Calculate_Angle(void)
{
	//GYRO_GetXYZ(gyro_rd);
	Gyro_claibrated_read();
	angle_x += gyro_rd[0] *DT ;
	angle_y += gyro_rd[1] *DT ;
	angle_z += gyro_rd[2] *DT ;
}
void Read_inputs(void)
{
	GamePad = GamePad_Read();
	tc =touchpad_read() ;
	//GYRO_GetXYZ(gyro_rd);
	y = angle_y * 180.0f  / 17600.0f ;
}
void Gyro_claibrated_read(void)
{
	GYRO_GetXYZ(gyro_rd);
	gyro_rd[0] -= X0 ;
	gyro_rd[1] -= Y0 ;
	gyro_rd[2] -= Z0 ;
}

void Small_tasks(Display *dis)
{
	GPIOG->ODR ^=1;
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
