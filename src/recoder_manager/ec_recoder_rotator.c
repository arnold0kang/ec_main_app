//
// Created by arnold on 3/23/2018.
//
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "ec_recoder_rotator.h"
#include "../common/ec_define.h"
#include "../module_manager/ec_module.h"
#include "../event/ec_event.h"
#include "../conf/ec_conf.h"
#include "../disk_manager/ec_disk_monitor.h"
#include "../log/ec_log.h"


static EC_VOID del_last_video (EC_VOID);
static EC_VOID on_event (EC_VOID *arg);


static EC_VOID *event_node = EC_NULL;
EC_VOID ec_record_raotator_run (EC_VOID)
{

    ec_event_handle *handle = ec_event_new(EC_EVENT_DISK_FULL);
    handle->eventCallback = on_event;
    event_node = ec_event_attache(handle);
}

EC_VOID ec_record_raotator_stop(EC_VOID)
{
    if (event_node)
    {
        ec_event_del(event_node);
        event_node = EC_NULL;
    }

}

static EC_VOID on_event (EC_VOID *arg)
{
    while (!ec_disk_is_enough())
    {
        del_last_video();
    }

}

static inline EC_BOOL check_file_ext(const EC_CHAR *filename, const EC_CHAR *ext)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename)
    {
        return EC_FALSE;
    }
    if (strstr(dot, ext))
    {
        return EC_TRUE;
    }
    else
    {
        return EC_FALSE;
    }

}



static EC_VOID del_last_video (EC_VOID)
{
    DIR *dir = EC_NULL;
    struct dirent *dirEnt;
    struct stat statbuf;
    struct stat delStatbuf;
    EC_CHAR delPath[BUFSIZ] = {'\0'};
    EC_CHAR tmpPath[BUFSIZ] = {'\0'};

    EC_MEMSET(&statbuf);
    EC_MEMSET(&delStatbuf);

    dir = opendir(EC_VIDEO_DIR_PATH);



    while ((dirEnt = readdir(dir))!= NULL)
    {

        if (strcmp (dirEnt->d_name, ".") == 0)
        {
            continue;
        }
        if (strcmp (dirEnt->d_name, "..") == 0)
        {
            continue;
        }
        if (dirEnt->d_type != DT_REG)
        {
            continue;
        }

        if (!check_file_ext(dirEnt->d_name, ".mp4")) //first mp4 file found
        {
            continue;
        }
        EC_MEMSET_2(tmpPath, BUFSIZ);
        snprintf (tmpPath, BUFSIZ-1, "%s/%s", EC_VIDEO_DIR_PATH, dirEnt->d_name);
        if (stat(tmpPath, &statbuf) == -1)
        {
            continue;
        }
        if (delStatbuf.st_mtim.tv_sec == 0 || delStatbuf.st_mtim.tv_sec > statbuf.st_mtim.tv_sec)
        {
            memcpy(&delStatbuf, &statbuf, sizeof(struct stat));
            EC_MEMSET_2(delPath, BUFSIZ);
            strcpy(delPath, tmpPath);
        }
    }
    dzlog_debug("rotate video file %s", delPath);
    unlink(delPath);

    closedir(dir);

    return;
}