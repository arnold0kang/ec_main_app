#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "../log/ec_log.h"
#include "ec_conf.h"
#include "../3th_src/cJSON.h"
#include "../event/ec_event.h"


#define EC_CONF_PATH						"/data/conf/king.json"

#define EC_CONF_WIFICONF_PATH				"/data/wifi/wifi.cfg"


static EC_VOID ec_conf_default(EC_VOID);
static EC_INT get_encoder_conf(cJSON *root);
static EC_INT get_video_conf(cJSON *root);
static EC_INT get_live_conf (cJSON *root);
static EC_INT get_snap_conf(cJSON *root);
static EC_INT get_osd_conf(cJSON *root);
static EC_INT get_recoder_conf(cJSON *root);;
static EC_INT get_stream_conf(cJSON *root);
static EC_INT get_alarm_conf(cJSON *root);
static EC_INT get_audio_conf (cJSON *root);
static EC_INT get_wifi_conf(EC_VOID);


static EC_INT set_encoder_conf(cJSON *root);
static EC_INT set_video_conf(cJSON *root);
static EC_INT set_recoder_conf(cJSON *root);
static EC_INT set_stream_conf(cJSON *root);
static EC_INT set_alarm_conf(cJSON *root);
static EC_INT set_wifi_conf(EC_VOID);

#define EC_JSON_GET_OBJ(NODE, ROOT, STR)  do { \
	NODE = cJSON_GetObjectItem(ROOT, STR); \
	if (!NODE) {return EC_FAILURE;} }while(0)

#define EC_JSON_GET_ARR(NODE, ROOT, INDEX) do{\
	NODE = cJSON_GetArrayItem(ROOT, INDEX); \
	if (!NODE) {return EC_FAILURE;} }while(0)

#define EC_GET_CONF_STR(KEY, VALUE) do {\
	if (KEY) {free (KEY); KEY=NULL;} \
	if (VALUE != NULL) {KEY=strdup (VALUE);}} while (0)

#define EC_GET_CONF_INT(KEY, VALUE) do {\
	KEY = VALUE;}while(0)

#define EC_JSON_GET_INT(ROOT, KEY, DATA) do {\
	cJSON *node = NULL; \
	EC_JSON_GET_OBJ(node, ROOT, KEY); \
	EC_GET_CONF_INT(DATA, node->valueint);}while (0) 

#define EC_JSON_GET_STR(ROOT, KEY, DATA) do {\
	cJSON *node = NULL; \
	EC_JSON_GET_OBJ(node, ROOT, KEY); \
	EC_GET_CONF_STR(DATA, node->valuestring);}while(0)

#define EC_JSON_SET_INT(ROOT, KEY, VALUE) do {\
	cJSON_AddNumberToObject(ROOT, KEY, VALUE);}while(0)

#define EC_JSON_SET_STR(ROOT, KEY, VALUE) do {\
	if (VALUE) {cJSON_AddStringToObject(ROOT, KEY, VALUE);} \
	else{cJSON_AddNullToObject(ROOT, KEY);}}while(0)

#define EC_JSON_SET_BOOL(ROOT, KEY, VALUE) do {\
	cJSON_AddBoolToObject(ROOT, KEY, VALUE);}while(0)

ec_conf_t  *g_conf = EC_NULL;



ec_conf_t *ec_conf_get(EC_VOID)
{
	EC_ERR_PTR;
	if (g_conf)
	{
		return g_conf;
	}

	EC_INT ret = EC_SUCCESS;
	EC_MALLOC(g_conf);


	FILE *fp = NULL;
	EC_INT fileLen = 0;
	EC_CHAR *fileStr = NULL;
	fp = fopen(EC_CONF_PATH, "rb");
	CHECK_NULL(fp, 0);

	fseek(fp, 0, SEEK_END);
	fileLen = ftell(fp); fseek(fp, 0, SEEK_SET); //get len of file;

	EC_MALLOC_WITH_SIZE(fileStr, fileLen + 1);

	ret = fread(fileStr, 1, fileLen, fp);
	CHECK_SUCCESS(ret, fileLen, 1);


	cJSON *json;
	json = cJSON_Parse(fileStr);
	CHECK_NULL(json, 2);

	
	ret = get_encoder_conf(json);
	CHECK_FAILED(ret, 3);
	ret = get_osd_conf(json);
	CHECK_FAILED(ret, 4);
	ret = get_recoder_conf(json);
	CHECK_FAILED(ret, 5);
	ret = get_stream_conf(json);
	CHECK_FAILED(ret, 6);
	ret = get_alarm_conf(json);
	CHECK_FAILED(ret, 7);

	ret = get_wifi_conf();
	CHECK_FAILED(ret, 9);


	cJSON_Delete(json);
    fclose(fp);
    EC_FREE(fileStr);
	dzlog_info("get config success");

	return g_conf;


	failed_9:
    EC_ERR_SET("get wifi config failed");
    failed_8:
    EC_ERR_SET("get audio config failed");
    failed_7:
    EC_ERR_SET("get alarm conf failed");
    failed_6:
    EC_ERR_SET("get stream conf failed");
    failed_5:
    EC_ERR_SET("get recorder conf failed");
    failed_4:
    EC_ERR_SET("get osd conf failed");
    failed_3:
    EC_ERR_SET("get encorde failed");
    cJSON_Delete(json);
    failed_2:
    EC_ERR_SET("parse json failed");
    EC_FREE(fileStr);
    failed_1:
    EC_ERR_SET("read failed");
    fclose(fp);
    failed_0:

	EC_ERR_OUT();
	ec_conf_default();
	return g_conf;
}

