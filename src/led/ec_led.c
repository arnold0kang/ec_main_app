#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ec_led.h"
#include "../event/ec_event.h"
#include "../wifi/ec_wifi.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../module_manager/ec_module.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../pwm/hi_pwm.h"
#include "ec_led_alg.h"
#include "../conf/ec_conf.h"


#define MODULE_NAME             "ec_led"
#define PWM_DEV_PATH            "/dev/pwm"

#define LED_GREEN_PWM               0
#define LED_RED_PWM                 1

static EC_VOID on_event (EC_VOID *arg);


static EC_INT pwm_fd = -1;


EC_INT ec_led_init (EC_VOID)
{
    pwm_fd  = hi_pwm_open(PWM_DEV_PATH);
    if (pwm_fd == -1)
    {
        dzlog_debug("open pwm for led failed");
        return EC_FAILURE;

    }
    PWM_DATA_S pwm_data;
    pwm_data.enable = 1;
    pwm_data.duty = PWM_HZ;
    pwm_data.period = PWM_HZ;

    pwm_data.pwm_num = LED_GREEN_PWM;
    hi_pwm_control(pwm_fd, &pwm_data);

    pwm_data.pwm_num = LED_RED_PWM;
    pwm_data.period = PWM_HZ * 3;
    hi_pwm_control(pwm_fd, &pwm_data);


    return EC_SUCCESS;
}
MODULE_ADD_INIT(MODULE_NAME, ec_led_init);


#define EVENT_TYPES  {              \
    EC_EVENT_WIFI_ON,               \
    EC_EVENT_WIFI_OFF,              \
    EC_EVENT_WIFI_CONNECT,          \
    EC_EVENT_WIFI_DISCONNECT,       \
    EC_EVENT_DATA_IN_TRANS,         \
    EC_EVENT_DATA_OUT_TRANS,        \
    EC_EVENT_RECODER_START,         \
    EC_EVENT_RECODER_STOP,          \
    EC_EVENT_CHARGE_ON,             \
    EC_EVENT_CHARGE_OFF,            \
    EC_EVENT_WAIT_POWER_OFF         \
    }


EC_INT ec_led_run (EC_VOID)
{
    ec_event_type eventTypes[] = EVENT_TYPES;

    ec_event_handle *h;

    EC_INT i;
    for (i = 0; i < ARRAY_SIZE(eventTypes); i++)
    {
        h = ec_event_new(eventTypes[i]);
        h->eventCallback = on_event ;
        ec_event_attache(h);
    }

    return EC_SUCCESS;
}
MODULE_ADD_RUN(MODULE_NAME, ec_led_run);


static EC_VOID on_event (EC_VOID *arg)
{
    ec_event_handle *h = (ec_event_handle *)arg;

    EC_INT duty, period, pwm_num;

    switch (h->eventType)
    {
        case EC_EVENT_WIFI_ON:
            duty = PWM_HZ;
            period = PWM_HZ * 3;
            pwm_num = LED_GREEN_PWM;
            break;
        case EC_EVENT_WIFI_CONNECT:
            duty = PWM_HZ;
            period = PWM_HZ * 2;
            pwm_num = LED_GREEN_PWM;
            break;
        case EC_EVENT_DATA_IN_TRANS:
            duty = PWM_HZ / 6;
            period = PWM_HZ /3;
            pwm_num = LED_GREEN_PWM;
            break;
        case EC_EVENT_DATA_OUT_TRANS:
            duty = PWM_HZ;
            period = PWM_HZ * 2;
            pwm_num = LED_GREEN_PWM;
            break;
        case EC_EVENT_WIFI_OFF:
            duty = PWM_HZ;
            period = PWM_HZ;
            pwm_num = LED_GREEN_PWM;
            break;
        case EC_EVENT_RECODER_START:
            duty = PWM_HZ / 6;
            period = PWM_HZ /3;
            pwm_num = LED_RED_PWM;
            break;
        case EC_EVENT_RECODER_STOP:
            duty = PWM_HZ * 2;
            period = PWM_HZ *3 ;
            pwm_num = LED_RED_PWM;
            break;
        case EC_EVENT_CHARGE_ON:
            duty = PWM_HZ;
            period = PWM_HZ * 3;
            pwm_num = LED_RED_PWM;
            break;
        case EC_EVENT_CHARGE_OFF:
            duty = PWM_HZ;
            period = PWM_HZ *3 ;
            pwm_num = LED_RED_PWM;
            break;
        case EC_EVENT_WAIT_POWER_OFF:
            duty = PWM_HZ;
            period = PWM_HZ;
            pwm_num = LED_RED_PWM;
            break;
        default:
            return;
    }

    PWM_DATA_S pwm_data = {.pwm_num = pwm_num, .period = period, .duty  = duty, .enable = 1};
    hi_pwm_control(pwm_fd, &pwm_data);
    if (h->eventType == EC_EVENT_WAIT_POWER_OFF)
    {
        pwm_data.pwm_num = LED_GREEN_PWM;
        hi_pwm_control(pwm_fd, &pwm_data);
    }

    return;
}