/*
 * gyro.c
 *
 *  Created on: Apr 23, 2021
 *      Author: Islam
 */


#include "gyroscope.h"
#include "l3gd20.h"
#include "stm32f429i_discovery.h"

#define GYRO_ERROR 1
#define GYRO_OK    0

static GYRO_DrvTypeDef *GyroscopeDrv;

uint8_t GYRO_Init(void)
{
	uint8_t ret = GYRO_ERROR;
	  uint16_t ctrl = 0x0000;
	  GYRO_InitTypeDef L3GD20_InitStructure;
	  GYRO_FilterConfigTypeDef L3GD20_FilterStructure={0,0};

	  if((L3gd20Drv.ReadID() == I_AM_L3GD20) )
	  {
	    /* Initialize the Gyroscope driver structure */
	    GyroscopeDrv = &L3gd20Drv;

	    /* MEMS configuration ----------------------------------------------------*/
	    /* Fill the Gyroscope structure */
	    L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
	    L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_1;
	    L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
	    L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;
	    L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Continous;
	    L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;
	    L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_500;

	    /* Configure MEMS: data rate, power mode, full scale and axes */
	    ctrl = (uint16_t) (L3GD20_InitStructure.Power_Mode | L3GD20_InitStructure.Output_DataRate | \
	                       L3GD20_InitStructure.Axes_Enable | L3GD20_InitStructure.Band_Width);

	    ctrl |= (uint16_t) ((L3GD20_InitStructure.BlockData_Update | L3GD20_InitStructure.Endianness | \
	                         L3GD20_InitStructure.Full_Scale) << 8);

	    /* Configure the Gyroscope main parameters */
	    GyroscopeDrv->Init(ctrl);

	    L3GD20_FilterStructure.HighPassFilter_Mode_Selection = L3GD20_HPM_NORMAL_MODE_RES;
	    L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;

	    ctrl = (uint8_t) ((L3GD20_FilterStructure.HighPassFilter_Mode_Selection |\
	                       L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency));

	    /* Configure the Gyroscope main parameters */
	    GyroscopeDrv->FilterConfig(ctrl) ;

	    GyroscopeDrv->FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);

	    ret = GYRO_OK;
	  }
	  else
	  {
	    ret = GYRO_ERROR;
	  }
	  return ret;
}

void GYRO_Reset(void)
{
	L3gd20Drv.Reset();
}

void GYRO_GetXYZ(float* RD_data)
{
	L3gd20Drv.GetXYZ(RD_data);
}
