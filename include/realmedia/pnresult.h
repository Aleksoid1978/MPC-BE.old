/****************************************************************************
 *
 *
 *  Copyright (C) 1995-1999 RealNetworks, Inc. All rights reserved.
 *
 *  http://www.real.com/devzone
 *
 *  This program contains proprietary
 *  information of Progressive Networks, Inc, and is licensed
 *  subject to restrictions on use and distribution.
 *
 *  This file contains the PN_RESULT type and it's associated result codes
 */

#ifndef _PNRESULT_H_
#define _PNRESULT_H_

/* Some files include this before pntypes.h. */
#include "pntypes.h"

typedef LONG32	PN_RESULT;

#ifndef _WIN32
    typedef PN_RESULT HRESULT;
#	define NOERROR 0
#   define FACILITY_ITF 4
#   define MAKE_HRESULT(sev,fac,code)						\
	((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) |   \
	((unsigned long)(code))) )
#   define SUCCEEDED(Status) (((unsigned long)(Status)>>31) == 0)
#   define FAILED(Status) (((unsigned long)(Status)>>31) != 0)
#else
#   ifndef _HRESULT_DEFINED
	typedef LONG32 HRESULT;
#   endif	/* _HRESULT_DEFINED */
#   include <winerror.h>
#endif /* _WIN32 */

#define MAKE_PN_RESULT(sev,fac,code) MAKE_HRESULT(sev, FACILITY_ITF,	    \
    ((fac << 6) | (code)))

#define SS_GLO 0  /* General errors 				*/
#define SS_NET 1  /* Networking errors				*/
#define SS_FIL 2  /* File errors				*/
#define SS_PRT 3  /* Protocol Error				*/
#define SS_AUD 4  /* Audio error				*/
#define SS_INT 5  /* General internal errors			*/
#define SS_USR 6  /* The user is broken.			*/
#define SS_MSC 7  /* Miscellaneous				*/
#define SS_DEC 8  /* Decoder errors				*/
#define SS_ENC 9  /* Encoder errors				*/
#define SS_REG 10 /* Registry (not Windows registry ;) errors	*/
#define SS_PPV 11 /* Pay Per View errors			*/
#define SS_RSC 12 /* Errors for PNXRES */
#define SS_UPG 13 /* Auto-upgrade & Certificate Errors          */
#define SS_PLY 14 /* RealPlayer/Plus specific errors (USE ONLY IN /rpmisc/pub/rpresult.h) */
#define SS_RMT 15 /* RMTools Errors				*/
#define SS_CFG 16 /* AutoConfig Errors				*/
#define SS_RPX 17 /* RealPix-related Errors */
#define SS_XML 18 /* XML-related Errors				*/

#define SS_DPR 63 /* Deprecated errors				*/

#define PNR_NOTIMPL                     MAKE_HRESULT(1,0,0x4001)
#define PNR_OUTOFMEMORY			MAKE_HRESULT(1,7,0x000e)
#define PNR_INVALID_PARAMETER		MAKE_HRESULT(1,7,0x0057)
#define PNR_NOINTERFACE                 MAKE_HRESULT(1,0,0x4002)
#define PNR_POINTER                     MAKE_HRESULT(1,0,0x4003)
#define PNR_HANDLE                      MAKE_HRESULT(1,7,0x0006)
#define PNR_ABORT                       MAKE_HRESULT(1,0,0x4004)
#define PNR_FAIL                        MAKE_HRESULT(1,0,0x4005)
#define PNR_ACCESSDENIED                MAKE_HRESULT(1,7,0x0005)
#define PNR_IGNORE			MAKE_HRESULT(1,0,0x0006)
#define PNR_OK				MAKE_HRESULT(0,0,0)


