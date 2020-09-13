//
// Created by arnold on 3/23/2018.
//

#ifndef EC_MAIN_APP_EC_RECODE_WORKER_H
#define EC_MAIN_APP_EC_RECODE_WORKER_H

#include "../common/ec_define.h"
#include "../libuv_wraper/ec_libuv.h"

EC_VOID ec_record_work_run (EC_VOID);
EC_VOID ec_record_work_stop (EC_VOID);
EC_BOOL ec_record_work_is_run (EC_VOID);

#endif //EC_MAIN_APP_EC_RECODE_WORKER_H
