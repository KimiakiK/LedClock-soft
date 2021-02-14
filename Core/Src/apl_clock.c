/*
 * apl_clock.c
 *
 *  Created on: Feb 3, 2021
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "drv_rtc.h"
#include "drv_led.h"
#include "apl_clock.h"

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

/********** Function **********/

/*=== 初期化関数 ===*/
void AplClockInit(void)
{

}

/*=== 周期関数 ===*/
void AplClockMain(void)
{
	time_t now_time;

	now_time = DrvRtcGetNowTime();

	DrvLedClearOutputState();
	DrvLedSetOutputState(now_time.second, 10);
	DrvLedSetOutputState(now_time.minute, 80);
	DrvLedSetOutputState(now_time.hour * 5, 255);
	DrvLedUpdateRequest();
}
