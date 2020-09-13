#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../common/ec_define.h"
#include "../conf/ec_conf.h"
#include "ec_hisi_sdk.h"
#include "hisi_50/audio_aac_adp.h"
#include "hisi_50/sample_comm.h"
#include "hisi_50/loadbmp.h"
#include "ec_osd.h"
#include "../module_manager/ec_module.h"
#include "../event/ec_event.h"
#include "ec_hisi_sdk_conf.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../utils/ec_utils.h"
#include "../log/ec_log.h"


#define MODULE_NAME         "ec_sdk"

static EC_VOID do_sdk_deInit(EC_VOID);

static EC_INT do_sdk_init(EC_VOID);

static EC_VOID onChargeEvent(EC_VOID *arg);

static EC_VOID doOnChargeOn(EC_VOID);

static EC_VOID doOnChargeOff(EC_VOID);

static EC_INT init_sdk_base(EC_VOID);

static EC_INT init_sdk_vi(EC_VOID);

static EC_INT init_sdk_vpss(EC_VOID);

static EC_INT init_sdk_osd(EC_VOID);

static EC_INT deinit_sdk_osd(EC_VOID);

static EC_INT start_sdk_vi(EC_VOID);

static EC_INT stop_sdk_vi(EC_VOID);

static EC_INT init_sdk_ai(EC_VOID);

static EC_INT deinit_sdk_ai(EC_VOID);


#define EC_SDK_MAX_POOL_CNT                    128
static VIDEO_NORM_E gEnNorm = VIDEO_ENCODING_MODE_NTSC;
static RGN_CANVAS_INFO_S canvasInfo;  //for osd info
static EC_BOOL sdk_stat = EC_FALSE;

static EC_VOID ec_live_signal(EC_LLONG pts);

static EC_INT live_write_index = 0;
static EC_INT live_read_index = 0;




EC_INT ec_hisi_sdk_init(EC_VOID)
{

}



EC_INT ec_hisi_sdk_run(EC_VOID)
{
    ec_hisi_sdk_start();
}
//MODULE_ADD_RUN(MODULE_NAME, ec_hisi_sdk_run);



static inline PAYLOAD_TYPE_E getPayLoad(EC_INT i)
{
    switch (i)
    {
        case EN_TYPE_H264:
            return PT_H264;
            break;
        case EN_TYPE_H265:
            return PT_H265;
            break;
        default:
            return PT_H264;
            break;
    }
}

static inline PIC_SIZE_E getPicSize(EC_INT i)
{
    switch (i)
    {
        case EN_SIZE_1080:
            return PIC_HD1080;
            break;
        case EN_SIZE_720:
            return PIC_HD720;
            break;
        default:
            return PIC_HD720;
            break;
    }
}

EC_INT ec_hisi_sdk_venc_start(EC_INT vpssChn, EC_INT vencChn, ec_video_conf_t *conf)
{
    HI_S32 s32Ret;
    SIZE_S stSize;
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;
    EC_ERR_PTR;


    PAYLOAD_TYPE_E payload = getPayLoad(conf->entype);
    PIC_SIZE_E enSize = getPicSize(conf->ensize);

    CHECK_SUCCESS(SAMPLE_COMM_SYS_GetPicSize(gEnNorm, enSize,
                                             &stSize), HI_SUCCESS, 0);

    stVpssChnMode.enChnMode = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble = HI_FALSE;
    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width = stSize.u32Width;
    stVpssChnMode.u32Height = stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_SEG;
    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;

    CHECK_SUCCESS(SAMPLE_COMM_VPSS_EnableChn(VPSS_GROUP, vpssChn,
                                             &stVpssChnAttr, &stVpssChnMode, HI_NULL),
                  HI_SUCCESS, 1);
    dzlog_debug("start venc wiht %d %d %d %d %d %d %d %d %d %d", vencChn, payload,
                gEnNorm, enSize, conf->rctype, conf->h264ProfileType, ROTATE_NONE, conf->framerate,
                conf->bitrate, conf->gop);
    CHECK_SUCCESS(SAMPLE_COMM_VENC_Start(vencChn, payload,
                                         gEnNorm, enSize,
                                         conf->rctype, conf->h264ProfileType, ROTATE_NONE,
                                         conf->framerate,
                                         conf->bitrate, conf->gop), HI_SUCCESS, 2);

    //set the frame depth


    CHECK_SUCCESS(SAMPLE_COMM_VENC_BindVpss(vencChn, VPSS_GROUP,
                                            vpssChn), HI_SUCCESS, 3);


    dzlog_debug("start venc %d %d success", vencChn, vpssChn);
    return EC_SUCCESS;

    failed_3:
    EC_ERR_SET("bind vpss failed");
    failed_2:
    EC_ERR_SET("start venc failed");
    failed_1:
    EC_ERR_SET("enable vpss failed");
    failed_0:
    EC_ERR_SET("get ensize failed");
    EC_ERR_OUT();

    return EC_FAILURE;
}

