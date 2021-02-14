/*
 * drv_rotary_encoder.h
 *
 *  Created on: Feb 3, 2021
 *      Author: kimi
 */

#ifndef INC_DRV_ROTARY_ENCODER_H_
#define INC_DRV_ROTARY_ENCODER_H_

/********** Include **********/

/********** Define **********/

enum SwStateType {
	SW_OFF = GPIO_PIN_SET,
	SW_ON = GPIO_PIN_RESET,
	SW_INIT = 0xFF
};

enum RotaryStateType {
	ROTARY_NONE = 0,
	ROTARY_CW,
	ROTARY_CCW
};

/********** Type **********/

typedef enum SwStateType sw_t;
typedef enum RotaryStateType rotary_t;

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvRoEncInit(void);
void DrvRoEncMain(void);
void DrvRoEncSetHandler(TIM_HandleTypeDef* htim);
sw_t DrvRoEncGetSwState(void);
rotary_t DrvRoEncGetRotaryState(void);

#endif /* INC_DRV_ROTARY_ENCODER_H_ */
