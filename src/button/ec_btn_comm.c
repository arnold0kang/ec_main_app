//
// Created by arnold on 10/29/2018.
//

#include "ec_btn_comm.h"
#include "../alarm/ec_alarm.h"
#include "../stream_manager/ec_stream.h"
#include "../recoder_manager/ec_record.h"
#include "../log/ec_log.h"
#include "../disk_manager/ec_usb_disk.h"
#include "../sdk/ec_hisi_sdk.h"
#include "../event/ec_event.h"
#include "../eeprom/ec_eeprom.h"
#include "../utils/ec_utils.h"
#include "../dev_stat/ec_dev_stat.h"
#include "../beep/ec_beep.h"
#include "../disk_manager/ec_disk_monitor.h"


EC_VOID ec_btn_do_poweroff (EC_VOID)
{
    //stop alarm
    ec_alarm_off ();
    //stop live
    ec_stream_off ();
    //stop recorder
    ec_record_off ();
    //stop log
    ec_log_off ();
    //卸载U盘
    ec_usb_disk_umount();
    //deinit sdk
    ec_hisi_sdk_stop();
    //发送关机事件
    BROAD_CAST(EC_EVENT_POWER_OFF);

    ec_beep_do_once();
    trun_off_dev();
    return;
}



EC_VOID ec_btn_do_reboot (EC_VOID)
{
    //stop alarm
    ec_alarm_off ();
    //stop live
    ec_stream_off ();
    //stop recorder
    ec_record_off ();
    //stop log
    ec_log_off ();
    //卸载U盘
    ec_usb_disk_umount();
    //deinit sdk
    ec_hisi_sdk_stop();
    //发送关机事件
    BROAD_CAST(EC_EVENT_POWER_OFF);

    ec_beep_do_once();
    reboot_dev();
    return;
}



#define  CHECK_UPDATE_FILE   "/udisk/ec_update.swu"

EC_VOID ec_btn_do_uncharge (EC_VOID)
{
    EC_ERR_PTR;
    BROAD_CAST(EC_EVENT_CHARGE_OFF);
    //init sdk
    ec_hisi_sdk_start();
    //mount disk
    CHECK_FAILED(ec_usb_disk_mount (), 0);

    if (ec_file_check(CHECK_UPDATE_FILE))
    {
        dzlog_debug("new version find reboot  for update");
        ec_btn_do_reboot ();
        return;
    }

    //upate time
    ec_update_time_from_eeprom();
    //write current version
    ec_version_write ();
    //start log
    ec_log_on();
    //stop eeprom
    king_iic_stop ();
    //read comfig from iic
    if (king_iic_get_conf() ==EC_FAILURE)
    {
        ec_beep_do_once();
        reboot_dev();
    }
    //unload usb ko
    ec_usb_ko_unload();
    //start sdk
 //   ec_hisi_sdk_start();
    //start recorde
    ec_record_on();
    //开启磁盘检测
    ec_disk_monitor_start();

    return;
    failed_0:
    EC_ERR_SET("mount disk failed");
}

EC_VOID ec_btn_do_charge (EC_VOID)
{
    //stop recorde
    ec_record_off();
    //stop live
    ec_stream_off();
   //停止磁盘容量检测
   ec_disk_monitor_stop();
    //stop alarm
    ec_alarm_off();
    //stop log
    ec_log_off();
    //umout disk
    ec_usb_disk_umount();
    //load usb ko
    ec_usb_ko_load();
    //stop sdk
    ec_hisi_sdk_stop();
    //start eeprom
    king_iic_start();
    BROAD_CAST(EC_EVENT_CHARGE_ON);
    return;
}

EC_VOID ec_btn_do_live(EC_VOID)
{
    if (CHARGE_ON())
    {
        return;
    }
    if (ec_stream_is_run())
    {
        ec_stream_off();
    }
    else
    {
        ec_stream_on();
    }

    return;
}

EC_VOID ec_btn_do_apmode (EC_VOID)
{
    //stop alarm
    ec_alarm_off ();
    //stop live
    ec_stream_off ();
    //stop recorder
    ec_record_off ();
    //stop log
    ec_log_off ();
    //发送关机事件
    BROAD_CAST(EC_EVENT_POWER_OFF);
    ec_ap_mode_set();

    ec_beep_do_once();
    //重启
    reboot_dev ();
}