/*
 * gpu.c
 *
 *  Created on: Apr 14, 2021
 *      Author: Islam
 */
#include "gpu.h"
#include "auxiliary.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f429i_discovery_sdram.h"
#include "ili9341.h"
#include "font8x8_basic.h"
#include "font_8x8_bitmap.h"

#define ENABLE_Pin GPIO_PIN_10
#define ENABLE_GPIO_Port GPIOF
#define B1_Pin GPIO_PIN_0
#define B1_GPIO_Port GPIOA
#define B5_Pin GPIO_PIN_3
#define B5_GPIO_Port GPIOA
#define VSYNC_Pin GPIO_PIN_4
#define VSYNC_GPIO_Port GPIOA
#define G2_Pin GPIO_PIN_6
#define G2_GPIO_Port GPIOA
#define R3_Pin GPIO_PIN_0
#define R3_GPIO_Port GPIOB
#define R6_Pin GPIO_PIN_1
#define R6_GPIO_Port GPIOB
#define A6_Pin GPIO_PIN_12
#define A6_GPIO_Port GPIOF
#define A7_Pin GPIO_PIN_13
#define A7_GPIO_Port GPIOF
#define A8_Pin GPIO_PIN_14
#define A8_GPIO_Port GPIOF
#define A9_Pin GPIO_PIN_15
#define A9_GPIO_Port GPIOF
#define A10_Pin GPIO_PIN_0
#define A10_GPIO_Port GPIOG
#define A11_Pin GPIO_PIN_1
#define A11_GPIO_Port GPIOG
#define D4_Pin GPIO_PIN_7
#define D4_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_8
#define D5_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_10
#define D7_GPIO_Port GPIOE
#define D8_Pin GPIO_PIN_11
#define D8_GPIO_Port GPIOE
#define D9_Pin GPIO_PIN_12
#define D9_GPIO_Port GPIOE
#define D10_Pin GPIO_PIN_13
#define D10_GPIO_Port GPIOE
#define D11_Pin GPIO_PIN_14
#define D11_GPIO_Port GPIOE
#define D12_Pin GPIO_PIN_15
#define D12_GPIO_Port GPIOE
#define G4_Pin GPIO_PIN_10
#define G4_GPIO_Port GPIOB
#define G5_Pin GPIO_PIN_11
#define G5_GPIO_Port GPIOB
#define D13_Pin GPIO_PIN_8
#define D13_GPIO_Port GPIOD
#define D14_Pin GPIO_PIN_9
#define D14_GPIO_Port GPIOD
#define D15_Pin GPIO_PIN_10
#define D15_GPIO_Port GPIOD
#define R7_Pin GPIO_PIN_6
#define R7_GPIO_Port GPIOG
#define DOTCLK_Pin GPIO_PIN_7
#define DOTCLK_GPIO_Port GPIOG
#define SDCLK_Pin GPIO_PIN_8
#define SDCLK_GPIO_Port GPIOG
#define HSYNC_Pin GPIO_PIN_6
#define HSYNC_GPIO_Port GPIOC
#define G6_Pin GPIO_PIN_7
#define G6_GPIO_Port GPIOC
#define R4_Pin GPIO_PIN_11
#define R4_GPIO_Port GPIOA
#define R5_Pin GPIO_PIN_12
#define R5_GPIO_Port GPIOA
#define R2_Pin GPIO_PIN_10
#define R2_GPIO_Port GPIOC
#define D2_Pin GPIO_PIN_0
#define D2_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_1
#define D3_GPIO_Port GPIOD
#define G7_Pin GPIO_PIN_3
#define G7_GPIO_Port GPIOD
#define B2_Pin GPIO_PIN_6
#define B2_GPIO_Port GPIOD
#define G3_Pin GPIO_PIN_10
#define G3_GPIO_Port GPIOG
#define B3_Pin GPIO_PIN_11
#define B3_GPIO_Port GPIOG
#define B4_Pin GPIO_PIN_12
#define B4_GPIO_Port GPIOG
#define B6_Pin GPIO_PIN_8
#define B6_GPIO_Port GPIOB
#define B7_Pin GPIO_PIN_9
#define B7_GPIO_Port GPIOB


#define SDRAM_BANK_ADDR       ((uint32_t)0xD0000000)
#define LCD_WIDTH  240
#define LCD_HEIGHT 320
#define BPP        2
#define BUFFER_SIZE            (LCD_WIDTH*LCD_HEIGHT)






//Static variables

