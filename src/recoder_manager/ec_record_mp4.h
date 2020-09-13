//
// Created by arnold on 10/29/2018.
//

#ifndef EC_MAIN_APP_EC_RECORD_MP4_H
#define EC_MAIN_APP_EC_RECORD_MP4_H

#include <mp4v2/mp4v2.h>
#include "../common/ec_define.h"


typedef struct
{

    MP4FileHandle mp4handle;
    MP4TrackId videoTrack;
    MP4TrackId audioTrack;

    EC_CHAR currentFileName[EC_FILE_NAME_MAX_LEN];
} ec_mp4_info;


EC_INT open_mp4_handle(ec_mp4_info *mp4_info);

EC_INT close_mp4_handle(ec_mp4_info *mp4_info);

EC_VOID close_mp4_handle_bg  (MP4FileHandle handle);

EC_BOOL mp4_have_bg (EC_VOID);

#endif //EC_MAIN_APP_EC_RECORD_MP4_H
