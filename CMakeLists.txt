cmake_minimum_required(VERSION 3.8)
project(ec_main_app)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_SYSTEM_NAME  Linux)
option(CROSS_COMPILE  "cross compile"  OFF)
option(IS_RELEASE     "build for release" OFF)





if(CROSS_COMPILE)
    set(CMAKE_C_COMPILER "arm-hisiv500-linux-gcc")
    set (CMAKE_EXE_LINKER_FLAGS "-pthread  -lstdc++ -ldl  -lm -mno-unaligned-access -fno-aggressive-loop-optimizations")
    set(CMAKE_C_FLAGS "-mno-unaligned-access -fno-aggressive-loop-optimizations -ffunction-sections -fdata-sections -mcpu=arm926ej-s ")

    ADD_DEFINITIONS(-DSENSOR_TYPE=SONY_IMX323_CMOS_1080P_30FPS  -Dhi3516cv300 -DHICHIP=0x3516C300 -DHI_ACODEC_TYPE_INNER)

    include_directories(/home/arnold/enforce_cam/ec_3th/freetype/_install/include
            /home/arnold/enforce_cam/ec_3th/libghttp/_install/include
            /home/arnold/enforce_cam/ec_3th/libmp4v2/install/include
            /home/arnold/enforce_cam/ec_3th/librtmp/_install/include
            /home/arnold/enforce_cam/ec_3th/libsdl/_install/include/SDL2
            /home/arnold/enforce_cam/ec_3th/libuv1.23.2/_install/include
            /home/arnold/enforce_cam/ec_3th/sdl2_ttf/_install/include
            /home/arnold/enforce_cam/ec_3th/zlib/_install/include
            /home/arnold/enforce_cam/ec_3th/zlog/_install/include
            /home/arnold/enforce_cam/ec_3th/libcurl/_install/include
            /home/arnold/enforce_cam/hisi_lib/050/inc
            /home/arnold/enforce_cam/hisi_lib/050/tool_incl
            )
else()
    include_directories(lib/zlog/inc lib/freetype/include lib/hisi_50/inc
            lib/lib_ghttp/include lib/libmp4/include lib/librtmp/inc lib/libsdl/include/
            lib/libsdl_ttf/include/SDL2  lib/libaac/inc lib/sqlite/include
            lib/libuv/include
            lib/libcurl/include)
    ADD_DEFINITIONS(-DSENSOR_TYPE=SONY_IMX323_CMOS_1080P_30FPS  -Dhi3516cv300 -DHICHIP=0x3516C300 -DHI_ACODEC_TYPE_INNER)
endif()

if(IS_RELEASE)
    ADD_DEFINITIONS(-DBUILD_TYPE=release)
    SET(BTSTR "release")
else()
    ADD_DEFINITIONS(-DBUILD_TYPE=debug)
    SET(BTSTR "debug")
endif()
STRING(TIMESTAMP TODAY "%Y%m%d%H%M%S")
ADD_DEFINITIONS(-DBUILD_TIME=${TODAY})


SET(MAJOR_VERSION 1)
SET(MINOR_VERSION 19)


SET(OUT_BIN ec_main_app-${BTSTR}-${MAJOR_VERSION}.${MINOR_VERSION}-${TODAY})



