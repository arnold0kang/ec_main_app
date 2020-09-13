#include <unistd.h>
#include "ec_osd.h"
#include "ec_hisi_sdk.h"
#include "../log/ec_log.h"
#include "../rtc/ec_rtc.h"
#include "SDL2/SDL_ttf.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../module_manager/ec_module.h"
#include "../conf/ec_conf.h"


#define EC_OSD_PATH_YAHEI						"/data/fonts/DejaVuSansMono_0.ttf"
#define MODULE_NAME         "ec_osd"



static struct
{
	TTF_Font *font;
	EC_VOID *bmp;
	SDL_Color forecol;
	EC_CHAR strBuf[BUFSIZ];
    uv_timer_t *osd_timer_handle;
}osd_stat;

//static EC_VOID *osd_work(EC_VOID *arg);

static EC_VOID osd_timer(uv_timer_t *handle);


EC_INT ec_osd_init (EC_VOID)
{
	//CHECK_SUCCESS(pthread_create(&(osd_stat.pid), NULL, osd_work, NULL), 0, 1);
	OSD_CONF_PTR;
	EC_ERR_PTR;
	CHECK_FAILED(TTF_Init(), 1);
	EC_MALLOC(osd_stat.osd_timer_handle);
	uv_timer_init(ec_looper, osd_stat.osd_timer_handle);

	osd_stat.font = TTF_OpenFont(EC_OSD_PATH_YAHEI, osdConf->fontSize);
	CHECK_NULL(osd_stat.font, 2);
	osd_stat.forecol.r = osdConf->color.redValue;
	osd_stat.forecol.g = osdConf->color.greenValue;
	osd_stat.forecol.b = osdConf->color.blackValue;
	osd_stat.forecol.a = osdConf->color.alphaValue;

	EC_MALLOC_WITH_SIZE(osd_stat.bmp, 1024 * 1024);

	return EC_SUCCESS;
	failed_3:
	EC_ERR_SET("malloc failed");
	failed_2:
	EC_ERR_SET("open font failed");
	failed_1:
	EC_ERR_SET("init ttf failed");
	failed_0:

	EC_ERR_OUT();
	return EC_FAILURE;
}
MODULE_ADD_INIT(MODULE_NAME, ec_osd_init);


EC_INT ec_osd_start(EC_VOID)
{

	uv_timer_start(osd_stat.osd_timer_handle, osd_timer, 1000, 1000);
	osd_timer(NULL); //for first time we need do this

	return EC_SUCCESS;
}


EC_INT ec_osd_stop(EC_VOID)
{


    uv_timer_stop(osd_stat.osd_timer_handle);

	return EC_SUCCESS;
}

extern  EC_VOID ec_hisi_sdk_update_osd(EC_VOID *bitmap, EC_INT bitMapSize);
static EC_VOID osd_timer(uv_timer_t *handle)
{
    DEV_CONF_PTR;
    SDL_Surface *text = EC_NULL;
    ec_rtc_get_time(osd_stat.strBuf, BUFSIZ);
    strcat(osd_stat.strBuf, "  ID:");
    strcat(osd_stat.strBuf, devConf->devid == EC_NULL ? "null": devConf->devid);
	strcat(osd_stat.strBuf, "/");
	strcat(osd_stat.strBuf, devConf->userid == EC_NULL ? "null": devConf->userid);
    //dzlog_debug("osd buf %s", osd_stat.strBuf);
    //text = TTF_RenderText_Solid(osd_stat.font, osd_stat.strBuf, osd_stat.forecol);
    text = TTF_RenderText_Blended_Wrapped(osd_stat.font, osd_stat.strBuf, osd_stat.forecol, 315);
    SDL_RWops *ops = SDL_RWFromMem(osd_stat.bmp, 1024 * 1024);
    CHECK_NULL(ops, 1);

    if (SDL_SaveBMP_RW(text, ops, 1) == -1)
    {
        dzlog_error("save sdl bmp failed");
        goto failed_1;
    }

	//dzlog_debug("w %d h %d", text->w, text->h);
	ec_hisi_sdk_update_osd(osd_stat.bmp, 0);
	SDL_FreeSurface(text);
	EC_MEMSET_2(osd_stat.strBuf, BUFSIZ);

	return;

failed_1:
	SDL_FreeSurface(text);
    EC_MEMSET_2(osd_stat.strBuf, BUFSIZ);
	return;
}