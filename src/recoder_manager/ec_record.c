
#include "../libuv_wraper/ec_libuv.h"
#include "ec_record.h"
#include "../module_manager/ec_module.h"
#include "ec_recode_worker.h"
#include "../event/ec_event.h"
#include "../conf/ec_conf.h"
#include "ec_recoder_rotator.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../utils/ec_utils.h"
#include "../disk_manager/ec_disk_monitor.h"


static EC_BOOL isRun = EC_FALSE;
static EC_VOID *event_node = EC_NULL;

static EC_VOID stop_when_disk_full (EC_VOID);

EC_VOID ec_record_on (EC_VOID)
{
    if (ec_stat_set(&isRun) == EC_FAILURE)
    {
        dzlog_debug(" recorde stat set failed, maybe run");
        return;
    }

    if (!ec_disk_is_enough())
    {
        ec_stat_unset(&isRun);
        dzlog_error("disk not enough");
        return;
    }

    DEV_CONF_PTR;

    if (devConf->devid  && strlen(devConf->devid)
        && devConf->userid && strlen(devConf->userid))
    {
        //start disk monitor
      //  ec_disk_monitor_start();
        //start recorde work


       ec_record_work_run();
        RECODER_CONF_PTR;
        if (recoderConf->mode == RECORD_MODE_RATOAE)
        {
             ec_record_raotator_run ();
        }
        else
        {
            stop_when_disk_full();
        }
    }
    else
    {
        dzlog_debug("user is or devid is null %s %s", devConf->userid, devConf->devid);
        ec_stat_unset(&isRun);
    }

    return;
}
EC_VOID ec_record_off (EC_VOID)
{
    if (ec_stat_unset(&isRun) == EC_FAILURE)
    {
        dzlog_debug("record not run, exit !");
        return ;
    }

    dzlog_debug("stopping record working");
    ec_record_work_stop();
    RECODER_CONF_PTR;
    if (recoderConf->mode == RECORD_MODE_RATOAE)
    {

        ec_record_raotator_stop ();
    }
    else
    {
        if (event_node)
        {
            ec_event_del(event_node);
            event_node = NULL;
            dzlog_debug("delete event for disk full check");
        }
    }

    while (ec_record_work_is_run())
    {
        cmh_wait_usec(500);
        dzlog_debug("wait for recorde down");
    }
    dzlog_debug("ec_record_off success");
    return;
}
EC_BOOL ec_record_is_on (EC_VOID)
{
    return isRun;
}




static EC_VOID on_event (EC_VOID *arg)
{
    ec_event_handle *h = (ec_event_handle *)arg;
    if (h->eventType == EC_EVENT_DISK_FULL)
    {
        dzlog_error("disk full , stop recording");
        ec_record_off();
    }


    return;
}

static EC_VOID stop_when_disk_full (EC_VOID)
{
    ec_event_handle *handle = ec_event_new(EC_EVENT_DISK_FULL);
    handle->eventCallback = on_event;
    event_node = ec_event_attache(handle);

    return;
}
