#ifndef GAMPAD_H_
    #define GAMPAD_H_

/*
 * Connection from GampPad side (male side M Not F)
 * V5+            -> 6
 * GND            -> 8
 * GPIOB3(SH_LD)  -> 3
 * GPIOB4(clk)    -> 4
 * GPIOB7(datain) -> 2
 *
 */

#include <stdint.h>
typedef uint8_t U8;

typedef struct 
{
	U8
		right  : 1 ,
		left   : 1 ,
		down   : 1 ,
		up     : 1 ,
		start  : 1 ,
		select : 1 ,
		red    : 1 ,
		yellow : 1 ;
}BUTTON ;

typedef enum {
	GP_NOT_CONNECTED =0 ,
	GP_CONNECTED
}GP_State ;


typedef struct
{
  union
  {
	U8 value ;
	//BUTTON button ;
    BUTTON button ;
  };
  GP_State state ;
}GAMEPAD_BUTTON ;

//Constants

#define GAMEPAD_RIGHT    0x01
#define GAMEPAD_LEFT     0x02
#define GAMEPAD_DOWN     0x04
#define GAMEPAD_UP       0x08

#define GAMEPAD_START    0x10
#define GAMEPAD_SELECT   0x20

#define GAMEPAD_RED      0x40
#define GAMEPAD_YELLOW   0x80

#define PRESSED  0
#define RELEASED 1

//buttons
#define BUTTON_RIGHT    0
#define BUTTON_LEFT     1
#define BUTTON_DOWN     2
#define BUTTON_UP       3

#define BUTTON_START    4
#define BUTTON_SELECT   5

#define BUTTON_RED      6
#define BUTTON_YELLOW   7

//Functions Prototypes
void    GamePad_Init(void) ;
GAMEPAD_BUTTON GamePad_Read(void) ;


#endif // GAMEPAD_H_
