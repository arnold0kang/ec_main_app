#ifndef EC_CONF_H
#define EC_CONF_H

#include "../common/ec_define.h"


#define EC_SERIAL_INDEX                    0
#define EC_DISK_PATH                     "/udisk"
#define EC_VIDEO_DIR_PATH                    "/udisk"
#define EC_DEFAULT_RECODE_PATH            "/udisk"
#define EC_SNAP_DIR_PATH                    "/udisk/snap"
#define  LOG_DIR                            "/udisk/log/"
#define POWER_BTN_KEEP_TIME                4
#define WIFI_BTN_KEEP_TIME                 4

#define PWM_HZ                              (3000000)



typedef enum
{
    EN_TYPE_H264,
    EN_TYPE_H265,
    EC_TYPE_JPEG
} ec_pic_en_type;

typedef enum
{
    H264_BASE_PROFILE,
    H264_MAIN_PROFILE,
    H264_HIGH_PROFILE,
} ec_h264_profile_type;

typedef enum
{
    EN_SIZE_4K,
    EN_SIZE_1080,
    EN_SIZE_720,
    EN_SIZE_480,
    EN_SIZE_INVALID
} ec_pic_en_size;

typedef enum
{
    RECORD_MODE_RATOAE = 0,
    RECORD_MODE_FULL_STOP
} ec_record_mode;

typedef struct
{
    ec_pic_en_type entype;
    ec_pic_en_size ensize;
    ec_h264_profile_type h264ProfileType;
    EC_USHORT framerate;
    EC_USHORT bitrate;
    EC_INT gop;
    EC_INT rctype;
} ec_video_conf_t;

typedef struct
{
    EC_INT voice;
}ec_audio_conf_t;



typedef enum
{
    EC_OSD_POSITON_LEFT_UP,
    EC_OSD_POSTION_LEFT_DOWN,
    EC_OSD_POSTION_RIGHT_UP,
    EC_OSD_POSTION_RIGHT_DOWN
} ec_osd_postion_type;
typedef struct
{
    //internal part
    EC_INT fontSize;
    struct
    {
        EC_UCHAR redValue;
        EC_UCHAR greenValue;
        EC_UCHAR blackValue;
        EC_UCHAR alphaValue;
    } color;
    ec_osd_postion_type osd_postion_type;
} ec_osd_conf_t;


typedef struct
{
    ec_record_mode mode;
    EC_UINT interval;
} ec_recoder_conf_t;

typedef struct
{
    EC_CHAR *devid;
    EC_CHAR *userid;
} ec_dev_conf_t;



typedef struct
{
    EC_CHAR *url;
} ec_alarm_conf_t;

typedef struct
{
    EC_CHAR *url;
} ec_streamsrv_conf_t;

typedef struct
{
    ec_video_conf_t videoConf;
    ec_video_conf_t liveConf;
    ec_video_conf_t snapConf;
    ec_audio_conf_t audioConf;
} ec_media_conf_t;


typedef enum
{
    EC_RECODER_MODE_ROTATE,
    EC_RECODER_MODE_FIX
} ec_recoder_mode;

typedef enum
{
    EC_WIFI_ENC_NONE,
    EC_WIFI_ENC_WPA2
} ec_wifi_enc_mode;


typedef struct
{
    EC_CHAR *ssid;
    EC_CHAR *password;
    ec_wifi_enc_mode encMode;
} ec_wifi_conf_t;


typedef struct
{
    ec_media_conf_t mediaConf;
    ec_osd_conf_t osdConf;
    ec_recoder_conf_t recordConf;
    ec_dev_conf_t devConf;
    ec_alarm_conf_t alarmConf;
    ec_streamsrv_conf_t streamSrvConf;
    ec_wifi_conf_t wifiConf;
} ec_conf_t;


typedef enum
{
    EC_CONF_MODULE_MEDIA,
    EC_CONF_MODULE_OSD,
    EC_CONF_MODULE_RECODER,
    EC_CONF_MODULE_HB
} ec_conf_module_type;


ec_conf_t *ec_conf_get(EC_VOID);

EC_VOID ec_conf_dump(EC_VOID);


EC_INT ec_conf_set_devid(EC_CHAR *devid);

EC_INT ec_conf_set_usrid(EC_CHAR *usrid);


static inline EC_INT get_video_width(ec_pic_en_size enSize)
{
    switch (enSize)
    {
        case EN_SIZE_1080:
            return 1920;
        case EN_SIZE_720:
            return 1080;
        default:
            return 1920;
    }
}

static inline EC_INT get_video_height(ec_pic_en_size enSize)
{
    switch (enSize)
    {
        case EN_SIZE_1080:
            return 1080;
        case EN_SIZE_720:
            return 720;
        default:
            return 1080;
    }
}

#define SERVER_CONF_PTR \
    ec_server_conf_t *serverConf = &(ec_conf_get()->serverConf)
#define DEV_CONF_PTR  \
    ec_dev_conf_t *devConf = &(ec_conf_get()->devConf)
#define ALARM_CONF_PTR    \
    ec_alarm_conf_t *alarmConf =  &(ec_conf_get()->alarmConf)

#define MEDIA_CONF_PTR \
    ec_media_conf_t *mediaConf = &(ec_conf_get()->mediaConf)

#define STREAMSRV_CONF_PTR \
    ec_streamsrv_conf_t *streamSrvConf =  &(ec_conf_get()->streamSrvConf)

#define RECODER_CONF_PTR \
    ec_recoder_conf_t *recoderConf = &(ec_conf_get()->recordConf)

#define VIDEO_CONF_PTR \
    ec_video_conf_t *videoConf = &(ec_conf_get()->mediaConf.videoConf)

#define OSD_CONF_PTR \
    ec_osd_conf_t *osdConf = &(ec_conf_get()->osdConf)

#define WIFI_CONF_PTR \
    ec_wifi_conf_t *wifiConf = &(ec_conf_get ()->wifiConf)

#define HB_CONF_PTR \
    ec_hb_conf_t *hbConf = &(ec_conf_get()->hbConf);


#endif