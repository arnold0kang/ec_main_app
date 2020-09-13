//
// Created by arnold on 4/12/2018.
//

#ifndef EC_MAIN_APP_EC_BEEP_H
#define EC_MAIN_APP_EC_BEEP_H

#include "../common/ec_define.h"

#define BEEP() \
    ec_beep_do_once ()

EC_VOID ec_beep_do_once (EC_VOID);

EC_VOID ec_beep_on (EC_VOID);

EC_VOID ec_beep_off (EC_VOID);

#endif //EC_MAIN_APP_EC_BEEP_H