#define PNR_INVALID_OPERATION		MAKE_PN_RESULT(1,SS_GLO,4)
#define PNR_INVALID_VERSION		MAKE_PN_RESULT(1,SS_GLO,5)
#define PNR_INVALID_REVISION		MAKE_PN_RESULT(1,SS_GLO,6)
#define PNR_NOT_INITIALIZED		MAKE_PN_RESULT(1,SS_GLO,7)
#define PNR_DOC_MISSING			MAKE_PN_RESULT(1,SS_GLO,8)
#define PNR_UNEXPECTED                  MAKE_PN_RESULT(1,SS_GLO,9)
#define PNR_INCOMPLETE			MAKE_PN_RESULT(1,SS_GLO,12)
#define PNR_BUFFERTOOSMALL		MAKE_PN_RESULT(1,SS_GLO,13)
#define PNR_UNSUPPORTED_VIDEO		MAKE_PN_RESULT(1,SS_GLO,14)
#define PNR_UNSUPPORTED_AUDIO		MAKE_PN_RESULT(1,SS_GLO,15)
#define PNR_INVALID_BANDWIDTH		MAKE_PN_RESULT(1,SS_GLO,16)
/* PNR_NO_RENDERER and PNR_NO_FILEFORMAT old value is being deprecated 
#define PNR_NO_FILEFORMAT		MAKE_PN_RESULT(1,SS_GLO,10)
#define PNR_NO_RENDERER			MAKE_PN_RESULT(1,SS_GLO,11)*/
#define PNR_NO_RENDERER			MAKE_PN_RESULT(1,SS_GLO,17)
#define PNR_NO_FILEFORMAT		MAKE_PN_RESULT(1,SS_GLO,17)
#define PNR_MISSING_COMPONENTS		MAKE_PN_RESULT(1,SS_GLO,17)
#define PNR_ELEMENT_NOT_FOUND		MAKE_PN_RESULT(0,SS_GLO,18)
#define PNR_NOCLASS			MAKE_PN_RESULT(0,SS_GLO,19)
#define PNR_CLASS_NOAGGREGATION		MAKE_PN_RESULT(0,SS_GLO,20)
#define PNR_NOT_LICENSED		MAKE_PN_RESULT(1,SS_GLO,21)
#define PNR_NO_FILESYSTEM		MAKE_PN_RESULT(1,SS_GLO,22)
#define PNR_REQUEST_UPGRADE		MAKE_PN_RESULT(1,SS_GLO,23)
#define PNR_AWAITING_LICENSE		MAKE_PN_RESULT(1,SS_GLO,24)

#define PNR_BUFFERING			MAKE_PN_RESULT(0,SS_NET,0)
#define PNR_PAUSED			MAKE_PN_RESULT(0,SS_NET,1)
#define PNR_NO_DATA			MAKE_PN_RESULT(0,SS_NET,2)
#define PNR_STREAM_DONE			MAKE_PN_RESULT(0,SS_NET,3)
#define PNR_NET_SOCKET_INVALID		MAKE_PN_RESULT(1,SS_NET,3)
#define PNR_NET_CONNECT			MAKE_PN_RESULT(1,SS_NET,4)
#define PNR_BIND			MAKE_PN_RESULT(1,SS_NET,5)
#define PNR_SOCKET_CREATE		MAKE_PN_RESULT(1,SS_NET,6)
#define PNR_INVALID_HOST		MAKE_PN_RESULT(1,SS_NET,7)
#define PNR_NET_READ			MAKE_PN_RESULT(1,SS_NET,8)
#define PNR_NET_WRITE			MAKE_PN_RESULT(1,SS_NET,9)
#define PNR_NET_UDP			MAKE_PN_RESULT(1,SS_NET,10)
#define PNR_RETRY			MAKE_PN_RESULT(1,SS_NET,11) /* XXX */
#define PNR_SERVER_TIMEOUT		MAKE_PN_RESULT(1,SS_NET,12)
#define PNR_SERVER_DISCONNECTED		MAKE_PN_RESULT(1,SS_NET,13)
#define PNR_WOULD_BLOCK			MAKE_PN_RESULT(1,SS_NET,14)
#define PNR_GENERAL_NONET		MAKE_PN_RESULT(1,SS_NET,15)
#define PNR_BLOCK_CANCELED		MAKE_PN_RESULT(1,SS_NET,16) /* XXX */
#define PNR_MULTICAST_JOIN		MAKE_PN_RESULT(1,SS_NET,17)
#define PNR_GENERAL_MULTICAST		MAKE_PN_RESULT(1,SS_NET,18)
#define PNR_MULTICAST_UDP		MAKE_PN_RESULT(1,SS_NET,19)
#define PNR_AT_INTERRUPT                MAKE_PN_RESULT(1,SS_NET,20)
#define PNR_MSG_TOOLARGE		MAKE_PN_RESULT(1,SS_NET,21)
#define PNR_NET_TCP			MAKE_PN_RESULT(1,SS_NET,22)
#define PNR_TRY_AUTOCONFIG		MAKE_PN_RESULT(1,SS_NET,23)
#define PNR_NOTENOUGH_BANDWIDTH		MAKE_PN_RESULT(1,SS_NET,24)
#define PNR_HTTP_CONNECT		MAKE_PN_RESULT(1,SS_NET,25)
#define PNR_PORT_IN_USE			MAKE_PN_RESULT(1,SS_NET,26)
#define PNR_LOADTEST_NOT_SUPPORTED	MAKE_PN_RESULT(1,SS_NET,27)

