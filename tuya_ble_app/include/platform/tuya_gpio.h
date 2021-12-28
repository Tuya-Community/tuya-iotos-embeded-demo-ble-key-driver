/**
 * @file tuya_gpio.h
 * @author lifan
 * @brief tuya gpio header file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_GPIO_H__
#define __TUYA_GPIO_H__

#include "tuya_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
typedef BYTE_T GPIO_RET;
#define GPIO_OK                 0x00
#define GPIO_ERR_INVALID_PARM   0x01
#define GPIO_ERR_MALLOC_FAILED  0x02
#define GPIO_ERR_CB_UNDEFINED   0x03

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef enum {
    TY_GPIOA_0 = 0,
    TY_GPIOA_1,
    TY_GPIOA_2,
    TY_GPIOA_3,
    TY_GPIOA_4,
    TY_GPIOA_5,
    TY_GPIOA_6,
    TY_GPIOA_7,
    TY_GPIOB_0 = 8,
    TY_GPIOB_1,
    TY_GPIOB_2,
    TY_GPIOB_3,
    TY_GPIOB_4,
    TY_GPIOB_5,
    TY_GPIOB_6,
    TY_GPIOB_7,
    TY_GPIOC_0 = 16,
    TY_GPIOC_1,
    TY_GPIOC_2,
    TY_GPIOC_3,
    TY_GPIOC_4,
    TY_GPIOC_5,
    TY_GPIOC_6,
    TY_GPIOC_7,
    TY_GPIOD_0 = 24,
    TY_GPIOD_1,
    TY_GPIOD_2,
    TY_GPIOD_3,
    TY_GPIOD_4,
    TY_GPIOD_5,
    TY_GPIOD_6,
    TY_GPIOD_7,
    TY_GPIO_MAX
} TY_GPIO_PORT_E;

typedef BYTE_T TY_GPIO_MODE_E;
#define TY_GPIO_PULLUP      0x00
#define TY_GPIO_PULLDOWN    0x01
#define TY_GPIO_FLOATING    0x02

typedef BYTE_T TY_GPIO_IRQ_TYPE_E;
#define TY_GPIO_IRQ_NONE    0x00
#define TY_GPIO_IRQ_RISING  0x01
#define TY_GPIO_IRQ_FALLING 0x02

typedef VOID_T (*TY_GPIO_IRQ_CB)(TY_GPIO_PORT_E port);

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief tuya gpio init
 * @param[in] port: gpio number
 * @param[in] in: TRUE - in, FALSE - out
 * @param[in] active_low: TRUE - active_low, FALSE - active high
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_init(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T in, IN CONST BOOL_T active_low);

/**
 * @brief tuya gpio input init
 * @param[in] port: gpio number
 * @param[in] mode: gpio mode
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_input_init(IN CONST TY_GPIO_PORT_E port, IN CONST TY_GPIO_MODE_E mode);

/**
 * @brief tuya gpio set input or output
 * @param[in] port: gpio number
 * @param[in] in: TRUE - in, FALSE - out
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_set_inout(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T in);

/**
 * @brief tuya gpio set mode
 * @param[in] port: gpio number
 * @param[in] mode: gpio mode
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_set_mode(IN CONST TY_GPIO_PORT_E port, IN CONST TY_GPIO_MODE_E mode);

/**
 * @brief tuya gpio write
 * @param[in] port: gpio number
 * @param[in] level: output level
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_write(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T level);

/**
 * @brief tuya gpio read
 * @param[in] port: gpio number
 * @return TRUE - high level, false - low level
 */
BOOL_T tuya_gpio_read(IN CONST TY_GPIO_PORT_E port);

/**
 * @brief tuya gpio interrupt init
 * @param[in] port: gpio number
 * @param[in] trig_type: trigger type
 * @param[in] irq_cb: interrupt callback function
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_irq_init(IN CONST TY_GPIO_PORT_E port, IN CONST TY_GPIO_IRQ_TYPE_E trig_type, IN TY_GPIO_IRQ_CB irq_cb);

/*
 * @brief tuya gpio irq handler
 * @param[in] none
 * @return none
 */
VOID_T tuya_gpio_irq_handler(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_GPIO_H__ */
