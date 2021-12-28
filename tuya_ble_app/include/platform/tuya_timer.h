/**
 * @file tuya_timer.h
 * @author lifan
 * @brief tuya timer header file
 * @version 1.0
 * @date 2021-09-18
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_TIMER_H__
#define __TUYA_TIMER_H__

#include "tuya_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef BYTE_T TIMER_RET;
#define TIMER_OK                0x00
#define TIMER_ERR_INTERNAL      0x01
#define TIMER_ERR_INVALID_PARM  0x02
#define TIMER_ERR_RSRC_OCCUPIED 0x03
#define TIMER_ERR_UNDEFINED     0x04

typedef BYTE_T TY_HW_TIMER_TYPE_E;
#define TY_TIMER_0              0x00
#define TY_TIMER_1              0x01
#define TY_TIMER_2              0x02

typedef BYTE_T TY_TIMER_WORK_TYPE_E;
#define TY_TIMER_SINGLE         0x00
#define TY_TIMER_REPEAT         0x01

typedef INT_T (*TY_TIMER_CB)();

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief tuya software timer init
 * @param[in] none
 * @return none
 */
TIMER_RET tuya_software_timer_init(VOID_T);

/**
 * @brief tuya software timer create
 * @param[in] cb_func: callback function
 * @param[in] intv_us: interval time (us)
 * @return none
 */
TIMER_RET tuya_software_timer_create(IN CONST UINT_T intv_us, IN TY_TIMER_CB cb_func);

/**
 * @brief tuya software timer delete
 * @param[in] cb_func: callback function
 * @return none
 */
TIMER_RET tuya_software_timer_delete(IN TY_TIMER_CB cb_func);

/**
 * @brief tuya hardware timer create
 * @param[in] type: timer type
 * @param[in] intv_us: interval time (us)
 * @param[in] cb_func: callback function
 * @param[in] repe: work type, single or repeat
 * @return TIMER_RET
 */
TIMER_RET tuya_hardware_timer_create(IN CONST TY_HW_TIMER_TYPE_E type, IN CONST UINT_T intv_us, IN TY_TIMER_CB cb_func, IN CONST TY_TIMER_WORK_TYPE_E work_type);

/**
 * @brief tuya hardware timer delete
 * @param[in] type: timer type
 * @return TIMER_RET
 */
TIMER_RET tuya_hardware_timer_delete(IN CONST TY_HW_TIMER_TYPE_E type);
/**
 * @brief tuya hardware timer start
 * @param[in] type: timer type
 * @return TIMER_RET
 */
TIMER_RET tuya_hardware_timer_start(IN CONST TY_HW_TIMER_TYPE_E type);

/**
 * @brief tuya hardware timer stop
 * @param[in] type: timer type
 * @return TIMER_RET
 */
TIMER_RET tuya_hardware_timer_stop(IN CONST TY_HW_TIMER_TYPE_E type);

/**
 * @brief tuya timer irq handler, must be called by "irq_handler()"
 * @param[in] none
 * @return none
 */
VOID_T tuya_timer_irq_handler(VOID_T);

/**
 * @brief tuya get clock time
 * @param[in] none
 * @return clock time
 */
UINT_T tuya_get_clock_time(VOID_T);

/**
 * @brief tuya get clock time
 * @param[in] prv_time: previous time
 * @param[in] time_diff_us: time difference for judgment
 * @return TRUE - exceed, FALSE - not exceed
 */
BOOL_T tuya_is_clock_time_exceed(IN CONST UINT_T prv_time, IN CONST UINT_T time_diff_us);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_TIMER_H__ */
