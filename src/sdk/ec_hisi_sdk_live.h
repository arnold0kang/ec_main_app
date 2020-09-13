//
// Created by arnold on 5/3/2018.
//

#ifndef EC_MAIN_APP_EC_HISI_SDK_LIVE_H
#define EC_MAIN_APP_EC_HISI_SDK_LIVE_H

#include "../common/ec_define.h"
#include "ec_hisi_sdk.h"

EC_INT ec_hisi_sdk_live_start (EC_INT *videoFd, EC_INT *audioFd);
EC_VOID ec_hisi_sdk_live_stop (EC_INT videoFd, EC_INT audioFd);
EC_VOID ec_hisi_sdk_live_pause (EC_VOID);
EC_VOID ec_hisi_sdk_live_resume (EC_VOID);

ec_h264_frame *ec_hisi_sdk_live_h264_get (EC_VOID);

ec_aac_frame *ec_hisi_sdk_live_aac_get (EC_VOID);



#endif //EC_MAIN_APP_EC_HISI_SDK_LIVE_H
