//
// Created by arnold on 3/23/2018.
//

#ifndef EC_MAIN_APP_EC_DISK_MONITOR_H
#define EC_MAIN_APP_EC_DISK_MONITOR_H
#include "../common/ec_define.h"

EC_VOID ec_disk_monitor_start (EC_VOID);
EC_VOID ec_disk_monitor_stop (EC_VOID);

EC_BOOL ec_disk_is_enough (EC_VOID);

#endif //EC_MAIN_APP_EC_DISK_MONITOR_H
