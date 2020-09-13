#include <stdio.h>
#include <sys/stat.h>
#include "ec_log.h"
#include "../rtc/ec_rtc.h"
#include "../event/ec_event_define.h"
#include "../event/ec_event.h"
#include "../conf/ec_conf.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../module_manager/ec_module.h"
#include "../utils/ec_utils.h"


#define  LOG_PREFIX         "log"
#define  MODULE_NAME        "ec_log"

static EC_VOID open_log_file (EC_VOID);
static EC_VOID close_log_file (EC_VOID);
static EC_VOID write_log_file (const EC_CHAR *logStr);
static EC_VOID on_event (EC_VOID *arg);
static FILE *logFp = EC_NULL;
static EC_BOOL is_power_on  =EC_TRUE;




EC_INT ec_log_run (EC_VOID)
{
    ec_event_type i;
    for (i = 0; i < EC_EVENT_ALL_CNT; i++)
    {
        ec_event_handle *h = ec_event_new(i);
        h->eventCallback =on_event;
        ec_event_attache(h);
    }
    return EC_SUCCESS;
}
//MODULE_ADD_RUN(MODULE_NAME, ec_log_run);

static EC_BOOL is_run = EC_FALSE;


EC_VOID ec_log_on (EC_VOID)
{
    if (ec_stat_set (&is_run) == EC_FAILURE)
    {
        return;
    }
    open_log_file();
    return;
}
EC_VOID ec_log_off (EC_VOID)
{
    if (ec_stat_unset(&is_run) == EC_FAILURE)
    {
        return;
    }
    close_log_file();
    return;
}


static EC_VOID open_log_file (EC_VOID)
{
    char fileBuf[BUFSIZ] = {'\0'};
    char tsBuf[64] = {'\0'};

    mkdir(LOG_DIR, 0644);
    ec_rtc_get_ts(tsBuf, 63);

    snprintf(fileBuf, BUFSIZ-1, "%s%s-%s.txt", LOG_DIR, LOG_PREFIX, tsBuf);
    logFp = fopen(fileBuf, "w+");
    dzlog_debug("start log");
    ec_disk_ref_add();
    return;
}
static EC_VOID close_log_file (EC_VOID)
{
    if (logFp)
    {
        fclose(logFp);
        logFp = EC_NULL;
    }
    ec_disk_ref_del();
    dzlog_debug("stop log");
}
static EC_VOID write_log_file (const EC_CHAR *logStr)
{
    if (!logFp || !is_run)
    {
        return;
    }
    DEV_CONF_PTR;
    EC_CHAR log[BUFSIZ] = {'\0'};
    EC_CHAR ts[64] = {'\0'};
    ec_rtc_get_time(ts, 63);
    snprintf(log, BUFSIZ-1, "%s[%s][%s] %s\r\n", ts, devConf->userid, devConf->devid, logStr);
    fwrite(log, strlen(log), 1, logFp);

    return;
}

static EC_VOID on_event (EC_VOID *arg)
{
    ec_event_handle *h = (ec_event_handle *)arg;
    switch (h->eventType)
    {
        case EC_CONF_VIDEO_CHANGED:
            break;
        case EC_CONF_AUDIO_CHANGED:
            break;
        case EC_CONF_SNAP_CHANGED:
            break;
        case EC_CONF_OSD_CHANGED:
            break;
        case EC_CONF_RECODER_CHANGED:
            break;
        case EC_CONF_HB_CHANGED:
            break;
        case EC_CONF_SERVER_CHANGED:
            break;
        case EC_CONF_DEV_CHANGED:
            break;
        case EC_EVENT_WIFI_ON:
            write_log_file("open wifi");
            break;
        case EC_EVENT_WIFI_OFF:
            write_log_file("close wifi");
            break;
        case EC_EVENT_WIFI_CONNECT:
            write_log_file("wifi connect success");
            break;
        case EC_EVENT_WIFI_DISCONNECT:
            write_log_file("wifi disconnect ");
            break;
        case EC_EVENT_DATA_IN_TRANS:
            //write_log_file("start rtmp live stream");
            break;
        case EC_EVENT_DATA_OUT_TRANS:
           // write_log_file("stop rtmp live stream");
            break;
        case EC_EVENT_REBOOT:
            write_log_file("reboot system");
            break;
        case EC_EVENT_DISK_FULL:
            write_log_file("disk is full!");
            break;
        case EC_EVENT_DISK_ENOUGH:
            write_log_file("disk is enough");
            break;
        case EC_EVENT_RECODER_START:
            write_log_file("start recorde video");
            break;
        case EC_EVENT_RECODER_STOP:
            write_log_file("start stop video");
            break;
        case EC_EVENT_CHARGE_ON:
            write_log_file("start to charge");
            break;
        case EC_EVENT_CHARGE_OFF:
            write_log_file("stop to charge");
            break;
        case EC_EVENT_ALARM_ON:
            write_log_file("get alarm event");
            break;
        case EC_EVENT_USB_ON:
            write_log_file("usb connect success");
            break;
        case EC_EVENT_USB_OFF:
            write_log_file("usb disconnect success");
            break;
        case EC_EVENT_POWER_OFF:
            write_log_file("powoer off");
            break;
        case EC_EVENT_USBSRV_ON:
            write_log_file("start usb serial connect");
            break;
        case EC_EVENT_USBSRV_OFF:
            write_log_file("stop usb serial server");
            break;
        case EC_EVENT_DISK_ON:
            write_log_file("mount video disk success");
            break;
        case EC_EVENT_DISK_OFF:
            write_log_file("umount video disk success");
            break;
        default:
            break;
    }
}