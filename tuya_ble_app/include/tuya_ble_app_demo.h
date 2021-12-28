/**
 * @file tuya_ble_app_demo.h
 * @author www.tuya.com
 * @brief application demo header file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_BLE_APP_DEMO_H__
#define __TUYA_BLE_APP_DEMO_H__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
#define APP_PRODUCT_ID      "xxxxxxxx"
#define APP_BUILD_FIRMNAME  "tuya_ble_sdk_demo_tlsr825x"

#define TY_APP_VER_NUM      0x0101
#define TY_APP_VER_STR      "1.1"

#define TY_HARD_VER_NUM     0x0100
#define TY_HARD_VER_STR     "1.0"

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief tuya_ble_app_init
 * @param[in] none
 * @return none
 */
void tuya_ble_app_init(void);

/**
 * @brief called by "irq_handler()"
 * @param[in] none
 * @return none
 */
void tuya_ble_app_irq_handler(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_BLE_APP_DEMO_H__ */