EC_INT ec_hisi_sdk_venc_stop(EC_INT vpssChn, EC_INT vencChn)
{
    EC_ERR_PTR;
    CHECK_SUCCESS(SAMPLE_COMM_VENC_UnBindVpss(vencChn, VPSS_GROUP, vpssChn), HI_SUCCESS, 0);
    CHECK_SUCCESS(SAMPLE_COMM_VENC_Stop(vencChn), HI_SUCCESS, 1);
    CHECK_SUCCESS(SAMPLE_COMM_VPSS_DisableChn(VPSS_GROUP, vpssChn), HI_SUCCESS, 1);

    return EC_SUCCESS;

    failed_1:
    EC_ERR_SET("stop venc failed");
    failed_0:
    EC_ERR_SET("unbind vpss failed");
    EC_ERR_OUT();
    return EC_FAILURE;
}

EC_INT ec_hisi_sdk_venc_get_fd(EC_INT vencChn)
{
    EC_INT fd = HI_MPI_VENC_GetFd(vencChn);

    return fd;
}

EC_INT ec_hisi_sdk_venc_free_fd(EC_INT vencChn, EC_INT fd)
{
    HI_MPI_VENC_CloseFd(vencChn);

    return EC_SUCCESS;
}


EC_INT ec_hisi_sdk_snap_start(EC_INT vpssChn, EC_INT vencChn, ec_video_conf_t *conf)
{
    SIZE_S stSize;
    HI_S32 s32Ret;
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;


    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gEnNorm, getPicSize(conf->ensize), &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("get pic size for snap failed with %0x", s32Ret);
        return EC_FAILURE;
    }

    stVpssChnMode.enChnMode = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble = HI_FALSE;
    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width = stSize.u32Width;
    stVpssChnMode.u32Height = stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;

    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VPSS_GROUP, vpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    CHECK_SUCCESS(s32Ret, HI_SUCCESS, 0);

    s32Ret = SAMPLE_COMM_VENC_SnapStart(vencChn, &stSize);
    CHECK_SUCCESS(s32Ret, HI_SUCCESS, 1);


    s32Ret = SAMPLE_COMM_VENC_BindVpss(vencChn, VPSS_GROUP, vpssChn);
    CHECK_SUCCESS(s32Ret, HI_SUCCESS, 2);

    return EC_SUCCESS;

    failed_2:
    SAMPLE_COMM_VENC_SnapStop(vencChn);
    failed_1:
    SAMPLE_COMM_VPSS_DisableChn(VPSS_GROUP, VENC_SNAP_CHN);
    failed_0:
    return EC_FAILURE;
}

EC_INT ec_hisi_sdk_snap_stop(EC_INT vpssChn, EC_INT vencChn)
{
    SAMPLE_COMM_VENC_UnBindVpss(vencChn, VPSS_GROUP, vpssChn);
    SAMPLE_COMM_VENC_SnapStop(vencChn);
    SAMPLE_COMM_VPSS_DisableChn(VPSS_GROUP, vpssChn);

    return EC_SUCCESS;
}

EC_INT ec_hisi_sdk_snap_get_fd(EC_INT vencChn)
{
    return ec_hisi_sdk_venc_get_fd(vencChn);
}

EC_INT ec_hisi_sdk_snap_free_fd(EC_INT vencChn, EC_INT fd)
{
    return ec_hisi_sdk_venc_free_fd(vencChn, fd);
}

