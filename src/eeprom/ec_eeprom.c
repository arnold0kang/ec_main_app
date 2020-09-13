#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

#include "../libuv_wraper/ec_libuv.h"
#include "ec_eeprom.h"
#include "../i2c/ec_iic_priv.h"
#include "../event/ec_event.h"
#include "../module_manager/ec_module.h"
#include "../power_manager/ec_power.h"
#include "../rtc/ec_rtc.h"
#include "../conf/ec_conf.h"
#include "../utils/ec_utils.h"

#define POWERINFO_SET_INTERVAL          30000
#define TIME_GET_INTERVAL               2000
#define EC_IIC_DEV            "/dev/i2c-1"
#define EC_IIC_ADDR            0xA0
#define EC_IIC_INTERVAL        5
#define EC_ADDR_OFFSET        0x00
#define EC_DEVID_ADDR            0x01
#define EC_DEVID_LEN            0x06
#define EC_USERID_ADDR        0x07
#define EC_USERID_LEN            0x08
#define EC_ADC_ADDR            0x0F
#define EC_ADC_LEN            0x01
#define EC_TS_ADDR            0x10
#define EC_TS_LEN                0x04
#define MODULE_NAME         "EC_EEPROM"

static struct
{
    uv_timer_t *powerInfoHandle;
    uv_timer_t *tsHandle;
} eeprom_stat = {EC_NULL, EC_NULL};

static EC_VOID onChargeCallback(EC_VOID *arg);

static EC_VOID startAllTimer(EC_VOID);

static EC_VOID stopAllTimer(EC_VOID);

static EC_VOID getConfig(EC_VOID);

static EC_VOID onSetPowerInfo(uv_timer_t *handle);

static EC_VOID onGetTime(uv_timer_t *handle);

static EC_INT eeprom_read(EC_INT addr, EC_INT len, EC_CHAR *buf);

static EC_INT eeprom_write(EC_INT addr, EC_INT len, EC_CHAR *buf);


static EC_INT ec_eeprom_init(EC_VOID)
{
    EC_MALLOC(eeprom_stat.powerInfoHandle);
    EC_MALLOC(eeprom_stat.tsHandle);

    uv_timer_init(ec_looper, eeprom_stat.tsHandle);
    uv_timer_init(ec_looper, eeprom_stat.powerInfoHandle);
    return EC_SUCCESS;
}

MODULE_ADD_INIT(MODULE_NAME, ec_eeprom_init);


EC_INT king_iic_start(EC_VOID)
{
  //  uv_timer_start(eeprom_stat.tsHandle, onGetTime,
  //                 TIME_GET_INTERVAL, TIME_GET_INTERVAL);

    uv_timer_start(eeprom_stat.powerInfoHandle, onSetPowerInfo,
                   POWERINFO_SET_INTERVAL, POWERINFO_SET_INTERVAL);

    return EC_SUCCESS;
}

EC_INT king_iic_stop(EC_VOID)
{
    uv_timer_stop(eeprom_stat.powerInfoHandle);

  //  uv_timer_stop(eeprom_stat.tsHandle);

    return EC_SUCCESS;
}

EC_INT king_iic_get_conf(EC_VOID)
{
    EC_CHAR devid[EC_DEVID_LEN + 1] = {'\0'};
    EC_CHAR userid[EC_USERID_LEN + 1] = {'\0'};
    EC_ERR_PTR;
    dzlog_info("get config from eeprom");

    //clean old devid & userid config first
    ec_conf_set_devid(EC_NULL);
    ec_conf_set_usrid(EC_NULL);

#ifndef FACTORY_MODE
    CHECK_FAILED(eeprom_read(EC_DEVID_ADDR, EC_DEVID_LEN, devid), 1);
    CHECK_FAILED(eeprom_read(EC_USERID_ADDR, EC_USERID_LEN, userid), 0);
#else
    strcpy (devid, "test");
    strcpy (userid, "test");
#endif
    dzlog_debug("get userid %s devid %s from eeprom", userid, devid);
    if (strlen(devid))
    {
        ec_conf_set_devid(devid);
    }
    if (strlen(userid))
    {
        ec_conf_set_usrid(userid);
    }

    return EC_SUCCESS;
    failed_1:
    EC_ERR_SET("get devid failed");
    failed_0:
    EC_ERR_SET("get user id failed");
    EC_ERR_OUT();
    return EC_FAILURE;
}


