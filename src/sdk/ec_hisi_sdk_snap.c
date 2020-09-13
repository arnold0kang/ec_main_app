//
// Created by arnold on 5/3/2018.
//
#include <unistd.h>
#include "hisi_50/sample_comm.h"
#include "ec_hisi_sdk.h"
#include "ec_hisi_sdk_snap.h"
#include "../conf/ec_conf.h"
#include "ec_hisi_sdk_conf.h"
#include "../log/ec_log.h"

static EC_VOID ec_hisi_sdk_savesnap (const EC_CHAR *fileName, VENC_STREAM_S* pstStream)
{
    FILE *fp = EC_NULL;

    fp = fopen (fileName, "wb");
    if (fp == NULL)
    {
        return;
    }

    if (SAMPLE_COMM_VENC_SaveJPEG(fp, pstStream) != HI_SUCCESS)
    {
        dzlog_error("save snap failed");
        goto failed;
    }


    fclose (fp);

    return;
    failed:
    fclose (fp);
    unlink (fileName);

    return;
}


EC_INT ec_hisi_sdk_snap_to_file (const EC_CHAR *fileName)
{
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 s32VencFd;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S stRecvParam;
    //VENC_STREAM_BUF_INFO_S stStreamBufInfo;
    MEDIA_CONF_PTR;

    //start snap venc module
    CHECK_FAILED(ec_hisi_sdk_snap_start(VPSS_SNAP_CHN, VENC_SNAP_CHN, &(mediaConf->snapConf)), 0);

    /******************************************
    step 2:  Start Recv Venc Pictures
   ******************************************/
    stRecvParam.s32RecvPicNum = 1;
    s32Ret = HI_MPI_VENC_StartRecvPicEx(VENC_SNAP_CHN, &stRecvParam);
    CHECK_SUCCESS(s32Ret, HI_SUCCESS, 1);

    /******************************************
     step 3:  recv picture
    ******************************************/
    s32VencFd = HI_MPI_VENC_GetFd(VENC_SNAP_CHN);
    if (s32VencFd < 0)
    {
        goto failed_3;
        //return HI_FAILURE;
    }
    FD_ZERO(&read_fds);
    FD_SET(s32VencFd, &read_fds);
    TimeoutVal.tv_sec  = 2;
    TimeoutVal.tv_usec = 0;
    s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if (s32Ret < 0)
    {
        dzlog_error("snap select failed!\n");
        goto failed_4;
    }
    else if (0 == s32Ret)
    {
        dzlog_error("snap time out!\n");
        return HI_FAILURE;
    }
    else
    {
        if (FD_ISSET(s32VencFd, &read_fds))
        {
            s32Ret = HI_MPI_VENC_Query(VENC_SNAP_CHN, &stStat);
            CHECK_SUCCESS(s32Ret, HI_SUCCESS, 5);
            /*******************************************************
            suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
             if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
             {              SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                return HI_SUCCESS;
             }
             *******************************************************/

            if (0 == stStat.u32CurPacks)
            {
                SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                goto failed_6;
            }
            stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            CHECK_NULL(stStream.pstPack, 7);

            stStream.u32PackCount = stStat.u32CurPacks;
            s32Ret = HI_MPI_VENC_GetStream(VENC_SNAP_CHN, &stStream, -1);
            CHECK_SUCCESS(s32Ret, HI_SUCCESS, 8);

            ec_hisi_sdk_savesnap(fileName, &stStream);

            s32Ret = HI_MPI_VENC_ReleaseStream(VENC_SNAP_CHN, &stStream);
            free(stStream.pstPack);
            stStream.pstPack = NULL;
        }
    }
    /******************************************
     step 4:  stop recv picture
    ******************************************/
    HI_MPI_VENC_StopRecvPic(VENC_SNAP_CHN);
    ec_hisi_sdk_snap_stop(VPSS_SNAP_CHN, VENC_SNAP_CHN);

    return EC_SUCCESS;

    failed_9:
    failed_8:
    free(stStream.pstPack);
    failed_7:
    failed_6:
    failed_5:
    failed_4:
    failed_3:
    HI_MPI_VENC_StopRecvPic(VENC_SNAP_CHN);
    failed_2:
    failed_1:
    ec_hisi_sdk_snap_stop(VPSS_SNAP_CHN, VENC_SNAP_CHN);
    failed_0:
    return EC_FAILURE;
}