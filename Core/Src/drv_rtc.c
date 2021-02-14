/*
 * drv_rtc.c
 *
 *  Created on: Feb 3, 2021
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "drv_rtc.h"

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

extern RTC_HandleTypeDef hrtc;

static time_t now_time;

/********** Function Prototype **********/

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvRtcInit(void)
{
	time_t init_time = {0};

	DrvRtcSetTime(init_time);

	/* RTCレジスタ書き込み保護解除 */
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	/* 時刻を常時読み出すためにBYPSHAD制御ビットをセット */
	RTC->CR |= 0x00000020;
	/* RTCレジスタ書き込み保護有効化 */
	RTC->WPR = 0x00;
}

/*=== 周期関数 ===*/
void DrvRtcMain(void)
{
	RTC_TimeTypeDef rtc_data;
	HAL_RTC_GetTime(&hrtc, &rtc_data, RTC_FORMAT_BIN);
	now_time.hour = rtc_data.Hours;
	now_time.minute = rtc_data.Minutes;
	now_time.second = rtc_data.Seconds;
}

/*=== 現在時刻取得関数 ===*/
time_t DrvRtcGetNowTime(void)
{
	return now_time;
}

/*=== 時刻設定関数 ===*/
void DrvRtcSetTime(time_t new_time)
{
	RTC_TimeTypeDef rtc_time;

	rtc_time.Hours = new_time.hour;
	rtc_time.Minutes = new_time.minute;
	rtc_time.Seconds = new_time.second;
	rtc_time.TimeFormat = RTC_HOURFORMAT12_AM;
	rtc_time.SubSeconds = 0;
	rtc_time.SecondFraction = 0;
	rtc_time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	rtc_time.StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
}
