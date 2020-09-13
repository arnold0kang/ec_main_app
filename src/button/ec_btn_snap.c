//
// Created by arnold on 10/28/2018.
//

#include "ec_btn_snap.h"
#include "../common/ec_define.h"
#include "../gpio/ec_gpio.h"
#include "../module_manager/ec_module.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../conf/ec_conf.h"
#include "../beep/ec_beep.h"
#include "../snap/ec_snap.h"
#include "../utils/ec_utils.h"
#include "ec_btn_comm.h"
#include "ec_btn_conf.h"
#include "../log/ec_log.h"

#define MODULE_NAME "BTN_SNAP"


static EC_VOID on_timer(uv_timer_t *handle);

static EC_VOID on_poll(uv_poll_t *handle, EC_INT status, EC_INT event);

static EC_VOID init_check(EC_VOID);

static EC_VOID start_check(EC_VOID);

static EC_VOID stop_check(EC_VOID);

static EC_VOID on_check(uv_timer_t *handle);

static EC_INT gpio_fd = -1;

EC_INT ec_btn_snap_init(EC_VOID)
{
    ec_gpio_export(SNAP_GPIO_PIN);
    ec_gpio_direction(SNAP_GPIO_PIN, EC_GPIO_IN);
    ec_gpio_setedge(SNAP_GPIO_PIN, EC_TRUE, EC_TRUE);

    return EC_SUCCESS;
}

MODULE_ADD_INIT(MODULE_NAME, ec_btn_snap_init);


EC_INT ec_btn_snap_run(EC_VOID)
{
    gpio_fd = ec_gpio_open(SNAP_GPIO_PIN);
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

    return EC_SUCCESS;

    failed_0:
    return EC_FAILURE;
}

MODULE_ADD_RUN(MODULE_NAME, ec_btn_snap_run);


static time_t current_btn_press_ts = 0;

static EC_VOID on_poll(uv_poll_t *handle, EC_INT status, EC_INT event)
{
    EC_INT ret = ec_gpio_read(gpio_fd);
    if (!ret)
    {
        ec_beep_on();
        current_btn_press_ts = time(NULL);
        start_check();
    }
    else
    {
        ec_beep_off();
        stop_check();
        time_t diff = time(EC_NULL) - current_btn_press_ts;
        if (diff < POWER_BTN_KEEP_TIME)
        {
            dzlog_debug("btn to snap");
            ec_snap_do();
        }

    }
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
    uv_timer_start(check_handle, on_check, POWER_BTN_KEEP_TIME * 1000, 0);

    return;
}

static EC_VOID stop_check(EC_VOID)
{
    uv_timer_stop(check_handle);

    return;
}

static EC_VOID on_check(uv_timer_t *handle)
{
    dzlog_debug("btn to power off");
    ec_btn_do_poweroff();
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
            if (current_press_cnt / 2 > POWER_BTN_KEEP_TIME)
            {
                dzlog_debug("btn to power off");
                ec_btn_do_poweroff();
            }
            else
            {
                dzlog_debug("btn to snap");
                ec_snap_do();
            }
        }

    }
    else
    {
        if (!ret)
        {
            current_press_cnt++;
            if (current_press_cnt / 2 > POWER_BTN_KEEP_TIME)
            {
                //需要关机
                dzlog_debug("btn to power off ");
                ec_btn_do_poweroff();
            }
        }
    }


}


