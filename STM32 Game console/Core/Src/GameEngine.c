/*
 * GameEngine.c
 *
 *  Created on: Apr 16, 2021
 *      Author: Islam
 */
#include "GameEngine.h"
#include "gpu.h"

#define TRANSPARARENT_COLOR_RGB888 0x52FF73
#define BPP        2
#define ASPECT_RATIO_W 12
#define ASPECT_RATIO_H 16

void  DrawSprite(Sprite* spr , ROT_MODE mode)
{
	if(spr->active)
	{
      void * data = (void *)spr->ImageData ;
      uint32_t spriteHeight = spr->Height;
      uint32_t spriteWidth = spr->Width;
      uint32_t x ;
      uint32_t y ;
      uint32_t offset ;

      switch(mode)
    	  {
    	  case ROT_ZERO :
    		 //
    		  x = spr->x ;
    		  y = spr->y ;
    		 // spriteWidth =  spr->Width;
    		 // spriteHeight = spr->Height;
    		  break ;

    	  case ROT_90 :
    		  spriteWidth  =  spr->Height ;
    		  spriteHeight =  spr->Width  ;
    		  x =  spr->y ;
    		  y = disp.Dis_h- spr->x -spriteHeight  ;

    		  break ;
    	  case ROT_180 :
    		  break;
    	  case ROT_270 :
    		  break;

    	  } // switch

    	  offset = BPP * (x + y*disp.Dis_w) ;

    	  if(spr->LayerId == SPRITE)

    	      Draw_image_buff(data, spriteWidth, spriteHeight, disp.draw_buff + offset);
      else
    	     Fill_color_buff(TRANSPARARENT_COLOR_RGB888, spriteWidth, spriteHeight, disp.draw_buff+offset);
	}//if
}

void UpdatePostion(Sprite* spr)
{

  if(++spr->vxDelay > spr->xDelay )

    {
    if(spr->x > (disp.Dis_w - spr->Width) || spr->x <0  )
       spr->xSpeed *= -1 ;
        spr->x += spr->xSpeed ;
        spr->vxDelay = 0 ;
    }
    if(++spr->vyDelay > spr->yDelay )
    {
      if(spr->y > (disp.Dis_h - spr->Height) || spr->y <0  )
             spr->ySpeed *= -1 ;
        spr->y += spr->ySpeed ;
        spr->vyDelay = 0 ;
    }

}

int collision( Sprite* spr1 ,  Sprite* spr2)
{
    if(spr1->x  > (spr2->x + spr2->Width  ) ||
      spr1->y  > (spr2->y + spr2->Height  ) ||
      spr2->x > (spr1->x + spr1->Width  ) ||
      spr2->y > (spr1->y + spr1->Height  ) )
   {
       return 0 ;
   }
   return 1 ;
}
