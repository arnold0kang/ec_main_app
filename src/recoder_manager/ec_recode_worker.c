//
// Created by arnold on 3/23/2018.
//
#include <unistd.h>
#include <sys/select.h>
#include "mp4v2/mp4v2.h"
#include "ec_recode_worker.h"
#include "../dev_stat/ec_dev_state_define.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../sdk/ec_hisi_sdk_recorder.h"
#include "../conf/ec_conf.h"
#include "../rtc/ec_rtc.h"
#include "../event/ec_event.h"
#include "../utils/ec_utils.h"
#include "../module_manager/ec_module.h"
#include "ec_record_mp4.h"

static EC_INT writeVideoFrame(ec_mp4_info *mp4_info, ec_h264_frame *newFrame);

static EC_INT writeAacFrame(ec_mp4_info *mp4_info, ec_aac_frame *newFrame);




static EC_INT is_need_run = EC_TRUE;
static EC_BOOL is_run = EC_FALSE;


static EC_VOID record_work(EC_VOID *arg);


EC_VOID ec_record_work_run(EC_VOID)
{
    if (ec_stat_set(&is_run) == EC_FAILURE)
    {
        return;
    }
    is_need_run = EC_TRUE;
    uv_thread_t tid = 0;
    uv_thread_create(&tid, record_work, NULL);
}

EC_VOID ec_record_work_stop(EC_VOID)
{
    is_need_run = EC_FALSE;
    while (is_run)
    {
        cmh_wait_usec(1000);
    }

    return;
}

EC_BOOL ec_record_work_is_run (EC_VOID)
{
    return is_run;
}


static EC_VOID record_work(EC_VOID *arg)
{
    RECODER_CONF_PTR;
    EC_INT videoFd, audioFd;
    ec_mp4_info mp4_info;
    //添加一个U盘引用
    dzlog_debug("recorde worker up");
    ec_disk_ref_add();
    fd_set readfds;

    CHECK_FAILED(ec_hisi_sdk_recorde_start(&videoFd, &audioFd), 0);
    CHECK_FAILED(open_mp4_handle(&mp4_info), 1);

    //create a timer for splice
    EC_INT splice_timer_fd = ec_timer_create(recoderConf->interval, 0);
    if (splice_timer_fd == EC_FAILURE)
    {
        goto failed_2;
    }


    EC_INT maxfd = videoFd > audioFd ? videoFd : audioFd;
    maxfd = maxfd > splice_timer_fd ? maxfd : splice_timer_fd;

    maxfd++;
    EC_INT ret;

    EC_INT video_frame_cnt = 0;
    EC_INT audio_frame_cnt = 0;

    BROAD_CAST(EC_EVENT_RECODER_START);

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(videoFd, &readfds);
        FD_SET(audioFd, &readfds);
        FD_SET(splice_timer_fd, &readfds);

        ret = select(maxfd, &readfds, NULL, NULL, NULL);
        if (ret == -1)
        {
            continue;
        }
        if (!is_need_run)
        {
            //写入最后一帧
            writeVideoFrame(&mp4_info, NULL);
            writeAacFrame(&mp4_info, NULL);
            BROAD_CAST(EC_EVENT_WAIT_POWER_OFF);
            //关闭文件
            close_mp4_handle(&mp4_info);
            break;
        }
        if (FD_ISSET(videoFd, &readfds))
        {
            ec_h264_frame *frame;
            frame = ec_hisi_sdk_recorde_h264_get();
            if (!frame)
            {
                continue;
            }
            if (writeVideoFrame(&mp4_info, frame) == EC_FAILURE)
            {
                is_need_run = 0;
            }
            video_frame_cnt++;
        }

        if (FD_ISSET(audioFd, &readfds))
        {
            ec_aac_frame *frame;

            frame = ec_hisi_sdk_recorde_aac_get();
            if (!frame)
            {
                continue;
            }
            if (writeAacFrame(&mp4_info, frame) == EC_FAILURE)
            {
                is_need_run = 0;
            }
            audio_frame_cnt++;
        }

        if (FD_ISSET(splice_timer_fd, &readfds))
        {
           // ec_hisi_sdk_recorde_pause();
            //写入最后一帧
            writeVideoFrame(&mp4_info, NULL);
            writeAacFrame(&mp4_info, NULL);
            //关闭文件
            close_mp4_handle_bg(mp4_info.mp4handle);

            //打开新的文件
            if (open_mp4_handle(&mp4_info) == EC_FAILURE)
            {
                dzlog_error("open mp4 handle failed");
                break;
            }
           // ec_hisi_sdk_recorde_resume();

            //update timer
            ec_timer_update(splice_timer_fd, recoderConf->interval, 0);
        }


    }

    ec_hisi_sdk_recorde_stop(videoFd, audioFd);
    ec_timer_close(splice_timer_fd);



    BROAD_CAST(EC_EVENT_RECODER_STOP);


    while(mp4_have_bg())
    {
        dzlog_error("recorde  module have back ground thread run, waiting");
        cmh_wait_sec(1);
    }

    ec_stat_unset(&is_run);
    dzlog_debug("recorde worker down");
    ec_disk_ref_del();


    return;

    failed_3:
    ec_timer_close(splice_timer_fd);
    failed_2:
    close_mp4_handle(&mp4_info);
    failed_1:
    ec_hisi_sdk_recorde_stop(videoFd, audioFd);
    failed_0:
    dzlog_error("record worker error");
    ec_disk_ref_del();
    ec_stat_unset(&is_run);
    BROAD_CAST(EC_EVENT_RECODER_STOP);
    return;

}


