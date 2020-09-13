//
// Created by arnold on 10/28/2018.
//
#include "../common/ec_define.h"
#include "../module_manager/ec_module.h"
#include "ec_btn_wifi.h"
#include "../gpio/ec_gpio.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../beep/ec_beep.h"
#include "../conf/ec_conf.h"
#include "ec_btn_comm.h"
#include "ec_btn_conf.h"
#include "../log/ec_log.h"

#define MODULE_NAME         "BTN_WIFI"


static EC_VOID on_timer(uv_timer_t *handle);

static EC_VOID on_poll(uv_poll_t *handle, EC_INT status, EC_INT event);

static EC_VOID init_check(EC_VOID);

static EC_VOID start_check(EC_VOID);

static EC_VOID stop_check(EC_VOID);

static EC_VOID on_check(uv_timer_t *handle);

static EC_INT gpio_fd = -1;

EC_INT ec_btn_wifi_init(EC_VOID)
{
    ec_gpio_export(WIFI_BTN_GPIO);
    ec_gpio_direction(WIFI_BTN_GPIO, EC_GPIO_IN);
    ec_gpio_setedge(WIFI_BTN_GPIO, EC_TRUE, EC_TRUE);

    return EC_SUCCESS;
}

MODULE_ADD_INIT(MODULE_NAME, ec_btn_wifi_init);


EC_INT ec_btn_wifi_run(EC_VOID)
{

    gpio_fd = ec_gpio_open(WIFI_BTN_GPIO);
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
    CHECK_SUCCESS(uv_poll_start(poll_handle, UV_PRIORITIZED, on_poll), 0, 0);
    init_check();
#endif


#ifdef FACTORY_MODE
    ec_btn_do_live();

#endif


    return EC_SUCCESS;
    failed_0:
    return EC_FAILURE;
}

MODULE_ADD_RUN(MODULE_NAME, ec_btn_wifi_run);


static EC_VOID on_poll(uv_poll_t *handle, EC_INT status, EC_INT event)
{
    EC_INT ret = ec_gpio_read(gpio_fd);
    if (!ret)
    {
        ec_beep_on();
        dzlog_debug("wifi btn press down");
        start_check();
    }
    else
    {
        ec_beep_off();
        dzlog_debug("wifi btn press up");
        stop_check();
    }

    return;
}

static uv_timer_t *check_handle;

static EC_VOID init_check(EC_VOID)
{
    EC_MALLOC(check_handle);
    uv_timer_init(ec_looper, check_handle);

    return;
}

static EC_VOID start_check(EC_VOID)
{
    uv_timer_start(check_handle, on_check, WIFI_BTN_KEEP_TIME * 1000, 0);

    return;
}

static EC_VOID stop_check(EC_VOID)
{
    uv_timer_stop(check_handle);

    return;
}


static EC_VOID on_check(uv_timer_t *handle)
{
    dzlog_debug("btn to wifi switch");
    EC_INT alarm_fd = ec_gpio_open(ALARM_GPIO_PIN);
    if (alarm_fd != EC_FAILURE)
    {
        EC_INT ret = ec_gpio_read(alarm_fd);
        ec_gpio_close(alarm_fd);
        if (!ret)
        {
            ec_btn_do_apmode();
        }
    }
#ifndef FACTORY_MODE
    ec_btn_do_live();
#else
    ec_btn_do_charge();
#endif

}


/*
 * 0 标识按键按下
 * 1 是正常状态
 */
static EC_INT current_press_cnt = 0;
static EC_INT last_check_value = 1;

static EC_VOID on_timer(uv_timer_t *handle)
{
    EC_BOOL flag = EC_FAILURE;
    EC_INT ret = ec_gpio_read(gpio_fd);

    flag = last_check_value != ret ? EC_TRUE : EC_FALSE;
    last_check_value = ret;

    if (flag) //按键发生切换
    {
        if (!ret) //按键按下
        {
            current_press_cnt = 0;
            ec_beep_on();
        }
        else //按键松开
        {
            ec_beep_off();

        }

    }
    else
    {
        if (!ret)
        {
            current_press_cnt++;
            if (current_press_cnt / 2 > WIFI_BTN_KEEP_TIME)
            {
                //需要切换流状态
                dzlog_debug("btn to wifi ");
                ec_btn_do_live();
            }
        }
    }


}