static EC_VOID onSetPowerInfo(uv_timer_t *handle)
{
#if 1
    EC_INT poerinfo = ec_power_current_info();
    //  dzlog_debug("set power info %d", poerinfo);
    eeprom_write(EC_ADC_ADDR, EC_ADC_LEN, (EC_CHAR *) &poerinfo);
#endif
    return;
}

static EC_VOID onGetTime(uv_timer_t *handle)
{
#if 1
    EC_UINT ts = 0;
    dzlog_debug("on get time");
    CHECK_FAILED(eeprom_read(EC_TS_ADDR, EC_TS_LEN, (EC_CHAR *) &ts), 0);
    if (0 == ts)
    {
        return;
    }
    // dzlog_debug("get ts %u", ts);
    ec_rtc_set_diff(ts);
    //reset ts zone
    ts = 0;
    CHECK_FAILED(eeprom_write(EC_TS_ADDR, EC_TS_LEN, (EC_CHAR *) &ts), 1);

    return;
    failed_1:
    dzlog_debug("reset ts zone failed");
    failed_0:
    eeprom_write(EC_TS_ADDR, EC_TS_LEN, (EC_CHAR *) &ts);
    dzlog_debug("read time failed");
#endif
    return;

}


EC_VOID ec_update_time_from_eeprom(void)
{
    EC_UINT ts = 0;
    dzlog_debug("update time");
    CHECK_FAILED(eeprom_read(EC_TS_ADDR, EC_TS_LEN, (EC_CHAR *) &ts), 0);
    if (0 == ts)
    {
        return;
    }
    // dzlog_debug("get ts %u", ts);
    ec_rtc_set_diff(ts);
    //reset ts zone

    return;

    failed_0:
    dzlog_debug("update time failed");

    return;
}


#define  EEPROM_MAX_TRY         5


static EC_INT eeprom_read(EC_INT addr, EC_INT len, EC_CHAR *buf)
{
    EC_INT ret = EC_SUCCESS;
    EC_INT i;
    EC_INT fd = open(EC_IIC_DEV, O_RDWR);
    CHECK_FAILED(fd, 0);

    for (i = 0; i < EEPROM_MAX_TRY; i++)
    {
        ret = ec_iic_read(fd, addr, buf, len);
        if (ret == EC_SUCCESS)
        {
            break;
        }
        cmh_wait_usec(500000);
    }
    CHECK_FAILED(ret, 1);
    CLOSE_FD(fd);
    return EC_SUCCESS;
    failed_1:
    CLOSE_FD(fd);
    failed_0:
    return EC_FAILURE;
}

static EC_INT eeprom_write(EC_INT addr, EC_INT len, EC_CHAR *buf)
{
    EC_INT ret = EC_SUCCESS;
    EC_INT i = 0;
    EC_INT fd = open(EC_IIC_DEV, O_RDWR);
    CHECK_FAILED(fd, 0);
    for (i = 0; i < EEPROM_MAX_TRY; i++)
    {
        ret  =ec_iic_write(fd, addr, buf, (EC_CHAR) len);
        if (ret == EC_SUCCESS)
        {
            break;
        }
        cmh_wait_usec(500000);
    }
    CHECK_FAILED(ret, 1);
    CLOSE_FD(fd);
    return EC_SUCCESS;
    failed_1:
    CLOSE_FD(fd);
    failed_0:
    return EC_FAILURE;
}
