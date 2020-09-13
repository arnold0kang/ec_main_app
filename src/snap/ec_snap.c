#include "ec_snap.h"
#include "../conf/ec_conf.h"
#include "../rtc/ec_rtc.h"
#include "../sdk/ec_hisi_sdk_snap.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../beep/ec_beep.h"
#include "../utils/ec_utils.h"

static EC_BOOL is_run = EC_FALSE;


static EC_VOID snap_work(uv_work_t *req);
static EC_VOID snap_after_work (uv_work_t *req, int status);

EC_VOID ec_snap_do (EC_VOID)
{

    if(ec_stat_set (&is_run) == EC_FAILURE)
    {
        return;
    }
    if (!DISK_READY())
    {
        dzlog_debug("disk not ready");
        return;
    }
    if (CHARGE_ON())
    {
        dzlog_debug("char on, do nothing");
        return;
    }
    mkdir(EC_SNAP_DIR_PATH, 0644);

    uv_work_t *req;
    EC_MALLOC(req);
    uv_queue_work(ec_looper, req, snap_work, snap_after_work);

    return;

}

static EC_VOID snap_work(uv_work_t *req)
{
    EC_CHAR snapName[BUFSIZ] = {'\0'};
    EC_CHAR tsStr[32] = {'\0'};
    DEV_CONF_PTR;

    ec_rtc_get_ts (tsStr, 31);
    snprintf(snapName, BUFSIZ -1, "%s/%s-%s-%s.jpg", EC_SNAP_DIR_PATH,
             devConf->devid, devConf->userid, tsStr);
    dzlog_debug("set snap file name %s", snapName);
    if (ec_hisi_sdk_snap_to_file(snapName)== EC_SUCCESS)
    {
        BEEP();
    }

}
static EC_VOID snap_after_work (uv_work_t *req, int status)
{
    EC_FREE(req);

    ec_stat_unset(&is_run);

    return;
}
