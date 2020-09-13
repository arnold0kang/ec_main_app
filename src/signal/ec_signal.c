//
// Created by arnold on 3/21/2018.
//
#include "../libuv_wraper/ec_libuv.h"
#include "../common/ec_define.h"
#include "ec_signal.h"
#include "../module_manager/ec_module.h"

#define MODULE_NAME "ec_signal"
static uv_signal_t handle;
static EC_VOID signal_callback (uv_signal_t *signal, int signum);

static EC_INT ec_signal_init (EC_VOID)
{
    uv_signal_init(ec_looper, &handle);

    return EC_SUCCESS;
}
MODULE_ADD_INIT(MODULE_NAME, ec_signal_init);

static EC_INT ec_signal_run (EC_VOID)
{
    uv_signal_start (&handle, signal_callback, SIGPIPE);

    return EC_SUCCESS;
}
MODULE_ADD_RUN(MODULE_NAME, ec_signal_run);

static EC_INT ec_signal_stop (EC_VOID)
{
    uv_signal_stop (&handle);
    return EC_SUCCESS;
}
MODULE_ADD_STOP(MODULE_NAME, ec_signal_stop);

static EC_INT ec_signal_exit (EC_VOID)
{
    return EC_SUCCESS;
}
MODULE_ADD_EXIT(MODULE_NAME, ec_signal_exit);


static EC_VOID signal_callback (uv_signal_t *signal, int signum)
{
    return ;
}


