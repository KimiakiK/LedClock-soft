/*
 * drv_led.c
 *
 *  Created on: Jan 30, 2021
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "main.h"
#include "drv_led.h"

/********** Define **********/

/* LEDの数 */
#define LED_NUM		(60)
/* LED Driver ICの数 */
#define IC_NUM		(4)

#define COMMAND_PWM_BUFFER_SIZE		(1 + LED_NUM / IC_NUM)

/* LED消灯 */
#define LED_OFF		(0x00)

/* I2C同期送信のタイムアウト [ms] */
#define I2C_TIMEOUT	(100)

/* OUTPUT ENABLE端子 */
#define PIN_OE	LED_OE_GPIO_Port, LED_OE_Pin
/* LED点灯許可 */
#define OUTPUT_ENABLE	GPIO_PIN_RESET
/* LED点灯禁止 */
#define OUTPUT_DISABLE	GPIO_PIN_SET

enum TransmitStateType {
	TX_STATE_IC1_TX=0,
	TX_STATE_IC2_TX,
	TX_STATE_IC3_TX,
	TX_STATE_IC4_TX,
	TX_STATE_IDLE=0xFF
};

enum TransmitRequestType {
	TX_REQ_NONE=0,
	TX_REQ_ON
};

/********** Type **********/

typedef enum TransmitStateType TransmitState;
typedef enum TransmitRequestType TransmitRequest;

/********** Constant **********/

const uint8_t i2c_address[IC_NUM] = {
	0x01 << 1, 0x02 << 1, 0x04 << 1, 0x08 << 1
};

const uint8_t command_mode_init[] = {
	0x80,		/* Register: MODE1 (Auto-Increment) */
	0x81,		/* MODE1: スリープ解除 */
	0x01		/* MODE2: オープンドレイン */
};

const uint8_t command_led_off[] = {
	0x82,		/* Register: PWM0 (Auto-Increment) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/* PWM0-15: DUTY 0% */
	0xFF,		/* GRPPWM: default */
	0x00,		/* GRPFREQ: default */
	0xAA, 0xAA, 0xAA, 0x2A	/* LEDOUT0-3: LED0-14はPWM出力、LED15はOFF */
};

/********** Variable **********/

extern I2C_HandleTypeDef hi2c1;

static TransmitState transmit_state;
static TransmitRequest transmit_request;
static uint8_t led_duty[LED_NUM];
static uint8_t command_pwm[IC_NUM][COMMAND_PWM_BUFFER_SIZE];

/********** Function Prototype **********/
static void drvLedTxPwm(void);
static void drvLedMakeTxData(void);
static void drvLedSyncTransmit(uint8_t address, uint8_t* data, uint16_t size);
static void drvLedAsyncTransmit(uint8_t address, uint8_t* data, uint16_t size);

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvLedInit(void)
{
	/* 変数初期化 */
	transmit_state = TX_STATE_IDLE;
	transmit_request = TX_REQ_NONE;
	DrvLedClearOutputState();

	/* LED点灯禁止 */
	HAL_GPIO_WritePin(PIN_OE, OUTPUT_DISABLE);

	/* スリープ解除 */
	for (uint8_t index=0; index<IC_NUM; index++) {
		drvLedSyncTransmit(i2c_address[index], command_mode_init, sizeof(command_mode_init));
	}

	/* LED出力初期化(全消灯) */
	for (uint8_t index=0; index<IC_NUM; index++) {
		drvLedSyncTransmit(i2c_address[index], command_led_off, sizeof(command_led_off));
	}

	/* LED点灯許可 */
	HAL_GPIO_WritePin(PIN_OE, OUTPUT_ENABLE);
}

/*=== 周期関数 ===*/
void DrvLedMain(void)
{
	if (transmit_request == TX_REQ_ON) {
		if (transmit_state == TX_STATE_IDLE) {
			drvLedTxPwm();
			transmit_request = TX_REQ_NONE;
		}
	}
}

/*=== I2C割り込み処理関数 ===*/
void DrvLedIrqI2c(void)
{
	drvLedTxPwm();
}

/*=== LED出力状態全クリア関数 ===*/
void DrvLedClearOutputState(void)
{
	for (uint8_t index=0; index<LED_NUM; index++) {	
		led_duty[index] = LED_OFF;
	}
}

/*=== LED出力状態設定関数 ===*/
void DrvLedSetOutputState(uint8_t pos, uint8_t duty)
{
	if (pos < LED_NUM) {
		led_duty[pos] = duty;
	}
}

/*=== LED出力更新要求関数 ===*/
void DrvLedUpdateRequest(void)
{
	transmit_request = TX_REQ_ON;
}

/*=== PWM設定送信関数 ===*/
static void drvLedTxPwm(void)
{
	switch (transmit_state) {
	case TX_STATE_IDLE:
		drvLedMakeTxData();
		drvLedAsyncTransmit(i2c_address[TX_STATE_IC1_TX], command_pwm[TX_STATE_IC1_TX], COMMAND_PWM_BUFFER_SIZE);
		transmit_state = TX_STATE_IC1_TX;
		break;
	case TX_STATE_IC1_TX:
		drvLedAsyncTransmit(i2c_address[TX_STATE_IC2_TX], command_pwm[TX_STATE_IC2_TX], COMMAND_PWM_BUFFER_SIZE);
		transmit_state = TX_STATE_IC2_TX;
		break;
	case TX_STATE_IC2_TX:
		drvLedAsyncTransmit(i2c_address[TX_STATE_IC3_TX], command_pwm[TX_STATE_IC3_TX], COMMAND_PWM_BUFFER_SIZE);
		transmit_state = TX_STATE_IC3_TX;
		break;
	case TX_STATE_IC3_TX:
		drvLedAsyncTransmit(i2c_address[TX_STATE_IC4_TX], command_pwm[TX_STATE_IC4_TX], COMMAND_PWM_BUFFER_SIZE);
		transmit_state = TX_STATE_IC4_TX;
		break;
	case TX_STATE_IC4_TX:
		transmit_state = TX_STATE_IDLE;
		break;
	}
}

/*=== PWM設定送信データ作成関数 ===*/
static void drvLedMakeTxData(void) {
	for (uint8_t ic=0; ic<IC_NUM; ic++) {
		command_pwm[ic][0] = 0x82;		/* Register: PWM0 (Auto-Increment) */
		for (uint8_t led=0; led<LED_NUM / IC_NUM; led++) {
			command_pwm[ic][led + 1] = led_duty[ic * (LED_NUM / IC_NUM) + led];
		}
	}
}

/*=== I2C同期送信関数 ===*/
static void drvLedSyncTransmit(uint8_t address, uint8_t* data, uint16_t size)
{
	HAL_I2C_Master_Transmit(&hi2c1, address, data, size, I2C_TIMEOUT);
}

/*=== I2C非同期送信関数 ===*/
static void drvLedAsyncTransmit(uint8_t address, uint8_t* data, uint16_t size)
{
	HAL_I2C_Master_Transmit_DMA(&hi2c1, address, data, size);
}