EC_INT ec_hisi_sdk_aenc_start(EC_INT aiChn, EC_INT aencChn)
{
    AENC_CHN_ATTR_S aencAttr;
    AENC_ATTR_AAC_S aacAttr;
    HI_S32 s32Ret;


    aencAttr.enType = PT_AAC;
    aencAttr.u32BufSize = 30;
    aencAttr.pValue = &aacAttr;
    aencAttr.u32PtNumPerFrm = 1024;


    aacAttr.enAACType = AAC_TYPE_AACLC;
    aacAttr.enBitRate = AAC_BPS_64K;
    aacAttr.enBitWidth = AUDIO_BIT_WIDTH_16;
    aacAttr.enSmpRate = AUDIO_SAMPLE_RATE_44100;
    aacAttr.enSoundMode = AUDIO_SOUND_MODE_MONO;
    aacAttr.enTransType = AAC_TRANS_TYPE_ADTS;
    aacAttr.s16BandWidth = 0;


    s32Ret = HI_MPI_AI_EnableChn(AI_DEV, aiChn);
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("enable ai channel  failed %#02x", s32Ret);
        return EC_FAILURE;
    }


    s32Ret = HI_MPI_AENC_CreateChn(aencChn, &aencAttr);
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("aenc create failed %#02x", s32Ret);
        goto failed_0;
    }


    s32Ret = SAMPLE_COMM_AUDIO_AencBindAi(AI_DEV, aiChn, aencChn);
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("bind aenc failed %#02x", s32Ret);
        goto failed_1;
    }
    dzlog_debug("start aenc %d %d %d success", AI_DEV, aiChn, aencChn);

    return EC_SUCCESS;


    failed_1:
    failed_0:

    return EC_FAILURE;

}

EC_INT ec_hisi_sdk_aenc_stop(EC_INT aiChn, EC_INT aencChn)
{

    SAMPLE_COMM_AUDIO_AencUnbindAi(AI_DEV, aiChn, aencChn);
    //disable
    HI_MPI_AI_DisableChn(AI_DEV, aiChn);
    //destroy
    HI_MPI_AENC_DestroyChn(aencChn);


    return EC_SUCCESS;
}

EC_INT ec_hisi_sdk_aenc_get_fd(EC_INT aencChn)
{

    return HI_MPI_AENC_GetFd(aencChn);


}

EC_INT ec_hisi_sdk_aenc_free_fd(EC_INT aencChn, EC_INT fd)
{
    return EC_SUCCESS;
}


ec_h264_frame *ec_hisi_sdk_get_h264(EC_INT vencChn)
{
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S liveStream;
    ec_h264_frame *frame;
    EC_ERR_PTR;

    EC_MEMSET(&liveStream);
    EC_MALLOC(frame);
    CHECK_SUCCESS(HI_MPI_VENC_Query(vencChn, &stStat), HI_SUCCESS, 1);
    if (0 == stStat.u32CurPacks)
    {
        EC_FREE(frame);
        return EC_NULL;
    }

    liveStream.u32PackCount = stStat.u32CurPacks;
    liveStream.pstPack = (VENC_PACK_S *) malloc(sizeof(VENC_PACK_S) * liveStream.u32PackCount);
    CHECK_NULL(liveStream.pstPack, 2);
    CHECK_SUCCESS(HI_MPI_VENC_GetStream(vencChn, &liveStream, HI_TRUE), HI_SUCCESS, 3);
    frame->pts = liveStream.pstPack[0].u64PTS / 1000;
    VENC_PACK_S *pstData;
    if (liveStream.u32PackCount == 4) //frome sdk
    {
        frame->keyFlag = EC_TRUE;

        //set sps
        pstData = &(liveStream.pstPack[0]);
        frame->spsLen = pstData->u32Len - pstData->u32Offset;
        EC_MALLOC_WITH_SIZE(frame->sps, frame->spsLen);
        memcpy(frame->sps, pstData->pu8Addr + pstData->u32Offset, frame->spsLen);

        //set pps
        pstData = &(liveStream.pstPack[1]);
        frame->ppsLen = pstData->u32Len - pstData->u32Offset;
        EC_MALLOC_WITH_SIZE(frame->pps, frame->ppsLen);
        memcpy(frame->pps, pstData->pu8Addr + pstData->u32Offset, frame->ppsLen);
        //set sei
        pstData = &(liveStream.pstPack[2]);
        frame->seiLen = pstData->u32Len - pstData->u32Offset;
        EC_MALLOC_WITH_SIZE(frame->sei, frame->seiLen);
        memcpy(frame->sei, pstData->pu8Addr + pstData->u32Offset, frame->seiLen);

        //set I frame
        pstData = &(liveStream.pstPack[3]);
        frame->frameLen = pstData->u32Len - pstData->u32Offset;
        EC_MALLOC_WITH_SIZE(frame->frame, frame->frameLen);
        memcpy(frame->frame, pstData->pu8Addr + pstData->u32Offset, frame->frameLen);
    }
    else
    {
        //set  frame
        pstData = &(liveStream.pstPack[0]);
        frame->frameLen = pstData->u32Len - pstData->u32Offset;
        EC_MALLOC_WITH_SIZE(frame->frame, frame->frameLen);
        memcpy(frame->frame, pstData->pu8Addr + pstData->u32Offset, frame->frameLen);
        frame->keyFlag = EC_FALSE;

    }

    HI_MPI_VENC_ReleaseStream(vencChn, &liveStream);
    EC_FREE(liveStream.pstPack);

    return frame;

    failed_4:
    EC_FREE(frame->sps);
    EC_FREE(frame->pps);
    EC_FREE(frame->sei);
    EC_FREE(frame->frame);
    HI_MPI_VENC_ReleaseStream(vencChn, &liveStream);
    failed_3:
    EC_FREE(liveStream.pstPack);
    EC_ERR_SET("get stream failed");
    failed_2:
    EC_ERR_SET("malloc failed");
    failed_1:
    EC_FREE(frame);
    EC_ERR_SET("query venc failed");
    failed_0:
    EC_ERR_SET("malloc video frame failed");
    EC_ERR_OUT();
    return EC_NULL;
}