#define EC_CONFIG_DUMP
#ifdef EC_CONFIG_DUMP
#define CONF_DUMP_BEGIN			fprintf(stderr, "<-----------------------config dump\n")
#define CONF_DUMP_END			fprintf(stderr, "------------------------>\n")
#define DUMP_BEGIN(MODULE)		fprintf(stderr, "\t<-----------------------" #MODULE  "\n")
#define DUMP_END				fprintf(stderr, "\t----------------------->\n")
#define DUMP_NUM(VALUE)			fprintf(stderr, "\t\t" #VALUE "-->%d\n", VALUE)
#define DUMP_STR(VALUE)			fprintf(stderr, "\t\t" #VALUE "-->%s\n", VALUE)

#endif // EC_CONFIG_DUMP


EC_VOID ec_conf_dump(EC_VOID)
{
	CONF_DUMP_BEGIN;

	DUMP_BEGIN(g_conf->mediaConf);
	DUMP_NUM(g_conf->mediaConf.snapConf.ensize);
	DUMP_NUM(g_conf->mediaConf.videoConf.bitrate);
	DUMP_NUM(g_conf->mediaConf.videoConf.ensize);
	DUMP_NUM(g_conf->mediaConf.videoConf.entype);
	DUMP_NUM(g_conf->mediaConf.videoConf.framerate);
	DUMP_NUM(g_conf->mediaConf.audioConf.voice);
	DUMP_END;




	DUMP_BEGIN(g_conf->recordConf);
	DUMP_NUM(g_conf->recordConf.mode);
	DUMP_NUM(g_conf->recordConf.interval);
	DUMP_END;

	DUMP_BEGIN(g_conf->devConf);
	DUMP_STR(g_conf->devConf.devid);
	DUMP_STR(g_conf->devConf.userid);
	DUMP_END;
	DUMP_BEGIN(g_conf->streamSrvConf);
	DUMP_STR(g_conf->streamSrvConf.url);
	DUMP_END;
	DUMP_BEGIN(g_conf->alarmConf);
	DUMP_STR(g_conf->alarmConf.url);
	DUMP_END;

	CONF_DUMP_END;
}

#undef EC_CONFIG_DUMP

EC_INT ec_conf_save(EC_VOID)
{
	cJSON *node = EC_NULL;
	cJSON *root = EC_NULL;
	FILE *fp = NULL;
	EC_ERR_PTR;

	dzlog_debug("%s", __func__);


	
	fp = fopen(EC_CONF_PATH, "w");
	CHECK_NULL(fp, 0);
	root = cJSON_CreateObject();
	CHECK_NULL(root, 1);
	cJSON_AddItemToObject(root, "encoder", node = cJSON_CreateObject());
	set_encoder_conf(node);

	cJSON_AddItemToObject(root, "recoder", node = cJSON_CreateObject());
	set_recoder_conf(node);
	cJSON_AddItemToObject(root, "stream", node = cJSON_CreateObject());
	set_stream_conf(node);
	cJSON_AddItemToObject(root, "alarm", node = cJSON_CreateObject());
	set_alarm_conf(node);


	EC_CHAR *str = cJSON_Print(root);
	//dzlog_debug("new config %s", str);
	fwrite(str, strlen(str), 1, fp);
	fclose(fp);
	free(str);
	cJSON_Delete(root);
	dzlog_debug("set json success");
	set_wifi_conf();
	dzlog_debug("set wifi success");


	return EC_SUCCESS;

	
failed_1:
	EC_ERR_SET("create json object failed");
	return EC_FAILURE;
failed_0:
	EC_ERR_SET("open config file failed");



	EC_ERR_OUT();
	return EC_FAILURE;

}


