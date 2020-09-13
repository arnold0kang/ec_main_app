//
// Created by arnold on 4/8/2018.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <netinet/tcp.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/timerfd.h>
#include <errno.h>

#include "ec_utils.h"
#include "ec_reg.h"
#include "../log/ec_log.h"


EC_BOOL ec_is_ip_get(EC_VOID)
{
    EC_INT fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strcpy(ifr.ifr_name, "wlan0");

    if (ioctl(fd, SIOCGIFADDR, &ifr) == -1)
    {
        goto failed_1;
    }

    close(fd);

    /* display result */
    //strcpy(buf, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    return EC_TRUE;
    failed_1:
    close(fd);
    return EC_FALSE;
}

EC_INT ec_do_system_cmd(EC_CHAR *cmd, EC_CHAR *param)
{
    system(cmd);
    return EC_SUCCESS;
}

EC_INT ec_do_system_cmd_2(EC_CHAR *cmd, EC_CHAR *retBuf, EC_INT retBufSize)
{
    FILE *fp = popen(cmd, "r");
    CHECK_NULL(fp, 0);
    fread(retBuf, retBufSize, 1, fp);
    dzlog_debug("cmd %s %s", cmd, retBuf);
    fclose(fp);

    return EC_SUCCESS;
    failed_0:
    return EC_FAILURE;
}


EC_VOID trun_off_dev(EC_VOID)
{

    dzlog_debug("trun off device success");
#if 0
    ec_reg_set(0x12098000, 0x32);
#else
    ec_do_system_cmd("poweroff", NULL);
#endif

    return;
}


EC_VOID reboot_dev(EC_VOID)
{
    ec_do_system_cmd("reboot", NULL);
}

static EC_VOID ec_wait(EC_UINT seconds, EC_ULLONG us)
{
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);

    if (fd == -1)
    {
        usleep(us);
        sleep(seconds);
        return;
    }
    uint64_t exp;
    struct itimerspec nv = {
            {0,       0},
            {seconds, us * 1000}
    };

    timerfd_settime(fd, 0, &nv, NULL);

    int ret = 0;
    while (1)
    {
        ret = read(fd, &exp, sizeof(uint64_t));
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            } else
            {
                //maybe not possible
            }
        }
        break;
    }

    close(fd);

    return;
}

EC_VOID cmh_wait_sec(EC_UINT seconds)
{
    ec_wait(seconds, 0);
}

EC_VOID cmh_wait_usec(EC_ULLONG usec)
{
    ec_wait(0, usec);
}

EC_INT ec_timer_create(EC_INT secs, EC_INT usecs)
{
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);


    uint64_t exp;
    struct itimerspec nv = {
            {0,    0},
            {secs, usecs * 1000}
    };

    timerfd_settime(fd, 0, &nv, NULL);

    return fd;
}

EC_INT ec_timer_update(EC_INT timer_fd, EC_INT secs, EC_INT usecs)
{
    uint64_t exp;
    struct itimerspec nv = {
            {0,    0},
            {secs, usecs * 1000}
    };

    timerfd_settime(timer_fd, 0, &nv, NULL);

    return EC_SUCCESS;
}

EC_INT ec_timer_update_us(EC_INT timer_fd, EC_INT secs, EC_INT usecs)
{
    uint64_t exp;
    struct itimerspec nv = {
            {0,    0},
            {secs, usecs * 1000}
    };

    timerfd_settime(timer_fd, 0, &nv, NULL);

    return EC_SUCCESS;
}


EC_VOID ec_timer_close(EC_INT timer_fd)
{
    close(timer_fd);
}


EC_VOID ec_ap_mode_set(EC_VOID)
{
    ec_do_system_cmd("echo  1 > /etc/apmod", NULL);
}

EC_VOID ec_version_write(EC_VOID)
{
    ec_do_system_cmd("rm -fr  /udisk/*.version", NULL);
    ec_do_system_cmd("touch /udisk/`cat /etc/fs_version`.version", NULL);
}

EC_BOOL ec_file_check(EC_CHAR *filename)
{
    if (access(filename, F_OK) == 0)
    {
        return EC_TRUE;
    } else
    {
        return EC_FALSE;
    }

}