/**
 * @file tuya_key.c
 * @author lifan
 * @brief key driver source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_key.h"
#include "tuya_ble_mem.h"
#include "tuya_timer.h"

/***********************************************************
************************micro define************************
***********************************************************/
#define KEY_SCAN_CYCLE_MS       10
#define KEY_PRESS_SHORT_TIME    50

/***********************************************************
***********************typedef define***********************
***********************************************************/
/* Key status */
typedef struct {
    BOOL_T cur_stat;
    BOOL_T prv_stat;
    UINT_T cur_time;
    UINT_T prv_time;
} KEY_STATUS_T;

/* Key manage */
typedef struct key_manage_s {
    struct key_manage_s *next;
    KEY_DEF_T *key_def_s;
    KEY_STATUS_T key_status_s;
} KEY_MANAGE_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC KEY_MANAGE_T *sg_key_mag_list = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/
STATIC INT_T __key_timeout_handler(VOID_T);

/**
 * @brief key gpio init
 * @param[in] pin: pin number
 * @param[in] active_low: TRUE - active low, FALSE - active high
 * @return none
 */
STATIC VOID_T __key_gpio_init(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T active_low)
{
    tuya_gpio_init(port, TRUE, active_low);
}

/**
 * @brief key register
 * @param[in] key_def: user key define
 * @return KEY_RET
 */
KEY_RET tuya_reg_key(IN KEY_DEF_T *key_def)
{
    /* check callback function */
    if (key_def->key_cb == NULL) {
        return KEY_ERR_CB_UNDEFINED;
    }

    /* allocate and clear for key_mag */
    KEY_MANAGE_T *key_mag = (KEY_MANAGE_T *)tuya_ble_malloc(SIZEOF(KEY_MANAGE_T));
    if (NULL == key_mag) {
        return KEY_ERR_MALLOC_FAILED;
    }

    /* update key manage list */
    key_mag->key_def_s = key_def;
    if (sg_key_mag_list) {
    	key_mag->next = sg_key_mag_list;
    } else {
        tuya_software_timer_create(KEY_SCAN_CYCLE_MS*1000, __key_timeout_handler);
    }
    sg_key_mag_list = key_mag;

    /* gpio init */
    __key_gpio_init(key_def->port, key_def->active_low);

    return KEY_OK;
}

/**
 * @brief key reset
 * @param[in] none
 * @return KEY_RET
 */
KEY_RET tuya_key_reset(VOID_T)
{
    KEY_MANAGE_T *key_mag_tmp = sg_key_mag_list;
    if (NULL == key_mag_tmp) {
        return KEY_ERR_CB_UNDEFINED;
    }
    while (key_mag_tmp) {
        __key_gpio_init(key_mag_tmp->key_def_s->port, key_mag_tmp->key_def_s->active_low);
        key_mag_tmp = key_mag_tmp->next;
    }
    tuya_software_timer_delete(__key_timeout_handler);
    tuya_software_timer_create(KEY_SCAN_CYCLE_MS*1000, __key_timeout_handler);
    return KEY_OK;
}

/**
 * @brief get the real-time status of the key
 * @param[in] port: key port
 * @param[in] active_low: TURE - active low, FALSE - active high
 * @return key_stat: TRUE - press, FALSE - release
 */
STATIC BOOL_T __get_key_stat(IN CONST TY_GPIO_PORT_E port, IN CONST UCHAR_T active_low)
{
    BOOL_T key_stat;
    if (active_low) {
        key_stat = tuya_gpio_read(port) == 0 ? TRUE : FALSE;
    } else {
        key_stat = tuya_gpio_read(port) == 0 ? FALSE : TRUE;
    }
    return key_stat;
}

/**
 * @brief update key status
 * @param[inout] key_mag: key manage information
 * @return none
 */
STATIC VOID_T __update_key_status(INOUT KEY_MANAGE_T *key_mag)
{
    BOOL_T key_stat;
    /* save previous status */
    key_mag->key_status_s.prv_stat = key_mag->key_status_s.cur_stat;
    key_mag->key_status_s.prv_time = key_mag->key_status_s.cur_time;
    /* get the real-time status */
    key_stat = __get_key_stat(key_mag->key_def_s->port, key_mag->key_def_s->active_low);
	/* update current status */
    if (key_stat != key_mag->key_status_s.cur_stat) {
        key_mag->key_status_s.cur_stat = key_stat;
        key_mag->key_status_s.cur_time = 0;
    } else {
        key_mag->key_status_s.cur_time += KEY_SCAN_CYCLE_MS;
    }
}

/**
 * @brief is key press over time
 * @param[in] key_status_s: key status
 * @param[in] over_time: over time used for judgment
 * @return TRUE or FALSE
 */