EC_INT ec_conf_set_video(ec_video_conf_t *conf)
{
	EC_GET_CONF_INT(g_conf->mediaConf.videoConf.bitrate, conf->bitrate);
	EC_GET_CONF_INT(g_conf->mediaConf.videoConf.framerate, conf->framerate);

	return EC_SUCCESS;
}

EC_INT ec_conf_set_recoder(ec_recoder_conf_t *conf)
{

	EC_GET_CONF_INT(g_conf->recordConf.mode, conf->mode);

	return EC_SUCCESS;
}



EC_INT ec_conf_set_stream(ec_streamsrv_conf_t * stream)
{

	EC_GET_CONF_STR(g_conf->streamSrvConf.url, stream->url);

	return EC_SUCCESS;
}

EC_INT ec_conf_set_alarm(ec_alarm_conf_t * alarm)
{

	EC_GET_CONF_STR(g_conf->alarmConf.url, alarm->url);


	return EC_SUCCESS;
}



EC_INT ec_conf_set_devid(EC_CHAR * devid)
{
	EC_GET_CONF_STR(g_conf->devConf.devid, devid);
	//ec_conf_save();

	return EC_SUCCESS;
}

EC_INT ec_conf_set_usrid(EC_CHAR * usrid)
{

	EC_GET_CONF_STR(g_conf->devConf.userid, usrid);

	return EC_SUCCESS;
}


EC_VOID ec_conf_get_video_resolution(EC_INT * resolution)
{
	VIDEO_CONF_PTR;

	dzlog_debug("%s", __func__);

	
	switch (videoConf->ensize)
	{
	case EN_SIZE_4K:
		resolution[0] = 3840;
		resolution[1] = 2160;
		break;
	case EN_SIZE_1080:
		resolution[0] = 1920;
		resolution[1] = 1080;
		break;
	case EN_SIZE_720:
		resolution[0] = 1280;
		resolution[1] = 720;
		break;
	case EN_SIZE_480:
		resolution[0] = 640;
		resolution[1] = 480;
		break;
	default:
		resolution[0] = 1920;
		resolution[1] = 1080;
		break;
	}

	return;
}

EC_INT ec_conf_set_video_conf(ec_video_conf_t * videoConf)
{

	EC_GET_CONF_INT(g_conf->mediaConf.videoConf.bitrate, videoConf->bitrate);
	EC_GET_CONF_INT(g_conf->mediaConf.videoConf.framerate, videoConf->framerate);

	return EC_SUCCESS;
}



ec_pic_en_size ec_conf_get_en_size(EC_INT width, EC_INT height)
{
	dzlog_debug("%s", __func__);

	switch (width)
	{
	case 1920:
		return EN_SIZE_1080;
	case 1280:
		return EN_SIZE_720;
	default:
		return EN_SIZE_INVALID;
	}

	return EN_SIZE_INVALID;
}


EC_INT ec_conf_set_recoder_conf(ec_recoder_conf_t * recoderConf)
{
	EC_GET_CONF_INT(g_conf->recordConf.interval, recoderConf->interval);
	EC_GET_CONF_INT(g_conf->recordConf.mode, recoderConf->mode);

	return EC_SUCCESS;
}

EC_INT ec_conf_set_wifi_conf(ec_wifi_conf_t * wifiConf)
{
	EC_GET_CONF_STR(g_conf->wifiConf.ssid, wifiConf->ssid);
	EC_GET_CONF_INT(g_conf->wifiConf.encMode, wifiConf->encMode);
	EC_GET_CONF_STR(g_conf->wifiConf.password, wifiConf->password);

	return EC_SUCCESS;
}


static EC_VOID ec_conf_default(EC_VOID)
{
	g_conf = calloc(1, sizeof(ec_conf_t));
	CHECK_NULL(g_conf, 0);
	g_conf->mediaConf.videoConf.bitrate = 8192;
	g_conf->mediaConf.videoConf.ensize = EN_SIZE_1080;
	g_conf->mediaConf.videoConf.entype = EN_TYPE_H264;
	g_conf->mediaConf.videoConf.framerate = 30;
	g_conf->mediaConf.audioConf.voice = 0;


	g_conf->recordConf.mode = EC_RECODER_MODE_ROTATE;
	g_conf->recordConf.interval = 60;


	g_conf->devConf.devid = strdup("test_dev");
	g_conf->devConf.userid = strdup("test_user");
	
	return;


failed_0:
	return;
}

