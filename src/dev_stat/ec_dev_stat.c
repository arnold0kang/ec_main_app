//
// Created by arnold on 3/22/2018.
//

#include <unistd.h>
#include "ec_dev_stat.h"
#include "../module_manager/ec_module.h"
#include "../event/ec_event.h"
#include "../sdk/hisi_11/memmap.h"
#include "../rtc/ec_rtc.h"
#include "../disk_manager/ec_usb_disk.h"
#include "../utils/ec_reg.h"
#include "../sdk/ec_hisi_sdk.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../utils/ec_utils.h"

#define MODULE_NAME     "ec_stat"

static EC_VOID onEventCallBack (EC_VOID *arg);




static struct  {
    volatile EC_BOOL isChargeOn;
    volatile EC_BOOL isDiskFull;
    volatile EC_BOOL isWifiOn;
    volatile EC_BOOL isWifiConnect;
    volatile EC_BOOL isRecordeRun;
    volatile EC_BOOL isStreamOn;
    volatile EC_BOOL isDiskReady;
    volatile EC_BOOL isUsbReady;
    volatile EC_BOOL isUsbSrvRun;
}dev_stat = {
        EC_FALSE,
        EC_FALSE,
        EC_FALSE,
        EC_FALSE,
        EC_FALSE,
        EC_FALSE,
        EC_FALSE,
        EC_FALSE,
        EC_FALSE
};

#define STAT_DEBUG          1

#define  STAT_CHARGE_ON             do{dev_stat.isChargeOn = EC_TRUE;       if(STAT_DEBUG){dzlog_debug ("isChargeOn stat %d", dev_stat.isChargeOn);}} while(0)
#define  STAT_CHARGE_OFF            do{dev_stat.isChargeOn = EC_FALSE;      if(STAT_DEBUG){dzlog_debug ("isChargeOn stat %d", dev_stat.isChargeOn);}} while(0)
#define  STAT_DISK_FULL_ON          do{dev_stat.isDiskFull = EC_TRUE;       if(STAT_DEBUG){dzlog_debug ("isDiskFull stat %d", dev_stat.isDiskFull);}} while(0)
#define  STAT_DISK_FULL_OFF         do{dev_stat.isDiskFull = EC_FALSE;      if(STAT_DEBUG){dzlog_debug ("isDiskFull stat %d", dev_stat.isDiskFull);}} while(0)
#define  STAT_WIFI_ON               do{dev_stat.isWifiOn = EC_TRUE;         if(STAT_DEBUG){dzlog_debug ("isWifiOn stat %d", dev_stat.isWifiOn);}} while(0)
#define  STAT_WIFI_OFF              do{dev_stat.isWifiOn = EC_FALSE;        if(STAT_DEBUG){dzlog_debug ("isWifiOn stat %d", dev_stat.isWifiOn);}} while(0)
#define  STAT_WIFI_CONN_ON          do{dev_stat.isWifiConnect = EC_TRUE;    if(STAT_DEBUG){dzlog_debug ("isWifiConnect stat %d", dev_stat.isWifiConnect);}} while(0)
#define  STAT_WIFI_CONN_OFF         do{dev_stat.isWifiConnect = EC_FALSE;   if(STAT_DEBUG){dzlog_debug ("isWifiConnect stat %d", dev_stat.isWifiConnect);}} while(0)
#define  STAT_RECODER_ON            do{dev_stat.isRecordeRun = EC_TRUE;     if(STAT_DEBUG){dzlog_debug ("isRecordeRun stat %d", dev_stat.isRecordeRun);}} while(0)
#define  STAT_RECODER_OFF           do{dev_stat.isRecordeRun = EC_FALSE;    if(STAT_DEBUG){dzlog_debug ("isRecordeRun stat %d", dev_stat.isRecordeRun);}} while(0)
#define  STAT_STREAM_ON             do{dev_stat.isStreamOn = EC_TRUE;       if(STAT_DEBUG){dzlog_debug ("isStreamOn stat %d", dev_stat.isStreamOn);}} while(0)
#define  STAT_STREAM_OFF            do{dev_stat.isStreamOn = EC_FALSE;      if(STAT_DEBUG){dzlog_debug ("isStreamOn stat %d", dev_stat.isStreamOn);}} while(0)
#define  STAT_DISK_READY_ON         do{dev_stat.isDiskReady = EC_TRUE;      if(STAT_DEBUG){dzlog_debug ("isDiskReady stat %d", dev_stat.isDiskReady);}} while(0)
#define  STAT_DISK_READY_OFF        do{dev_stat.isDiskReady = EC_FALSE;     if(STAT_DEBUG){dzlog_debug ("isDiskReady stat %d", dev_stat.isDiskReady);}} while(0)
#define  STAT_USB_ON                do{dev_stat.isUsbReady = EC_TRUE;       if(STAT_DEBUG){dzlog_debug ("isUsbReady stat %d", dev_stat.isUsbReady);}} while(0)
#define  STAT_USB_OFF               do{dev_stat.isUsbReady = EC_FALSE;      if(STAT_DEBUG){dzlog_debug ("isUsbReady stat %d", dev_stat.isUsbReady);}} while(0)
#define  STAT_USB_SRV_ON            do{dev_stat.isUsbSrvRun = EC_TRUE;      if(STAT_DEBUG){dzlog_debug ("isUsbSrvRun stat %d", dev_stat.isUsbSrvRun);}} while(0)
#define  STAT_USB_SRV_OFF           do{dev_stat.isUsbSrvRun = EC_FALSE;     if(STAT_DEBUG){dzlog_debug ("isUsbSrvRun stat %d", dev_stat.isUsbSrvRun);}} while(0)