ec_aac_frame *ec_hisi_sdk_get_aac(EC_INT aenChn)
{


    HI_S32 s32Ret;
    ec_aac_frame *frame;

    EC_MALLOC(frame);


    AUDIO_STREAM_S audioStreamLive;
    EC_MEMSET(&(audioStreamLive));
    s32Ret = HI_MPI_AENC_GetStream(aenChn, &(audioStreamLive), 0);
    CHECK_SUCCESS(s32Ret, HI_SUCCESS, 0);
    frame->dataLen = audioStreamLive.u32Len;
    frame->pts = audioStreamLive.u64TimeStamp / 1000;
    EC_MALLOC_WITH_SIZE(frame->data, frame->dataLen);
    memcpy(frame->data, audioStreamLive.pStream, frame->dataLen);

    HI_MPI_AENC_ReleaseStream(aenChn, &(audioStreamLive));



    return frame;

    failed_0:
    EC_FREE(frame);
    return NULL;


}

EC_VOID ec_hisi_sdk_update_osd(EC_VOID *bitmap, EC_INT bitMapSize)
{
    //return;
    if (!bitmap)
    {
        return;
    }
    //   dzlog_debug("bitMapSize %d", bitMapSize);

    HI_U16 bmpType = *(HI_U16 *) bitmap;
    OSD_BITMAPFILEHEADER *header = (OSD_BITMAPFILEHEADER *) ((EC_CHAR *) bitmap + sizeof(HI_U16));
    OSD_BITMAPINFO *info = (OSD_BITMAPINFO *) ((EC_CHAR *) header + sizeof(OSD_BITMAPFILEHEADER));
    EC_VOID *bmpBody = (EC_VOID *) ((EC_CHAR *) bitmap + header->bfOffBits);
    HI_U16 bpp = info->bmiHeader.biBitCount / 8;
    HI_U32 stride = info->bmiHeader.biWidth * bpp;


    if (stride % 4)
    {
        stride = (stride & 0xfffc) + 4;
    }
    EC_INT w, h;
    if (info->bmiHeader.biWidth > OSD_WIDTH || info->bmiHeader.biHeight > OSD_HEIGHT)
    {
        dzlog_error("osd bmp map is to large");
        return;
    }

    for (h = 0; h < info->bmiHeader.biHeight; h++)
    {
        for (w = 0; w < info->bmiHeader.biWidth; w++)
        {
            memcpy((EC_VOID *) (canvasInfo.u32VirtAddr) +
                   h * canvasInfo.u32Stride + w * bpp, bmpBody +
                                                       ((info->bmiHeader.biHeight - 1) - h) * stride + w * bpp, bpp);
        }
    }
    HI_MPI_RGN_UpdateCanvas(OSD_CHN);

    return;
}

