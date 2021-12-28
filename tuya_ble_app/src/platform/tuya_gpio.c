/**
 * @file tuya_gpio.c
 * @author lifan
 * @brief tuya gpio source file for TLSR825x
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_gpio.h"
#include "tuya_ble_mem.h"
#include "gpio_8258.h"

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct ty_gpio_irq_mag_s {
    struct ty_gpio_irq_mag_s *next;
    TY_GPIO_PORT_E port;
    TY_GPIO_IRQ_CB irq_cb;
} TY_GPIO_IRQ_MAG_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC GPIO_PinTypeDef sg_pf_pin_list[] = {
    GPIO_PA0,
    GPIO_PA1,
          -1,
          -1,
          -1,
          -1,
          -1,
          -1,
          -1,
    GPIO_PB1,
          -1,
          -1,
    GPIO_PB4,
    GPIO_PB5,
    GPIO_PB6,
    GPIO_PB7,
    GPIO_PC0,
    GPIO_PC1,
    GPIO_PC2,
    GPIO_PC3,
    GPIO_PC4,
          -1,
          -1,
          -1,
          -1,
          -1,
    GPIO_PD2,
    GPIO_PD3,
    GPIO_PD4,
          -1,
          -1,
    GPIO_PD7
};

STATIC TY_GPIO_IRQ_MAG_T *sg_rise_mag_list = NULL;
STATIC TY_GPIO_IRQ_MAG_T *sg_fall_mag_list = NULL;

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
GPIO_RET tuya_gpio_init(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T in, IN CONST BOOL_T active_low)
{
    if (port >= TY_GPIO_MAX) {
        return GPIO_ERR_INVALID_PARM;
    }
    if (-1 == sg_pf_pin_list[port]) {
        return GPIO_ERR_INVALID_PARM;
    }

    gpio_set_func(sg_pf_pin_list[port], AS_GPIO);
    if (in) {
        gpio_set_input_en(sg_pf_pin_list[port], TRUE);
        gpio_set_output_en(sg_pf_pin_list[port], FALSE);
        if (active_low) {
            gpio_setup_up_down_resistor(sg_pf_pin_list[port], PM_PIN_PULLUP_10K);
        } else {
            gpio_setup_up_down_resistor(sg_pf_pin_list[port], PM_PIN_PULLDOWN_100K);
        }
    } else {
        gpio_set_input_en(sg_pf_pin_list[port], FALSE);
        gpio_set_output_en(sg_pf_pin_list[port], TRUE);
        if (active_low) {
            gpio_write(sg_pf_pin_list[port], TRUE);
        } else {
            gpio_write(sg_pf_pin_list[port], FALSE);
        }
    }

    return GPIO_OK;
}

/**
 * @brief gpio set mode
 * @param[in] port: gpio number
 * @param[in] mode: gpio mode
 * @return none
 */
STATIC VOID_T __gpio_set_mode(IN CONST TY_GPIO_PORT_E port, IN CONST TY_GPIO_MODE_E mode)
{
    switch (mode) {
    case TY_GPIO_PULLUP:
        gpio_setup_up_down_resistor(sg_pf_pin_list[port], PM_PIN_PULLUP_10K);
        break;
    case TY_GPIO_PULLDOWN:
        gpio_setup_up_down_resistor(sg_pf_pin_list[port], PM_PIN_PULLDOWN_100K);
        break;
    case TY_GPIO_FLOATING:
        gpio_setup_up_down_resistor(sg_pf_pin_list[port], PM_PIN_UP_DOWN_FLOAT);
        break;
    default:
        break;
    }
}

/**
 * @brief tuya gpio input init
 * @param[in] port: gpio number
 * @param[in] mode: gpio mode
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_input_init(IN CONST TY_GPIO_PORT_E port, IN CONST TY_GPIO_MODE_E mode)
{
    if (port >= TY_GPIO_MAX) {
        return GPIO_ERR_INVALID_PARM;
    }
    if (-1 == sg_pf_pin_list[port]) {
        return GPIO_ERR_INVALID_PARM;
    }

    gpio_set_func(sg_pf_pin_list[port], AS_GPIO);
    gpio_set_input_en(sg_pf_pin_list[port], TRUE);
    gpio_set_output_en(sg_pf_pin_list[port], FALSE);
    __gpio_set_mode(port, mode);

    return GPIO_OK;
}

/**
 * @brief tuya gpio set input or output
 * @param[in] port: gpio number
 * @param[in] in: TRUE - in, FALSE - out
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_set_inout(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T in)
{
    if (port >= TY_GPIO_MAX) {
        return GPIO_ERR_INVALID_PARM;
    }
    if (-1 == sg_pf_pin_list[port]) {
        return GPIO_ERR_INVALID_PARM;
    }

    if (in) {
        gpio_set_input_en(sg_pf_pin_list[port], TRUE);
        gpio_set_output_en(sg_pf_pin_list[port], FALSE);
    } else {
        gpio_set_input_en(sg_pf_pin_list[port], FALSE);
        gpio_set_output_en(sg_pf_pin_list[port], TRUE);
    }

    return GPIO_OK;
}

/**
 * @brief tuya gpio set mode
 * @param[in] port: gpio number
 * @param[in] mode: gpio mode
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_set_mode(IN CONST TY_GPIO_PORT_E port, IN CONST TY_GPIO_MODE_E mode)
{
    if (port >= TY_GPIO_MAX) {
        return GPIO_ERR_INVALID_PARM;
    }
    if (-1 == sg_pf_pin_list[port]) {
        return GPIO_ERR_INVALID_PARM;
    }

    __gpio_set_mode(port, mode);

    return GPIO_OK;
}

/**
 * @brief tuya gpio write
 * @param[in] port: gpio number
 * @param[in] level: output level
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_write(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T level)
{
    if (port >= TY_GPIO_MAX) {
        return GPIO_ERR_INVALID_PARM;
    }
    if (-1 == sg_pf_pin_list[port]) {
        return GPIO_ERR_INVALID_PARM;
    }

    gpio_write(sg_pf_pin_list[port], level);

    return GPIO_OK;
}

/**
 * @brief tuya gpio read
 * @param[in] port: gpio number
 * @return TRUE - high level, false - low level
 */
