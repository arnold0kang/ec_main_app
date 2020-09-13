//
// Created by arnold on 4/19/2018.
//

#ifndef EC_MAIN_APP_EC_HISI_SDK_CONF_H
#define EC_MAIN_APP_EC_HISI_SDK_CONF_H

//sdk resource config
#define VI_DEV                  0
#define VI_CHN                  0
#define VPSS_GROUP              0
#define VPSS_RECORDE_CHN        0
#define VPSS_LIVE_CNH           1
#define VPSS_SNAP_CHN           2
#define VENC_GRP                0
#define VENC_RECORDE_CHN        1
#define VENC_LIVE_CHN           0
#define VENC_SNAP_CHN           2
#define AI_DEV                  0
#define AI_RECORDE_CHN          0
#define AENC_RECORDE_CHN        0
#define AI_LIVE_CHN             0
#define AENC_LIVE_CHN           1
#define OSD_CHN                 0

#define OSD_WIDTH           400
#define OSD_HEIGHT          100 //this value should be fit wiht bigmap height, be caution
#define OSD_OFF_SET         20

static inline EC_UINT osd_width (EC_VOID)
{
    return OSD_WIDTH;
}

static inline EC_UINT osd_height (EC_VOID)
{
    return OSD_HEIGHT;
}

static inline EC_INT osd_x_positon (ec_video_conf_t *conf)
{
    MEDIA_CONF_PTR;
    OSD_CONF_PTR;
    switch (osdConf->osd_postion_type)
    {
        case EC_OSD_POSITON_LEFT_UP:
        case EC_OSD_POSTION_LEFT_DOWN:
            return OSD_OFF_SET;
        case EC_OSD_POSTION_RIGHT_UP:
        case EC_OSD_POSTION_RIGHT_DOWN:
            return get_video_width(conf->ensize) - OSD_WIDTH - OSD_OFF_SET;
        default:
            return OSD_OFF_SET;
    }
}
static inline EC_INT osd_y_postion (ec_video_conf_t *conf)
{
    OSD_CONF_PTR;
    switch (osdConf->osd_postion_type)
    {
        case EC_OSD_POSITON_LEFT_UP:
        case EC_OSD_POSTION_RIGHT_UP:
            return OSD_OFF_SET;
        case EC_OSD_POSTION_LEFT_DOWN:
        case EC_OSD_POSTION_RIGHT_DOWN:
            return get_video_height(conf->ensize) - OSD_HEIGHT;
        default:
            return OSD_OFF_SET;
    }
}



#endif //EC_MAIN_APP_EC_HISI_SDK_CONF_H
