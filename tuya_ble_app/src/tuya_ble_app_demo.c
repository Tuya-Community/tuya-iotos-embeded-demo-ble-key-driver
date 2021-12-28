/**
 * @file tuya_ble_app_demo.c
 * @author www.tuya.com
 * @brief application demo main file, storing SDK initialization functions, message processing functions, etc.
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_heap.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_api.h"
#include "tuya_ble_port.h"
#include "tuya_ble_main.h"
#include "tuya_ble_secure.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_sdk_version.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_event.h"
#include "tuya_ble_app_demo.h"
#include "tuya_ble_log.h"
#include "tuya_ota.h"

#include "tuya_ble_common.h"
#include "tuya_demo_key_driver.h"
#include "tuya_timer.h"

/***********************************************************
************************micro define************************
***********************************************************/
#define APP_CUSTOM_EVENT_1  1
#define APP_CUSTOM_EVENT_2  2
#define APP_CUSTOM_EVENT_3  3
#define APP_CUSTOM_EVENT_4  4
#define APP_CUSTOM_EVENT_5  5

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct {
    uint8_t data[50];
} custom_data_type_t;

/***********************************************************
***********************variable define**********************
***********************************************************/
static tuya_ble_device_param_t device_param = {0};

static const char auth_key_test[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
static const char device_id_test[] = "xxxxxxxxxxxxxxxx";
static const uint8_t mac_test[6] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB}; // The actual MAC address is : AB:89:67:45:23:01

//static uint8_t dp_data_test[8] = {0x6A, 0x05, 0x05, 0x00, 0x00, 0x00, 0x80, 0x02};
static uint8_t dp_data_array[255+3];
static uint16_t dp_data_len = 0;

custom_data_type_t custom_data;

static uint16_t sn = 0;
static uint32_t time_stamp = 1587795793;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief custom_data_process
 * @param[in] evt_id
 * @param[in] data
 * @return none
 */
void custom_data_process(int32_t evt_id, void *data)
{
    custom_data_type_t *event_1_data;
    TUYA_APP_LOG_DEBUG("custom event id = %d", evt_id);
    switch (evt_id) {
    case APP_CUSTOM_EVENT_1:
        event_1_data = (custom_data_type_t *)data;
        TUYA_APP_LOG_HEXDUMP_DEBUG("received APP_CUSTOM_EVENT_1 data:",event_1_data->data,50);
        break;
    case APP_CUSTOM_EVENT_2:
        break;
    case APP_CUSTOM_EVENT_3:
        break;
    case APP_CUSTOM_EVENT_4:
        break;
    case APP_CUSTOM_EVENT_5:
        break;
    default:
        break;
    }
}

/**
 * @brief custom_evt_1_send_test
 * @param[in] data
 * @return none
 */
void custom_evt_1_send_test(uint8_t data)
{
    tuya_ble_custom_evt_t event;

    for (uint8_t i=0; i<50; i++) {
        custom_data.data[i] = data;
    }
    event.evt_id = APP_CUSTOM_EVENT_1;
    event.custom_event_handler = (void *)custom_data_process;
    event.data = &custom_data;
    tuya_ble_custom_event_send(event);
}

/**
 * @brief tuya_ble_get_mac
 * @param[in] mac[6]
 * @return none
 */
void tuya_ble_get_mac(uint8_t mac[6])
{
	memcpy(mac, tuya_ble_current_para.auth_settings.mac, 6);
}

/**
 * @brief tuya_cb_handler
 * @param[in] event
 * @return none
 */
