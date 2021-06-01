/*
 * gpu.h
 *
 *  Created on: Apr 14, 2021
 *      Author: Islam
 */
#include "stdint.h"


#ifndef INC_GPU_H_
#define INC_GPU_H_


typedef struct
{
	uint32_t Rendr_buf ;
	uint32_t draw_buff ;
	uint32_t Dis_w ;
	uint32_t Dis_h ;
}Display;

extern Display disp ;

void InitDisplay(uint32_t background);
void Draw_image_buff(void * img , uint32_t width , uint32_t height , uint32_t buff);
void Fill_color_buff(uint32_t color , uint32_t width , uint32_t height , uint32_t buff );
void Draw_Pixel(uint32_t x , uint32_t y , uint32_t color);
void renderChr(uint32_t x , uint32_t y , uint32_t color,char c);
void Draw_chr_fast(uint32_t x , uint32_t y  , char c);
void Draw_String_fast(uint32_t x , uint32_t y ,char *str);
void renderStr(uint32_t x , uint32_t y , uint32_t color,char *str);
void FlipBuffers(void);
uint32_t Is_Render_cplt(void);
void Render_flush(void);

#endif /* INC_GPU_H_ */
