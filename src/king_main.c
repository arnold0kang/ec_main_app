#include  <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>




#include "common/ec_define.h"
#include "conf/ec_conf.h"
#include "conf/ec_version.h"
#include "sdk/ec_hisi_sdk.h"
#include "module_manager/ec_module.h"
#include "libuv_wraper/ec_libuv.h"
#include "event/ec_event.h"
#include "log/ec_log.h"


#define ZLOG_CONF_PATH      "/data/conf/zlog.conf"



EC_INT main(EC_INT argc, EC_CHAR **argv)
{
	EC_ERR_PTR;
	ec_conf_t  *conf= EC_NULL;

	dzlog_init(ZLOG_CONF_PATH, "enforcecam");
	zlog_profile();

#if 0
	if (argc != 2)
	{
		daemon(0, 0);
	}
	else
	{
		fprintf(stderr, "version: %s\n", VERSION_STR);
	}
#else
    fprintf(stderr, "version: %s\n", VERSION_STR);
#endif

	system("sysctl -w net.ipv4.tcp_syn_retries=1");
    dzlog_info("init watchdog success");
    //init config
    CHECK_NULL(ec_conf_get(), 1);
    ec_conf_dump ();
    dzlog_info("get config success");
    //init event
    CHECK_FAILED(ec_event_init(), 2);
    dzlog_info("init event server success");
    //init sdk

    //dzlog_info("init sdk success");
    //init all module
    CHECK_FAILED(ec_module_init(), 4);
    dzlog_info("init module success");
    //start all module
    CHECK_FAILED(ec_module_run(), 5);
    dzlog_info ("run module sucess");
    //start uv

   // getchar ();
    ec_libuv_run(); //



    failed_5:
    EC_ERR_SET("start libuv failed");
    failed_4:
    EC_ERR_SET("start module failed");
    failed_3:
    EC_ERR_SET("init module failed");
    failed_2:
    EC_ERR_SET("init sdk failed");
    failed_1:
    EC_ERR_SET("init config failed");
    failed_0:
    EC_ERR_SET("start watchdog failed");
    EC_ERR_OUT();
    return EC_FAILURE;

}

