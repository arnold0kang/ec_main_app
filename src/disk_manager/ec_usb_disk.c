//
// Created by arnold on 4/19/2018.
//
#include <stdio.h>
#include <sys/mount.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ec_usb_disk.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../utils/ec_ko.h"
#include "../utils/ec_reg.h"
#include "../utils/ec_utils.h"
#include "../conf/ec_conf.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../event/ec_event.h"


static EC_INT load_usb_ko(EC_VOID);

static EC_INT unload_usb_ko(EC_VOID);

static EC_INT mount_disk(EC_VOID);

static EC_INT umount_disk(EC_VOID);

static EC_INT fix_disk(EC_VOID);

static EC_INT clean_fix_tmp_data(EC_VOID);

static EC_INT fix_disk_fmt(EC_VOID);

static EC_INT create_snap_dir(EC_VOID);


EC_INT ec_usb_disk_mount(EC_VOID)
{
    if (mount_disk() == EC_SUCCESS)
    {
        BROAD_CAST (EC_EVENT_DISK_ON);
        return EC_SUCCESS;
    }
    else
    {
        return EC_FAILURE;
    }
}

EC_INT ec_usb_disk_umount(EC_VOID)
{
    sync();
    ec_do_system_cmd("sync", NULL);

    if (umount_disk() == EC_FAILURE)
    {
        BROAD_CAST(EC_EVENT_DISK_OFF);
        return EC_FAILURE;
    }
   // create_snap_dir();

    return EC_SUCCESS;
}

EC_INT ec_usb_ko_load(EC_VOID)
{
    if (load_usb_ko() == EC_SUCCESS)
    {
        return EC_SUCCESS;
    }
    else
    {
        return EC_FAILURE;
    }
}

EC_INT ec_usb_ko_unload(EC_VOID)
{
    if (unload_usb_ko() == EC_SUCCESS)
    {
        return EC_SUCCESS;
    }
    else
    {
        return EC_FAILURE;
    }
}


typedef struct
{
    EC_CHAR *ko_name;
    EC_CHAR *param;
} ko_info_s;

#define KO_PATH "/home/usb_serial_ko/"

static ko_info_s ko_load_list[] = {
        {KO_PATH "configfs.ko",           EC_NULL},
        {KO_PATH "udc-hisi.ko",           EC_NULL},
        {KO_PATH "libcomposite.ko",       EC_NULL},
        {KO_PATH "u_serial.ko",           EC_NULL},
        {KO_PATH "usb_f_acm.ko",          EC_NULL},
        {KO_PATH "usb_f_mass_storage.ko", EC_NULL},
        {KO_PATH "g_multi.ko", "file=/dev/mmcblk0p8 luns=1 stall=0 removable=1"}
};

static EC_CHAR *ko_unload_list[] = {
        "g_multi.ko",
        "usb_f_mass_storage.ko",
        "usb_f_acm.ko",
        "u_serial.ko",
        "libcomposite.ko",
        "udc-hisi.ko",
        "configfs.ko"
};

static EC_INT load_usb_ko(EC_VOID)
{
#if 0
#if 1
    EC_INT index;
    for (index = 0; index < ARRAY_SIZE(ko_load_list); index++)
    {
        if (ec_ko_load(ko_load_list[index].ko_name, ko_load_list[index].param) == EC_FAILURE)
        {
            dzlog_error("load ko %s failed", ko_load_list[index].ko_name);
            return EC_FAILURE;
        }
        else
        {
            dzlog_info("load %s success", ko_load_list[index].ko_name);
        }
    }
    //ec_reg_set( 0x1203005C, 0x800311a0);
#else
    //set the reg for hi3516Cv300
    ec_reg_set (0x1203005C, 0x800311a0);

#endif
#else
    ec_do_system_cmd("/usr/bin/usb_load", NULL);

#endif
    return EC_SUCCESS;
}

static EC_INT unload_usb_ko(EC_VOID)
{
#if 0
#if 1
    ec_do_system_cmd("/home/usb_serial_ko/unload.sh", NULL);
#else
    ec_reg_set (0x1203005C, 0x000311a0);
#endif
#else
    ec_do_system_cmd("/usr/bin/usb_unload", NULL);
#endif
    return EC_SUCCESS;
}

