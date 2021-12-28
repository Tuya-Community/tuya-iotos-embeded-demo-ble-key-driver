/**
 * @file tuya_timer.c
 * @author lifan
 * @brief tuya timer source file for TLSR825x
 * @version 1.0
 * @date 2021-09-18
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_timer.h"
#include "tuya_ble_log.h"
#include "blt_soft_timer.h"
#include "timer.h"

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef BYTE_T TY_TIMER_STAT_E;
#define TY_TIMER_UNUSED      0x00
#define TY_TIMER_USED_IDLE   0x01
#define TY_TIMER_USED_BUSY   0x02

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC TY_TIMER_STAT_E sg_hw_timer_status[3] = {0, 0, 0};
STATIC TY_TIMER_WORK_TYPE_E sg_hw_timer_work_type[3] = {0, 0, 0};
STATIC TY_TIMER_CB sg_hw_timer_cb_lst[3] = {NULL, NULL, NULL};

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief tuya software timer init
 * @param[in] none
 * @return TIMER_RET
 */
TIMER_RET tuya_software_timer_init(VOID_T)
{
    blt_soft_timer_init();
    return TIMER_OK;
}

/**
 * @brief tuya software timer create
 * @param[in] cb_func: callback function
 * @param[in] intv_us: interval time (us)
 * @return TIMER_RET
 */
TIMER_RET tuya_software_timer_create(IN CONST UINT_T intv_us, IN TY_TIMER_CB cb_func)
{
    if (FALSE == blt_soft_timer_add(cb_func, intv_us)) {
        TUYA_APP_LOG_INFO("Software timer create failed.");
        return TIMER_ERR_INTERNAL;
    }
    return TIMER_OK;
}

/**
 * @brief tuya software timer delete
 * @param[in] cb_func: callback function
 * @return TIMER_RET
 */
TIMER_RET tuya_software_timer_delete(IN TY_TIMER_CB cb_func)
{
    if (FALSE == blt_soft_timer_delete(cb_func)) {
        TUYA_APP_LOG_INFO("Software timer delete failed.");
        return TIMER_ERR_INTERNAL;
    }
    return TIMER_OK;
}

/**
 * @brief is hardware timer used
 * @param[in] type: timer type
 * @return TRUE - used, FALSE - not used
 */
