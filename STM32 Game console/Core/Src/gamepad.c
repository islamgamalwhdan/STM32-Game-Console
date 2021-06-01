
//Includes
#include "gamepad.h"
#include "stm32f4xx_hal.h"

//Constants

#define GAMEPAD_DATAIN0 GPIO_PIN_7
#define GAMEPAD_SH_LD   GPIO_PIN_3
#define GAMEPAD_CLK     GPIO_PIN_4
#define GAMEPAD_PORT    GPIOB

//Functions definitions 

void GamePad_Init(void) 
{
  GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.Pin = GAMEPAD_SH_LD|GAMEPAD_CLK;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH   ;
  HAL_GPIO_Init(GAMEPAD_PORT, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = GAMEPAD_DATAIN0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GAMEPAD_PORT, &GPIO_InitStruct);
	
	GAMEPAD_PORT->ODR = 0 ;
	
}

GAMEPAD_BUTTON GamePad_Read(void) 
{
	GAMEPAD_BUTTON GamePad ;

	HAL_GPIO_WritePin(GAMEPAD_PORT , GAMEPAD_CLK , GPIO_PIN_RESET) ;
	HAL_GPIO_WritePin(GAMEPAD_PORT , GAMEPAD_SH_LD , GPIO_PIN_RESET) ;
	
	HAL_GPIO_WritePin(GAMEPAD_PORT , GAMEPAD_SH_LD , GPIO_PIN_SET) ;
	HAL_GPIO_WritePin(GAMEPAD_PORT , GAMEPAD_SH_LD , GPIO_PIN_RESET) ;
	for ( int i = 0 ;  i < 8 ; i++)
	{
		GamePad.value = (GamePad.value <<1)| HAL_GPIO_ReadPin(GAMEPAD_PORT , GAMEPAD_DATAIN0) ;

		//clock
		HAL_GPIO_WritePin(GAMEPAD_PORT , GAMEPAD_CLK  , GPIO_PIN_SET)   ;
		HAL_GPIO_WritePin(GAMEPAD_PORT , GAMEPAD_CLK  , GPIO_PIN_RESET) ;
	}
	//rd_value.value = ~ rd_value.value ;
	GamePad.state = GamePad.value ? GP_CONNECTED : GP_NOT_CONNECTED ;
	return GamePad ;
}
