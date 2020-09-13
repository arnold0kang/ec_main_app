//
// Created by arnold on 10/29/2018.
//
#include <unistd.h>
#include <pthread.h>
#include "ec_record_mp4.h"
#include "../conf/ec_conf.h"
#include "../rtc/ec_rtc.h"
#include "../utils/ec_utils.h"
#include "../log/ec_log.h"


static EC_INT getAVCProfileIndication(EC_VOID)
{
    VIDEO_CONF_PTR;

    switch (videoConf->h264ProfileType)
    {
        case H264_BASE_PROFILE:
            return 66;
        case H264_MAIN_PROFILE:
            return 77;
        case H264_HIGH_PROFILE:
            return 100;
        default:
            return 77;
            break;
    }
}

static EC_INT get_profile_compat(EC_VOID)
{
    return 0x00;
}

static EC_INT getAVCLevlIndication(EC_VOID)
{
    return 0x28;
}


EC_INT open_mp4_handle(ec_mp4_info *mp4_info)
{
    EC_CHAR tsBuf[128] = {'\0'};
    DEV_CONF_PTR;
    VIDEO_CONF_PTR;


    MP4LogSetLevel(MP4_LOG_INFO);

    //getStrTime(tsBuf, 127);
    ec_rtc_get_ts(tsBuf, 127);
    EC_MEMSET_2(mp4_info->currentFileName, EC_FILE_NAME_MAX_LEN);
    snprintf(mp4_info->currentFileName, EC_FILE_NAME_MAX_LEN - 1,
             "%s/%s-%s-%s.mp4", EC_DEFAULT_RECODE_PATH, devConf->devid, devConf->userid, tsBuf);
    dzlog_info("new file : %s", mp4_info->currentFileName);
    mp4_info->mp4handle = MP4Create(mp4_info->currentFileName, 0);
    //set video part
    if (mp4_info->mp4handle == MP4_INVALID_FILE_HANDLE)
    {
        dzlog_error("mp4 Createex failed");
        return EC_FAILURE;
    }
    CHECK_SUCCESS(MP4SetTimeScale(mp4_info->mp4handle, 90000), 1, 0);


#if 0
    mp4_info->videoTrack = MP4AddH264VideoTrack(mp4_info->mp4handle, 90000,
                                                90000 / videoConf->framerate,
                                                get_video_width(videoConf->ensize),
                                                get_video_height(videoConf->ensize),
                                                getAVCProfileIndication(),
                                                get_profile_compat(),
                                                getAVCLevlIndication(),
                                                3/*nalu header 0x00 0x00 0x01*/);
#else
    mp4_info->videoTrack = MP4AddH264VideoTrack(mp4_info->mp4handle, 90000,
                                                90000 / videoConf->framerate,
                                                1920,
                                                1080,
                                                77,
                                               0x00,
                                                0x28,
                                                3/*nalu header 0x00 0x00 0x01*/);
#endif
    if (mp4_info->videoTrack == MP4_INVALID_TRACK_ID)
    {
        dzlog_error("add video track failed");
        goto failed_0;
    }
    MP4SetVideoProfileLevel(mp4_info->mp4handle, 0x04);
#if 1
    //below audio config is should be recode
    //for some config is from ec_hisi_sdk.c audio init part
    //why this config is so bad, for there is no way to set better
    mp4_info->audioTrack = MP4AddAudioTrack(mp4_info->mp4handle,
                                            22050/*from aacAttr.enSmpRate = AUDIO_SAMPLE_RATE_22050*/,
                                            1024/* from aencAttr.u32PtNumPerFrm = 882 */,
                                            MP4_MPEG4_AUDIO_TYPE);
    if (mp4_info->audioTrack == MP4_INVALID_TRACK_ID)
    {
        dzlog_error("add video track failed");
        goto failed_0;
    }
    MP4SetAudioProfileLevel(mp4_info->mp4handle, 0x4);
    //for 22050 is   00010 0111 0001 000
    //for 44100 is   00010 0100  0001 000  //https://blog.csdn.net/dxpqxb/article/details/42266873
   // uint8_t config[2] = {0x13, 0x88}; for 22050
    uint8_t config[2] = {0x12, 0x08}; //for 44100
    CHECK_SUCCESS(MP4SetTrackESConfiguration(mp4_info->mp4handle, mp4_info->audioTrack, config, 2), 1, 0);
    MP4SetAudioProfileLevel(mp4_info->mp4handle, 0x02);
#endif


    return EC_SUCCESS;

    failed_0:
    MP4Close(mp4_info->mp4handle, 0);
    mp4_info->mp4handle = NULL;
    unlink(mp4_info->currentFileName);
    return EC_FAILURE;
}


EC_INT close_mp4_handle(ec_mp4_info *mp4_info)
{
    if (mp4_info->mp4handle)
    {
        //flush cache first

        MP4Close(mp4_info->mp4handle, 0);
        mp4_info->mp4handle = EC_NULL;
    }
    // dzlog_debug("mv file %s to %s", TMP_FILE_NAME, recode_stat.currentFileName);
    //  rename(TMP_FILE_NAME, recode_stat.currentFileName);
    //resetVideoDuration();
    //  resetAudioDuration();

    dzlog_debug("close mp4 file success");

    return EC_SUCCESS;

}

static EC_INT current_thread_cnt = 0;


static EC_VOID *close_thread(EC_VOID *arg)
{
    ec_atomic_add(&current_thread_cnt);
    MP4FileHandle  h = (MP4FileHandle)arg;
    MP4Close(h, 0);
    ec_atomic_sub(&current_thread_cnt);
}

EC_VOID close_mp4_handle_bg  (MP4FileHandle handle)
{
    pthread_t pid;
    pthread_create(&pid, NULL, close_thread, handle);
}

EC_BOOL mp4_have_bg (EC_VOID)
{
    return current_thread_cnt;
}