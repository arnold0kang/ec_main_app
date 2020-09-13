//
// Created by arnold on 4/12/2018.
//

#include <unistd.h>
#include "ec_beep.h"

#include "../module_manager/ec_module.h"
#include "../pwm/hi_pwm.h"
#include "../conf/ec_conf.h"

#define MODULE_NAME                     "ec_beep"
#define EC_BEEP_PWM_PATH                 "/dev/pwm"
#define EC_BEEP_PWM_INDEX                2
#define EC_BEEP_DUTY                     550 //420
#define EC_BEEP_PERIOD                   1100 //600
#define EC_BEEP_WAIT_INTERVAL            100

static EC_INT pwm_handle = EC_FAILURE;



EC_INT ec_beep_init (EC_VOID)
{
    pwm_handle = hi_pwm_open(EC_BEEP_PWM_PATH);
    if (pwm_handle == EC_FAILURE)
    {
        return EC_FAILURE;
    }
    else
    {
        return EC_SUCCESS;
    }
}
MODULE_ADD_INIT(MODULE_NAME, ec_beep_init);


EC_VOID ec_beep_do_once (EC_VOID)
{

    PWM_DATA_S pwm_data = {
            EC_BEEP_PWM_INDEX, EC_BEEP_DUTY, EC_BEEP_PERIOD * 2, EC_TRUE
    };

    hi_pwm_control(pwm_handle, &pwm_data);
    usleep (10);
    pwm_data.enable = EC_FALSE;
    hi_pwm_control(pwm_handle, &pwm_data);

    return;
}

EC_VOID ec_beep_on (EC_VOID)
{
    PWM_DATA_S pwm_data = {
            EC_BEEP_PWM_INDEX, PWM_HZ, PWM_HZ * 2, EC_TRUE
    };

    hi_pwm_control(pwm_handle, &pwm_data);
    return;
}

EC_VOID ec_beep_off (EC_VOID)
{
    PWM_DATA_S pwm_data = {
            EC_BEEP_PWM_INDEX, PWM_HZ, PWM_HZ * 2, EC_FALSE
    };

    hi_pwm_control(pwm_handle, &pwm_data);

    return;
}