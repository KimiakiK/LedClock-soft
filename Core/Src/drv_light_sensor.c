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

extern ADC_HandleTypeDef hadc1;

/********** Function Prototype **********/

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvLightInit(void)
{

}

/*=== 周期関数 ===*/
void DrvLightMain(void)
{
volatile uint32_t ad_value;
    
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    ad_value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
}
