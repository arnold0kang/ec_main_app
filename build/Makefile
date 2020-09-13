################################################################################
##@Copyright: 
##@Name: Makefile
##@Desp: the smart glass project's make file
##@Auther: arnold.kang
##@History:
##...........2014-11-07 23:20:51 "create this file"
################################################################################

PRJ_NAME = "smart glass project"
CROSS_COMPILE = arm-hisiv500-linux-

CC = $(CROSS_COMPILE)gcc
CPP=$(CROSS_COMPILE)gcc
strip = $(CROSS_COMPILE)strip

PRJ = enforceCam
PRJ_DIR = /root/enforce_cam/ec_main_app
BUILD_DIR = $(PRJ_DIR)/build
CODE_DIR = $(PRJ_DIR)/src
DOC_DIR = $(PRJ_DIR)/doc
LIB_DIR = $(PRJ_DIR)/lib
TEST_DIR = $(PRJ_DIR)/test

#the code and include file path
CODE_INC = $(CODE_DIR)
CODE_SRC = $(CODE_DIR)

#sdk include file and source file
SDK_DIR = $(LIB_DIR)/hisi_11
SDK_SRC = $(SDK_DIR)/src
SDK_INC = $(SDK_DIR)/inc
SDK_LIB = $(SDK_DIR)/lib

LIB_RTMP=$(LIB_DIR)/librtmp
RTMP_INC=$(LIB_RTMP)/inc
RTMP_LIB=$(LIB_RTMP)/lib

LIB_ZLOG=$(LIB_DIR)/zlog
ZLOG_INC=$(LIB_ZLOG)/inc
ZLOG_LIB=$(LIB_ZLOG)/lib

LIB_MP4=$(LIB_DIR)/libmp4
MP4_INC=$(LIB_MP4)/include
MP4_LIB=$(LIB_MP4)/lib

LIB_AAC=$(LIB_DIR)/libaac
AAC_INC=$(LIB_AAC)/inc
AAC_LIB=$(LIB_AAC)/lib

LIB_FFMPEG=$(LIB_DIR)/ffmpeg
FFMPEG_INC=$(LIB_FFMPEG)/include
FFMPEG_LIB=$(LIB_FFMPEG)/lib

LIB_SDL=$(LIB_DIR)/libsdl
SDL_INC=$(LIB_SDL)/include/SDL2
SDL_LIB=$(LIB_SDL)/lib

LIB_FRREETYPE=$(LIB_DIR)/freetype
FREETYPE_LIB=$(LIB_FRREETYPE)/lib

LIB_GHTTP=$(LIB_DIR)/libghttp
GHTTP_INC=$(LIB_GHTTP)/include
GHTTP_LIB=$(LIB_GHTTP)/lib

SRC_DIR = $(CODE_SRC) \
		  $(SDK_SRC)

INC_DIR = $(CODE_INC) \
		  $(SDK_INC)  \
		  $(RTMP_INC) \
		  $(ZLOG_INC)

INC_FLAG = -I$(CODE_INC) \
		   -I$(SDK_INC) \
		   -I$(SDK_SRC) \
		   -I$(RTMP_INC) \
		   -I$(ZLOG_INC) \
		   -I$(MP4_INC) \
		   -I$(AAC_INC) \
		   -I$(SDL_INC) \
		   -I$(GHTTP_INC) \
		   -I$(FFMPEG_INC)
	


BIN=enforceCam

HIS_LIB=$(foreach d,$(SDK_LIB),$(wildcard $(addprefix $(d)/*,.a)))

LIB=$(SDK_LIB)/libmpi.a \
	$(SDK_LIB)/libive.a \
	$(SDK_LIB)/libmd.a    \
	$(SDK_LIB)/lib_hiae.a \
	$(SDK_LIB)/libisp.a \
	$(SDK_LIB)/libsns_imx323.a  \
	$(SDK_LIB)/lib_hiawb.a \
	$(SDK_LIB)/lib_hiaf.a \
	$(SDK_LIB)/lib_hidefog.a \
	$(SDK_LIB)/libdnvqe.a  \
	$(SDK_LIB)/libupvqe.a \
	$(SDK_LIB)/libVoiceEngine.a \
	$(SDK_LIB)/libaacenc.a \
	$(SDK_LIB)/libaacdec.a \
	$(RTMP_LIB)/librtmp.a  \
	$(ZLOG_LIB)/libzlog.a  \
	$(AAC_LIB)/libfaac.a \
	$(SDL_LIB)/libSDL2.a \
	$(SDL_LIB)/libSDL2_ttf.a \
	$(FREETYPE_LIB)/libfreetype.a \
        $(GHTTP_LIB)/libghttp.a \
        $(FFMPEG_LIB)/libavformat.a \
        $(FFMPEG_LIB)/libavfilter.a \
	$(FFMPEG_LIB)/libavcodec.a \
	$(FFMPEG_LIB)/libavdevice.a \
	$(FFMPEG_LIB)/libswscale.a \
	$(FFMPEG_LIB)/libavutil.a \
	$(FFMPEG_LIB)/libswresample.a \



CFLAG= -Wall  -lm -lpthread -ldl -lstdc++ -Dhi3516cv300 -DHICHIP=0x3516C300 -DSENSOR_TYPE=SONY_IMX323_CMOS_1080P_30FPS -lmp4v2 -L/root/enforce_cam/ec_main_app/lib/libmp4/lib -lwpa_client -L/root/enforce_cam/ec_main_app/lib/wpa_client/lib -DHI_ACODEC_TYPE_INNER -mno-unaligned-access -fno-aggressive-loop-optimizations -ffunction-sections -fdata-sections -mcpu=arm926ej-s
SRC=$(foreach d,$(SRC_DIR),$(wildcard $(addprefix $(d)/*,.c)))
INC=$(foreach d,$(INC_DIR),$(wildcard $(addprefix $(d)/*,.h)))
OBJ=$(SRC:%.c=%.o)


all: ${BIN}

install:
	mv $(BIN)  /home/nfs/enforceCam/


${BIN}: ${OBJ} ${INC}
	${CPP} $(INC_FLAG) -o $@ ${OBJ} ${CFLAG} ${LIB}
%.o: %.c $(INC)
	${CC} $(INC_FLAG) -c ${CFLAG} $< -o $@

clean:
	    rm -f ${BIN} ${OBJ}