EC_INT ec_hisi_sdk_venc_pause(EC_INT vencChn)
{
    HI_MPI_VENC_StopRecvPic(vencChn);
    if (vencChn == VENC_RECORDE_CHN)
    {
        SAMPLE_COMM_VENC_UnBindVpss(vencChn, VPSS_GROUP, VPSS_RECORDE_CHN);
    }
    else
    {
        SAMPLE_COMM_VENC_UnBindVpss(vencChn, VPSS_GROUP, VPSS_LIVE_CNH);
    }


    return EC_SUCCESS;
}

EC_INT ec_hisi_sdk_venc_resume(EC_INT vencChn)
{
    if (vencChn == VENC_RECORDE_CHN)
    {
        SAMPLE_COMM_VENC_BindVpss(vencChn, VPSS_GROUP, VPSS_RECORDE_CHN);
    }
    else
    {
        SAMPLE_COMM_VENC_BindVpss(vencChn, VPSS_GROUP, VPSS_LIVE_CNH);
    }
    HI_MPI_VENC_StartRecvPic(vencChn);

    return EC_SUCCESS;
}

EC_INT ec_hisi_sdk_aenc_pause(EC_INT aiChn, EC_INT aencChn)
{
    SAMPLE_COMM_AUDIO_AencUnbindAi(AI_DEV, aiChn, aencChn);


    return EC_SUCCESS;
}

EC_INT ec_hisi_sdk_aenc_resume(EC_INT aiChn, EC_INT aencChn)
{
    SAMPLE_COMM_AUDIO_AencBindAi(AI_DEV, aiChn, aencChn);

    return EC_SUCCESS;
}

EC_VOID ec_hisi_sdk_start(EC_VOID)
{
    if (ec_stat_set(&sdk_stat) == EC_FAILURE)
    {
        return;
    }
    do_sdk_init();
    start_sdk_vi();
    //now start a timer
    ec_osd_start();
}

EC_VOID ec_hisi_sdk_stop(EC_VOID)
{
    if (ec_stat_unset(&sdk_stat) == EC_FAILURE)
    {
        return;
    }
    //停止OSD
    ec_osd_stop();
    //

    //停止视频编码相关
#if 0
    ec_hisi_sdk_venc_stop(VPSS_RECORDE_CHN, VENC_RECORDE_CHN);
    ec_hisi_sdk_venc_stop(VPSS_LIVE_CNH, VENC_LIVE_CHN);
    ec_hisi_sdk_snap_stop(VPSS_SNAP_CHN, VENC_SNAP_CHN);
#endif
    HI_MPI_VENC_ResetChn(VENC_RECORDE_CHN);
    HI_MPI_VENC_ResetChn(VENC_SNAP_CHN);
    HI_MPI_VENC_ResetChn(VENC_LIVE_CHN);
    //停止VI
    stop_sdk_vi();
    //停止VPSS通道绑定
    SAMPLE_COMM_VPSS_Stop(VPSS_GROUP, 3); //live snap recorde 一共三个通道

    deinit_sdk_osd();
    //注销音频
    deinit_sdk_ai();

    do_sdk_deInit();

    return;
}


static EC_VOID do_sdk_deInit(EC_VOID)
{
    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

}


static EC_INT do_sdk_init(EC_VOID)
{

    init_sdk_base();
    init_sdk_vi();
    init_sdk_vpss();
    init_sdk_osd();
    init_sdk_ai();

    dzlog_debug("init hisi sdk success");
    return EC_SUCCESS;
}


static EC_INT init_sdk_base(EC_VOID)
{
    EC_ERR_PTR;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    PIC_SIZE_E enSize;
    VB_CONF_S vbConf;
    HI_U32 blkSize;
    HI_S32 s32Ret;
    SIZE_S stSize;

    //deinit
    HI_MPI_AENC_DestroyChn(AENC_RECORDE_CHN);
    HI_MPI_AENC_DestroyChn(AENC_LIVE_CHN);
    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

    memset(&vbConf, 0, sizeof(VB_CONF_S));

    vbConf.u32MaxPoolCnt = EC_SDK_MAX_POOL_CNT;
    SAMPLE_COMM_VI_GetSizeBySensor(&enSize);

    blkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gEnNorm, enSize, SAMPLE_PIXEL_FORMAT,
                                               SAMPLE_SYS_ALIGN_WIDTH);
    //dzlog_debug("blksize %d", blkSize);

    vbConf.astCommPool[0].u32BlkSize = blkSize;  //for recoder
    vbConf.astCommPool[0].u32BlkCnt = 10;//15;
