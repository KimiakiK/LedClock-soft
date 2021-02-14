/*
 * drv_light_sensor.c
 *
 *  Created on: Feb 3, 2021
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "drv_light_sensor.h"

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static ADC_HandleTypeDef* hadc_ptr;

/********** Function Prototype **********/

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvLightInit(void)
{

}

/*=== 周期関数 ===*/
void DrvLightMain(void)
{

}

/*=== ハンドラ設定関数 ===*/
void DrvLightSetHandler(ADC_HandleTypeDef* hadc)
{
	hadc_ptr = hadc;
}
