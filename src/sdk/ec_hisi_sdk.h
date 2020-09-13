#ifndef EC_HISI_SDK_H
#define EC_HISI_SDK_H

#include "../common/ec_define.h"
#include "../conf/ec_conf.h"
//#include "zlog.h"


typedef struct ec_video_frame_s
{
    EC_UINT pts;
    EC_CHAR *sps;
    EC_UINT spsLen;
    EC_CHAR *pps;
    EC_UINT ppsLen;
    EC_CHAR *sei;
    EC_UINT seiLen;
    EC_UCHAR *frame;
    EC_UINT frameLen;
    EC_BOOL keyFlag;
} ec_h264_frame;

#define FREE_H264(FRAME_PTR) do{EC_FREE(FRAME_PTR->sps); \
    EC_FREE(FRAME_PTR->pps); \
    EC_FREE(FRAME_PTR->sei); \
    EC_FREE(FRAME_PTR->frame);\
    EC_FREE(FRAME_PTR);}while(0)

typedef struct ec_audio_frame_s
{
    EC_UINT pts;
    EC_UCHAR *data;
    EC_UINT dataLen;
} ec_aac_frame;

#define FREE_AAC(FRAME_PTR) do{EC_FREE(FRAME_PTR->data); \
    EC_FREE(FRAME_PTR);}while(0)


EC_INT ec_hisi_sdk_venc_start(EC_INT vpssChn, EC_INT vencChn, ec_video_conf_t *conf);

EC_INT ec_hisi_sdk_venc_stop(EC_INT vpssChn, EC_INT vencChn);

EC_INT ec_hisi_sdk_venc_get_fd(EC_INT vencChn);

EC_INT ec_hisi_sdk_venc_free_fd(EC_INT vencChn, EC_INT fd);

EC_INT ec_hisi_sdk_snap_start(EC_INT vpssChn, EC_INT vencChn, ec_video_conf_t *conf);

EC_INT ec_hisi_sdk_snap_stop(EC_INT vpssChn, EC_INT vencChn);

EC_INT ec_hisi_sdk_snap_get_fd(EC_INT vencChn);

EC_INT ec_hisi_sdk_snap_free_fd(EC_INT vencChn, EC_INT fd);


EC_INT ec_hisi_sdk_aenc_start(EC_INT aiChn, EC_INT aencChn);

EC_INT ec_hisi_sdk_aenc_stop(EC_INT aiChn, EC_INT aencChn);

EC_INT ec_hisi_sdk_aenc_get_fd(EC_INT aencChn);

EC_INT ec_hisi_sdk_aenc_free_fd(EC_INT aencChn, EC_INT fd);

EC_INT ec_hisi_sdk_venc_pause(EC_INT vencChn);

EC_INT ec_hisi_sdk_venc_resume(EC_INT vencChn);

EC_INT ec_hisi_sdk_aenc_pause(EC_INT aiChn, EC_INT aencChn);

EC_INT ec_hisi_sdk_aenc_resume(EC_INT aiChn, EC_INT aencChn);

EC_VOID ec_hisi_sdk_start(EC_VOID);

EC_VOID ec_hisi_sdk_stop(EC_VOID);

ec_h264_frame *ec_hisi_sdk_get_h264(EC_INT vencChn);

ec_aac_frame *ec_hisi_sdk_get_aac(EC_INT aenChn);


//直播通道专属API

EC_INT ec_hisi_sdk_aenc_start_2 (EC_VOID);
EC_VOID ec_hisi_sdk_aenc_stop_2(EC_VOID);
EC_INT ec_hisi_sdk_aenc_pause_2(EC_VOID);
EC_INT ec_hisi_sdk_aenc_resume_2(EC_VOID);
ec_aac_frame *ec_hisi_sdk_get_aac_2(EC_VOID);
EC_VOID ec_hisi_sdk_free_aac_2(ec_aac_frame *frame);


#endif