static ec_h264_frame *lastVideoFrame = EC_NULL;
static ec_aac_frame *lastAudioFrame = EC_NULL;

#if 0
static EC_INT writeVideoFrame(ec_mp4_info *mp4_info, ec_h264_frame *newFrame)
{
    if (lastVideoFrame == NULL)
    {
        lastVideoFrame = newFrame;
        return EC_SUCCESS;
    }
    else
    {
        FREE_H264(lastVideoFrame);
        lastVideoFrame = newFrame;
    }

   return EC_SUCCESS;
}
static EC_INT writeAacFrame(ec_mp4_info *mp4_info, ec_aac_frame *newFrame)
{
    if (lastAudioFrame == EC_NULL)
    {

        lastAudioFrame = newFrame;
        return EC_SUCCESS;
    }
    FREE_AAC(lastAudioFrame);
    lastAudioFrame = newFrame;

}
#else
static EC_INT writeVideoFrame(ec_mp4_info *mp4_info, ec_h264_frame *newFrame)
{
    MP4Duration duration = MP4_INVALID_DURATION;
    EC_INT vaccSize;
    EC_INT ret;

    if (lastVideoFrame == NULL) //cache first frame
    {
        lastVideoFrame = newFrame;

        return EC_SUCCESS;
    }
    if (newFrame != NULL) //last frame
    {
        duration = (newFrame->pts - lastVideoFrame->pts) * 90;
    }

  //  dzlog_debug("write video last %d  ", lastVideoFrame->pts);
    if (lastVideoFrame->keyFlag)
    {
        //set sps
        MP4AddH264SequenceParameterSet(mp4_info->mp4handle, mp4_info->videoTrack,
                                       lastVideoFrame->sps + 4, lastVideoFrame->spsLen - 4);
        //set pps
        MP4AddH264PictureParameterSet(mp4_info->mp4handle, mp4_info->videoTrack,
                                      lastVideoFrame->pps + 4, lastVideoFrame->ppsLen - 4);

#if 0
        //set sei
        MP4AddH264PictureParameterSet(recode_stat.mp4handle, recode_stat.videoTrack, frame.sei + 4, frame.seiLen - 4);
#endif
        //now set I frame
        vaccSize = lastVideoFrame->frameLen - 4;
        lastVideoFrame->frame[0] = (vaccSize & 0xff000000) >> 24;
        lastVideoFrame->frame[1] = (vaccSize & 0x00ff0000) >> 16;
        lastVideoFrame->frame[2] = (vaccSize & 0x0000ff00) >> 8;
        lastVideoFrame->frame[3] = vaccSize & 0x000000ff;
        ret = MP4WriteSample(mp4_info->mp4handle, mp4_info->videoTrack, lastVideoFrame->frame,
                             lastVideoFrame->frameLen,
                             duration, 0, 1);

        CHECK_SUCCESS(ret, 1, 0);  //return TRUE(1) for success, FALSE(0) for failed
    }
    else
    {
        vaccSize = lastVideoFrame->frameLen - 4;
        lastVideoFrame->frame[0] = (vaccSize & 0xff000000) >> 24;
        lastVideoFrame->frame[1] = (vaccSize & 0x00ff0000) >> 16;
        lastVideoFrame->frame[2] = (vaccSize & 0x0000ff00) >> 8;
        lastVideoFrame->frame[3] = vaccSize & 0x000000ff;
        ret = MP4WriteSample(mp4_info->mp4handle, mp4_info->videoTrack, lastVideoFrame->frame,
                             lastVideoFrame->frameLen,
                             duration, 0, 0);
        CHECK_SUCCESS(ret, 1, 0);  //return TRUE(1) for success, FALSE(0) for failed
    }

    FREE_H264(lastVideoFrame);
    lastVideoFrame = newFrame;

    return EC_SUCCESS;

    failed_0:
    dzlog_error("write video frame failed");
    //free now
    FREE_H264(newFrame);
    FREE_H264(lastVideoFrame);
    return EC_FAILURE;
}


static EC_UINT audioTickCorrect = 0; //use for correct audio offset
static EC_INT writeAacFrame(ec_mp4_info *mp4_info, ec_aac_frame *newFrame)
{
    MP4Duration duration = MP4_INVALID_DURATION;
    EC_ULLONG tmp_durration = 0;

    if (lastAudioFrame == EC_NULL)
    {
        audioTickCorrect = 0;
        lastAudioFrame = newFrame;
        return EC_SUCCESS;
    }

    if (newFrame)
    {
#if 1
        tmp_durration = (newFrame->pts - lastAudioFrame->pts) * 2205;
        audioTickCorrect += tmp_durration % 100;
        duration = tmp_durration / 100 + audioTickCorrect / 100;
        audioTickCorrect = audioTickCorrect % 100;
#else
        duration = (newFrame->pts - lastAudioFrame->pts) * 22.05;
#endif
    }
  //  dzlog_debug("write audio last %d  ", lastAudioFrame->pts);

    EC_INT ret = MP4WriteSample(mp4_info->mp4handle, mp4_info->audioTrack,
                                (const uint8_t *) (lastAudioFrame->data) + 7, lastAudioFrame->dataLen - 7, duration, 0,
                                1);

    CHECK_SUCCESS(ret, 1, 0);  //return TRUE(1) for success, FALSE(0) for failed

    FREE_AAC(lastAudioFrame);
    lastAudioFrame = newFrame;

    return EC_SUCCESS;

    failed_0:
    dzlog_error("wrte audio frame failed");
    FREE_AAC(newFrame);
    FREE_AAC(lastAudioFrame);
    return EC_FAILURE;
}
#endif