static LTDC_HandleTypeDef hltdc;
static DMA2D_HandleTypeDef hdma2d;
static uint32_t background_img;
Display disp ;

//Private Functions
static void LTDC_Init(void);
static void DMA2D_wait(void);


void InitDisplay(uint32_t background)
{
	disp.Rendr_buf = (uint32_t)SDRAM_BANK_ADDR; ;
	disp.draw_buff = (uint32_t)(disp.Rendr_buf +1024*1024*2);
	disp.Dis_w = LCD_WIDTH ;
	disp.Dis_h = LCD_HEIGHT;
	background_img = background ;
	BSP_SDRAM_Init();
	ili9341_Init();
	LTDC_Init();
}

void Draw_image_buff(void * img , uint32_t width , uint32_t height , uint32_t buff)
{
	hdma2d.Instance = DMA2D;
	hdma2d.Init.Mode = DMA2D_M2M  ;
	hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
    hdma2d.Init.OutputOffset = disp.Dis_w - width;

	hdma2d.LayerCfg[1].InputOffset = 0;
	hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
	hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	hdma2d.LayerCfg[1].InputAlpha = 0;
	HAL_DMA2D_Init(&hdma2d);
	HAL_DMA2D_ConfigLayer(&hdma2d, 1);

	HAL_DMA2D_Start(&hdma2d, (uint32_t)img , buff , width , height) ;
	DMA2D_wait() ;
}

void Fill_color_buff(uint32_t color , uint32_t width , uint32_t height , uint32_t buff )
{
	hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_R2M  ;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
	hdma2d.Init.OutputOffset = disp.Dis_w - width;

    hdma2d.LayerCfg[1].InputOffset = 0;
	hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
	hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	hdma2d.LayerCfg[1].InputAlpha = 0;
	HAL_DMA2D_Init(&hdma2d);
	HAL_DMA2D_ConfigLayer(&hdma2d, 1);

	HAL_DMA2D_Start(&hdma2d, color , buff , width , height) ;
	DMA2D_wait() ;
}

void Draw_Pixel(uint32_t x , uint32_t y , uint32_t color)
{

	*((__IO uint16_t*) (disp.draw_buff + BPP*(x + y*disp.Dis_w))) = color ;
}

void renderChr(uint32_t x , uint32_t y , uint32_t color,char c)
{
    char *Ptable = (char *)font8x8_basic[(uint32_t)c];

    for(int i =0 ; i< CHAR_HEIGHT ; i++)
    {
        for(int j = 0 ; j < CHAR_WIDTH ; j++)
        {
           if((Ptable[i]>>j)&1)
		       Draw_Pixel(x+j,y+i,color) ;
        }
    }
}

void Draw_chr_fast(uint32_t x , uint32_t y  , char c)
{
	void *chr_bitmap = (void *)font_8x8_bitmap[(uint32_t)c] ;

	Draw_image_buff(chr_bitmap, CHAR_WIDTH , CHAR_HEIGHT , disp.draw_buff + BPP*(x + y*disp.Dis_w) ) ;
}

void renderStr(uint32_t x , uint32_t y , uint32_t color,char *str)
{
	uint32_t first_x = x ;
	while(*str)
	 {
		if( *str == '\n')
				{
			        x = first_x   ;
					y += CHAR_HEIGHT ;
					str++ ;
					continue ;
				}
        renderChr(x , y , color ,*str++) ;
        x += CHAR_WIDTH ;
	 }
}

void Draw_String_fast(uint32_t x , uint32_t y ,char *str)
{
	uint32_t first_x = x ;
	while(*str)
	{
		if( *str == '\n')
		{
			x = first_x ;
			y += CHAR_HEIGHT ;
			str++ ;
			continue ;
		}
		Draw_chr_fast(x , y , *str++) ;
		x += CHAR_WIDTH ;
	}
}
void FlipBuffers(void)
{
  uint32_t temp = disp.Rendr_buf ;

  disp.Rendr_buf = disp.draw_buff ;
  disp.draw_buff = temp ;
  LTDC_Layer2->CFBAR = disp.Rendr_buf ;

  __HAL_LTDC_ENABLE_IT(&hltdc, LTDC_IT_RR);
  LTDC->SRCR = LTDC_SRCR_VBR;  // reload shadow registers on vertical blank
}


uint32_t Is_Render_cplt(void)
{
	return (hltdc.Instance->ISR) & LTDC_ISR_RRIF ;
}

