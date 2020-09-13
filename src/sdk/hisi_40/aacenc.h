/*****************************************************************************
*             Copyright 2004 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_spdif.h
* Description:
*
* History:
* Version   Date         Author     DefectNum    Description
* 0.01      2006-11-01   z40717    NULL         Create this file.
*
*****************************************************************************/
/**
 * \file
 * \brief Describes the information about AACENC.
 */

#ifndef _AACENC_H
#define _AACENC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif     /* __cpluscplus */
#endif  /* __cpluscplus */

#include "hi_type.h"

/********************************Macro Definition********************************/
/** \addtogroup      AACENC */
/** @{ */  /** <!-- [AACENC] */

#ifdef MONO_ONLY
#define MAX_CHANNELS 1 /**<aacenc encoder channels*/
#else
#define MAX_CHANNELS 2
#endif

#define AACENC_BLOCKSIZE 1024 /**<aacenc blocksize*/
/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      AACENC */
/** @{ */  /** <!-- [AACENC] */

/**Defines AACENC quality*/
typedef enum
{
    AU_QualityExcellent = 0,
    AU_QualityHigh   = 1,
    AU_QualityMedium = 2,
    AU_QualityLow = 3,
} AuQuality;

/**Defines AACENC format*/
typedef enum
{
    AACLC = 0,              /**<AAC-LC format*/
    EAAC = 1,               /**<HEAAC or AAC+  or aacPlusV1*/
    EAACPLUS = 2,           /**<AAC++ or aacPlusV2*/
    AACLD = 3,              /**<AAC LD(Low Delay)*/
    AACELD = 4,             /**<AAC ELD(Low Delay)*/
} AuEncoderFormat;

/**Defines AACENC container*/
typedef enum
{
    AACENC_ADTS = 0,
    AACENC_LOAS = 1,
    AACENC_LATM_MCP1 = 2,
} AACENCTransportType;

/**Defines AACENC configuration*/
typedef  struct
{
    AuQuality       quality;
    AuEncoderFormat coderFormat;
    HI_S16          bitsPerSample;
    HI_S32          sampleRate;    /**<audio file sample rate */
    HI_S32          bitRate;       /**<encoder bit rate in bits/sec */
    HI_S16          nChannelsIn;   /**<number of channels on input (1,2) */
    HI_S16          nChannelsOut;  /**<number of channels on output (1,2) */
    HI_S16          bandWidth;     /**<targeted audio bandwidth in Hz */
    AACENCTransportType transtype;
} AACENC_CONFIG;

typedef struct AAC_ENCODER AAC_ENCODER_S;

/**Defines AACENC version*/
typedef struct hiAACENC_VERSION_S
{
    HI_U8 aVersion[64];
} AACENC_VERSION_S;

/** @} */  /** <!-- ==== Structure Definition End ==== */

/******************************* API declaration *****************************/
/** \addtogroup      AACENC */
/** @{ */  /** <!--  [AACENC] */

/**
\brief Get version information.
\attention \n
N/A
\param[in] pVersion       version describe struct
\retval ::HI_SUCCESS   : Success
\retval ::HI_FAILURE          : FAILURE
\see \n
N/A
*/
HI_S32  HI_AACENC_GetVersion(AACENC_VERSION_S* pVersion);

/**
\brief get reasonable default configuration.
\attention \n
N/A
\param[in] pstConfig    pointer to an configuration information structure
\retval ::HI_SUCCESS   : Success
\retval ::HI_FAILURE          : FAILURE
\see \n
N/A
*/
HI_S32  AACInitDefaultConfig(AACENC_CONFIG* pstConfig);

/**
\brief allocate and initialize a new encoder instance.
\attention \n
N/A
\param[in] phAacPlusEnc    pointer to an configuration information structure
\param[in] pstConfig    pointer to an configuration information structure
\retval ::HI_SUCCESS   : Success
\retval ::HI_FAILURE   : FAILURE
\see \n
N/A
*/
HI_S32  AACEncoderOpen(AAC_ENCODER_S** phAacPlusEnc, AACENC_CONFIG* pstConfig);

/**
\brief allocate and initialize a new encoder instance
\attention \n
N/A
\param[in] hAacPlusEnc    pointer to an configuration information structure
\param[in] ps16PcmBuf    BLOCKSIZE*nChannels audio samples,interleaved
\param[in] pu8Outbuf    pointer to output buffer,(must be 6144/8*MAX_CHANNELS bytes large)
\param[in] ps32NumOutBytes    number of bytes in output buffer after processing
\retval ::HI_SUCCESS   : Success
\retval ::HI_FAILURE   : FAILURE
\see \n
N/A
*/
HI_S32  AACEncoderFrame(AAC_ENCODER_S* hAacPlusEnc, HI_S16* ps16PcmBuf,
                        HI_U8* pu8Outbuf, HI_S32* ps32NumOutBytes);

/**
\brief close encoder device.
\attention \n
N/A
\param[in] hAacPlusEnc    pointer to an configuration information structure
\retval N/A
\see \n
N/A
*/
HI_VOID AACEncoderClose (AAC_ENCODER_S* hAacPlusEnc);
/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif     /* __cpluscplus */
#endif  /* __cpluscplus */

#endif  /* _AACENC_H */