#define LISTEN_EVENTS { \
    EC_EVENT_CHARGE_ON, \
    EC_EVENT_CHARGE_OFF, \
    EC_EVENT_WIFI_OFF, \
    EC_EVENT_WIFI_ON, \
    EC_EVENT_WIFI_CONNECT, \
    EC_EVENT_WIFI_DISCONNECT, \
    EC_EVENT_RECODER_START, \
    EC_EVENT_RECODER_STOP, \
    EC_EVENT_DATA_IN_TRANS, \
    EC_EVENT_DATA_OUT_TRANS, \
    EC_EVENT_POWER_OFF, \
    EC_EVENT_DISK_ENOUGH, \
    EC_EVENT_DISK_FULL, \
    EC_EVENT_USB_ON, \
    EC_EVENT_USB_OFF, \
    EC_EVENT_USBSRV_ON, \
    EC_EVENT_USBSRV_OFF, \
    EC_EVENT_DISK_OFF, \
    EC_EVENT_DISK_ON \
    }



static EC_INT ec_dev_stat_run (EC_VOID)
{
    ec_event_handle  *h;
    ec_event_type  listenEventList[] =LISTEN_EVENTS;

    EC_UINT i = 0;
    for (i = 0; i < ARRAY_SIZE(listenEventList); i++ )
    {
        h = ec_event_new(listenEventList[i]);
        CHECK_NULL(h, 0);
        h->eventCallback = onEventCallBack;
        CHECK_NULL(ec_event_attache(h), 0);
    }


    return EC_SUCCESS;
    failed_0:
    return EC_FAILURE;
}
MODULE_ADD_RUN(MODULE_NAME, ec_dev_stat_run);



EC_BOOL ec_dev_wifi_is_run (EC_VOID)
{
    return dev_stat.isWifiOn;
}

EC_BOOL ec_dev_disk_is_full (EC_VOID)
{
    return dev_stat.isDiskFull;
}

EC_BOOL ec_dev_recoder_is_run (EC_VOID)
{
    return dev_stat.isRecordeRun;
}

EC_BOOL ec_dev_charge_is_on (EC_VOID)
{
    return dev_stat.isChargeOn;
}