static EC_INT get_encoder_conf(cJSON *root)
{
	EC_ERR_PTR;
	EC_INT ret = EC_SUCCESS;
	cJSON *json = EC_NULL;
	
	EC_JSON_GET_OBJ(json, root, "encoder");


	ret = get_video_conf(json);
	CHECK_FAILED(ret, 1);

	ret = get_snap_conf(json);
	CHECK_FAILED(ret, 1);


	ret = get_live_conf(json);
	CHECK_FAILED(ret, 1);

	CHECK_FAILED(get_audio_conf(json), 1);

	return EC_SUCCESS;
failed_1:
	EC_ERR_SET("read encoder config failed");
	EC_ERR_OUT();
	return EC_FAILURE;
}

static EC_INT get_video_conf(cJSON *root)
{
	cJSON *json = EC_NULL;
	dzlog_debug("%s", __func__);

	EC_JSON_GET_OBJ(json, root, "video");
    EC_JSON_GET_INT(json, "bitrate", g_conf->mediaConf.videoConf.bitrate);
   // EC_JSON_GET_INT(json, "framerate", g_conf->mediaConf.videoConf.framerate);
	g_conf->mediaConf.videoConf.framerate = 30; //设置为不可更改
    g_conf->mediaConf.videoConf.gop = g_conf->mediaConf.videoConf.framerate; //
#if 0 //use default
	EC_JSON_GET_INT(json, "entype", g_conf->mediaConf.videoConf.entype);
	EC_JSON_GET_INT(json, "ensize", g_conf->mediaConf.videoConf.ensize);
#else
    g_conf->mediaConf.videoConf.entype = EN_TYPE_H264;
    g_conf->mediaConf.videoConf.ensize = EN_SIZE_1080;
   // g_conf->mediaConf.videoConf.framerate = 25; //25fps

    g_conf->mediaConf.videoConf.rctype = 1; //VBR
    g_conf->mediaConf.videoConf.h264ProfileType = H264_HIGH_PROFILE;
#endif


	return EC_SUCCESS;
}


static EC_INT get_live_conf (cJSON *root)
{
    cJSON *json = EC_NULL;
    EC_JSON_GET_OBJ(json, root, "live");
    EC_JSON_GET_INT(json, "bitrate", g_conf->mediaConf.liveConf.bitrate);
    EC_JSON_GET_INT(json, "framerate", g_conf->mediaConf.liveConf.framerate);
    EC_JSON_GET_INT(json, "gop", g_conf->mediaConf.liveConf.gop);


    g_conf->mediaConf.liveConf.entype = EN_TYPE_H264;
    g_conf->mediaConf.liveConf.ensize = EN_SIZE_720;
//    g_conf->mediaConf.liveConf.framerate = g_conf->mediaConf.liveConf.framerate; //30fps
//	g_conf->mediaConf.liveConf.gop = g_conf->mediaConf.liveConf.framerate;
    //g_conf->mediaConf.liveConf.bitrate = 1024;//1M
    g_conf->mediaConf.liveConf.rctype = 0;//CBR
    g_conf->mediaConf.liveConf.h264ProfileType = H264_MAIN_PROFILE;

    return EC_SUCCESS;
}


static EC_INT get_snap_conf(cJSON *root)
{
#if 0
	dzlog_debug("%s", __func__);

	cJSON *json = EC_NULL;

	EC_JSON_GET_OBJ(json, root, "image");

	EC_JSON_GET_INT(json, "ensize", g_conf->mediaConf.snapConf.ensize);
#else
    g_conf->mediaConf.snapConf.ensize  = EN_SIZE_1080;
#endif

	return EC_SUCCESS;
}

static EC_INT get_osd_conf(cJSON *root)
{
	//internal part
    g_conf->osdConf.fontSize = 25;
    g_conf->osdConf.osd_postion_type = EC_OSD_POSTION_LEFT_DOWN;
    //color
    g_conf->osdConf.color.redValue = 0xff;
    g_conf->osdConf.color.greenValue = 0xFF;
    g_conf->osdConf.color.blackValue = 0xFF;
    g_conf->osdConf.color.alphaValue = 0x00;

	return EC_SUCCESS;
}


static EC_INT get_recoder_conf(cJSON *root)
{
	cJSON *json = EC_NULL;
	dzlog_debug("%s", __func__);

	EC_JSON_GET_OBJ(json, root, "recoder");

	EC_JSON_GET_INT(json, "mode", g_conf->recordConf.mode);
	EC_JSON_GET_INT(json, "interval", g_conf->recordConf.interval);

	return EC_SUCCESS;
}





