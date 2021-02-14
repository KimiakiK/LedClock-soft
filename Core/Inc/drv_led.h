/*
 * drv_led.h
 *
 *  Created on: Jan 30, 2021
 *      Author: kimi
 */

#ifndef INC_DRV_LED_H_
#define INC_DRV_LED_H_

/********** Include **********/

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvLedInit(void);
void DrvLedMain(void);
void DrvLedIrqI2c(void);
void DrvLedClearOutputState(void);
void DrvLedSetOutputState(uint8_t pos, uint8_t duty);
void DrvLedUpdateRequest(void);

#endif /* INC_DRV_LED_H_ */
