/*
 * sys_platform.c
 *
 *  Created on: Feb 3, 2021
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "apl_clock.h"
#include "drv_led.h"
#include "drv_light_sensor.h"
#include "drv_rotary_encoder.h"
#include "drv_rtc.h"
#include "sys_platform.h"

/********** Define **********/

#define TIMx_SR_UIF		(0x00000001)

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

extern TIM_HandleTypeDef htim1;

/********** Function Prototype **********/

/********** Function **********/

/*=== 初期化関数 ===*/
void SysPlatformInit(void)
{
	DrvLedInit();
	DrvLightInit();
	DrvRoEncInit();
	DrvRtcInit();
	AplClockInit();

	/* 周期タイマ開始 */
	HAL_TIM_Base_Start(&htim1);
}

/*=== 周期関数 ===*/
void SysPlatformMain(void)
{
	DrvLightMain();
	DrvRoEncMain();
	DrvRtcMain();
	AplClockMain();
	DrvLedMain();

	/* 1msタイマー満了待ち */
	while ((TIM1->SR & TIMx_SR_UIF) != TIMx_SR_UIF) {
		/* 処理無し */
	}
	/* タイマーフラグクリア */
	TIM1->SR = 0;
}