#define PNR_AT_END			MAKE_PN_RESULT(0,SS_FIL,0)
#define PNR_INVALID_FILE		MAKE_PN_RESULT(1,SS_FIL,1)
#define PNR_INVALID_PATH		MAKE_PN_RESULT(1,SS_FIL,2)
#define PNR_RECORD			MAKE_PN_RESULT(1,SS_FIL,3)
#define PNR_RECORD_WRITE		MAKE_PN_RESULT(1,SS_FIL,4)
#define PNR_TEMP_FILE			MAKE_PN_RESULT(1,SS_FIL,5)
#define PNR_ALREADY_OPEN                MAKE_PN_RESULT(1,SS_FIL,6)
#define PNR_SEEK_PENDING                MAKE_PN_RESULT(1,SS_FIL,7)
#define PNR_CANCELLED                   MAKE_PN_RESULT(1,SS_FIL,8)
#define PNR_FILE_NOT_FOUND              MAKE_PN_RESULT(1,SS_FIL,9)
#define PNR_WRITE_ERROR                 MAKE_PN_RESULT(1,SS_FIL,10)
#define PNR_FILE_EXISTS                 MAKE_PN_RESULT(1,SS_FIL,11)
#define	PNR_FILE_NOT_OPEN		MAKE_PN_RESULT(1,SS_FIL,12)
#define PNR_ADVISE_PREFER_LINEAR	MAKE_PN_RESULT(0,SS_FIL,13)
#define PNR_PARSE_ERROR                 MAKE_PN_RESULT(1,SS_FIL,14)

#define PNR_BAD_SERVER			MAKE_PN_RESULT(1,SS_PRT,0)
#define PNR_ADVANCED_SERVER		MAKE_PN_RESULT(1,SS_PRT,1)
#define PNR_OLD_SERVER			MAKE_PN_RESULT(1,SS_PRT,2)
#define PNR_REDIRECTION			MAKE_PN_RESULT(0,SS_PRT,3) /* XXX */
#define PNR_SERVER_ALERT		MAKE_PN_RESULT(1,SS_PRT,4)
#define PNR_PROXY			MAKE_PN_RESULT(1,SS_PRT,5)
#define PNR_PROXY_RESPONSE		MAKE_PN_RESULT(1,SS_PRT,6)
#define PNR_ADVANCED_PROXY		MAKE_PN_RESULT(1,SS_PRT,7)
#define PNR_OLD_PROXY			MAKE_PN_RESULT(1,SS_PRT,8)
#define PNR_INVALID_PROTOCOL		MAKE_PN_RESULT(1,SS_PRT,9)
#define PNR_INVALID_URL_OPTION		MAKE_PN_RESULT(1,SS_PRT,10)
#define PNR_INVALID_URL_HOST		MAKE_PN_RESULT(1,SS_PRT,11)
#define PNR_INVALID_URL_PATH		MAKE_PN_RESULT(1,SS_PRT,12)
#define PNR_HTTP_CONTENT_NOT_FOUND      MAKE_PN_RESULT(1,SS_PRT,13)
#define PNR_NOT_AUTHORIZED              MAKE_PN_RESULT(1,SS_PRT,14)
#define PNR_UNEXPECTED_MSG              MAKE_PN_RESULT(1,SS_PRT,15)
#define PNR_BAD_TRANSPORT               MAKE_PN_RESULT(1,SS_PRT,16)
#define PNR_NO_SESSION_ID               MAKE_PN_RESULT(1,SS_PRT,17)
#define PNR_PROXY_DNR			MAKE_PN_RESULT(1,SS_PRT,18)
#define PNR_PROXY_NET_CONNECT		MAKE_PN_RESULT(1,SS_PRT,19)

