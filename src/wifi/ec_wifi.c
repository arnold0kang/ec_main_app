#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <netinet/tcp.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/if.h>
#include "../log/ec_log.h"
#include "ec_wifi.h"
#include "../event/ec_event.h"

#include "../libuv_wraper/ec_libuv.h"
#include "../module_manager/ec_module.h"
#include "../utils/ec_utils.h"

static EC_VOID do_wifi_on (EC_VOID);
static EC_VOID do_wifi_off (EC_VOID);

static EC_INT wifi_ref = 0;
static EC_BOOL is_wifi_on = EC_FALSE;

EC_INT ec_wifi_on (EC_VOID)
{
    ec_atomic_add(&wifi_ref);
    if (is_wifi_on)
    {
        return EC_SUCCESS;
    }
    else
    {
        do_wifi_on();
    }
    dzlog_debug("current wifi ref %d", wifi_ref);
    return EC_SUCCESS;
}
EC_INT ec_wifi_off (EC_VOID)
{
    ec_atomic_sub(&wifi_ref);
    if (wifi_ref < 0)
    {
        dzlog_error("wifi ref  < 0 %d", wifi_ref);
    }
    if (wifi_ref == 0)
    {
        do_wifi_off();
        ec_stat_unset(&is_wifi_on);
    }
    dzlog_debug("current wifi ref %d", wifi_ref);

    return EC_SUCCESS;
}

EC_VOID ec_wifi_restart (EC_VOID)
{
    do_wifi_off();
    do_wifi_on();
}

EC_BOOL ec_wifi_is_on (EC_VOID)
{
    int fd;
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

    dzlog_debug("wlan0 addr %s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    close(fd);

    /* display result */
    //strcpy(buf, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    return EC_TRUE;
    failed_1:
    close(fd);
    return  EC_FALSE;

}

#define WIFI_ON_CMD     "sv up /home/ec_service/wifi"
#define WIFI_OFF_CMD     "sv down /home/ec_service/wifi"
#define DHCP_ON_CMD     "sv up /home/ec_service/dhcp"
#define DHCP_OFF_CMD     "sv down /home/ec_service/dhcp"
static EC_VOID do_wifi_on (EC_VOID)
{
    if (ec_stat_set(&is_wifi_on) == EC_FAILURE)
    {
        return;
    }
    BROAD_CAST(EC_EVENT_WIFI_ON);
    ec_do_system_cmd(WIFI_ON_CMD, NULL);
    ec_do_system_cmd(DHCP_ON_CMD, NULL);

    return;
}
static EC_VOID do_wifi_off (EC_VOID)
{
    BROAD_CAST(EC_EVENT_WIFI_OFF);
    ec_do_system_cmd(DHCP_OFF_CMD, NULL);
    ec_do_system_cmd(WIFI_OFF_CMD, NULL);

    ec_stat_unset(&is_wifi_on);

    return;
}