void Render_flush(void)
{
__HAL_LTDC_DISABLE_IT(&hltdc, LTDC_IT_RR);
__HAL_LTDC_CLEAR_FLAG(&hltdc, LTDC_FLAG_RR);
hltdc.State = HAL_LTDC_STATE_READY;
__HAL_UNLOCK(&hltdc);
}

static void DMA2D_wait(void)
{
	Aux * aux = Get_AuxiliaryStatus() ;
	while(HAL_DMA2D_PollForTransfer(&hdma2d, 0) == HAL_TIMEOUT)
	{
		//During waiting DMA2D to finish it's work we can do some small tasks
		if(aux->SmallTasks) aux->SmallTasks() ;
	}
}

void LTDC_Init(void)
{

  LTDC_LayerCfgTypeDef pLayerCfg  , pLayerCfg1;

  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 9;
  hltdc.Init.VerticalSync = 1;
  hltdc.Init.AccumulatedHBP = 29;
  hltdc.Init.AccumulatedVBP = 3;
  hltdc.Init.AccumulatedActiveW = 269;
  hltdc.Init.AccumulatedActiveH = 323;
  hltdc.Init.TotalWidth = 279;
  hltdc.Init.TotalHeigh = 327;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  HAL_LTDC_Init(&hltdc);

  //Background layer
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 240;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 320;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = background_img;
  pLayerCfg.ImageWidth = 240;
  pLayerCfg.ImageHeight = 320;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;

  //Foreground layer
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 240;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 320;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg1.Alpha = 255;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg1.FBStartAdress = disp.Rendr_buf;
  pLayerCfg1.ImageWidth = 240;
  pLayerCfg1.ImageHeight = 320;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;

  HAL_LTDC_ConfigColorKeying_NoReload(&hltdc, 0x52FF73, 1);
  HAL_LTDC_EnableColorKeying_NoReload(&hltdc, 1);

  HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0);

  HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1);

  __HAL_LTDC_ENABLE_IT(&hltdc, LTDC_IT_RR);
  LTDC->SRCR = LTDC_SRCR_VBR;  // reload shadow registers on vertical blank

}

void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d)
{
  if(hdma2d->Instance==DMA2D)
  {
  /* USER CODE BEGIN DMA2D_MspInit 0 */

  /* USER CODE END DMA2D_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_DMA2D_CLK_ENABLE();
  /* USER CODE BEGIN DMA2D_MspInit 1 */

  /* USER CODE END DMA2D_MspInit 1 */
  }

}


/**
* @brief LTDC MSP Initialization
* This function configures the hardware resources used in this example
* @param hltdc: LTDC handle pointer
* @retval None
*/
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hltdc->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspInit 0 */

  /* USER CODE END LTDC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_LTDC_CLK_ENABLE();

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**LTDC GPIO Configuration
    PF10     ------> LTDC_DE
    PA3     ------> LTDC_B5
    PA4     ------> LTDC_VSYNC
    PA6     ------> LTDC_G2
    PB0     ------> LTDC_R3
    PB1     ------> LTDC_R6
    PB10     ------> LTDC_G4
    PB11     ------> LTDC_G5
    PG6     ------> LTDC_R7
    PG7     ------> LTDC_CLK
    PC6     ------> LTDC_HSYNC
    PC7     ------> LTDC_G6
    PA11     ------> LTDC_R4
    PA12     ------> LTDC_R5
    PC10     ------> LTDC_R2
    PD3     ------> LTDC_G7
    PD6     ------> LTDC_B2
    PG10     ------> LTDC_G3
    PG11     ------> LTDC_B3
    PG12     ------> LTDC_B4
    PB8     ------> LTDC_B6
    PB9     ------> LTDC_B7
    */
    GPIO_InitStruct.Pin = ENABLE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(ENABLE_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = B5_Pin|VSYNC_Pin|G2_Pin|R4_Pin
                          |R5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = R3_Pin|R6_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = G4_Pin|G5_Pin|B6_Pin|B7_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = R7_Pin|DOTCLK_Pin|B3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = HSYNC_Pin|G6_Pin|R2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = G7_Pin|B2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = G3_Pin|B4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* LTDC interrupt Init */
  // HAL_NVIC_SetPriority(LTDC_IRQn, 5, 0);
    //HAL_NVIC_EnableIRQ(LTDC_IRQn);
  /* USER CODE BEGIN LTDC_MspInit 1 */

  /* USER CODE END LTDC_MspInit 1 */
  }

}