STATIC BOOL_T __is_key_press_over_time(IN CONST KEY_STATUS_T key_status_s, IN CONST UINT_T over_time)
{
    if (key_status_s.cur_stat == TRUE) {
        if ((key_status_s.cur_time >= over_time) &&
            (key_status_s.prv_time < over_time)) {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * @brief is key from release to release over time and less time
 * @param[in] key_status_s: key status
 * @param[in] over_time: over time used for judgment
 * @param[in] less_time: less time used for judgment
 * @return TRUE or FALSE
 */
STATIC BOOL_T __is_key_release_to_release_over_time_and_less_time(IN CONST KEY_STATUS_T key_status_s, IN CONST UINT_T over_time, IN CONST UINT_T less_time)
{
    if ((key_status_s.prv_stat == TRUE) &&
        (key_status_s.cur_stat == FALSE)) {
        if ((key_status_s.prv_time >= over_time) &&
            (key_status_s.prv_time < less_time)) {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * @brief detect and handle key event
 * @param[inout] key_mag: key manage information
 * @return none
 */
STATIC VOID_T __detect_and_handle_key_event(INOUT KEY_MANAGE_T *key_mag)
{
    KEY_PRESS_TYPE_E type;
    BOOL_T time_exchange;
    UINT_T long_time1, long_time2;

    /* compare long_time1 and long time2 */
    if (key_mag->key_def_s->long_press_time2 >= key_mag->key_def_s->long_press_time1) {
        long_time1 = key_mag->key_def_s->long_press_time1;
        long_time2 = key_mag->key_def_s->long_press_time2;
        time_exchange = FALSE;
    } else {
        long_time1 = key_mag->key_def_s->long_press_time2;
        long_time2 = key_mag->key_def_s->long_press_time1;
        time_exchange = TRUE;
    }
    /* judge key status */
    if ((long_time2 != 0) && (long_time1 != 0)) {
        if (__is_key_press_over_time(key_mag->key_status_s, long_time2)) {
            type = LONG_PRESS_FOR_TIME2;
            goto KEY_EVENT;
        }
        if (__is_key_release_to_release_over_time_and_less_time(key_mag->key_status_s, long_time1, long_time2)) {
            type = LONG_PRESS_FOR_TIME1;
            goto KEY_EVENT;
        }
        if (__is_key_release_to_release_over_time_and_less_time(key_mag->key_status_s, KEY_PRESS_SHORT_TIME, long_time1)){
            type = SHORT_PRESS;
            goto KEY_EVENT;
        }
    } else if ((long_time2 != 0) && (long_time1 == 0)) {
        if (__is_key_press_over_time(key_mag->key_status_s, long_time2)) {
            type = LONG_PRESS_FOR_TIME2;
            goto KEY_EVENT;
        }
        if (__is_key_release_to_release_over_time_and_less_time(key_mag->key_status_s, KEY_PRESS_SHORT_TIME, long_time2)){
            type = SHORT_PRESS;
            goto KEY_EVENT;
        }
    } else if ((long_time2 == 0) && (long_time1 != 0)) {
        if (__is_key_press_over_time(key_mag->key_status_s, long_time1)) {
            type = LONG_PRESS_FOR_TIME1;
            goto KEY_EVENT;
        }
        if (__is_key_release_to_release_over_time_and_less_time(key_mag->key_status_s, KEY_PRESS_SHORT_TIME, long_time1)){
            type = SHORT_PRESS;
            goto KEY_EVENT;
        }
    } else {
        if (__is_key_press_over_time(key_mag->key_status_s, KEY_PRESS_SHORT_TIME)) {
            type = SHORT_PRESS;
            goto KEY_EVENT;
        }
    }
    return;
    /* handle key event */
KEY_EVENT:
    /* if the time parameter has been exchanged before the judgment */
    if (time_exchange) {
        /* exchanged the event type  */
        if (type == LONG_PRESS_FOR_TIME2) {
            type = LONG_PRESS_FOR_TIME1;
        } else if (type == LONG_PRESS_FOR_TIME1) {
            type = LONG_PRESS_FOR_TIME2;
        } else {
            ;
        }
    }
    /* execute the callback function */
    key_mag->key_def_s->key_cb(type);
}

/**
 * @brief key timeout handler
 * @param[in] none
 * @return none
 */
STATIC INT_T __key_timeout_handler(VOID_T)
{
    KEY_MANAGE_T *key_mag_tmp = sg_key_mag_list;
    if (NULL == key_mag_tmp) {
        return 0;
    }
    while (key_mag_tmp) {
        __update_key_status(key_mag_tmp);
        __detect_and_handle_key_event(key_mag_tmp);
        key_mag_tmp = key_mag_tmp->next;
    }
    return 0;
}