#define PNR_AUDIO_DRIVER		MAKE_PN_RESULT(1,SS_AUD,0)
#define PNR_LATE_PACKET			MAKE_PN_RESULT(1,SS_AUD,1)
#define PNR_OVERLAPPED_PACKET		MAKE_PN_RESULT(1,SS_AUD,2)
#define PNR_OUTOFORDER_PACKET		MAKE_PN_RESULT(1,SS_AUD,3)
#define PNR_NONCONTIGUOUS_PACKET	MAKE_PN_RESULT(1,SS_AUD,4)

#define PNR_OPEN_NOT_PROCESSED		MAKE_PN_RESULT(1,SS_INT,0)

#define PNR_EXPIRED			MAKE_PN_RESULT(1,SS_USR,0)

#define PNR_INVALID_INTERLEAVER		MAKE_PN_RESULT(1,SS_DPR,0)
#define PNR_BAD_FORMAT			MAKE_PN_RESULT(1,SS_DPR,1)
#define PNR_CHUNK_MISSING		MAKE_PN_RESULT(1,SS_DPR,2)
#define PNR_INVALID_STREAM              MAKE_PN_RESULT(1,SS_DPR,3)
#define PNR_DNR                         MAKE_PN_RESULT(1,SS_DPR,4)
#define PNR_OPEN_DRIVER                 MAKE_PN_RESULT(1,SS_DPR,5)
#define PNR_UPGRADE                     MAKE_PN_RESULT(1,SS_DPR,6)
#define PNR_NOTIFICATION                MAKE_PN_RESULT(1,SS_DPR,7)
#define PNR_NOT_NOTIFIED                MAKE_PN_RESULT(1,SS_DPR,8)
#define PNR_STOPPED                     MAKE_PN_RESULT(1,SS_DPR,9)
#define PNR_CLOSED                      MAKE_PN_RESULT(1,SS_DPR,10)
#define PNR_INVALID_WAV_FILE            MAKE_PN_RESULT(1,SS_DPR,11)
#define PNR_NO_SEEK                     MAKE_PN_RESULT(1,SS_DPR,12)

#define PNR_DEC_INITED			MAKE_PN_RESULT(1,SS_DEC,0)
#define PNR_DEC_NOT_FOUND		MAKE_PN_RESULT(1,SS_DEC,1)
#define PNR_DEC_INVALID			MAKE_PN_RESULT(1,SS_DEC,2)
#define PNR_DEC_TYPE_MISMATCH		MAKE_PN_RESULT(1,SS_DEC,3)
#define PNR_DEC_INIT_FAILED		MAKE_PN_RESULT(1,SS_DEC,4)
#define PNR_DEC_NOT_INITED		MAKE_PN_RESULT(1,SS_DEC,5)
#define PNR_DEC_DECOMPRESS		MAKE_PN_RESULT(1,SS_DEC,6)
#define PNR_OBSOLETE_VERSION		MAKE_PN_RESULT(1,SS_DEC,7)