#if 1
    vbConf.astCommPool[1].u32BlkSize = blkSize; //for live
    vbConf.astCommPool[1].u32BlkCnt = 10;//15;
    vbConf.astCommPool[2].u32BlkSize = blkSize;
    vbConf.astCommPool[2].u32BlkCnt = 5;//10;  //for snap, we dont need too many cache
#endif
    s32Ret = SAMPLE_COMM_SYS_Init(&vbConf);
    CHECK_SUCCESS(s32Ret, HI_SUCCESS, 0);

    return EC_SUCCESS;

    failed_0:
    return EC_FAILURE;
}

static EC_INT init_sdk_vi(EC_VOID)
{
    return EC_SUCCESS;
}

static EC_INT init_sdk_vpss(EC_VOID)
{
    PIC_SIZE_E enSize;
    SIZE_S stSize;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize);
    CHECK_SUCCESS(SAMPLE_COMM_SYS_GetPicSize(gEnNorm, enSize, &stSize),
                  HI_SUCCESS, 0);
    stVpssGrpAttr.u32MaxW = stSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSize.u32Height;
    stVpssGrpAttr.bIeEn = HI_FALSE;
    stVpssGrpAttr.bNrEn = HI_TRUE;
    stVpssGrpAttr.bHistEn = HI_FALSE;
    stVpssGrpAttr.bDciEn = HI_FALSE;
    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
    stVpssGrpAttr.bSharpenEn = HI_TRUE;
    CHECK_SUCCESS(SAMPLE_COMM_VPSS_StartGroup(VPSS_GROUP, &stVpssGrpAttr),
                  HI_SUCCESS, 1);

    return EC_SUCCESS;
    //should do some deinit , but im lazy
    failed_1:
    failed_0:
    return EC_FAILURE;
}

static EC_INT init_sdk_osd(EC_VOID)
{
    HI_S32 i;
    HI_S32 s32Ret;
    MPP_CHN_S stChn;
    HI_U32 u32layer = 0;
    RGN_ATTR_S stRgnAttrSet;
    RGN_CHN_ATTR_S stChnAttr;
    MEDIA_CONF_PTR;


    stRgnAttrSet.enType = OVERLAYEX_RGN;
    stRgnAttrSet.unAttr.stOverlayEx.enPixelFmt = PIXEL_FORMAT_RGB_8888;
    stRgnAttrSet.unAttr.stOverlayEx.stSize.u32Width = osd_width();
    stRgnAttrSet.unAttr.stOverlayEx.stSize.u32Height = osd_height();
    stRgnAttrSet.unAttr.stOverlayEx.u32BgColor = 0x000003e0;

    s32Ret = HI_MPI_RGN_Create(OSD_CHN, &stRgnAttrSet);
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("create rgn failed %#02x", s32Ret);
        goto failed;
    }


    //recorder part
    stChn.enModId = HI_ID_VPSS;
    stChn.s32DevId = VPSS_GROUP;
    stChn.s32ChnId = VPSS_RECORDE_CHN;
    stChnAttr.bShow = HI_TRUE;
    stChnAttr.enType = OVERLAYEX_RGN;
#if 1
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = osd_x_positon(&(mediaConf->videoConf));
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = osd_y_postion(&(mediaConf->videoConf));
    stChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha = 0;
    stChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha = 128;
    stChnAttr.unChnAttr.stOverlayExChn.u32Layer = u32layer;
#else
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = HI_TRUE;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = MORETHAN_LUM_THRESH;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height= 16;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width= 16;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X= osd_x_positon(&(mediaConf->videoConf));
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y= osd_y_postion(&(mediaConf->videoConf));
    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha= 0;
    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha= 0;