STATIC BOOL_T __is_hardware_timer_used(IN CONST TY_HW_TIMER_TYPE_E type)
{
    if (sg_hw_timer_status[type] > TY_TIMER_UNUSED) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief is hardware timer busy
 * @param[in] type: timer type
 * @return TRUE - busy, FALSE - not busy
 */
STATIC BOOL_T __is_hardware_timer_busy(IN CONST TY_HW_TIMER_TYPE_E type)
{
    if (sg_hw_timer_status[type] == TY_TIMER_USED_BUSY) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief mask hardware timer status
 * @param[in] type: timer type
 * @param[in] stat: timer status
 * @return none
 */
STATIC VOID_T __mask_hardware_timer_status(IN CONST TY_HW_TIMER_TYPE_E type, IN CONST TY_TIMER_STAT_E stat)
{
    sg_hw_timer_status[type] = stat;
}

/**
 * @brief tuya hardware timer create
 * @param[in] type: timer type
 * @param[in] intv_us: interval time (us)
 * @param[in] cb_func: callback function
 * @param[in] work_type: work type, single or repeat
 * @return TIMER_RET
 */
TIMER_RET tuya_hardware_timer_create(IN CONST TY_HW_TIMER_TYPE_E type, IN CONST UINT_T intv_us, IN TY_TIMER_CB cb_func, IN CONST TY_TIMER_WORK_TYPE_E work_type)
{
    if (type > TY_TIMER_2) {
        return TIMER_ERR_INVALID_PARM;
    }
    if (TRUE == __is_hardware_timer_used(type)) {
        return TIMER_ERR_RSRC_OCCUPIED;
    }

    switch (type) {
    case TY_TIMER_0:
        timer0_set_mode(TIMER_MODE_SYSCLK, 0, (intv_us * CLOCK_SYS_CLOCK_1US));
        break;
    case TY_TIMER_1:
        timer1_set_mode(TIMER_MODE_SYSCLK, 0, (intv_us * CLOCK_SYS_CLOCK_1US));
        break;
    case TY_TIMER_2:
        timer2_set_mode(TIMER_MODE_SYSCLK, 0, (intv_us * CLOCK_SYS_CLOCK_1US));
        break;
    default:
        break;
    }
    sg_hw_timer_cb_lst[type] = cb_func;
    sg_hw_timer_work_type[type] = work_type;

    timer_start(type);
    __mask_hardware_timer_status(type, TY_TIMER_USED_BUSY);

    return TIMER_OK;
}

/**
 * @brief tuya hardware timer delete
 * @param[in] type: timer type
 * @return TIMER_RET
 */
TIMER_RET tuya_hardware_timer_delete(IN CONST TY_HW_TIMER_TYPE_E type)
{
    if (type > TY_TIMER_2) {
        return TIMER_ERR_INVALID_PARM;
    }
    if (FALSE == __is_hardware_timer_used(type)) {
        return TIMER_ERR_UNDEFINED;
    }
    timer_stop(type);
    __mask_hardware_timer_status(type, TY_TIMER_UNUSED);
    return TIMER_OK;
}

/**
 * @brief tuya hardware timer start
 * @param[in] type: timer type
 * @return TIMER_RET
 */
TIMER_RET tuya_hardware_timer_start(IN CONST TY_HW_TIMER_TYPE_E type)
{
    if (type > TY_TIMER_2) {
        return TIMER_ERR_INVALID_PARM;
    }
    if (FALSE == __is_hardware_timer_used(type)) {
        return TIMER_ERR_UNDEFINED;
    }

    timer_start(type);
    __mask_hardware_timer_status(type, TY_TIMER_USED_BUSY);

    return TIMER_OK;
}

/**
 * @brief tuya hardware timer stop
 * @param[in] type: timer type
 * @return TIMER_RET
 */
TIMER_RET tuya_hardware_timer_stop(IN CONST TY_HW_TIMER_TYPE_E type)
{
    if (type > TY_TIMER_2) {
        return TIMER_ERR_INVALID_PARM;
    }
    if (FALSE == __is_hardware_timer_used(type)) {
        return TIMER_ERR_UNDEFINED;
    }
    timer_stop(type);
    __mask_hardware_timer_status(type, TY_TIMER_USED_IDLE);
    return TIMER_OK;
}

/**
 * @brief hardware timer irq handler
 * @param[in] type: timer type
 * @return none
 */
STATIC VOID_T __hardware_timer_irq_handler(IN CONST TY_HW_TIMER_TYPE_E type)
{
    if (sg_hw_timer_work_type[type] == TY_TIMER_SINGLE) {
        timer_stop(type);
        __mask_hardware_timer_status(type, TY_TIMER_USED_IDLE);
    }
    if (sg_hw_timer_cb_lst[type] != NULL) {
        sg_hw_timer_cb_lst[type]();
    }
}

/**
 * @brief tuya timer irq handler, must be called by "irq_handler()"
 * @param[in] none
 * @return none
 */
VOID_T tuya_timer_irq_handler(VOID_T)
{
	if(reg_tmr_sta & FLD_TMR_STA_TMR0){
		reg_tmr_sta = FLD_TMR_STA_TMR0;
        __hardware_timer_irq_handler(TY_TIMER_0);
    }
	if(reg_tmr_sta & FLD_TMR_STA_TMR1){
		reg_tmr_sta = FLD_TMR_STA_TMR1;
        __hardware_timer_irq_handler(TY_TIMER_1);
    }
	if(reg_tmr_sta & FLD_TMR_STA_TMR2){
		reg_tmr_sta = FLD_TMR_STA_TMR2;
        __hardware_timer_irq_handler(TY_TIMER_2);
    }
}

/**
 * @brief tuya get clock time
 * @param[in] none
 * @return clock time
 */
UINT_T tuya_get_clock_time(VOID_T)
{
	return clock_time();
}

/**
 * @brief tuya get clock time
 * @param[in] prv_time: previous time
 * @param[in] time_diff_us: time difference for judgment
 * @return TRUE - exceed, FALSE - not exceed
 */
BOOL_T tuya_is_clock_time_exceed(IN CONST UINT_T prv_time, IN CONST UINT_T time_diff_us)
{
	if (clock_time_exceed(prv_time, time_diff_us)) {
        return TRUE;
    } else {
        return FALSE;
    }
}
