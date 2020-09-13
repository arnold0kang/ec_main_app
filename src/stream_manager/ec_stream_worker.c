#include <pthread.h>
#include "srs_librtmp.h"
#include "ec_stream_worker.h"
#include "../common/ec_define.h"
#include "../event/ec_event.h"
#include "../sdk/ec_hisi_sdk_live.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../module_manager/ec_module.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../utils/ec_utils.h"
#include "../wifi/ec_wifi.h"
#include "../conf/ec_conf.h"


static EC_VOID stream_worker(EC_VOID *arg);

static srs_rtmp_t *connect_server(EC_VOID);

static EC_VOID close_server(srs_rtmp_t *handle);

static EC_INT send_video_frame(srs_rtmp_t *handle);

static EC_INT send_audio_frame(srs_rtmp_t *handle);

static EC_INT stream_failed(EC_INT ret);


static EC_BOOL is_run = EC_FALSE;
static EC_BOOL is_need_run = EC_FALSE;


EC_VOID ec_stream_worker_run(EC_VOID)
{
    if (ec_stat_set(&is_run) == EC_FAILURE)
    {
        return;
    }
    is_need_run = EC_TRUE;
    uv_thread_t tid;
    CHECK_SUCCESS(uv_thread_create(&tid, stream_worker, NULL), 0, 0);


    return;
    failed_0:
    is_need_run = EC_FALSE;
    ec_stat_unset(&is_run);
}

EC_VOID ec_stream_worker_stop(EC_VOID)
{
    is_need_run = EC_FALSE;
}


EC_BOOL ec_stream_worker_is_run(EC_VOID)
{
    return is_run;
}


static EC_VOID stream_worker(EC_VOID *arg)
{

    ec_wifi_on();
    while (1)
    {
        if (ec_wifi_is_on())
        {
            break;
        }
        if (!is_need_run)
        {
            goto end;
        }
        cmh_wait_sec(1);
    }

    EC_INT videoFd, audioFd;
    if (ec_hisi_sdk_live_start(&videoFd, &audioFd) == EC_FAILURE)
    {
        goto end;
    }
    EC_INT maxfd;
    maxfd = videoFd > audioFd ? videoFd : audioFd;

    maxfd++;

    while (1)
    {
        srs_rtmp_t *rtmp_handle = connect_server();
        if (!rtmp_handle)
        {
            if (is_need_run)
            {
                continue;
            }
            else
            {
                break;
            }
        }


        BROAD_CAST(EC_EVENT_DATA_IN_TRANS);


        fd_set readfds;
        EC_INT ret;

        while (1)
        {
            FD_ZERO(&readfds);
            FD_SET(videoFd, &readfds);
            FD_SET(audioFd, &readfds);


            ret = select(maxfd, &readfds, NULL, NULL, NULL);
            if (ret < 0)
            {
                continue;
            }
            if (FD_ISSET(videoFd, &readfds))
            {
                if (send_video_frame(rtmp_handle) == EC_FAILURE)
                {
                    dzlog_error("send video frame failed");
                    break;
                }

            }
            if (FD_ISSET(audioFd, &readfds))
            {


                if (send_audio_frame(rtmp_handle) == EC_FAILURE)
                {
                    dzlog_debug("send audio frame failed");
                    break;
                }
                //  ec_timer_update(audioFd, 0, 10000);
            }

            if (!is_need_run)
            {
                break;
            }
        }
        BROAD_CAST(EC_EVENT_DATA_OUT_TRANS);

        close_server(rtmp_handle);

        if (!is_need_run)
        {
            break;
        }
    }
    ec_hisi_sdk_live_stop(videoFd, audioFd);
    //  is_run = EC_FALSE;
    ec_wifi_off();
    ec_stat_unset(&is_run);


    return;


    end:
    //  is_run = EC_FALSE;
    ec_stat_unset(&is_run);

    ec_wifi_off();
    return;
}