#endif

    s32Ret = HI_MPI_RGN_AttachToChn(OSD_CHN, &stChn, &stChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("attach osd to recoder vpss channel failed %#02x", s32Ret);
        goto failed;
    }

    //for snap
    stChn.s32ChnId = VPSS_SNAP_CHN;
    stChnAttr.bShow = HI_TRUE;
    stChnAttr.enType = OVERLAYEX_RGN;
#if  1
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = osd_x_positon(&(mediaConf->videoConf));
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = osd_y_postion(&(mediaConf->videoConf));
    stChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha = 0;
    stChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha = 128;
    stChnAttr.unChnAttr.stOverlayExChn.u32Layer = u32layer;
#else
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = HI_TRUE;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = MORETHAN_LUM_THRESH;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height= 16;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width= 16;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X= osd_x_positon(&(mediaConf->videoConf));
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y= osd_y_postion(&(mediaConf->videoConf));
    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha= 0;
    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha= 0;
#endif
    s32Ret = HI_MPI_RGN_AttachToChn(OSD_CHN, &stChn, &stChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("attach osd to snap vpss channel failed %#02x", s32Ret);
        goto failed;
    }


    //for live
    stChn.s32ChnId = VPSS_LIVE_CNH;
    stChnAttr.bShow = HI_TRUE;
    stChnAttr.enType = OVERLAYEX_RGN;
#if 1
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = osd_x_positon(&(mediaConf->liveConf));
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = osd_y_postion(&(mediaConf->liveConf));
    stChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha = 0;
    stChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha = 128;
    stChnAttr.unChnAttr.stOverlayExChn.u32Layer = u32layer;
#else
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = HI_TRUE;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = MORETHAN_LUM_THRESH;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height= 16;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width= 16;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X= osd_x_positon(&(mediaConf->liveConf));
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y= osd_y_postion(&(mediaConf->liveConf));
    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha= 0;
    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha= 0;
#endif
    s32Ret = HI_MPI_RGN_AttachToChn(OSD_CHN, &stChn, &stChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("attach osd to live vpss channel  failed %#02x", s32Ret);
        goto failed;
    }

    //now we get the cavasinfo
    EC_MEMSET_2(&(canvasInfo), sizeof(canvasInfo));
    s32Ret = HI_MPI_RGN_GetCanvasInfo(OSD_CHN, &(canvasInfo));
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("HI_MPI_RGN_GetCanvasInfo 1 failed %#02x", s32Ret);
        goto failed;
    }

    return EC_SUCCESS;

    failed:
    return EC_FAILURE;
}

static EC_INT deinit_sdk_osd(EC_VOID)
{
    HI_MPI_RGN_Destroy(OSD_CHN);
}

static EC_INT start_sdk_vi(EC_VOID)
{
    SAMPLE_VI_CONFIG_S stViConfig;
    HI_S32 s32Ret;

    //init vi first
    EC_MEMSET(&stViConfig);
    stViConfig.enViMode = SENSOR_TYPE;
    stViConfig.enRotate = ROTATE_NONE;
    stViConfig.enNorm = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    stViConfig.enWDRMode = WDR_MODE_NONE;
    stViConfig.enFrmRate = SAMPLE_FRAMERATE_DEFAULT;

    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    CHECK_SUCCESS(s32Ret, EC_SUCCESS, 0);

    //now bind to vpss
    CHECK_SUCCESS(SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode),
                  HI_SUCCESS, 1);


    return EC_SUCCESS;
    failed_1:
    failed_0:
    return EC_FAILURE;
}

static EC_INT stop_sdk_vi(EC_VOID)
{
    SAMPLE_VI_CONFIG_S stViConfig;

    EC_MEMSET(&stViConfig);
    stViConfig.enViMode = SENSOR_TYPE;
    stViConfig.enRotate = ROTATE_NONE;
    stViConfig.enNorm = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    stViConfig.enWDRMode = WDR_MODE_NONE;
    stViConfig.enFrmRate = SAMPLE_FRAMERATE_DEFAULT;

    //unbind first
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
    //stop now
    SAMPLE_COMM_VI_StopVi(&stViConfig);

    return EC_SUCCESS;
}

