/**
 * @file tuya_demo_key_driver.c
 * @author lifan
 * @brief key driver demo source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_demo_key_driver.h"
#include "tuya_key.h"
#include "tuya_ble_log.h"
#include "tuya_ble_common.h"

/***********************************************************
************************micro define************************
***********************************************************/
#define DP_ID_KEY_EVENT     101

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef BYTE_T KEY_EVENT_E;
#define KEY1_SHORT_PRESS    0x00
#define KEY1_LONG_PRESS1    0x01
#define KEY1_LONG_PRESS2    0x02
#define KEY2_SHORT_PRESS    0x03
#define KEY2_LONG_PRESS1    0x04
#define KEY2_LONG_PRESS2    0x05

/***********************************************************
***********************variable define**********************
***********************************************************/
/* KEY user define */
STATIC VOID_T __key1_cb(KEY_PRESS_TYPE_E type);
STATIC VOID_T __key2_cb(KEY_PRESS_TYPE_E type);
KEY_DEF_T key1_def_s = {
    .port = TY_GPIOB_7,
    .active_low = TRUE,
    .long_press_time1 = 2000,
    .long_press_time2 = 5000,
    .key_cb = __key1_cb
};
KEY_DEF_T key2_def_s = {
    .port = TY_GPIOB_1,
    .active_low = TRUE,
    .long_press_time1 = 8000,
    .long_press_time2 = 3000,
    .key_cb = __key2_cb
};
/* KEY event data */
STATIC KEY_EVENT_E sg_key_event = KEY1_SHORT_PRESS;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief report one dp data
 * @param[in] dp_id: DP ID
 * @param[in] dp_type: DP type
 * @param[in] dp_len: DP data length
 * @param[in] dp_data: DP data
 * @return none
 */
STATIC VOID_T __report_one_dp_data(IN CONST UCHAR_T dp_id, IN CONST UCHAR_T dp_type, IN CONST UCHAR_T dp_len, IN CONST UCHAR_T *dp_data)
{
    UCHAR_T i, repo_array[255+3];

    repo_array[0] = dp_id;
    repo_array[1] = dp_type;
    repo_array[2] = dp_len;
    for (i = 0; i < dp_len; i++) {
        repo_array[3 + i] = *(dp_data + (dp_len-i-1));
    }
    tuya_ble_dp_data_report(repo_array, dp_len + 3);
}

/**
 * @brief key1 callback function
 * @param[in] type: key event type
 * @return none
 */
STATIC VOID_T __key1_cb(KEY_PRESS_TYPE_E type)
{
    switch (type) {
    case SHORT_PRESS:
        sg_key_event = KEY1_SHORT_PRESS;
        TUYA_APP_LOG_INFO("key1 short pressed.");
        break;
    case LONG_PRESS_FOR_TIME1:
        sg_key_event = KEY1_LONG_PRESS1;
        TUYA_APP_LOG_INFO("key1 long pressed for time1.");
        break;
    case LONG_PRESS_FOR_TIME2:
        sg_key_event = KEY1_LONG_PRESS2;
        TUYA_APP_LOG_INFO("key1 long pressed for time2.");
        break;
    default:
        break;
    }
    __report_one_dp_data(DP_ID_KEY_EVENT, DT_ENUM, 1, &sg_key_event);
}

/**
 * @brief key2 callback function
 * @param[in] type: key event type
 * @return none
 */
STATIC VOID_T __key2_cb(KEY_PRESS_TYPE_E type)
{
    switch (type) {
    case SHORT_PRESS:
        sg_key_event = KEY2_SHORT_PRESS;
        TUYA_APP_LOG_INFO("key2 short pressed.");
        break;
    case LONG_PRESS_FOR_TIME1:
        sg_key_event = KEY2_LONG_PRESS1;
        TUYA_APP_LOG_INFO("key2 long pressed for time1.");
        break;
    case LONG_PRESS_FOR_TIME2:
        sg_key_event = KEY2_LONG_PRESS2;
        TUYA_APP_LOG_INFO("key2 long pressed for time2.");
        break;
    default:
        break;
    }
    __report_one_dp_data(DP_ID_KEY_EVENT, DT_ENUM, 1, &sg_key_event);
}

/**
 * @brief key driver init
 * @param[in] none
 * @return none
 */
VOID_T tuya_key_driver_init(VOID_T)
{
    KEY_RET ret = KEY_OK;
    /* register mode key */
    ret = tuya_reg_key(&key1_def_s);
    if (KEY_OK != ret) {
        TUYA_APP_LOG_ERROR("key1 init error: %d", ret);
    }
    /* register reset key */
    ret = tuya_reg_key(&key2_def_s);
    if (KEY_OK != ret) {
        TUYA_APP_LOG_ERROR("key2 init error: %d", ret);
    }
}