EC_BOOL ec_dev_stream_is_on (EC_VOID)
{
    return dev_stat.isStreamOn;
}

EC_BOOL ec_dev_wifi_is_connect (EC_VOID)
{
    return dev_stat.isWifiConnect;
}

EC_BOOL ec_dev_disk_ready (EC_VOID)
{
 //   dzlog_debug("DiskStat %d", dev_stat.isDiskReady);
    return dev_stat.isDiskReady ;
}


static EC_INT       disk_ref_count = 0;


EC_VOID ec_disk_ref_add (EC_VOID)
{
    ec_atomic_add(&disk_ref_count);
    dzlog_debug("current dis ref %d", disk_ref_count);
}
EC_VOID ec_disk_ref_del (EC_VOID)
{
    ec_atomic_sub(&disk_ref_count);
    if (disk_ref_count < 0)
    {
        dzlog_error("disk ref < 0 ");
    }
    dzlog_debug("current dis ref %d", disk_ref_count);
}
EC_BOOL ec_disk_is_inuse (EC_VOID)
{
    if (disk_ref_count > 0)
    {
        return EC_TRUE;
    }
    else
    {
        return EC_FALSE;
    }
}

static EC_INT       usb_ref_count = 0;
EC_VOID ec_usb_ref_add (EC_VOID)
{
    ec_atomic_add(&usb_ref_count);
}
EC_VOID ec_usb_ref_del (EC_VOID)
{
    ec_atomic_sub(&usb_ref_count);
    if (usb_ref_count < 0)
    {
        dzlog_error("usb ref < 0 ");
    }
}


EC_BOOL ec_usb_is_inuse (EC_VOID)
{
    if (usb_ref_count > 0)
    {
        return EC_TRUE;
    }
    else
    {
        return EC_FALSE;
    }
}



static EC_VOID onEventCallBack (EC_VOID *arg)
{
    ec_event_handle  *h = (ec_event_handle *)arg;
    switch (h->eventType)
    {
        case EC_EVENT_CHARGE_ON:
            STAT_CHARGE_ON;
            break;
        case EC_EVENT_CHARGE_OFF:
            STAT_CHARGE_OFF;
            break;
        case EC_EVENT_WIFI_OFF:
            STAT_WIFI_OFF;
            STAT_WIFI_CONN_OFF;
            break;
        case EC_EVENT_WIFI_ON:
            STAT_WIFI_ON;
            break;
        case EC_EVENT_RECODER_START:
            STAT_RECODER_ON;
            break;
        case EC_EVENT_RECODER_STOP:
            STAT_RECODER_OFF;
            break;
        case EC_EVENT_DATA_IN_TRANS:
            STAT_STREAM_ON;
            break;
        case EC_EVENT_DATA_OUT_TRANS:
            STAT_STREAM_OFF;
            break;
        case EC_EVENT_WIFI_CONNECT:
            STAT_WIFI_CONN_ON;
            break;
        case EC_EVENT_WIFI_DISCONNECT:
            STAT_WIFI_CONN_OFF;
            break;
        case EC_EVENT_DISK_FULL:
            STAT_DISK_FULL_ON;
            break;
        case EC_EVENT_DISK_ENOUGH:
            STAT_DISK_FULL_OFF;
            break;
        case EC_EVENT_USB_OFF:
            STAT_USB_OFF;
            break;
        case EC_EVENT_USB_ON:
            STAT_USB_ON;
            break;
        case EC_EVENT_USBSRV_ON:
            STAT_USB_SRV_ON;
            break;
        case EC_EVENT_USBSRV_OFF:
            STAT_USB_SRV_OFF;
            break;
        case EC_EVENT_DISK_ON:
            STAT_DISK_READY_ON;
            break;
        case EC_EVENT_DISK_OFF:
            STAT_DISK_READY_OFF;
            break;
        default:
            break;
    }
    return;
}




