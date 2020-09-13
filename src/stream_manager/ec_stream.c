//
// Created by arnold on 3/24/2018.
//
#include "../common/ec_define.h"
#include "ec_stream.h"
#include "../event/ec_event_define.h"
#include "../event/ec_event.h"
#include "../module_manager/ec_module.h"
#include "ec_stream_worker.h"
#include "../utils/ec_utils.h"
#include "../conf/ec_conf.h"
#include "../log/ec_log.h"

#define MODULE_NAME         "ec_stream"



static EC_BOOL is_run = EC_FALSE;

EC_VOID ec_stream_on (EC_VOID)
{
    STREAMSRV_CONF_PTR;

    if (streamSrvConf->url== NULL || strlen(streamSrvConf->url) == 0)
    {
        dzlog_info("stream url is null");
        return;
    }

    if(ec_stat_set(&is_run) == EC_FAILURE)
    {
        return;
    }
    dzlog_debug("trun stream  on");

    ec_stream_worker_run();
}
EC_VOID ec_stream_off (EC_VOID)
{
    dzlog_debug("trun stream  off");
    ec_stream_worker_stop();
    while(ec_stream_worker_is_run())
    {
        cmh_wait_usec(500);
    }
    ec_stat_unset(&is_run);

    return;
}
EC_BOOL ec_stream_is_run (EC_VOID)
{
    return is_run;
}


