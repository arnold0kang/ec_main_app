//
// Created by arnold on 5/3/2018.
//

#include <mpi_venc.h>
#include "ec_hisi_sdk_recorder.h"
#include "ec_hisi_sdk_conf.h"
#include "../conf/ec_conf.h"
#include "../log/ec_log.h"


EC_INT ec_hisi_sdk_recorde_start (EC_INT *videoFd, EC_INT *audioFd)
{
    MEDIA_CONF_PTR;
    if (videoFd)
    {
        CHECK_FAILED(ec_hisi_sdk_venc_start(VPSS_RECORDE_CHN, VENC_RECORDE_CHN, &(mediaConf->videoConf)), 0);
        HI_MPI_VENC_SetMaxStreamCnt(VENC_RECORDE_CHN, 30);
        *videoFd = ec_hisi_sdk_venc_get_fd (VENC_RECORDE_CHN);
    }
    if (audioFd)
    {
        CHECK_FAILED(ec_hisi_sdk_aenc_start(AI_RECORDE_CHN, AENC_RECORDE_CHN), 1);
        *audioFd  = ec_hisi_sdk_aenc_get_fd(AENC_RECORDE_CHN);
    }
    dzlog_debug ("get recode videofd %d audiofd %d", *videoFd, *audioFd);

    return EC_SUCCESS;
    failed_1:
    ec_hisi_sdk_venc_stop (VPSS_RECORDE_CHN, VENC_RECORDE_CHN);
    failed_0:
    return EC_FAILURE;
}
EC_VOID ec_hisi_sdk_recorde_stop (EC_INT videoFd, EC_INT audioFd)
{
    if (videoFd)
    {
        ec_hisi_sdk_venc_free_fd (VENC_RECORDE_CHN, videoFd);
        ec_hisi_sdk_venc_stop (VPSS_RECORDE_CHN, VENC_RECORDE_CHN);
    }
    if (audioFd)
    {
        ec_hisi_sdk_aenc_free_fd (AENC_RECORDE_CHN, audioFd);
        ec_hisi_sdk_aenc_stop(AI_RECORDE_CHN,AENC_RECORDE_CHN);
    }

    return;
}
EC_VOID ec_hisi_sdk_recorde_pause (EC_VOID)
{
    ec_hisi_sdk_venc_pause(VENC_RECORDE_CHN);
    ec_hisi_sdk_aenc_pause(AI_RECORDE_CHN, AENC_RECORDE_CHN);
}
EC_VOID ec_hisi_sdk_recorde_resume (EC_VOID)
{
    ec_hisi_sdk_venc_resume(VENC_RECORDE_CHN);
    ec_hisi_sdk_aenc_resume(AI_RECORDE_CHN, AENC_RECORDE_CHN);
}

ec_h264_frame *ec_hisi_sdk_recorde_h264_get (EC_VOID)
{
    return ec_hisi_sdk_get_h264 (VENC_RECORDE_CHN);
}


ec_aac_frame *ec_hisi_sdk_recorde_aac_get (EC_VOID)
{
    return ec_hisi_sdk_get_aac(AENC_RECORDE_CHN);
}
