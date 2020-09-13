//
// Created by arnold on 5/3/2018.
//
#include "hisi_50/sample_comm.h"
#include "ec_hisi_sdk_live.h"
#include "ec_hisi_sdk_conf.h"
#include "ec_hisi_sdk.h"
#include "../conf/ec_conf.h"

#if 1
EC_INT ec_hisi_sdk_live_start (EC_INT *videoFd, EC_INT *audioFd)
{
    MEDIA_CONF_PTR;
    if (videoFd)
    {
        CHECK_FAILED(ec_hisi_sdk_venc_start(VPSS_LIVE_CNH, VENC_LIVE_CHN, &(mediaConf->liveConf)), 0);
        HI_MPI_VENC_SetMaxStreamCnt(VENC_LIVE_CHN, 2);
        *videoFd = ec_hisi_sdk_venc_get_fd (VENC_LIVE_CHN);
    }
    if (audioFd)
    {
        CHECK_FAILED(ec_hisi_sdk_aenc_start(AI_LIVE_CHN, AENC_LIVE_CHN), 1);
        *audioFd  = ec_hisi_sdk_aenc_get_fd(AENC_LIVE_CHN);
    }

    return EC_SUCCESS;
    failed_1:
    ec_hisi_sdk_venc_stop (VPSS_LIVE_CNH, VENC_LIVE_CHN);
    failed_0:
    return EC_FAILURE;
}
EC_VOID ec_hisi_sdk_live_stop (EC_INT videoFd, EC_INT audioFd)
{
    if (videoFd)
    {
        ec_hisi_sdk_venc_free_fd (VENC_LIVE_CHN, videoFd);
        ec_hisi_sdk_venc_stop (VPSS_LIVE_CNH, VENC_LIVE_CHN);
    }
    if (audioFd)
    {
        ec_hisi_sdk_aenc_free_fd (AENC_LIVE_CHN, audioFd);
        ec_hisi_sdk_aenc_stop(AI_LIVE_CHN, AENC_LIVE_CHN);
    }

    return;
}
EC_VOID ec_hisi_sdk_live_pause (EC_VOID)
{
     ec_hisi_sdk_venc_pause(VENC_LIVE_CHN);
     ec_hisi_sdk_aenc_pause(AI_LIVE_CHN,AENC_LIVE_CHN);
}
EC_VOID ec_hisi_sdk_live_resume (EC_VOID)
{
     ec_hisi_sdk_venc_resume(VENC_LIVE_CHN);
     ec_hisi_sdk_aenc_resume(AI_LIVE_CHN, VENC_LIVE_CHN);
}

ec_h264_frame *ec_hisi_sdk_live_h264_get (EC_VOID)
{
    return ec_hisi_sdk_get_h264 (VENC_LIVE_CHN);
}


ec_aac_frame *ec_hisi_sdk_live_aac_get (EC_VOID)
{
    return ec_hisi_sdk_get_aac(AENC_LIVE_CHN);
}

#else


EC_INT ec_hisi_sdk_live_start(EC_INT *videoFd, EC_INT *audioFd)
{
    MEDIA_CONF_PTR;
    EC_INT ret;
    if (videoFd)
    {
        CHECK_FAILED(ec_hisi_sdk_venc_start(VPSS_LIVE_CNH, VENC_LIVE_CHN, &(mediaConf->liveConf)), 0);
        HI_MPI_VENC_SetMaxStreamCnt(VENC_LIVE_CHN, 2);
        *videoFd = ec_hisi_sdk_venc_get_fd(VENC_LIVE_CHN);
    }
    if (audioFd)
    {
        ret = ec_hisi_sdk_aenc_start_2();
        if (ret == EC_FAILURE)
        {
            goto failed_1;
        }
        *audioFd = ret;
    }

    return EC_SUCCESS;
    failed_1:
    ec_hisi_sdk_venc_stop(VPSS_LIVE_CNH, VENC_LIVE_CHN);
    failed_0:
    return EC_FAILURE;

}

EC_VOID ec_hisi_sdk_live_stop(EC_INT videoFd, EC_INT audioFd)
{
    if (videoFd)
    {
        ec_hisi_sdk_venc_free_fd (VENC_LIVE_CHN, videoFd);
        ec_hisi_sdk_venc_stop (VPSS_LIVE_CNH, VENC_LIVE_CHN);
    }
    if (audioFd)
    {
       ec_hisi_sdk_aenc_stop_2();
    }

    return;
}

EC_VOID ec_hisi_sdk_live_pause(EC_VOID)
{
    ec_hisi_sdk_aenc_pause_2();

    return;
}

EC_VOID ec_hisi_sdk_live_resume(EC_VOID)
{
    ec_hisi_sdk_aenc_resume_2();

    return;
}

ec_h264_frame *ec_hisi_sdk_live_h264_get(EC_VOID)
{
    return ec_hisi_sdk_get_h264 (VENC_LIVE_CHN);
}

ec_aac_frame *ec_hisi_sdk_live_aac_get(EC_VOID)
{
    return ec_hisi_sdk_get_aac_2();
}

EC_VOID ec_hisi_sdk_live_aac_free (ec_aac_frame *frame)
{
    ec_hisi_sdk_free_aac_2(frame);
}

#endif