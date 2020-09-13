//
// Created by arnold on 3/24/2018.
//
#include "../common/ec_define.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../module_manager/ec_module.h"
#include "../event/ec_event.h"


#if 0

uv_timer_t startStreamHandle;
uv_timer_t stopStreamHandle;
#define MODULE_NAME "stream_test"
static EC_VOID onStart (uv_timer_t *handle);
static EC_VOID onStop (uv_timer_t *handle);

static EC_INT ec_stream_test_init (EC_VOID)
{
    uv_timer_init(ec_looper, &startStreamHandle);
    uv_timer_init(ec_looper, &stopStreamHandle);

    return EC_SUCCESS;
}
MODULE_ADD_INIT (MODULE_NAME, ec_stream_test_init);
static EC_INT ec_stream_test_run (EC_VOID)
{
    uv_timer_start(&startStreamHandle, onStart, 10000, 0);
    uv_timer_start (&stopStreamHandle, onStop, 30000, 0);

    return EC_SUCCESS;
}
MODULE_ADD_RUN(MODULE_NAME, ec_stream_test_run);
static EC_INT ec_stream_test_stop (EC_VOID)
{

}
static EC_INT ec_stream_test_exit (EC_VOID)
{

}

static EC_VOID onStart (uv_timer_t *handle)
{
    uv_timer_start (&stopStreamHandle, onStop, 30000, 0);
    BROAD_CAST (EC_EVENT_WIFI_ON);
}
static EC_VOID onStop (uv_timer_t *handle)
{
    uv_timer_start(&startStreamHandle, onStart, 10000, 0);
    BROAD_CAST(EC_EVENT_WIFI_OFF);
}

#endif