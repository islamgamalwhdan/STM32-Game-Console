/*
 * gyroscope.h
 *
 *  Created on: Apr 23, 2021
 *      Author: Islam
 */

#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_


#include "stdint.h"

typedef struct
{
	float x , y ,z ;
}GYRO_RD;
uint8_t GYRO_Init(void);
void GYRO_Reset(void);
void GYRO_GetXYZ(float *rd);
#endif /* GYROSCOPE_H_ */