static EC_INT init_sdk_ai(EC_VOID)
{
    AIO_ATTR_S stAioAttr;
    EC_ERR_PTR;
    HI_S32 s32Ret;
    HI_VOID *pAiVqeAttr = NULL;
    MEDIA_CONF_PTR;

    stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_44100;
    // stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
    stAioAttr.u32EXFlag = 0;
    stAioAttr.u32FrmNum = 2;
    stAioAttr.u32PtNumPerFrm = 1024;
    stAioAttr.u32ChnCnt = 1;   //只有左声道
    stAioAttr.u32ClkSel = 0;



    // s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr, mediaConf->audioConf.voice);
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr, mediaConf->audioConf.voice);
    CHECK_SUCCESS(s32Ret, HI_SUCCESS, 0);


#if 0
    AI_TALKVQE_CONFIG_S stAiVqeTalkAttr;
    memset(&stAiVqeTalkAttr, 0, sizeof(AI_TALKVQE_CONFIG_S));
    stAiVqeTalkAttr.s32WorkSampleRate    = AUDIO_SAMPLE_RATE_16000;
    stAiVqeTalkAttr.s32FrameSample       = 1024;
    stAiVqeTalkAttr.enWorkstate          = VQE_WORKSTATE_NOISY;
    //aec part回声抵消，不需要
    stAiVqeTalkAttr.stAecCfg.bUsrMode    = HI_FALSE;
    stAiVqeTalkAttr.stAecCfg.s8CngMode   = 0;
    //自动增益部分
    stAiVqeTalkAttr.stAgcCfg.bUsrMode    = HI_TRUE;
    stAiVqeTalkAttr.stAgcCfg.s8TargetLevel = -2;
    stAiVqeTalkAttr.stAgcCfg.s8NoiseFloor = -40;
    stAiVqeTalkAttr.stAgcCfg.s8MaxGain = 15;
    stAiVqeTalkAttr.stAgcCfg.s8AdjustSpeed = 10;
    stAiVqeTalkAttr.stAgcCfg.s8ImproveSNR = 2;
    stAiVqeTalkAttr.stAgcCfg.s8UseHighPassFilt = 0;
    stAiVqeTalkAttr.stAgcCfg.s8OutputMode = 0;
    stAiVqeTalkAttr.stAgcCfg.s16NoiseSupSwitch = 1;
    //语音降噪 采样率不支持
    stAiVqeTalkAttr.stAnrCfg.bUsrMode    = HI_FALSE;
    //高通滤波
    stAiVqeTalkAttr.stHpfCfg.bUsrMode    = HI_TRUE;
    stAiVqeTalkAttr.stHpfCfg.enHpfFreq   = AUDIO_HPF_FREQ_150;
    stAiVqeTalkAttr.stHdrCfg.bUsrMode    = HI_FALSE;

    stAiVqeTalkAttr.u32OpenMask = AI_TALKVQE_MASK_AEC | AI_TALKVQE_MASK_AGC | AI_TALKVQE_MASK_ANR | AI_TALKVQE_MASK_HPF;

    pAiVqeAttr = (HI_VOID *)&stAiVqeTalkAttr;
#endif


    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AI_DEV, stAioAttr.u32ChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, HI_FALSE,
                                       pAiVqeAttr, HI_TRUE);
    CHECK_SUCCESS(s32Ret, HI_SUCCESS, 1);
    //  AI_CHN_PARAM_S chnParam = { 10 };
    // s32Ret =  HI_MPI_AI_SetChnParam(0, 0, &chnParam);


    dzlog_debug("start ai success");
//init aac module
    s32Ret = HI_MPI_AENC_AacInit();
    if (HI_SUCCESS != s32Ret)
    {
        dzlog_error("aenc init failed %#02x", s32Ret);
        goto failed_0;
    }
    return EC_SUCCESS;

    failed_1:
    failed_0:
    dzlog_error("init hisi sdk ai failed");
    return EC_FAILURE;

}

static EC_INT deinit_sdk_ai(EC_VOID)
{
    ec_hisi_sdk_aenc_stop(AI_RECORDE_CHN, AENC_RECORDE_CHN);
    ec_hisi_sdk_aenc_stop(AI_LIVE_CHN, AENC_LIVE_CHN);
    SAMPLE_COMM_AUDIO_StopAi(AI_DEV, 2, HI_FALSE, HI_FALSE);
    return EC_SUCCESS;
}
