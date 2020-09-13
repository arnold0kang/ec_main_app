//
// Created by arnold on 3/21/2018.
//

#ifndef EC_MAIN_APP_EC_EVENT_DEFINE_H
#define EC_MAIN_APP_EC_EVENT_DEFINE_H



typedef enum {
    EC_CONF_VIDEO_CHANGED,
    EC_CONF_AUDIO_CHANGED,
    EC_CONF_SNAP_CHANGED,
    EC_CONF_OSD_CHANGED,
    EC_CONF_RECODER_CHANGED,
    EC_CONF_HB_CHANGED,
    EC_CONF_SERVER_CHANGED,
    EC_CONF_DEV_CHANGED,
    EC_EVENT_WIFI_ON,
    EC_EVENT_WIFI_OFF,
    EC_EVENT_WIFI_CONNECT,
    EC_EVENT_WIFI_DISCONNECT,
    EC_EVENT_DATA_IN_TRANS,
    EC_EVENT_DATA_OUT_TRANS,
    EC_EVENT_REBOOT,
    EC_EVENT_DISK_FULL,
    EC_EVENT_DISK_ENOUGH,
    EC_EVENT_RECODER_START,
    EC_EVENT_RECODER_STOP,
    EC_EVENT_CHARGE_ON,
    EC_EVENT_CHARGE_OFF,
    EC_EVENT_ALARM_ON,
    EC_EVENT_USB_ON,
    EC_EVENT_USB_OFF,
    EC_EVENT_POWER_OFF,
    EC_EVENT_USBSRV_ON,
    EC_EVENT_USBSRV_OFF,
    EC_EVENT_DISK_ON,
    EC_EVENT_DISK_OFF,
    EC_EVENT_WAIT_POWER_OFF,
    EC_EVENT_ALL_CNT
}ec_event_type;

#endif //EC_MAIN_APP_EC_EVENT_DEFINE_H