#define PNR_ENC_FILE_TOO_SMALL		MAKE_PN_RESULT(1,SS_ENC,0)
#define PNR_ENC_UNKNOWN_FILE		MAKE_PN_RESULT(1,SS_ENC,1)
#define PNR_ENC_BAD_CHANNELS		MAKE_PN_RESULT(1,SS_ENC,2)
#define PNR_ENC_BAD_SAMPSIZE		MAKE_PN_RESULT(1,SS_ENC,3)
#define PNR_ENC_BAD_SAMPRATE		MAKE_PN_RESULT(1,SS_ENC,4)
#define PNR_ENC_INVALID			MAKE_PN_RESULT(1,SS_ENC,5)
#define PNR_ENC_NO_OUTPUT_FILE		MAKE_PN_RESULT(1,SS_ENC,6)
#define PNR_ENC_NO_INPUT_FILE		MAKE_PN_RESULT(1,SS_ENC,7)
#define PNR_ENC_NO_OUTPUT_PERMISSIONS	MAKE_PN_RESULT(1,SS_ENC,8)
#define PNR_ENC_BAD_FILETYPE		MAKE_PN_RESULT(1,SS_ENC,9)
#define PNR_ENC_INVALID_VIDEO		MAKE_PN_RESULT(1,SS_ENC,10)
#define PNR_ENC_INVALID_AUDIO		MAKE_PN_RESULT(1,SS_ENC,11)
#define PNR_ENC_NO_VIDEO_CAPTURE	MAKE_PN_RESULT(1,SS_ENC,12)
#define PNR_ENC_INVALID_VIDEO_CAPTURE	MAKE_PN_RESULT(1,SS_ENC,13)
#define PNR_ENC_NO_AUDIO_CAPTURE	MAKE_PN_RESULT(1,SS_ENC,14)
#define PNR_ENC_INVALID_AUDIO_CAPTURE	MAKE_PN_RESULT(1,SS_ENC,15)
#define PNR_ENC_TOO_SLOW_FOR_LIVE	MAKE_PN_RESULT(1,SS_ENC,16)
#define PNR_ENC_ENGINE_NOT_INITIALIZED	MAKE_PN_RESULT(1,SS_ENC,17)
#define PNR_ENC_CODEC_NOT_FOUND		MAKE_PN_RESULT(1,SS_ENC,18)
#define PNR_ENC_CODEC_NOT_INITIALIZED	MAKE_PN_RESULT(1,SS_ENC,19)
#define PNR_ENC_INVALID_INPUT_DIMENSIONS MAKE_PN_RESULT(1,SS_ENC,20)
#define PNR_ENC_MESSAGE_IGNORED		MAKE_PN_RESULT(1,SS_ENC,21)
#define PNR_ENC_NO_SETTINGS		MAKE_PN_RESULT(1,SS_ENC,22)
#define PNR_ENC_NO_OUTPUT_TYPES		MAKE_PN_RESULT(1,SS_ENC,23)
#define PNR_ENC_IMPROPER_STATE		MAKE_PN_RESULT(1,SS_ENC,24)
#define PNR_ENC_INVALID_SERVER		MAKE_PN_RESULT(1,SS_ENC,25)
#define PNR_ENC_INVALID_TEMP_PATH	MAKE_PN_RESULT(1,SS_ENC,26)
#define PNR_ENC_MERGE_FAIL		MAKE_PN_RESULT(1,SS_ENC,27)
#define PNR_BIN_DATA_NOT_FOUND		MAKE_PN_RESULT(0,SS_ENC,28)    
#define PNR_BIN_END_OF_DATA		MAKE_PN_RESULT(0,SS_ENC,29)    
#define PNR_BIN_DATA_PURGED		MAKE_PN_RESULT(1,SS_ENC,30)
#define PNR_BIN_FULL			MAKE_PN_RESULT(1,SS_ENC,31)    
#define PNR_BIN_OFFSET_PAST_END		MAKE_PN_RESULT(1,SS_ENC,32)    
#define PNR_ENC_NO_ENCODED_DATA		MAKE_PN_RESULT(1,SS_ENC,33)
#define PNR_ENC_INVALID_DLL		MAKE_PN_RESULT(1,SS_ENC,34)
#define PNR_NOT_INDEXABLE		MAKE_PN_RESULT(1,SS_ENC,35)
#define PNR_ENC_NO_BROWSER		MAKE_PN_RESULT(1,SS_ENC,36)
#define PNR_ENC_NO_FILE_TO_SERVER	MAKE_PN_RESULT(1,SS_ENC,37)
#define PNR_ENC_INSUFFICIENT_DISK_SPACE MAKE_PN_RESULT(1,SS_ENC,38)

