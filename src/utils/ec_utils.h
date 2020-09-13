//
// Created by arnold on 4/8/2018.
//

#ifndef EC_MAIN_APP_EC_UTILS_H
#define EC_MAIN_APP_EC_UTILS_H
#include <stdlib.h>
#include "../common/ec_define.h"
EC_BOOL  ec_is_ip_get (EC_VOID);

EC_INT ec_do_system_cmd (EC_CHAR *cmd, EC_CHAR *param);

EC_INT ec_do_system_cmd_2 (EC_CHAR *cmd, EC_CHAR *retBuf, EC_INT retBufSize);

static inline EC_LLONG  ec_get_missends_ts (EC_VOID)
{
    struct timeval tv = {0, 0};
    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 +tv.tv_usec / 10000;
}

#define SEC2MIS(X)    (X*1000)


EC_VOID trun_off_dev(EC_VOID);

EC_VOID reboot_dev (EC_VOID);


static inline EC_INT  ec_stat_set(EC_BOOL *stat)
{
    if (*stat == EC_TRUE)
    {
        return EC_FAILURE;
    }
    if(  __sync_bool_compare_and_swap(stat, EC_FALSE, EC_TRUE))
    {
        return EC_SUCCESS;
    }

    return EC_FAILURE;
}

static inline EC_INT ec_stat_unset(EC_BOOL *stat)
{
    if (*stat == EC_FALSE)
    {
        return EC_FAILURE;
    }
    if( __sync_bool_compare_and_swap(stat, EC_TRUE, EC_FALSE))
    {
        return EC_SUCCESS;
    }
    return EC_FAILURE;
}



static inline EC_VOID ec_atomic_add (EC_INT *num)
{
    __sync_fetch_and_add(num, 1);

    return;
}

static inline EC_VOID ec_atomic_sub (EC_INT *num)
{
    __sync_fetch_and_sub(num, 1);

    return ;
}

EC_VOID cmh_wait_sec (EC_UINT seconds);

EC_VOID cmh_wait_usec (EC_ULLONG usec);

EC_INT ec_timer_create(EC_INT secs, EC_INT usecs);


EC_INT ec_timer_update(EC_INT timer_fd, EC_INT secs, EC_INT usecs);

EC_VOID ec_timer_close(EC_INT timer_fd);

EC_VOID ec_ap_mode_set (EC_VOID);

EC_VOID ec_version_write (EC_VOID);

EC_BOOL ec_file_check (EC_CHAR  *filename);


#endif //EC_MAIN_APP_EC_UTILS_H
