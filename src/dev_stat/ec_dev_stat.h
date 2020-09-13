//
// Created by arnold on 3/22/2018.
//

#ifndef EC_MAIN_APP_EC_DEV_STAT_H
#define EC_MAIN_APP_EC_DEV_STAT_H

#include "ec_dev_state_define.h"
#include "../log/ec_log.h"



#define WIFI_ON() \
    ec_dev_wifi_is_run()
#define DISK_FULL() \
    ec_dev_disk_is_full()
#define RECODER_RUN() \
    ec_dev_recoder_is_run()
#define CHARGE_ON() \
    ec_dev_charge_is_on()
#define WIFI_CONNECT() \
    ec_dev_wifi_is_connect()
#define STREAM_ON() \
    ec_dev_stream_is_on()

#define DISK_READY() \
    ec_dev_disk_ready ()

#define DISK_INUSE() \
    ec_disk_is_inuse ()

#define USB_INUSE() \
    ec_usb_is_inuse ()


EC_BOOL ec_dev_wifi_is_run (EC_VOID);

EC_BOOL ec_dev_disk_is_full (EC_VOID);

EC_BOOL ec_dev_recoder_is_run (EC_VOID);

EC_BOOL ec_dev_charge_is_on (EC_VOID);

EC_BOOL ec_dev_stream_is_on (EC_VOID);

EC_BOOL ec_dev_wifi_is_connect (EC_VOID);

EC_BOOL ec_dev_disk_ready (EC_VOID);



EC_VOID ec_disk_ref_add (EC_VOID);
EC_VOID ec_disk_ref_del (EC_VOID);
EC_BOOL ec_disk_is_inuse (EC_VOID);

EC_VOID ec_usb_ref_add (EC_VOID);
EC_VOID ec_usb_ref_del (EC_VOID);
EC_BOOL ec_usb_is_inuse (EC_VOID);




#endif //EC_MAIN_APP_EC_DEV_STAT_H