static void tuya_cb_handler(tuya_ble_cb_evt_param_t* event)
{
    int16_t result = 0;
    switch (event->evt) {
    case TUYA_BLE_CB_EVT_CONNECTE_STATUS:
    	//tuya_uart_send_ble_state();
        TUYA_APP_LOG_INFO("received tuya ble conncet status update event, current connect status = %d", event->connect_status);
        break;
    case TUYA_BLE_CB_EVT_DP_WRITE:
        dp_data_len = event->dp_write_data.data_len;
        memset(dp_data_array, 0, sizeof(dp_data_array));
        memcpy(dp_data_array, event->dp_write_data.p_data, dp_data_len);
        TUYA_APP_LOG_HEXDUMP_DEBUG("received dp write data :", dp_data_array, dp_data_len);
        sn = 0;
        tuya_ble_dp_data_report(dp_data_array, dp_data_len);//1
        //custom_evt_1_send_test(dp_data_len);
        //tuya_ble_dp_data_report(dp_data_test, sizeof(dp_data_test));
        ///tuya_uart_send_ble_dpdata(dp_data_array, dp_data_len);
        break;
    case TUYA_BLE_CB_EVT_DP_DATA_REPORT_RESPONSE:
        TUYA_APP_LOG_INFO("received dp data report response result code =%d", event->dp_response_data.status);
        //tuya_ble_dp_data_with_flag_report(sn, REPORT_FOR_CLOUD_PANEL, dp_data_array, dp_data_len); //2       
        //sn++;
        break;
    case TUYA_BLE_CB_EVT_DP_DATA_WTTH_TIME_REPORT_RESPONSE:
        TUYA_APP_LOG_INFO("received dp data report response result code =%d", event->dp_response_data.status);
        break;
    case TUYA_BLE_CB_EVT_DP_DATA_WITH_FLAG_REPORT_RESPONSE:
        TUYA_APP_LOG_INFO("received dp data with flag report response sn = %d , flag = %d , result code =%d",
                          event->dp_with_flag_response_data.sn,
                          event->dp_with_flag_response_data.mode,
                          event->dp_with_flag_response_data.status);
        if (event->dp_with_flag_response_data.mode == REPORT_FOR_CLOUD_PANEL) {
            tuya_ble_dp_data_with_flag_report(sn, REPORT_FOR_CLOUD, dp_data_array, dp_data_len);//3
        } else if(event->dp_with_flag_response_data.mode == REPORT_FOR_CLOUD) {
            tuya_ble_dp_data_with_flag_report(sn, REPORT_FOR_PANEL, dp_data_array, dp_data_len);//4
        } else if(event->dp_with_flag_response_data.mode == REPORT_FOR_PANEL) {
            tuya_ble_dp_data_with_flag_report(sn, REPORT_FOR_NONE, dp_data_array, dp_data_len);//5
        } else if(event->dp_with_flag_response_data.mode == REPORT_FOR_NONE) {
            tuya_ble_dp_data_with_flag_and_time_report(sn, REPORT_FOR_CLOUD_PANEL, time_stamp, dp_data_array, dp_data_len);//6
        } else {

        }
        sn++;
        break;
    case TUYA_BLE_CB_EVT_DP_DATA_WITH_FLAG_AND_TIME_REPORT_RESPONSE:
        TUYA_APP_LOG_INFO("received dp data with flag and time report response sn = %d , flag = %d , result code =%d",
                          event->dp_with_flag_and_time_response_data.sn,
                          event->dp_with_flag_and_time_response_data.mode,
                          event->dp_with_flag_and_time_response_data.status);
        if(event->dp_with_flag_and_time_response_data.mode == REPORT_FOR_CLOUD_PANEL) {
            tuya_ble_dp_data_with_flag_and_time_report(sn, REPORT_FOR_CLOUD,time_stamp, dp_data_array,dp_data_len);//7
        } else if(event->dp_with_flag_and_time_response_data.mode == REPORT_FOR_CLOUD) {
            tuya_ble_dp_data_with_flag_and_time_report(sn, REPORT_FOR_PANEL, time_stamp, dp_data_array, dp_data_len);//8
        } else if(event->dp_with_flag_and_time_response_data.mode == REPORT_FOR_PANEL) {
            tuya_ble_dp_data_with_flag_and_time_report(sn, REPORT_FOR_NONE, time_stamp, dp_data_array, dp_data_len);//9
        } else {

        }
        sn++;
        break;
    case TUYA_BLE_CB_EVT_UNBOUND:
        TUYA_APP_LOG_INFO("received unbound req");
        break;
    case TUYA_BLE_CB_EVT_ANOMALY_UNBOUND:
        TUYA_APP_LOG_INFO("received anomaly unbound req");
        break;
    case TUYA_BLE_CB_EVT_DEVICE_RESET:
        TUYA_APP_LOG_INFO("received device reset req");
        break;
    case TUYA_BLE_CB_EVT_DP_QUERY:
        TUYA_APP_LOG_INFO("received TUYA_BLE_CB_EVT_DP_QUERY event");
        uart_to_ble_enable = 1;
        //tuya_ble_dp_data_report(dp_data_array, dp_data_len);
        break;
    case TUYA_BLE_CB_EVT_OTA_DATA:
        tuya_ota_proc(event->ota_data.type, event->ota_data.p_data, event->ota_data.data_len);
        break;
    case TUYA_BLE_CB_EVT_NETWORK_INFO:
        TUYA_APP_LOG_INFO("received net info : %s", event->network_data.p_data);
        tuya_ble_net_config_response(result);
        break;
    case TUYA_BLE_CB_EVT_WIFI_SSID:
        break;
    case TUYA_BLE_CB_EVT_TIME_STAMP:
        TUYA_APP_LOG_INFO("received unix timestamp : %s ,time_zone : %d", event->timestamp_data.timestamp_string, event->timestamp_data.time_zone);
        break;
    case TUYA_BLE_CB_EVT_TIME_NORMAL:
        break;
    case TUYA_BLE_CB_EVT_DATA_PASSTHROUGH:
        TUYA_APP_LOG_HEXDUMP_DEBUG("received ble passthrough data :", event->ble_passthrough_data.p_data, event->ble_passthrough_data.data_len);
        tuya_ble_data_passthrough(event->ble_passthrough_data.p_data, event->ble_passthrough_data.data_len);
        break;
    default:
        TUYA_APP_LOG_WARNING("app_tuya_cb_queue msg: unknown event type 0x%04x", event->evt);
        break;
    }
}

