//
// Created by arnold on 4/19/2018.
//

#ifndef EC_MAIN_APP_EC_USB_DISK_H
#define EC_MAIN_APP_EC_USB_DISK_H

#include "../common/ec_define.h"


EC_INT ec_usb_disk_mount (EC_VOID);

EC_INT ec_usb_disk_umount (EC_VOID);

EC_BOOL is_mount (EC_VOID);

EC_INT ec_usb_ko_load (EC_VOID);

EC_INT ec_usb_ko_unload (EC_VOID);



#endif //EC_MAIN_APP_EC_USB_DISK_H
