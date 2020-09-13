//
// Created by arnold on 10/28/2018.
//

#include "ec_btn_alarm.h"
#include "../common/ec_define.h"
#include "../module_manager/ec_module.h"
#include "../gpio/ec_gpio.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../beep/ec_beep.h"
#include "../alarm/ec_alarm.h"
#include "ec_btn_conf.h"
#include "../log/ec_log.h"

#define MODULE_NAME     "btn_alarm"



static EC_VOID on_timer (uv_timer_t *handle);
static EC_VOID on_poll (uv_poll_t *handle, EC_INT status, EC_INT event);

static EC_INT gpio_fd = -1;


EC_INT ec_btn_alarm_init (EC_VOID)
{
    ec_gpio_export(ALARM_GPIO_PIN);
    ec_gpio_direction(ALARM_GPIO_PIN, EC_GPIO_IN);
    ec_gpio_setedge(ALARM_GPIO_PIN, EC_TRUE, EC_TRUE);

    return EC_SUCCESS;
}
MODULE_ADD_INIT(MODULE_NAME, ec_btn_alarm_init);


EC_INT ec_btn_alarm_run (EC_VOID)
{

#ifndef FACTORY_MODE
    gpio_fd = ec_gpio_open(ALARM_GPIO_PIN);
    CHECK_FAILED(gpio_fd, 0);

#if 0
    uv_timer_t *timer_handle = EC_NULL;
    EC_MALLOC(timer_handle);

    CHECK_SUCCESS(uv_timer_init(ec_looper, timer_handle), 0, 0);
    CHECK_SUCCESS(uv_timer_start(timer_handle, on_timer, 500, 500), 0, 0);
#else
    uv_poll_t *poll_handle;
    EC_MALLOC(poll_handle);
    CHECK_SUCCESS(uv_poll_init(ec_looper, poll_handle, gpio_fd), 0, 0);
    CHECK_SUCCESS(uv_poll_start(poll_handle, UV_PRIORITIZED , on_poll), 0, 0);
#endif

#endif

    return EC_SUCCESS;

    failed_0:
    return EC_FAILURE;
}
MODULE_ADD_RUN(MODULE_NAME, ec_btn_alarm_run);




static EC_VOID on_poll (uv_poll_t *handle, EC_INT status, EC_INT event)
{
    EC_INT ret = ec_gpio_read(gpio_fd);
    EC_INT wifi_fp = ec_gpio_open(WIFI_BTN_GPIO);
    if (wifi_fp != EC_FAILURE)
    {
        EC_INT wifi_value = ec_gpio_read(wifi_fp);
        ec_gpio_close(wifi_fp);
        if (!wifi_value)
        {
            return;
        }
    }


    if (!ret)
    {
        ec_beep_on();
        ec_alarm_on();
    }
    else
    {
        dzlog_debug("alarm off");
        ec_beep_off();
    }
}

static EC_INT last_check_value = 1; //default button value

static EC_VOID on_timer (uv_timer_t *handle)
{
    EC_BOOL flag = EC_FAILURE;
    EC_INT ret = ec_gpio_read(gpio_fd);

    flag = last_check_value != ret ? EC_TRUE : EC_FALSE;
    last_check_value = ret;

    if (!flag)
    {
        return;
    }

    if (!ret)
    {
        dzlog_debug("alarm on");
        ec_beep_on();
        ec_alarm_on();
    }
    else
    {
        dzlog_debug("alarm off");
        ec_beep_off();
    }

    return;
}