#define PNR_RMT_USAGE_ERROR			MAKE_PN_RESULT(1,SS_RMT,1)
#define PNR_RMT_INVALID_ENDTIME		MAKE_PN_RESULT(1,SS_RMT,2)
#define PNR_RMT_MISSING_INPUT_FILE	MAKE_PN_RESULT(1,SS_RMT,3)
#define PNR_RMT_MISSING_OUTPUT_FILE		MAKE_PN_RESULT(1,SS_RMT,4)
#define PNR_RMT_INPUT_EQUALS_OUTPUT_FILE	MAKE_PN_RESULT(1,SS_RMT,5)
#define PNR_RMT_UNSUPPORTED_AUDIO_VERSION	MAKE_PN_RESULT(1,SS_RMT,6)
#define PNR_RMT_DIFFERENT_AUDIO				MAKE_PN_RESULT(1,SS_RMT,7)
#define PNR_RMT_DIFFERENT_VIDEO				MAKE_PN_RESULT(1,SS_RMT,8)
#define PNR_RMT_PASTE_MISSING_STREAM		MAKE_PN_RESULT(1,SS_RMT,9)
#define PNR_RMT_END_OF_STREAM			MAKE_PN_RESULT(1,SS_RMT,10)
#define PNR_RMT_IMAGE_MAP_PARSE_ERROR	MAKE_PN_RESULT(1,SS_RMT,11)
#define PNR_RMT_INVALID_IMAGEMAP_FILE	MAKE_PN_RESULT(1,SS_RMT,12)
#define PNR_RMT_EVENT_PARSE_ERROR		MAKE_PN_RESULT(1,SS_RMT,13)
#define PNR_RMT_INVALID_EVENT_FILE		MAKE_PN_RESULT(1,SS_RMT,14)
#define PNR_RMT_INVALID_OUTPUT_FILE		MAKE_PN_RESULT(1,SS_RMT,15)
#define PNR_RMT_INVALID_DURATION		MAKE_PN_RESULT(1,SS_RMT,16)
#define PNR_RMT_NO_DUMP_FILES			MAKE_PN_RESULT(1,SS_RMT,17)
#define PNR_RMT_NO_EVENT_DUMP_FILE		MAKE_PN_RESULT(1,SS_RMT,18)
#define PNR_RMT_NO_IMAP_DUMP_FILE		MAKE_PN_RESULT(1,SS_RMT,19)
#define PNR_RMT_NO_DATA					MAKE_PN_RESULT(1,SS_RMT,20)
#define PNR_RMT_EMPTY_STREAM			MAKE_PN_RESULT(1,SS_RMT,21)
#define PNR_RMT_READ_ONLY_FILE			MAKE_PN_RESULT(1,SS_RMT,22)
#define PNR_RMT_PASTE_MISSING_AUDIO_STREAM	MAKE_PN_RESULT(1,SS_RMT,23)
#define PNR_RMT_PASTE_MISSING_VIDEO_STREAM	MAKE_PN_RESULT(1,SS_RMT,24)


#define PNR_PROP_NOT_FOUND		MAKE_PN_RESULT(1,SS_REG,1)
#define PNR_PROP_NOT_COMPOSITE		MAKE_PN_RESULT(1,SS_REG,2)
#define PNR_PROP_DUPLICATE		MAKE_PN_RESULT(1,SS_REG,3)
#define PNR_PROP_TYPE_MISMATCH		MAKE_PN_RESULT(1,SS_REG,4)
#define PNR_PROP_ACTIVE			MAKE_PN_RESULT(1,SS_REG,5)
#define PNR_PROP_INACTIVE		MAKE_PN_RESULT(1,SS_REG,6)

#define PNR_COULDNOTINITCORE		MAKE_PN_RESULT(1,SS_MSC,1)
#define PNR_PERFECTPLAY_NOT_SUPPORTED	MAKE_PN_RESULT(1,SS_MSC,2)
#define PNR_NO_LIVE_PERFECTPLAY		MAKE_PN_RESULT(1,SS_MSC,3)
#define PNR_PERFECTPLAY_NOT_ALLOWED	MAKE_PN_RESULT(1,SS_MSC,4)
#define PNR_NO_CODECS			MAKE_PN_RESULT(1,SS_MSC,5)
#define PNR_SLOW_MACHINE		MAKE_PN_RESULT(1,SS_MSC,6)
#define PNR_FORCE_PERFECTPLAY		MAKE_PN_RESULT(1,SS_MSC,7)
#define PNR_INVALID_HTTP_PROXY_HOST	MAKE_PN_RESULT(1,SS_MSC,8)
#define PNR_INVALID_METAFILE		MAKE_PN_RESULT(1,SS_MSC,9)
#define PNR_BROWSER_LAUNCH		MAKE_PN_RESULT(1,SS_MSC,10)
#define PNR_VIEW_SOURCE_NOCLIP		MAKE_PN_RESULT(1,SS_MSC,11)
#define PNR_VIEW_SOURCE_DISSABLED	MAKE_PN_RESULT(1,SS_MSC,12)