static srs_rtmp_t *connect_server(EC_VOID)
{
    STREAMSRV_CONF_PTR;
    DEV_CONF_PTR;
    EC_CHAR rtmpUrl[BUFSIZ] = {'\0'};

    snprintf(rtmpUrl, BUFSIZ - 1, "%s/%s", streamSrvConf->url,
             devConf->devid);
    dzlog_debug("stream url %s", rtmpUrl);
    dzlog_debug("trying to connect to rtmp server");
    while (1)
    {
        srs_rtmp_t *rtmphandle = srs_rtmp_create(rtmpUrl);
        if (rtmphandle == EC_NULL)
        {
            return NULL;
        }
   //     srs_rtmp_set_timeout(rtmphandle, 6000, 6000);

        if (srs_rtmp_handshake(rtmphandle) == 0)
        { //handshake success
            if (srs_rtmp_connect_app(rtmphandle) == 0) //set app success
            {
                if (srs_rtmp_publish_stream(rtmphandle) == 0) //set publish success
                {
                    dzlog_debug("success");
                    return rtmphandle;
                }
            }
        }
        srs_rtmp_destroy(rtmphandle);
        if (!is_need_run)
        {
            return EC_NULL;
        }
        cmh_wait_sec(1);
    }

    return NULL;
}

static EC_VOID close_server(srs_rtmp_t *handle)
{
    srs_rtmp_destroy(handle);

    return;
}

#if 1

static EC_INT do_send_video(srs_rtmp_t *handle, EC_VOID *frame, EC_INT frameLen, EC_INT pts)
{
    EC_INT ret;


    ret = srs_h264_write_raw_frames(handle, frame, frameLen, pts, pts);

    return stream_failed(ret);
}

static EC_INT do_send_audio(srs_rtmp_t *handle, EC_VOID *frame, EC_INT frameLen, EC_INT pts)
{
#if 1
    EC_INT ret;


    ret = srs_audio_write_raw_frame(handle, 10, 3, 1, 0, frame, frameLen, pts);


    return stream_failed(ret);
#else
    return EC_SUCCESS;


#endif
}

#else
static EC_INT do_send_video (srs_rtmp_t *handle, EC_VOID *frame, EC_INT frameLen, EC_INT pts)
{
    return EC_SUCCESS;
}

static EC_INT do_send_audio (srs_rtmp_t  *handle, EC_VOID *frame, EC_INT frameLen, EC_INT pts)
{
    return EC_SUCCESS;
}
#endif


static EC_INT send_video_frame(srs_rtmp_t *handle)
{
    ec_h264_frame *frame = ec_hisi_sdk_live_h264_get();
    if (frame == EC_NULL) //no frame?
    {
        return EC_SUCCESS; //skip this
    }


    if (frame->keyFlag)
    {
        CHECK_FAILED(do_send_video(handle, frame->sps, frame->spsLen, frame->pts), 0);
        CHECK_FAILED(do_send_video(handle, frame->pps, frame->ppsLen, frame->pts), 0);
        CHECK_FAILED(do_send_video(handle, frame->sei, frame->seiLen, frame->pts), 0);
    }
    // dzlog_debug("send frame size %d %d", frame->frameLen, frame->pts - last_video_pts);

    CHECK_FAILED(do_send_video(handle, frame->frame, frame->frameLen, frame->pts), 0);


    FREE_H264(frame);


    return EC_SUCCESS;

    failed_0:

    FREE_H264(frame);
    return EC_FAILURE;
}


#if 0
static EC_INT do_send_audio (srs_rtmp_t  *handle, EC_VOID *frame, EC_INT frameLen, EC_INT pts)
{
#if 1
    EC_INT ret;


    ret = srs_audio_write_raw_frame(handle, 10, 3, 1, 0, frame, frameLen, pts);


    return stream_failed(ret);
#else
    return EC_SUCCESS;


#endif
}
#endif


static EC_INT send_audio_frame(srs_rtmp_t *handle)
{
    ec_aac_frame *frame;
    frame = ec_hisi_sdk_live_aac_get();
    if (!frame)
    {
        dzlog_debug("audio frame is null");
        return EC_SUCCESS;
    }


    CHECK_FAILED(do_send_audio(handle, frame->data, frame->dataLen, frame->pts), 0);

    FREE_AAC(frame);
    return EC_SUCCESS;

    failed_0:
    FREE_AAC(frame);

    return EC_FAILURE;
}


static EC_INT stream_failed(EC_INT ret)
{
    if (ret != 0)
    {
        if (srs_h264_is_dvbsp_error(ret))
        {
            // srs_human_trace("ignore drop video error, code=%d", ret);
        }
        else if (srs_h264_is_duplicated_sps_error(ret))
        {
            //   srs_human_trace("ignore duplicated sps, code=%d", ret);
        }
        else if (srs_h264_is_duplicated_pps_error(ret))
        {
            //  srs_human_trace("ignore duplicated pps, code=%d", ret);
        }
        else
        {
            srs_human_trace("send h264 raw data failed. ret=%d", ret);
            return EC_FAILURE;
        }
    }
    return EC_SUCCESS;
}