add_executable(${OUT_BIN}
        src/3th_src/cJSON.c
        src/3th_src/cJSON.h
        src/alarm/ec_alarm.c
        src/alarm/ec_alarm.h
        src/conf/ec_conf.c
        src/conf/ec_conf.h
        src/common/ec_define.h
        src/sdk/ec_hisi_sdk.c
        src/sdk/ec_hisi_sdk.h
        src/eeprom/ec_eeprom.c
        src/eeprom/ec_eeprom.h
        src/i2c/ec_iic_priv.c
        src/i2c/ec_iic_priv.h
        src/led/ec_led.c
        src/led/ec_led.h
        src/log/ec_log.c
        src/log/ec_log.h
        src/power_manager/battery/ec_lsadc.c
        src/power_manager/battery/ec_lsadc.h
        src/king_main.c
        src/sdk/ec_osd.c
        src/sdk/ec_osd.h
        src/recoder_manager/ec_record.c
        src/recoder_manager/ec_record.h
        src/rtc/ec_rtc.c
        src/rtc/ec_rtc.h
        src/rtc/hi_rtc.h
        src/snap/ec_snap.c
        src/snap/ec_snap.h
        src/stream_manager/ec_stream_worker.c
        src/stream_manager/ec_stream_worker.h
        src/wifi/ec_wifi.c
        src/wifi/ec_wifi.h
        src/3th_src/rs232.c
        src/3th_src/rs232.h
        src/stream_manager/srs_librtmp.h src/conf/ec_version.h
        src/module_manager/ec_module.c
        src/module_manager/ec_module.h
        src/libuv_wraper/ec_libuv.c
        src/event/ec_event.c
        src/event/ec_event.h
        src/libuv_wraper/ec_libuv.h
        src/power_manager/ec_power.c src/power_manager/ec_power.h
        src/signal/ec_signal.c src/signal/ec_signal.h src/event/ec_event_define.h
        src/dev_stat/ec_dev_stat.c src/dev_stat/ec_dev_stat.h src/dev_stat/ec_dev_state_define.h
        src/disk_manager/ec_disk_monitor.c src/disk_manager/ec_disk_monitor.h
        src/recoder_manager/ec_recoder_rotator.c src/recoder_manager/ec_recoder_rotator.h
        src/recoder_manager/ec_recode_worker.c src/recoder_manager/ec_recode_worker.h
        src/stream_manager/ec_stream_test.c src/stream_manager/ec_stream.c
        src/stream_manager/ec_stream.h
        src/eeprom/ec_eeprom_test.c src/utils/ec_utils.c src/utils/ec_utils.h
        src/pwm/hi_pwm.c src/pwm/hi_pwm.h src/beep/ec_beep.c
        src/beep/ec_beep.h src/auth/ec_auth.c src/auth/ec_auth.h
        src/sdk/ec_hisi_sdk_conf.h src/disk_manager/ec_usb_disk.c
        src/disk_manager/ec_usb_disk.h src/utils/ec_ko.c src/utils/ec_ko.h
        src/utils/ec_reg.c src/utils/ec_reg.h
        src/sdk/ec_hisi_sdk_live.c src/sdk/ec_hisi_sdk_live.h
        src/sdk/ec_hisi_sdk_recorder.c src/sdk/ec_hisi_sdk_recorder.h src/sdk/ec_hisi_sdk_snap.c
        src/sdk/ec_hisi_sdk_snap.h
        src/sdk/hisi_50/loadbmp.c
        src/sdk/hisi_50/loadbmp.h
        src/sdk/hisi_50/sample_comm.h
        src/sdk/hisi_50/sample_comm_audio.c
        src/sdk/hisi_50/sample_comm_isp.c
        src/sdk/hisi_50/sample_comm_ive.c
        src/sdk/hisi_50/sample_comm_ive.h
        src/sdk/hisi_50/sample_comm_sys.c
        src/sdk/hisi_50/sample_comm_venc.c
        src/sdk/hisi_50/sample_comm_vi.c
        src/sdk/hisi_50/sample_comm_vo.c
        src/sdk/hisi_50/sample_comm_vpss.c
        src/sdk/hisi_50/memmap.c
        src/sdk/hisi_50/memmap.h
        src/sdk/hisi_50/aacenc.h
        src/sdk/hisi_50/audio_aac_adp.c
        src/sdk/hisi_50/audio_aac_adp.h
        src/gpio/ec_gpio.c
        src/gpio/ec_gpio.h
        src/button/ec_btn_wifi.c
        src/button/ec_btn_wifi.h
        src/button/ec_btn_alarm.c
        src/button/ec_btn_alarm.h
        src/button/ec_btn_snap.c
        src/button/ec_btn_snap.h
        src/button/ec_btn_charge.c
        src/button/ec_btn_charge.h
        src/button/ec_btn_comm.c
        src/button/ec_btn_comm.h
        src/recoder_manager/ec_record_mp4.c
        src/recoder_manager/ec_record_mp4.h
        src/led/ec_led_alg.h
        src/button/ec_btn_conf.h
        src/recoder_manager/ec_recorde_alarm.c
        src/recoder_manager/ec_recorde_alarm.h)


target_link_libraries(${OUT_BIN}
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/librtmp.a"
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/libzlog.a"
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/libSDL2.a"
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/libSDL2_ttf.a"
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/libfreetype.a"
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/libghttp.a"
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/libuv.a"
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/libmp4v2.a"
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/libz.a"
        "/home/arnold/enforce_cam/ec_3th/_install_ulibc/lib/libcurl.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libmpi.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libive.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libmd.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/lib_hiae.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libisp.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libsns_imx323.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/lib_hiawb.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/lib_hiaf.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/lib_hidefog.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libdnvqe.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libupvqe.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libVoiceEngine.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libaacenc.a"
        "/home/arnold/enforce_cam/hisi_lib/050/lib/libaacdec.a"
)





