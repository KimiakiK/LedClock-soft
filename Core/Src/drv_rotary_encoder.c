/*
 * drv_rotary_encoder.c
 *
 *  Created on: Feb 3, 2021
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "main.h"
#include "drv_rotary_encoder.h"

/********** Define **********/

/* ロータリーエンコーダSW端子 */
#define ENCODER_SW_PIN		ENCODER_SW_GPIO_Port, ENCODER_SW_Pin

/* ロータリーエンコーダ値 */
#define ENCODER_VALUE		TIM2->CNT

/* スイッチ確定時間 */
#define SW_FIX_COUNT		(4)

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

extern TIM_HandleTypeDef htim2;

static sw_t sw_state;
static sw_t old_sw_state;
static uint8_t sw_count;

static rotary_t rotary_state;
static uint32_t old_encoder;

/********** Function Prototype **********/

static void DrvRoEncCheckSwState(void);
static void DrvRoEncCheckEncoder(void);

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvRoEncInit(void)
{
	/* 変数初期化 */
	sw_state = SW_INIT;
	old_sw_state = sw_state;
	sw_count = 0;
	rotary_state = ROTARY_NONE;
	old_encoder = ENCODER_VALUE;

	/* エンコーダ入力受付開始 */
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
}

/*=== 周期関数 ===*/
void DrvRoEncMain(void)
{
	DrvRoEncCheckSwState();
	DrvRoEncCheckEncoder();
}

/*=== SW状態取得関数 ===*/
sw_t DrvRoEncGetSwState(void)
{
	return sw_state;
}

/*=== 回転状態取得関数 ===*/
rotary_t DrvRoEncGetRotaryState(void)
{
	return rotary_state;
}

/*=== SW状態判定関数 ===*/
static void DrvRoEncCheckSwState(void)
{
	sw_t new_sw_state;

	new_sw_state = (sw_t)HAL_GPIO_ReadPin(ENCODER_SW_PIN);

	if (new_sw_state == old_sw_state) {
		if (sw_count <= SW_FIX_COUNT) {
			sw_count ++;
		} else {
			sw_state = new_sw_state;
		}
	} else {
		sw_count = 0;
	}

	old_sw_state = new_sw_state;
}

/*=== ロータリーエンコーダ回転判定関数 ===*/
static void DrvRoEncCheckEncoder(void)
{
	uint32_t new_encoder;
	int32_t encoder_diff;

	new_encoder = ENCODER_VALUE;
	encoder_diff = new_encoder - old_encoder;

	/* 1回の回転で値が2変わるので差分が2以上の時に回転と判定する */
	if (encoder_diff > 1) {
		rotary_state = ROTARY_CCW;
		old_encoder = new_encoder;
	} else if (encoder_diff < -1) {
		rotary_state = ROTARY_CW;
		old_encoder = new_encoder;
	} else {
		rotary_state = ROTARY_NONE;
	}
}
