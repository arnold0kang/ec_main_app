//
// Created by arnold on 12/13/2018.
//

#include "ec_recorde_alarm.h"
#include "../conf/ec_conf.h"
#include "../module_manager/ec_module.h"
#include "../event/ec_event.h"
#include "../beep/ec_beep.h"

#define MODULE_NAME     "EC_RECORDE_ALARM"

static EC_VOID ec_disk_full_alarm (EC_VOID);
static EC_VOID on_event (EC_VOID *arg);

EC_INT ec_rcorde_alarm_run (void)
{
    ec_disk_full_alarm();

    return EC_SUCCESS;
}
MODULE_ADD_RUN(MODULE_NAME, ec_rcorde_alarm_run);

static EC_VOID ec_disk_full_alarm (EC_VOID)
{
    RECODER_CONF_PTR;

    if (recoderConf->mode == RECORD_MODE_RATOAE)
    {
        return;
    }

    ec_event_type events[] = {EC_EVENT_DISK_FULL, EC_EVENT_DISK_ENOUGH, EC_EVENT_CHARGE_ON};
    EC_INT i;
    for (i = 0; i < ARRAY_SIZE(events); i++)
    {
        ec_event_handle *handle = ec_event_new(EC_EVENT_DISK_FULL);
        handle->eventCallback = on_event;
        ec_event_attache(handle);
    }

}


static EC_VOID on_event (EC_VOID *arg)
{
    ec_event_handle *h = (ec_event_handle *)arg;
    switch (h->eventType)
    {
        case EC_EVENT_DISK_ENOUGH:
            ec_beep_off();
            break;
        case EC_EVENT_DISK_FULL:
            ec_beep_on();
            break;
        case EC_EVENT_CHARGE_ON:
            ec_beep_off();
            break;
        default:
            break;

    }

    return;
}