#define	PNR_RESOURCE_NOT_CACHED		MAKE_PN_RESULT(1,SS_RSC,1)
#define PNR_RESOURCE_NOT_FOUND		MAKE_PN_RESULT(1,SS_RSC,2)
#define PNR_RESOURCE_CLOSE_FILE_FIRST	MAKE_PN_RESULT(1,SS_RSC,3)
#define PNR_RESOURCE_NODATA		MAKE_PN_RESULT(1,SS_RSC,4)
#define PNR_RESOURCE_BADFILE		MAKE_PN_RESULT(1,SS_RSC,5)
#define PNR_RESOURCE_PARTIALCOPY	MAKE_PN_RESULT(1,SS_RSC,6)

#define PNR_PPV_NO_USER			MAKE_PN_RESULT(1,SS_PPV,0)
#define PNR_PPV_GUID_READ_ONLY		MAKE_PN_RESULT(1,SS_PPV,1)
#define PNR_PPV_GUID_COLLISION		MAKE_PN_RESULT(1,SS_PPV,2)
#define PNR_REGISTER_GUID_EXISTS	MAKE_PN_RESULT(1,SS_PPV,3)
#define PNR_PPV_AUTHORIZATION_FAILED    MAKE_PN_RESULT(1,SS_PPV,4)
#define PNR_PPV_OLD_PLAYER		MAKE_PN_RESULT(1,SS_PPV,5)
#define PNR_PPV_ACCOUNT_LOCKED		MAKE_PN_RESULT(1,SS_PPV,6)
// #define PNR_PPV_PROTOCOL_IGNORES	MAKE_PN_RESULT(1,SS_PPV,7)
#define PNR_PPV_DBACCESS_ERROR          MAKE_PN_RESULT(1,SS_PPV,8)
#define PNR_PPV_USER_ALREADY_EXISTS     MAKE_PN_RESULT(1,SS_PPV,9)

// auto-upgrade (RealUpdate) errors
#define PNR_UPG_AUTH_FAILED		MAKE_PN_RESULT(1,SS_UPG,0)
#define PNR_UPG_CERT_AUTH_FAILED	MAKE_PN_RESULT(1,SS_UPG,1)
#define PNR_UPG_CERT_EXPIRED		MAKE_PN_RESULT(1,SS_UPG,2)
#define PNR_UPG_CERT_REVOKED		MAKE_PN_RESULT(1,SS_UPG,3)
#define PNR_UPG_RUP_BAD			MAKE_PN_RESULT(1,SS_UPG,4)

// auto-config errors
#define PNR_AUTOCFG_SUCCESS		MAKE_PN_RESULT(1,SS_CFG,0)
#define PNR_AUTOCFG_FAILED		MAKE_PN_RESULT(1,SS_CFG,1)
#define PNR_AUTOCFG_ABORT		MAKE_PN_RESULT(1,SS_CFG,2)

#define PNR_FAILED			PNR_FAIL

#ifdef _WIN16
/*typedef UINT				MMRESULT;*/
#else
#ifdef _WIN32
#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
#ifdef _WINCE
#undef E_NOTIMPL 
#undef E_OUTOFMEMORY
#undef E_INVALIDARG 
#undef E_NOINTERFACE
#undef E_POINTER    
#undef E_HANDLE     
#undef E_ABORT      
#undef E_FAIL       
#undef E_ACCESSDENIED
#endif
#define E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80004001L)
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80004003L)
#define E_HANDLE                         _HRESULT_TYPEDEF_(0x80070006L)
#define E_ABORT                          _HRESULT_TYPEDEF_(0x80004004L)
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)
#define E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80070005L)
#else
#define S_OK                    PNR_OK
#define E_NOTIMPL               PNR_NOTIMPL
#define E_INVALIDARG            PNR_INVALID_PARAMETER
#define E_NOINTERFACE           PNR_NOINTERFACE
#define E_POINTER               PNR_POINTER
#define E_HANDLE                PNR_HANDLE
#define E_ABORT                 PNR_ABORT
#define E_FAIL                  PNR_FAIL
#define E_ACCESSDENIES          PNR_ACCESSDENIED
#endif	/* _WIN32 */
#endif	/* _WIN16 */

#define PN_STATUS_OK            PNR_OK
#define PN_STATUS_FAILED        E_FAIL

#endif /* _PNRESULT_H_ */