#define MOUNT_CHECK_FILE   "/proc/mounts"
#define MOUNT_DISK_PATH     "/dev/mmcblk0p8"

#define MOUNT_TYPE          "vfat"
#define MOUNT_FLAG          (MS_NOATIME)


EC_BOOL is_mount(EC_VOID)
{
    int ret = 0;
    EC_CHAR buf[BUFSIZ] = {'\0'};
    FILE *fp = fopen(MOUNT_CHECK_FILE, "r");
    //CHECK_NULL(fp, 0);
    fread(buf, BUFSIZ - 1, 1, fp);
    fclose(fp);

    if (strstr(buf, EC_VIDEO_DIR_PATH))
    {
        return EC_TRUE;
    }
    else
    {
        return EC_FALSE;
    }


}


#define MAX_MOUNT_TRY           5

static EC_INT mount_disk(EC_VOID)
{
    EC_INT i = MAX_MOUNT_TRY;

    do
    {
        mount(MOUNT_DISK_PATH, EC_VIDEO_DIR_PATH, MOUNT_TYPE, MOUNT_FLAG, "errors=panic");
        if (is_mount())
        {
            dzlog_info("mount %s to %s success", MOUNT_DISK_PATH, EC_VIDEO_DIR_PATH);
            clean_fix_tmp_data();
            return EC_SUCCESS;
        }
        else
        {
            //尝试修复后再次挂载
            fix_disk_fmt();
        }
    } while (i--);

    dzlog_error("mount disk %s to %s failed", MOUNT_DISK_PATH, EC_VIDEO_DIR_PATH);
    return EC_FAILURE;
}

#define MAX_UMOUNT_TRY              10

static EC_INT umount_disk(EC_VOID)
{

    EC_INT i = MAX_UMOUNT_TRY;
    // umount2(MOUNT_POINT, MNT_DETACH);
    umount(EC_VIDEO_DIR_PATH);
    do
    {
        if (!is_mount())
        {
            dzlog_debug("umount %s success", EC_VIDEO_DIR_PATH);
            return EC_SUCCESS;
        }
        else
        {
            dzlog_debug("umount failed waiting...");
            cmh_wait_sec(1);
        }
    } while (i--);

    dzlog_error("unmount disk %s failed", EC_VIDEO_DIR_PATH);
    return EC_FAILURE;
}


#define FIX_DISK_CMD            "fsck.fat -a -w /dev/mmcblk0p8"

static EC_INT fix_disk(EC_VOID)
{
    return ec_do_system_cmd(FIX_DISK_CMD, EC_NULL);
}

#undef FIX_DISK_CMD

#define CLEAN_FIX_TMP_DATA_CMD      "rm -fr /udisk/*.REC  /udisk/snap/*.REC"

static EC_INT clean_fix_tmp_data(EC_VOID)
{
    return ec_do_system_cmd(CLEAN_FIX_TMP_DATA_CMD, EC_NULL);
}

#undef CLEAN_FIX_TMP_DATA_CMD

#define DISK_FIX_PATH          "/dev/mmcblk0p8"
#define DISK_CHECK_TYPE_CMD     "blkid /dev/mmcblk0p8"
#define DISK_FORMAT_CMD         "mkfs.vfat /dev/mmcblk0p8"

static EC_INT fix_disk_fmt(EC_VOID)
{
    EC_CHAR buf[BUFSIZ] = {'\0'};

    if (ec_do_system_cmd_2(DISK_CHECK_TYPE_CMD, buf, BUFSIZ - 1) == EC_FAILURE)
    {
        dzlog_error("check %s 's file system type failed", DISK_FIX_PATH);
        return EC_FAILURE;
    }
    if (strstr(buf, "fat"))
    {
        fix_disk();
        return EC_SUCCESS;
    }
    else
    {   //not FAT ? format this partition ?
        dzlog_info("current %s format is %s, need reformat", DISK_FIX_PATH, buf);
        ec_do_system_cmd(DISK_FORMAT_CMD, EC_NULL);
    }

    return EC_SUCCESS;
}

#undef DISK_FIX_PATH
#undef DISK_CHECK_TYPE_CMD
#undef DISK_FORMAT_CMD


static EC_INT create_snap_dir(EC_VOID)
{


    return EC_SUCCESS;
}
