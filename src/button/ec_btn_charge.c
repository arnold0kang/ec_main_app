//
// Created by arnold on 10/28/2018.
//
#include "../common/ec_define.h"
#include "ec_btn_charge.h"
#include "../gpio/ec_gpio.h"
#include "../module_manager/ec_module.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../beep/ec_beep.h"
#include "../conf/ec_conf.h"
#include "ec_btn_comm.h"
#include "ec_btn_conf.h"
#include "../log/ec_log.h"

#define MODULE_NAME  "btn_charge"


static EC_VOID on_timer(uv_timer_t *handle);

static EC_VOID on_poll(uv_poll_t *handle, EC_INT status, EC_INT event);


static EC_INT gpio_fd = -1;

EC_INT ec_btn_charge_init(EC_VOID)
{
    ec_gpio_export(CHARGE_GPIO_PIN);
    ec_gpio_direction(CHARGE_GPIO_PIN, EC_GPIO_IN);
    ec_gpio_setedge(CHARGE_GPIO_PIN, EC_TRUE, EC_TRUE);

    return EC_SUCCESS;
}

MODULE_ADD_INIT(MODULE_NAME, ec_btn_charge_init);

EC_INT ec_btn_charge_run(EC_VOID)
{
#ifndef FACTORY_MODE
    gpio_fd = ec_gpio_open(CHARGE_GPIO_PIN);
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
#endif

#else
    ec_btn_do_uncharge();
#endif


    return EC_SUCCESS;

    failed_0:
    return EC_FAILURE;
}

MODULE_ADD_RUN(MODULE_NAME, ec_btn_charge_run);

static EC_VOID on_poll(uv_poll_t *handle, EC_INT status, EC_INT event)
{
    EC_INT ret = ec_gpio_read(gpio_fd);
    ec_beep_do_once();
    if (ret)
    {
        //电源插上
        dzlog_debug("get charge plug in signal");
        ec_btn_do_charge();
    }
    else
    {
        //电源拔掉
        dzlog_debug("get charge plug out signal");
        ec_btn_do_uncharge();
    }
}


static EC_INT last_check_value = 0; //default button value


/*
 * 0 普通状态
 * 1 插上电源
 */
static EC_VOID on_timer(uv_timer_t *handle)
{
    EC_BOOL flag = EC_FAILURE;
    EC_INT ret = ec_gpio_read(gpio_fd);

    flag = last_check_value != ret ? EC_TRUE : EC_FALSE;
    last_check_value = ret;

    if (!flag)
    {
        return;
    }

    if (ret)
    {
        //电源插上
        dzlog_debug("get charge plug in signal");
        ec_btn_do_charge();
    }
    else
    {
        //电源拔掉
        dzlog_debug("get charge plug out signal");
        ec_btn_do_uncharge();
    }

    return;
}


