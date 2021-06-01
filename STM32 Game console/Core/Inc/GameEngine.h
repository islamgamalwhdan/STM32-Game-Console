/*
 * GameEngine.h
 *
 *  Created on: Apr 16, 2021
 *      Author: Islam
 */

#ifndef INC_GAMEENGINE_H_
#define INC_GAMEENGINE_H_

#include "stdint.h"

typedef enum {BACKGROUND , SPRITE}LayerType;

typedef enum {ROT_ZERO= 0 , ROT_90 , ROT_180 , ROT_270}ROT_MODE;
typedef struct
{
  int       x       , y       ;
  int       Width   , Height  ;
  int       vxDelay , vyDelay ;
  int       xDelay  , yDelay  ;
  int       xSpeed  , ySpeed  ;
  int       active            ;
  const uint16_t* ImageData   ;
  LayerType LayerId          ;
}Sprite ;


void  DrawSprite(Sprite* spr , ROT_MODE mode);
void UpdatePostion(Sprite* spr);
int collision( Sprite* spr1 ,  Sprite* spr2);
#endif /* INC_GAMEENGINE_H_ */
