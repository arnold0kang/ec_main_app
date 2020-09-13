//
// Created by arnold on 3/23/2018.
// this module used check disk free space

#include <sys/statvfs.h>
#include "ec_disk_monitor.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../module_manager/ec_module.h"
#include "../event/ec_event.h"
#include "../dev_stat/ec_dev_stat.h"
#include "ec_usb_disk.h"
#include "../conf/ec_conf.h"

#define MODULE_NAME         "disk_monitor"

#define DEFAULT_INTERVAL                5000 //5s
#define RESERVED_DISK_SPACE            (20000LL) //

static EC_VOID onMonitor(uv_timer_t *handlde);

static EC_INT getNextInterval(EC_LLONG currentFree);

static EC_BOOL lastCheckFull = EC_FALSE;

static EC_VOID onEvent(EC_VOID *arg);

static EC_LLONG ec_disk_get_free(EC_VOID);

static EC_LLONG ec_disk_get_total(EC_VOID);


static uv_timer_t *diskMonitorhandle = EC_NULL;

static EC_INT ec_disk_monitor_init(EC_VOID)
{
    EC_MALLOC(diskMonitorhandle);
    uv_timer_init(ec_looper, diskMonitorhandle);
}

MODULE_ADD_INIT(MODULE_NAME, ec_disk_monitor_init);


EC_VOID ec_disk_monitor_start(EC_VOID)
{
    uv_timer_start(diskMonitorhandle, onMonitor, 1000, DEFAULT_INTERVAL);

    return;
}

EC_VOID ec_disk_monitor_stop(EC_VOID)
{
    uv_timer_stop(diskMonitorhandle);

    return;
}


EC_BOOL ec_disk_is_enough(EC_VOID)
{
    EC_LLONG current_free = ec_disk_get_free();
    return (current_free > RESERVED_DISK_SPACE);
}


static EC_VOID onMonitor(uv_timer_t *handle)
{
    EC_LLONG currentFree;
    EC_BOOL currentFull;

#if 1
    if (!DISK_READY())
    {
        dzlog_debug("disk not ready, skip");
        return;
    }
#endif

    currentFree = ec_disk_get_free();
    CHECK_FAILED(currentFree, 0);

    if (currentFree <= RESERVED_DISK_SPACE)
    {
        currentFull = EC_TRUE;
    }
    else
    {
        currentFull = EC_FALSE;
    }

    dzlog_debug("lastfull %d currentFull %d", lastCheckFull, currentFull);


    if (currentFull != lastCheckFull)
    {
        if (currentFull)
        {
            BROAD_CAST(EC_EVENT_DISK_FULL);
        }
        else
        {

            BROAD_CAST(EC_EVENT_DISK_ENOUGH);
        }
    }


    lastCheckFull = currentFull;

    uv_timer_set_repeat(handle, getNextInterval(currentFree));

    return;

    failed_0:
    return;
}

static EC_INT getNextInterval(EC_LLONG currentFree)
{
    EC_INT nextCheck = (currentFree / RESERVED_DISK_SPACE) * 1000 + 1000; //加1S防止结果为0的情况
    dzlog_debug("current free %lld next disk  check interval %d", currentFree, nextCheck);

    return nextCheck > DEFAULT_INTERVAL ? nextCheck : DEFAULT_INTERVAL;
}


EC_LLONG ec_disk_get_free(EC_VOID)
{
    struct statvfs vfsBuf;

    EC_MEMSET(&vfsBuf);
    CHECK_FAILED(statvfs(EC_DISK_PATH, &vfsBuf), 0);

    return vfsBuf.f_bfree;

    failed_0:
    return EC_FAILURE;
}

EC_LLONG ec_disk_get_total(EC_VOID)
{
    return EC_FAILURE;
}