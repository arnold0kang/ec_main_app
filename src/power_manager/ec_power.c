//
// Created by arnold on 18-1-11.
//

#include "ec_power.h"
#include "battery/ec_lsadc.h"
#include "../event/ec_event.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../module_manager/ec_module.h"
#include "../beep/ec_beep.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../button/ec_btn_comm.h"

#define MODULE_NAME     "ec_power"
#define DEFAULT_CHECK_INTERVAL          30000 //30s
#define POWER_MIN_ALARM                 10
#define POWER_MIN_POWEROFF              7
static EC_INT current_power = 0;
static uv_timer_t powerCheckHandle;
static uv_timer_t lowPowerAlarmHandle;
static EC_BOOL isInAlaram = EC_FALSE;
static EC_VOID onPowerCheck (uv_timer_t *handle);
static EC_VOID low_power_alarm (EC_VOID);
static EC_VOID onChargeEvent (EC_VOID *arg);

static EC_INT ec_power_init (EC_VOID)
{
    uv_timer_init(ec_looper,  &powerCheckHandle);
    uv_timer_init (ec_looper, &lowPowerAlarmHandle);

    //start a handle listen on chage mode
    ec_event_handle *h = ec_event_new(EC_EVENT_CHARGE_ON);
    h->eventCallback =onChargeEvent;
    ec_event_attache(h);
    return EC_SUCCESS;
}
MODULE_ADD_INIT(MODULE_NAME, ec_power_init);

static EC_INT ec_power_run (EC_VOID)
{
    uv_timer_start(&powerCheckHandle, onPowerCheck, 1000, DEFAULT_CHECK_INTERVAL);
    return EC_SUCCESS;
}
MODULE_ADD_RUN(MODULE_NAME ,ec_power_run);


static EC_INT ec_power_stop (EC_VOID)
{
    uv_timer_stop (&powerCheckHandle);
    return EC_SUCCESS;
}
MODULE_ADD_STOP(MODULE_NAME, ec_power_stop);
static EC_INT ec_power_exit(EC_VOID)
{
    return EC_SUCCESS;
}
MODULE_ADD_EXIT(MODULE_NAME, ec_power_exit);


static EC_VOID onPowerCheck (uv_timer_t *handle)
{
    current_power = king_lsadc_read();
    dzlog_debug("current power %d", current_power);
    if (current_power < POWER_MIN_POWEROFF)
    {
        //LOW POWER!!!
        if (!CHARGE_ON())
        {
            ec_btn_do_poweroff();
        }

    }
    if (current_power < POWER_MIN_ALARM)
    {
        //should we start low power alarm?
        if(!CHARGE_ON())
        {
            low_power_alarm ();
        }

    }

    return ;
}

EC_INT ec_power_current_info (EC_VOID)
{
    return current_power;
}

#define LOW_POWER_ALRAM_INTERVAL                5000 //5sec

static EC_VOID doAlarm (uv_timer_t *handle)
{
    BEEP();
}

static EC_VOID low_power_alarm (EC_VOID)
{
    if (isInAlaram)
    {
        return; //already in alarm, just quit
    }
    isInAlaram = EC_TRUE;
    uv_timer_start (&lowPowerAlarmHandle, doAlarm, LOW_POWER_ALRAM_INTERVAL, LOW_POWER_ALRAM_INTERVAL);

    return;
}

static EC_VOID onChargeEvent (EC_VOID *arg)
{
    if (isInAlaram)
    {
        uv_timer_stop(&lowPowerAlarmHandle);
        isInAlaram = EC_FALSE;
    }

    return;
}