BOOL_T tuya_gpio_read(IN CONST TY_GPIO_PORT_E port)
{
    if (port >= TY_GPIO_MAX) {
        return 0;
    }
    if (-1 == sg_pf_pin_list[port]) {
        return 0;
    }

    return gpio_read(sg_pf_pin_list[port]);
}

/**
 * @brief tuya gpio interrupt init
 * @param[in] port: gpio number
 * @param[in] trig_type: trigger type
 * @param[in] irq_cb: interrupt callback function
 * @return GPIO_RET
 */
GPIO_RET tuya_gpio_irq_init(IN CONST TY_GPIO_PORT_E port, IN CONST TY_GPIO_IRQ_TYPE_E trig_type, IN TY_GPIO_IRQ_CB irq_cb)
{
    if (port >= TY_GPIO_MAX) {
        return GPIO_ERR_INVALID_PARM;
    }
    if (-1 == sg_pf_pin_list[port]) {
        return GPIO_ERR_INVALID_PARM;
    }

    /* allocate and clear for hall_sw_mag */
    TY_GPIO_IRQ_MAG_T *irq_mag_tmp = (TY_GPIO_IRQ_MAG_T *)tuya_ble_malloc(SIZEOF(TY_GPIO_IRQ_MAG_T));
    if (NULL == irq_mag_tmp) {
        return GPIO_ERR_MALLOC_FAILED;
    }
    irq_mag_tmp->port = port;
    irq_mag_tmp->irq_cb = irq_cb;

    switch (trig_type) {
    case TY_GPIO_IRQ_NONE:
        break;
    case TY_GPIO_IRQ_RISING:
        if (sg_rise_mag_list) {
    	    irq_mag_tmp->next = sg_rise_mag_list;
        }
        sg_rise_mag_list = irq_mag_tmp;
        gpio_set_interrupt_pol(sg_pf_pin_list[port], pol_rising);
	    reg_irq_src = FLD_IRQ_GPIO_RISC0_EN;
	    reg_irq_mask |= FLD_IRQ_GPIO_RISC0_EN;
	    gpio_en_interrupt_risc0(sg_pf_pin_list[port], TRUE);
        break;
    case TY_GPIO_IRQ_FALLING:
        if (sg_fall_mag_list) {
    	    irq_mag_tmp->next = sg_fall_mag_list;
        }
        sg_fall_mag_list = irq_mag_tmp;
        gpio_set_interrupt_pol(sg_pf_pin_list[port], pol_falling);
	    reg_irq_src = FLD_IRQ_GPIO_RISC1_EN;
	    reg_irq_mask |= FLD_IRQ_GPIO_RISC1_EN;
	    gpio_en_interrupt_risc1(sg_pf_pin_list[port], TRUE);
        break;
    default:
        break;
    }

    return GPIO_OK;
}

/**
 * @brief gpio rising irq handler
 * @param[in] none
 * @return none
 */
STATIC VOID_T __gpio_irq_rising_handler(VOID_T)
{
    TY_GPIO_IRQ_MAG_T *irq_mag_tmp = sg_rise_mag_list;
    while (irq_mag_tmp) {
        if (gpio_read(sg_pf_pin_list[irq_mag_tmp->port])) {
            irq_mag_tmp->irq_cb(irq_mag_tmp->port);
        }
        irq_mag_tmp = irq_mag_tmp->next;
    }
}

/**
 * @brief gpio falling irq handler
 * @param[in] none
 * @return none
 */
STATIC VOID_T __gpio_irq_falling_handler(VOID_T)
{
    TY_GPIO_IRQ_MAG_T *irq_mag_tmp = sg_fall_mag_list;
    while (irq_mag_tmp) {
        if (!gpio_read(sg_pf_pin_list[irq_mag_tmp->port])) {
            irq_mag_tmp->irq_cb(irq_mag_tmp->port);
        }
        irq_mag_tmp = irq_mag_tmp->next;
    }
}

/*
 * @brief tuya gpio irq handler, must be called by "irq_handler()"
 * @param[in] none
 * @return none
 */
VOID_T tuya_gpio_irq_handler(VOID_T)
{
	if(reg_irq_src & FLD_IRQ_GPIO_RISC0_EN){
		reg_irq_src = FLD_IRQ_GPIO_RISC0_EN;
        __gpio_irq_rising_handler();
    }
	if(reg_irq_src & FLD_IRQ_GPIO_RISC1_EN){
		reg_irq_src = FLD_IRQ_GPIO_RISC1_EN;
        __gpio_irq_falling_handler();
    }
}
