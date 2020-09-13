#include <unistd.h>
#include "pthread.h"
#include "../conf/ec_conf.h"
#include "../event/ec_event.h"
#include "ec_alarm.h"
#include "../log/ec_log.h"
#include "../wifi/ec_wifi.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../utils/ec_utils.h"
#include "curl/curl.h"


#define  MODULE_NAME        "ec_alarm"

static EC_BOOL isRun = EC_FALSE;

static EC_VOID alarm_work(EC_VOID *arg);

EC_VOID ec_alarm_on(EC_VOID)
{
    //
    ALARM_CONF_PTR;
    if (alarmConf->url == NULL || strlen(alarmConf->url) ==0 )
    {
        dzlog_info("alarm url is null");
        return;
    }
    dzlog_debug("start alarm");
    if (ec_stat_set(&isRun) == EC_FAILURE)
    {
        return; //already run ?
    }
    uv_thread_t tid;
    CHECK_SUCCESS(uv_thread_create(&tid, alarm_work, EC_NULL), 0, 0);

    return;

    failed_0:
    dzlog_error("start alarm thread failed");
}

EC_VOID ec_alarm_off(EC_VOID)
{
    if (ec_stat_unset(&isRun) == EC_FAILURE)
    {
        return;
    }
    ec_wifi_off();

    return;
}

static EC_INT alarm_elapsed_secs = 0;
#define MAX_ALARM_WAIT_TIME            60

static EC_VOID alarm_work(EC_VOID *arg)
{
    ALARM_CONF_PTR;
    DEV_CONF_PTR;

    EC_ERR_PTR;

    if (alarmConf->url == NULL || strlen(alarmConf->url) == 0)
    {
        return;
    }


    curl_global_init(CURL_GLOBAL_ALL);
    //开启wifi
    CHECK_FAILED(ec_wifi_on(), 0);
    for (alarm_elapsed_secs = 0; alarm_elapsed_secs < MAX_ALARM_WAIT_TIME; alarm_elapsed_secs++)
    {
        if (ec_wifi_is_on())
        {
            break;
        }
        else
        {
            cmh_wait_sec(1);
        }
    }
    if (alarm_elapsed_secs == MAX_ALARM_WAIT_TIME)
    {
        goto failed_1;
    }

    //发送报警
    EC_CHAR post_field[BUFSIZ] = {'\0'};
    CURLcode res;


#if 0
    sprintf(url, "http://%s:%d/manageServer/AlarmReport?devId=%s",
            alarmConf->addr, alarmConf->port, devConf->devid);
#else

    sprintf(post_field, "devId=%s", devConf->devid);
#endif

    dzlog_debug("alarm post url: %s?%s", alarmConf->url, post_field);

    EC_INT sendCnt = 3;

    while (sendCnt-- >= 0)
    {
        CURL *curl;
        curl = curl_easy_init();
        if (!curl)
        {
            continue;
        }

        curl_easy_setopt(curl, CURLOPT_URL, alarmConf->url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_field);
        res = curl_easy_perform(curl);
        cmh_wait_usec(100000);
        curl_easy_cleanup(curl);
        if (res == CURLE_OK)
        {
            break;
        }
        else
        {
            dzlog_error("curl_easy_perform() failed: %s",
                        curl_easy_strerror(res));
            cmh_wait_sec(3);
        }
    }


    curl_global_cleanup();
    ec_stat_unset(&isRun);
    ec_wifi_off();
    return;


    failed_4:
    failed_3:
    failed_2:
    failed_1:
    dzlog_error("wait wifi failed");
    failed_0:
    dzlog_error("trun wifi failed");
    ec_stat_unset(&isRun);
    ec_wifi_off();
    curl_global_cleanup();
    return;
}

