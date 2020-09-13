//
// Created by arnold on 4/12/2018.
//

#ifndef EC_MAIN_APP_HI_PWM_H
#define EC_MAIN_APP_HI_PWM_H

#include "../common/ec_define.h"

typedef struct hiPWM_DATA_S
{
    unsigned char pwm_num;  //0:PWM0,1:PWM1,2:PWM2,3:PWMII0,4:PWMII1,5:PWMII2
    unsigned int  duty;
    unsigned int  period;
    unsigned char enable;
} PWM_DATA_S;

#define PWM_CMD_WRITE      0x01
#define PWM_CMD_READ       0x03



EC_INT hi_pwm_open (const EC_CHAR *dev_path);


EC_INT hi_pwm_control (EC_INT handle, PWM_DATA_S *pwmData);

EC_INT hi_pwm_close (EC_INT handle);

#endif //EC_MAIN_APP_HI_PWM_H
