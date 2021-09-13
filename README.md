# Tuya IoTOS Embeded Demo Bluetooth LE Key Driver

[English](./README.md) | [中文](./README_zh.md)

<br>

## Introduction

This demo is based on [Tuya IoT Platform](https://iot.tuya.com/), Tuya Smart APP, IoTOS Embeded Ble SDK, using Tuya BLE series modules and 2 keys to quickly build a key driver program. When the key event occurs, the device will update the key information to the Tuya Smart APP for display. The demo provides a key registration interface, and the user can set the key input pin, active level, long press time (2 options) and key trigger callback function.

<br>


## Quick start

### Development environment setup

- Install the integrated development environment (IDE) as per the requirements of the original chip SDK.
- Find the download URL of the Tuya BLE SDK Demo Project from the following table. Refer to the `README.md` file under each branch to import the project.

|   Platform   |  Model   |                       Download Address                       |
| :----------: | :------: | :----------------------------------------------------------: |
|    Nordic    | nrf52832 | [tuya_ble_sdk_Demo_Project_nrf52832.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_nrf52832.git) |
|   Realtek    | RTL8762C | [tuya_ble_sdk_Demo_Project_rtl8762c.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_rtl8762c.git) |
|    Telink    | TLSR825x | [tuya_ble_sdk_Demo_Project_tlsr8253.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_tlsr8253.git) |
| Silicon Labs |   BG21   |                         Coming soon.                         |
|    Beken     | BK3431Q  | [Tuya_ble_sdk_demo_project_bk3431q.git](https://github.com/TuyaInc/Tuya_ble_sdk_demo_project_bk3431q.git) |
|    Beken     |  BK3432  | [ tuya_ble_sdk_Demo_Project_bk3432.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_bk3432.git) |
|   Cypress    |  Psoc63  | [tuya_ble_sdk_Demo_Project_PSoC63.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_PSoC63.git) |

<br>

### Compile and burn

- Edit code

  1. In `tuya_ble_app_demo.h`, specify the PID of the product you have created on the [Tuya IoT Platform](https://iot.tuya.com/).

     ```
      #define APP_PRODUCT_ID     "xxxxxxxx"
     ```

     Change `xxxxxxxx` to the PID.

  2. In `tuya_ble_app_demo.h`, specify the `authkey` and `UUID`.

     ```
      static const char auth_key_test[] = "yyyyyyyy";
      static const char device_id_test[] = "zzzzzzzz";
     ```

     Change `yyyyyyyy` to the `authkey` and `zzzzzzzz` to the `UUID`.

- Compile code

  Compile the edited code, download the code to the hardware, and run it. You may need to download the stack and bootloader depending on your chip models. Check the logs and use the third-party Bluetooth debugging app (such as LightBlue for iOS) to verify the Bluetooth broadcast.

<br>

### File description
```
├── src         /* Source code files */
|    ├── sdk
|    |    └── tuya_uart_common_handler.c        /* Code for UART communication */
|    ├── driver
|    |    └── tuya_key.c                        /* Key driver */
|    ├── tuya_ble_app_demo.c                    /* Entry file of application layer */
|    └── tuya_demo_key_driver.c                 /* Key driver application demo code */
|
└── include     /* Header files */
     ├── common
     |    └── tuya_common.h                     /* Common types and marco define */
     ├── sdk
     |    ├── custom_app_uart_common_handler.h  /* Code for UART communication */
     |    ├── custom_app_product_test.h         /* Implementation of custom production test items */
     |    └── custom_tuya_ble_config.h          /* Application configuration file */
     ├── driver
     |    └── tuya_key.h                        /* Key driver */
     ├── tuya_ble_app_demo.h                    /* Entry file of application layer */
     └── tuya_demo_key_driver.h                	/* Key driver application demo code */
```

<br>

### Application entry

Entry file: /tuya_ble_app/tuya_ble_app_demo.c

- `void tuya_ble_app_init(void)` is executed to initialize Tuya IoTOS Embedded Bluetooth LE SDK. This function is executed only once.
- `void app_exe()` is used to execute the application code. It is executed in a loop.

<br>

### Data point (DP)

|   Function name    | tuya_ble_dp_data_report                                      |
| :----------------: | :----------------------------------------------------------- |
| Function prototype | tuya_ble_status_t tuya_ble_dp_data_report(uint8_t *p_data,uint32_t len); |
|  Feature overview  | Reports DP data.                                             |
|     Parameter      | `p_data [in]`: DP data. `len[in]`: data length. It cannot exceed `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`. |
|    Return value    | `TUYA_BLE_SUCCESS`: sent successfully. <br/>`TUYA_BLE_ERR_INVALID_PARAM`: invalid parameter. <br/>`TUYA_BLE_ERR_INVALID_STATE`: failed to send data due to the current Bluetooth connection, such as Bluetooth disconnected. <br/>`TUYA_BLE_ERR_NO_MEM`: failed to request memory resources. <br/>`TUYA_BLE_ERR_INVALID_LENGTH`: data length error. <br/>`TUYA_BLE_ERR_NO_EVENT`: other errors. |
|      Remarks       | `application` calls this function to send DP data to the mobile app. |

Parameter description:

The [Tuya IoT Platform](https://iot.tuya.com/) manages data through DPs. The data generated by any device is abstracted into a DP. DP data consists of four parts, as described below.

- `Dp_id`: the DP ID of a data point defined on the Tuya IoT platform. It is one byte.

- `Dp_type`: the data type. It is one byte.
  - `#define DT_RAW 0`: raw type.
  - `#define DT_BOOL 1`: Boolean type.
  - `#define DT_VALUE 2`: value type. The value range is specified when a DP of value type is created on the Tuya IoT Platform.
  - `#define DT_STRING 3`: string type.
  - `#define DT_ENUM 4`: enum type.
  - `#define DT_BITMAP 5`: bitmap type.
- `Dp_len`: It can be one byte or two bytes. Currently, Bluetooth only supports one byte, so the data of a single DP can be up to 255 bytes.

- `Dp_data`: the DP data, with `dp_len` byte(s).

The data that the parameter `p_data` points to must be packaged in the following format for reporting.

| DP 1 data |         |        |         |  –   | DP n data |         |        |         |
| :-------: | :-----: | :----: | :-----: | :--: | :-------: | :-----: | :----: | :-----: |
|     1     |    2    |   3    |    4    |  –   |     n     |   n+1   |  n+2   |   n+3   |
|   Dp_id   | Dp_type | Dp_len | Dp_data |  –   |   Dp_id   | Dp_type | Dp_len | Dp_data |

When this function is called, the maximum data length of is `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`, which is 255+3 currently.

<br>

### Pin configuration

| Peripheral | I/O |
| ----- | ---- |
| KEY1 | PB7 |
| KEY2 | PB1 |

<br>

## Reference

- [BLE SDK Guide](https://developer.tuya.com/en/docs/iot/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9#title-13-The%20callback%20event%20of%20tuya%20ble%20sdk)
- [BLE SDK Demo Overview](https://developer.tuya.com/en/docs/iot/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o)
- [Tuya Project Hub](https://developer.tuya.com/demo)

<br>

## Technical Support

You can get support from Tuya with the following methods:

+ [Tuya AI+IoT Developer Platform](https://developer.tuya.com/en/)
+ [Help Center](https://support.tuya.com/en/help)
+ [Service & Support](https://service.console.tuya.com)

<br>