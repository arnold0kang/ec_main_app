//
// Created by arnold on 4/12/2018.
//
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_pwm.h"



EC_INT hi_pwm_open (const EC_CHAR *dev_path)
{
    return open(dev_path, 0);
}


EC_INT hi_pwm_control (EC_INT handle, PWM_DATA_S *pwmData)
{
    ioctl(handle, PWM_CMD_WRITE, pwmData);

    return EC_SUCCESS;
}

EC_INT hi_pwm_close (EC_INT handle)
{
    close (handle);

    return EC_SUCCESS;
}