static EC_INT get_stream_conf(cJSON *root)
{
	cJSON *json = EC_NULL;

	EC_JSON_GET_OBJ(json, root, "stream");

	EC_JSON_GET_STR(json, "url", g_conf->streamSrvConf.url);

	return EC_SUCCESS;
}


static EC_INT get_alarm_conf(cJSON *root)
{
	cJSON *json = EC_NULL;

	EC_JSON_GET_OBJ(json, root, "alarm");

	EC_JSON_GET_STR(json, "url", g_conf->alarmConf.url);

	return EC_SUCCESS;
}

static EC_INT get_audio_conf (cJSON *root)
{
    cJSON *json = EC_NULL;
    EC_JSON_GET_OBJ(json, root, "audio");
    EC_JSON_GET_INT(json, "voice", g_conf->mediaConf.audioConf.voice);

    if (g_conf->mediaConf.audioConf.voice < -50)
    {
        g_conf->mediaConf.audioConf.voice  = -50;
    }
    if (g_conf->mediaConf.audioConf.voice > 50)
    {
        g_conf->mediaConf.audioConf.voice = 50;
    }
   // g_conf->mediaConf.audioConf.voice +=19; //根据SDK手册上，合理设置范围为19~50
    return EC_SUCCESS;
}

static EC_INT get_wifi_conf(EC_VOID)
{
	return EC_SUCCESS;
}


static EC_INT set_encoder_conf(cJSON *root)
{
	cJSON *node = EC_NULL;

	CHECK_NULL(root, 1);

	cJSON_AddItemToObject(root, "video", node = cJSON_CreateObject());
	set_video_conf(node);



	return EC_SUCCESS;

failed_1:
	return EC_FAILURE;
}
static EC_INT set_video_conf(cJSON *root)
{
	CHECK_NULL(root, 1);

	EC_JSON_SET_INT(root, "bitrate", g_conf->mediaConf.videoConf.bitrate);
	EC_JSON_SET_INT(root, "framerate", g_conf->mediaConf.videoConf.framerate);

	return EC_SUCCESS;
failed_1:
	return EC_FAILURE;
}


static EC_INT set_recoder_conf(cJSON *root)
{
	CHECK_NULL(root, 1);

	EC_JSON_SET_INT(root, "mode", g_conf->recordConf.mode);
	EC_JSON_SET_INT(root, "interval", g_conf->recordConf.interval);

	return EC_SUCCESS;

failed_1:
	return EC_FAILURE;
}


static EC_INT set_stream_conf(cJSON *root)
{
	CHECK_NULL(root, 1);

	EC_JSON_SET_STR(root, "url", g_conf->streamSrvConf.url);

	return EC_SUCCESS;

failed_1:
	return EC_FAILURE;

}
static EC_INT set_alarm_conf(cJSON *root)
{
	CHECK_NULL(root, 1);

	EC_JSON_SET_STR(root, "url", g_conf->alarmConf.url);

	return EC_SUCCESS;

failed_1:
	return EC_FAILURE;
}

const EC_CHAR *a = "ctrl_interface=/var/run/wpa_supplicant\nupdate_config=1\nnetwork={\ndisabled=1\nssid=\"%s\"\nkey_mgmt=WPA-PSK\npsk=\"%s\"\n}\n";
//const EC_CHAR *a = "update_config = 1\neapol_version = 2\nautoscan =periodic:1\nnetwork = {\nssid = \"%s\"\npsk = \"%s\"\nproto = RSN\nkey_mgmt = WPA-PSK\npairwise = CCMP\ngroup = CCMP\nbgscan = \"simple:1:-65:2\"\n}\n";

const EC_CHAR *b = "ctrl_interface=/var/run/wpa_supplicant\nupdate_config=1\nnetwork={\ndisabled=1\nssid=\"%s\"\nkey_mgmt=NONE\n}\n";

static EC_INT set_wifi_conf(EC_VOID)
{
	FILE *fp = EC_NULL;
	EC_CHAR buf[BUFSIZ] = { '\0' };

	if (g_conf->wifiConf.ssid == EC_NULL || strlen (g_conf->wifiConf.ssid) ==0)
	{
		return EC_SUCCESS; //IGNORE THIS
	}

	if (g_conf->wifiConf.password)
	{
		snprintf(buf, BUFSIZ - 1, a, g_conf->wifiConf.ssid, g_conf->wifiConf.password);
	}
	else
	{
		snprintf(buf, BUFSIZ - 1, b, g_conf->wifiConf.ssid);
	}
	fp = fopen(EC_CONF_WIFICONF_PATH, "w");
	CHECK_NULL(fp, 0);
	fwrite(buf, strlen(buf), 1, fp);
	fclose(fp);


failed_0:
	return EC_FAILURE;
}