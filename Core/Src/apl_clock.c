/*
 * apl_clock.c
 *
 *  Created on: Feb 3, 2021
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "drv_led.h"
#include "drv_rotary_encoder.h"
#include "drv_rtc.h"
#include "apl_clock.h"

/********** Define **********/

/* 時刻設定遷移SW ON時間 */
#define SW_ON_TIME_GO_SETTING	(1000)
/* 設定決定SW ON時間 */
#define SW_ON_TIME_SETTING_OK	(50)

#define HOUR_MAX		(12)
#define MINUTE_MAX		(60)

enum DispStateType {
	DISP_NORMAL = 0,
	DISP_SETTING_HOUR,
	DISP_SETTING_MINUTE
};

/********** Type **********/

typedef enum DispStateType disp_t;

/********** Constant **********/

/********** Variable **********/

static disp_t disp_state;
static time_t pwm_duty;
static uint8_t setting_duty;
static time_t setting_time;
static uint32_t sw_on_count;


/********** Function Prototype **********/

void aplClockTransitionDispState(void);
void aplClockProcessDispState(void);

void aplClockDispTime(void);
void aplClockDispSettingHour(void);
void aplClockDispSettingMinute(void);

void aplClockCheckSw(void);

/********** Function **********/

/*=== 初期化関数 ===*/
void AplClockInit(void)
{
	disp_state = DISP_NORMAL;
	pwm_duty.hour = 255;
	pwm_duty.minute = 80;
	pwm_duty.second = 10;
	setting_duty = 5;
	setting_time = DrvRtcGetNowTime();
	sw_on_count = 0;
}

/*=== 周期関数 ===*/
void AplClockMain(void)
{
	/* 表示状態遷移 */
	aplClockTransitionDispState();

	/* 表示状態処理 */
	aplClockProcessDispState();
}

/*=== 表示状態遷移関数 ===*/
void aplClockTransitionDispState(void)
{
	/* SW状態判定 */
	aplClockCheckSw();

	switch (disp_state) {
	case DISP_NORMAL:
		if (sw_on_count == SW_ON_TIME_GO_SETTING) {
			/* 時刻設定開始 */
			disp_state = DISP_SETTING_HOUR;
			setting_time = DrvRtcGetNowTime();
		}
		break;
	case DISP_SETTING_HOUR:
		if (sw_on_count == SW_ON_TIME_SETTING_OK) {
			/* 時の設定完了 */
			disp_state = DISP_SETTING_MINUTE;
		}
		break;
	case DISP_SETTING_MINUTE:
		if (sw_on_count == SW_ON_TIME_SETTING_OK) {
			/* 分の設定完了、時刻表示開始 */
			disp_state = DISP_NORMAL;
			/* 設定時刻を反映 */
			setting_time.second = 0;
			DrvRtcSetTime(setting_time);
		}
		break;
	default:
		/* 処理無し */
		break;
	}
}

/*=== 表示状態処理関数 ===*/
void aplClockProcessDispState(void)
{
	rotary_t rotary_state;

	rotary_state = DrvRoEncGetRotaryState();

	switch (disp_state) {
	case DISP_NORMAL:
		/* 現在時刻表示 */
		aplClockDispTime();
		DrvRoEncSetLed(ROENC_LED_OFF);
		break;
	case DISP_SETTING_HOUR:
		if (rotary_state == ROTARY_CW) {
			setting_time.hour = (setting_time.hour + 1) % HOUR_MAX;
		} else if (rotary_state == ROTARY_CCW) {
			setting_time.hour = (setting_time.hour + HOUR_MAX - 1) % HOUR_MAX;
		}
		/* 時の設定表示 */
		aplClockDispSettingHour();
		DrvRoEncSetLed(ROENC_LED_BLUE);
		break;
	case DISP_SETTING_MINUTE:
		if (rotary_state == ROTARY_CW) {
			setting_time.minute = (setting_time.minute + 1) % MINUTE_MAX;
		} else if (rotary_state == ROTARY_CCW) {
			setting_time.minute = (setting_time.minute + MINUTE_MAX - 1) % MINUTE_MAX;
		}
		/* 分の設定表示 */
		aplClockDispSettingMinute();
		DrvRoEncSetLed(ROENC_LED_GREEN);
		break;
	default:
		/* 処理無し */
		break;
	}
}

/*=== 現在時刻表示関数 ===*/
void aplClockDispTime(void)
{
	time_t now_time;

	now_time = DrvRtcGetNowTime();

	DrvLedClearOutputState();
	DrvLedSetOutputState(now_time.second, pwm_duty.second);
	DrvLedSetOutputState(now_time.minute, pwm_duty.minute);
	DrvLedSetOutputState(now_time.hour * 5, pwm_duty.hour);
	DrvLedUpdateRequest();
}

/*=== 時の設定表示関数 ===*/
void aplClockDispSettingHour(void)
{
	DrvLedClearOutputState();

	for (uint8_t index=0; index<HOUR_MAX; index++) {
			DrvLedSetOutputState(index * 5, setting_duty);
	}

	DrvLedSetOutputState(setting_time.hour * 5, pwm_duty.hour);

	DrvLedUpdateRequest();
}

/*=== 分の設定表示関数 ===*/
void aplClockDispSettingMinute(void)
{
	DrvLedClearOutputState();

	for (uint8_t index=0; index<MINUTE_MAX; index++) {
			DrvLedSetOutputState(index, setting_duty);
	}

	DrvLedSetOutputState(setting_time.minute, pwm_duty.minute);

	DrvLedUpdateRequest();
}

/*=== SW状態判定関数 ===*/
void aplClockCheckSw(void)
{
	sw_t sw_state;

	sw_state = DrvRoEncGetSwState();

	if (sw_state == SW_ON) {
		sw_on_count ++;
	} else {
		sw_on_count = 0;
	}
}