/**
 * @brief tuya_ble_app_init
 * @param[in] none
 * @return none
 */
void tuya_ble_app_init(void)
{
    device_param.device_id_len = 16;    //If use the license stored by the SDK,initialized to 0, Otherwise 16 or 20.

    if (device_param.device_id_len == 16) {
        memcpy(device_param.auth_key, (void *)auth_key_test, AUTH_KEY_LEN);
        memcpy(device_param.device_id, (void *)device_id_test, DEVICE_ID_LEN);
        memcpy(device_param.mac_addr.addr, mac_test, 6);
        device_param.mac_addr.addr_type = TUYA_BLE_ADDRESS_TYPE_RANDOM;
    }
    device_param.p_type = TUYA_BLE_PRODUCT_ID_TYPE_PID;
    device_param.product_id_len = 8;
    memcpy(device_param.product_id, APP_PRODUCT_ID, 8);
    device_param.firmware_version = TY_APP_VER_NUM;
    device_param.hardware_version = TY_HARD_VER_NUM;

    tuya_ble_sdk_init(&device_param);
    tuya_ble_callback_queue_register(tuya_cb_handler);

    tuya_ota_init();
	app_ringbuffer_init();

    tuya_log_init();
    elog_set_output_enabled(false);

    //tuya_flash_init();
    tuya_ota_init();

    ty_factory_flag = 0;
    //tuya_timer_start(TIMER_FIRST,1000);

    tuya_print_sysInfor();
    TUYA_APP_LOG_INFO("app version : "TY_APP_VER_STR);

    tuya_software_timer_init();
    tuya_key_driver_init();
}

/**
 * @brief app_exe
 * @param[in] none
 * @return none
 */
void app_exe()
{

}

/**
 * @brief called by "irq_handler()"
 * @param[in] none
 * @return none
 */
void tuya_ble_app_irq_handler(void)
{
    tuya_timer_irq_handler();
}
