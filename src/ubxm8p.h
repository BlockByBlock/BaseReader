/****************************************************************************
 *   Copyright (c) 2012-2014 PX4 Development Team. All rights reserved.
 *
 *	 Original Work from ubx.cpp by
 * 	 @author Thomas Gubler <thomasgubler@student.ethz.ch>
 *   @author Julian Oes <joes@student.ethz.ch>
 *   @author Anton Babushkin <anton.babushkin@me.com>
 *   @author Beat Kueng <beat-kueng@gmx.net>
 *   @author Hannes Delago
 * 
 ****************************************************************************/

 /**
 * @file ubxm8p.h
 *
 * U-Blox M8P protocol implementation. 
 * @author Yeap Bing Cheng <ybingcheng@gmail.com>
 * (rework from px4 gpsdriver for non-mavlink use)
 *
 * @see https://www.u-blox.com/sites/default/files/products/documents/u-bloxM8_ReceiverDescrProtSpec_%28UBX-13003221%29_Public.pdf
 */

#ifndef UBXM8P_H_
#define UBXM8P_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "rtcm.h"
#include "definitions.h"

/* MON_VER from u-blox modules can be ~190 bytes */
#ifndef GPS_READ_BUFFER_SIZE
#define GPS_READ_BUFFER_SIZE 250
#endif

#define UBX_SYNC1 0xB5
#define UBX_SYNC2 0x62

/* Message Classes */
#define UBX_CLASS_NAV		0x01
#define UBX_CLASS_INF		0x04
#define UBX_CLASS_ACK		0x05
#define UBX_CLASS_CFG		0x06
#define UBX_CLASS_MON		0x0A
#define UBX_CLASS_RTCM3	0xF5 /**< This is undocumented (?) */

/* Message IDs */
#define UBX_ID_NAV_POSLLH	0x02
#define UBX_ID_NAV_DOP		0x04
#define UBX_ID_NAV_SOL		0x06
#define UBX_ID_NAV_PVT		0x07
#define UBX_ID_NAV_VELNED	0x12
#define UBX_ID_NAV_TIMEUTC	0x21
#define UBX_ID_NAV_SVINFO	0x30
#define UBX_ID_NAV_SAT		0x35
#define UBX_ID_NAV_SVIN  	0x3B
#define UBX_ID_NAV_RELPOSNED  	0x3C
#define UBX_ID_INF_DEBUG  	0x04
#define UBX_ID_INF_ERROR  	0x00
#define UBX_ID_INF_NOTICE  	0x02
#define UBX_ID_INF_WARNING 	0x01
#define UBX_ID_ACK_NAK		0x00
#define UBX_ID_ACK_ACK		0x01
#define UBX_ID_CFG_PRT		0x00
#define UBX_ID_CFG_MSG		0x01
#define UBX_ID_CFG_RATE		0x08
#define UBX_ID_CFG_CFG		0x09
#define UBX_ID_CFG_NAV5		0x24
#define UBX_ID_CFG_SBAS		0x16
#define UBX_ID_CFG_TMODE3	0x71
#define UBX_ID_MON_VER		0x04
#define UBX_ID_MON_HW		0x09

#define UBX_ID_RTCM3_1005	0x05	/**< Stationary RTK reference station ARP */
#define UBX_ID_RTCM3_1074	0x4A	/**< GPS MSM4 */
#define UBX_ID_RTCM3_1077	0x4D	/**< GPS MSM7 */
#define UBX_ID_RTCM3_1084	0x54	/**< GLONASS MSM4 */
#define UBX_ID_RTCM3_1087	0x57	/**< GLONASS MSM7 */
#define UBX_ID_RTCM3_1124	0x7C	/**< BeiDou MSM4 */
#define UBX_ID_RTCM3_1127	0x7F	/**< BeiDou MSM7 */
#define UBX_ID_RTCM3_1230	0xE6	/**< GLONASS code-phase biases */
#define UBX_ID_RTCM3_4072	0xFE	/**< Reference station PVT (u-blox proprietary RTCM Message) */

/* Message Classes & IDs */
#define UBX_MSG_NAV_POSLLH	((UBX_CLASS_NAV) | UBX_ID_NAV_POSLLH << 8)
#define UBX_MSG_NAV_SOL		((UBX_CLASS_NAV) | UBX_ID_NAV_SOL << 8)
#define UBX_MSG_NAV_DOP		((UBX_CLASS_NAV) | UBX_ID_NAV_DOP << 8)
#define UBX_MSG_NAV_PVT		((UBX_CLASS_NAV) | UBX_ID_NAV_PVT << 8)
#define UBX_MSG_NAV_VELNED	((UBX_CLASS_NAV) | UBX_ID_NAV_VELNED << 8)
#define UBX_MSG_NAV_TIMEUTC	((UBX_CLASS_NAV) | UBX_ID_NAV_TIMEUTC << 8)
#define UBX_MSG_NAV_SVINFO	((UBX_CLASS_NAV) | UBX_ID_NAV_SVINFO << 8)
#define UBX_MSG_NAV_SAT	((UBX_CLASS_NAV) | UBX_ID_NAV_SAT << 8)
#define UBX_MSG_NAV_SVIN	((UBX_CLASS_NAV) | UBX_ID_NAV_SVIN << 8)
#define UBX_MSG_NAV_RELPOSNED	((UBX_CLASS_NAV) | UBX_ID_NAV_RELPOSNED << 8)
#define UBX_MSG_INF_DEBUG	((UBX_CLASS_INF) | UBX_ID_INF_DEBUG << 8)
#define UBX_MSG_INF_ERROR	((UBX_CLASS_INF) | UBX_ID_INF_ERROR << 8)
#define UBX_MSG_INF_NOTICE	((UBX_CLASS_INF) | UBX_ID_INF_NOTICE << 8)
#define UBX_MSG_INF_WARNING	((UBX_CLASS_INF) | UBX_ID_INF_WARNING << 8)
#define UBX_MSG_ACK_NAK		((UBX_CLASS_ACK) | UBX_ID_ACK_NAK << 8)
#define UBX_MSG_ACK_ACK		((UBX_CLASS_ACK) | UBX_ID_ACK_ACK << 8)
#define UBX_MSG_CFG_PRT		((UBX_CLASS_CFG) | UBX_ID_CFG_PRT << 8)
#define UBX_MSG_CFG_MSG		((UBX_CLASS_CFG) | UBX_ID_CFG_MSG << 8)
#define UBX_MSG_CFG_RATE	((UBX_CLASS_CFG) | UBX_ID_CFG_RATE << 8)
#define UBX_MSG_CFG_CFG		((UBX_CLASS_CFG) | UBX_ID_CFG_CFG << 8)
#define UBX_MSG_CFG_NAV5	((UBX_CLASS_CFG) | UBX_ID_CFG_NAV5 << 8)
#define UBX_MSG_CFG_SBAS	((UBX_CLASS_CFG) | UBX_ID_CFG_SBAS << 8)
#define UBX_MSG_CFG_TMODE3	((UBX_CLASS_CFG) | UBX_ID_CFG_TMODE3 << 8)
#define UBX_MSG_MON_HW		((UBX_CLASS_MON) | UBX_ID_MON_HW << 8)
#define UBX_MSG_MON_VER		((UBX_CLASS_MON) | UBX_ID_MON_VER << 8)
#define UBX_MSG_RTCM3_1005	((UBX_CLASS_RTCM3) | UBX_ID_RTCM3_1005 << 8)
#define UBX_MSG_RTCM3_1077	((UBX_CLASS_RTCM3) | UBX_ID_RTCM3_1077 << 8)
#define UBX_MSG_RTCM3_1087	((UBX_CLASS_RTCM3) | UBX_ID_RTCM3_1087 << 8)

/* RX NAV-PVT message content details */
/*   Bitfield "valid" masks */
#define UBX_RX_NAV_PVT_VALID_VALIDDATE		0x01	/**< validDate (Valid UTC Date) */
#define UBX_RX_NAV_PVT_VALID_VALIDTIME		0x02	/**< validTime (Valid UTC Time) */
#define UBX_RX_NAV_PVT_VALID_FULLYRESOLVED	0x04	/**< fullyResolved (1 = UTC Time of Day has been fully resolved (no seconds uncertainty)) */

/*   Bitfield "flags" masks */
#define UBX_RX_NAV_PVT_FLAGS_GNSSFIXOK		0x01	/**< gnssFixOK (A valid fix (i.e within DOP & accuracy masks)) */
#define UBX_RX_NAV_PVT_FLAGS_DIFFSOLN		0x02	/**< diffSoln (1 if differential corrections were applied) */
#define UBX_RX_NAV_PVT_FLAGS_PSMSTATE		0x1C	/**< psmState (Power Save Mode state (see Power Management)) */
#define UBX_RX_NAV_PVT_FLAGS_HEADVEHVALID	0x20	/**< headVehValid (Heading of vehicle is valid) */
#define UBX_RX_NAV_PVT_FLAGS_CARRSOLN		0xC0	/**< Carrier phase range solution (RTK mode) *

/* RX NAV-TIMEUTC message content details */
/*   Bitfield "valid" masks */
#define UBX_RX_NAV_TIMEUTC_VALID_VALIDTOW	0x01	/**< validTOW (1 = Valid Time of Week) */
#define UBX_RX_NAV_TIMEUTC_VALID_VALIDKWN	0x02	/**< validWKN (1 = Valid Week Number) */
#define UBX_RX_NAV_TIMEUTC_VALID_VALIDUTC	0x04	/**< validUTC (1 = Valid UTC Time) */
#define UBX_RX_NAV_TIMEUTC_VALID_UTCSTANDARD	0xF0	/**< utcStandard (0..15 = UTC standard identifier) */

/* TX CFG-PRT message contents */
#define UBX_TX_CFG_PRT_PORTID		0x01		/**< UART1 */
#define UBX_TX_CFG_PRT_PORTID_USB	0x03		/**< USB */
#define UBX_TX_CFG_PRT_PORTID_SPI	0x04		/**< SPI */
#define UBX_TX_CFG_PRT_MODE		0x000008D0	/**< 0b0000100011010000: 8N1 */
#define UBX_TX_CFG_PRT_MODE_SPI	0x00000100
#define UBX_TX_CFG_PRT_BAUDRATE		38400		/**< choose 38400 as GPS baudrate */
#define UBX_TX_CFG_PRT_INPROTOMASK_GPS	((1<<5) | 0x01)	/**< RTCM3 in and UBX in */
#define UBX_TX_CFG_PRT_INPROTOMASK_RTCM	(0x01)	/**< UBX in */
#define UBX_TX_CFG_PRT_OUTPROTOMASK_GPS	(0x01)			/**< UBX out */
#define UBX_TX_CFG_PRT_OUTPROTOMASK_RTCM	((1<<5) | 0x01)		/**< RTCM3 out and UBX out */

/* TX CFG-RATE message contents */
#define UBX_TX_CFG_RATE_MEASINTERVAL	200		/**< 200ms for 5Hz */
#define UBX_TX_CFG_RATE_NAVRATE		1		/**< cannot be changed */
#define UBX_TX_CFG_RATE_TIMEREF		0		/**< 0: UTC, 1: GPS time */

/* TX CFG-NAV5 message contents */
#define UBX_TX_CFG_NAV5_MASK		0x0005		/**< Only update dynamic model and fix mode */
#define UBX_TX_CFG_NAV5_FIXMODE		2		/**< 1 2D only, 2 3D only, 3 Auto 2D/3D */

/* TX CFG-SBAS message contents */
#define UBX_TX_CFG_SBAS_MODE_ENABLED	1				/**< SBAS enabled */
#define UBX_TX_CFG_SBAS_MODE_DISABLED	0				/**< SBAS disabled */
#define UBX_TX_CFG_SBAS_MODE		UBX_TX_CFG_SBAS_MODE_DISABLED	/**< SBAS enabled or disabled */

/* TX CFG-TMODE3 message contents */
#define UBX_TX_CFG_TMODE3_FLAGS     	1 	    	/**< start survey-in */
#define UBX_TX_CFG_TMODE3_SVINMINDUR    (3*60)		/**< survey-in: minimum duration [s] (higher=higher precision) */
#define UBX_TX_CFG_TMODE3_SVINACCLIMIT  (10000)	/**< survey-in: position accuracy limit 0.1[mm] */

/*** u-blox protocol binary message and payload definitions ***/
#pragma pack(push, 1)

/* General: Header */
typedef struct {
	uint8_t		sync1;
	uint8_t		sync2;
	uint16_t	msg;
	uint16_t	length;
} ubx_header_t;

/* General: Checksum */
typedef struct {
	uint8_t		ck_a;
	uint8_t		ck_b;
} ubx_checksum_t ;

/* Rx NAV-POSLLH */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	int32_t		lon;		/**< Longitude [1e-7 deg] */
	int32_t		lat;		/**< Latitude [1e-7 deg] */
	int32_t		height;		/**< Height above ellipsoid [mm] */
	int32_t		hMSL;		/**< Height above mean sea level [mm] */
	uint32_t	hAcc;  		/**< Horizontal accuracy estimate [mm] */
	uint32_t	vAcc;  		/**< Vertical accuracy estimate [mm] */
} ubx_payload_rx_nav_posllh_t;

/* Rx NAV-DOP */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	uint16_t	gDOP;		/**< Geometric DOP [0.01] */
	uint16_t	pDOP;		/**< Position DOP [0.01] */
	uint16_t	tDOP;		/**< Time DOP [0.01] */
	uint16_t	vDOP;		/**< Vertical DOP [0.01] */
	uint16_t	hDOP;		/**< Horizontal DOP [0.01] */
	uint16_t	nDOP;		/**< Northing DOP [0.01] */
	uint16_t	eDOP;		/**< Easting DOP [0.01] */
} ubx_payload_rx_nav_dop_t;

/* Rx NAV-SOL */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	int32_t		fTOW;		/**< Fractional part of iTOW (range: +/-500000) [ns] */
	int16_t		week;		/**< GPS week */
	uint8_t		gpsFix;		/**< GPSfix type: 0 = No fix, 1 = Dead Reckoning only, 2 = 2D fix, 3 = 3d-fix, 4 = GPS + dead reckoning, 5 = time only fix */
	uint8_t		flags;
	int32_t		ecefX;
	int32_t		ecefY;
	int32_t		ecefZ;
	uint32_t	pAcc;
	int32_t		ecefVX;
	int32_t		ecefVY;
	int32_t		ecefVZ;
	uint32_t	sAcc;
	uint16_t	pDOP;		/**< Position DOP [0.01] */
	uint8_t		reserved1;
	uint8_t		numSV;		/**< Number of SVs used in Nav Solution */
	uint32_t	reserved2;
} ubx_payload_rx_nav_sol_t;

/* Rx NAV-PVT (ubx8) */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	uint16_t	year; 		/**< Year (UTC)*/
	uint8_t		month; 		/**< Month, range 1..12 (UTC) */
	uint8_t		day; 		/**< Day of month, range 1..31 (UTC) */
	uint8_t		hour; 		/**< Hour of day, range 0..23 (UTC) */
	uint8_t		min; 		/**< Minute of hour, range 0..59 (UTC) */
	uint8_t		sec;		/**< Seconds of minute, range 0..60 (UTC) */
	uint8_t		valid; 		/**< Validity flags (see UBX_RX_NAV_PVT_VALID_...) */
	uint32_t	tAcc; 		/**< Time accuracy estimate (UTC) [ns] */
	int32_t		nano;		/**< Fraction of second (UTC) [-1e9...1e9 ns] */
	uint8_t		fixType;	/**< GNSSfix type: 0 = No fix, 1 = Dead Reckoning only, 2 = 2D fix, 3 = 3d-fix, 4 = GNSS + dead reckoning, 5 = time only fix */
	uint8_t		flags;		/**< Fix Status Flags (see UBX_RX_NAV_PVT_FLAGS_...) */
	uint8_t		reserved1;
	uint8_t		numSV;		/**< Number of SVs used in Nav Solution */
	int32_t		lon;		/**< Longitude [1e-7 deg] */
	int32_t		lat;		/**< Latitude [1e-7 deg] */
	int32_t		height;		/**< Height above ellipsoid [mm] */
	int32_t		hMSL;		/**< Height above mean sea level [mm] */
	uint32_t	hAcc;  		/**< Horizontal accuracy estimate [mm] */
	uint32_t	vAcc;  		/**< Vertical accuracy estimate [mm] */
	int32_t		velN;		/**< NED north velocity [mm/s]*/
	int32_t		velE;		/**< NED east velocity [mm/s]*/
	int32_t		velD;		/**< NED down velocity [mm/s]*/
	int32_t		gSpeed;		/**< Ground Speed (2-D) [mm/s] */
	int32_t		headMot;	/**< Heading of motion (2-D) [1e-5 deg] */
	uint32_t	sAcc;		/**< Speed accuracy estimate [mm/s] */
	uint32_t	headAcc;	/**< Heading accuracy estimate (motion and vehicle) [1e-5 deg] */
	uint16_t	pDOP;		/**< Position DOP [0.01] */
	uint16_t	reserved2;
	uint32_t	reserved3;
	int32_t		headVeh;	/**< (ubx8+ only) Heading of vehicle (2-D) [1e-5 deg] */
	uint32_t	reserved4;	/**< (ubx8+ only) */
} ubx_payload_rx_nav_pvt_t;
#define UBX_PAYLOAD_RX_NAV_PVT_SIZE_UBX7	(sizeof(ubx_payload_rx_nav_pvt_t) - 8)
#define UBX_PAYLOAD_RX_NAV_PVT_SIZE_UBX8	(sizeof(ubx_payload_rx_nav_pvt_t))

/* Rx NAV-TIMEUTC */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	uint32_t	tAcc; 		/**< Time accuracy estimate (UTC) [ns] */
	int32_t		nano;		/**< Fraction of second, range -1e9 .. 1e9 (UTC) [ns] */
	uint16_t	year; 		/**< Year, range 1999..2099 (UTC) */
	uint8_t		month; 		/**< Month, range 1..12 (UTC) */
	uint8_t		day; 		/**< Day of month, range 1..31 (UTC) */
	uint8_t		hour; 		/**< Hour of day, range 0..23 (UTC) */
	uint8_t		min; 		/**< Minute of hour, range 0..59 (UTC) */
	uint8_t		sec;		/**< Seconds of minute, range 0..60 (UTC) */
	uint8_t		valid; 		/**< Validity Flags (see UBX_RX_NAV_TIMEUTC_VALID_...) */
} ubx_payload_rx_nav_timeutc_t;

/* Rx NAV-SVINFO Part 1 */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	uint8_t		numCh; 		/**< Number of channels */
	uint8_t		globalFlags;
	uint16_t	reserved2;
} ubx_payload_rx_nav_svinfo_part1_t;

/* Rx NAV-SVINFO Part 2 (repeated) */
typedef struct {
	uint8_t		chn; 		/**< Channel number, 255 for SVs not assigned to a channel */
	uint8_t		svid; 		/**< Satellite ID */
	uint8_t		flags;
	uint8_t		quality;
	uint8_t		cno;		/**< Carrier to Noise Ratio (Signal Strength) [dbHz] */
	int8_t		elev; 		/**< Elevation [deg] */
	int16_t		azim; 		/**< Azimuth [deg] */
	int32_t		prRes; 		/**< Pseudo range residual [cm] */
} ubx_payload_rx_nav_svinfo_part2_t;

/* Rx NAV-SVIN (survey-in info) */
typedef struct {
	uint8_t     version;
	uint8_t     reserved1[3];
	uint32_t	iTOW;
	uint32_t    dur;
	int32_t     meanX;
	int32_t     meanY;
	int32_t     meanZ;
	int8_t      meanXHP;
	int8_t      meanYHP;
	int8_t      meanZHP;
	int8_t      reserved2;
	uint32_t    meanAcc;
	uint32_t    obs;
	uint8_t     valid;
	uint8_t     active;
	uint8_t     reserved3[2];
} ubx_payload_rx_nav_svin_t;

/* Rx NAV-VELNED */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	int32_t		velN;		/**< North velocity component [cm/s]*/
	int32_t		velE;		/**< East velocity component [cm/s]*/
	int32_t		velD;		/**< Down velocity component [cm/s]*/
	uint32_t	speed;		/**< Speed (3-D) [cm/s] */
	uint32_t	gSpeed;		/**< Ground speed (2-D) [cm/s] */
	int32_t		heading;	/**< Heading of motion 2-D [1e-5 deg] */
	uint32_t	sAcc;		/**< Speed accuracy estimate [cm/s] */
	uint32_t	cAcc;		/**< Course / Heading accuracy estimate [1e-5 deg] */
} ubx_payload_rx_nav_velned_t;

/* Rx MON-HW (ubx6) */
typedef struct {
	uint32_t	pinSel;
	uint32_t	pinBank;
	uint32_t	pinDir;
	uint32_t	pinVal;
	uint16_t	noisePerMS;
	uint16_t	agcCnt;
	uint8_t		aStatus;
	uint8_t		aPower;
	uint8_t		flags;
	uint8_t		reserved1;
	uint32_t	usedMask;
	uint8_t		VP[25];
	uint8_t		jamInd;
	uint16_t	reserved3;
	uint32_t	pinIrq;
	uint32_t	pullH;
	uint32_t	pullL;
} ubx_payload_rx_mon_hw_ubx6_t;

/* Rx MON-HW (ubx7+) */
typedef struct {
	uint32_t	pinSel;
	uint32_t	pinBank;
	uint32_t	pinDir;
	uint32_t	pinVal;
	uint16_t	noisePerMS;
	uint16_t	agcCnt;
	uint8_t		aStatus;
	uint8_t		aPower;
	uint8_t		flags;
	uint8_t		reserved1;
	uint32_t	usedMask;
	uint8_t		VP[17];
	uint8_t		jamInd;
	uint16_t	reserved3;
	uint32_t	pinIrq;
	uint32_t	pullH;
	uint32_t	pullL;
} ubx_payload_rx_mon_hw_ubx7_t;

/* Rx MON-VER Part 1 */
typedef struct {
	uint8_t		swVersion[30];
	uint8_t		hwVersion[10];
} ubx_payload_rx_mon_ver_part1_t;

/* Rx MON-VER Part 2 (repeated) */
typedef struct {
	uint8_t		extension[30];
} ubx_payload_rx_mon_ver_part2_t;

/* Rx ACK-ACK */
typedef	union {
	uint16_t	msg;
	struct {
		uint8_t	clsID;
		uint8_t	msgID;
	};
} ubx_payload_rx_ack_ack_t;

/* Rx ACK-NAK */
typedef	union {
	uint16_t	msg;
	struct {
		uint8_t	clsID;
		uint8_t	msgID;
	};
} ubx_payload_rx_ack_nak_t;

/* Tx CFG-PRT */
typedef struct {
	uint8_t		portID;
	uint8_t		reserved0;
	uint16_t	txReady;
	uint32_t	mode;
	uint32_t	baudRate;
	uint16_t	inProtoMask;
	uint16_t	outProtoMask;
	uint16_t	flags;
	uint16_t	reserved5;
} ubx_payload_tx_cfg_prt_t;

/* Tx CFG-RATE */
typedef struct {
	uint16_t	measRate;	/**< Measurement Rate, GPS measurements are taken every measRate milliseconds */
	uint16_t	navRate;	/**< Navigation Rate, in number of measurement cycles. This parameter cannot be changed, and must be set to 1 */
	uint16_t	timeRef;	/**< Alignment to reference time: 0 = UTC time, 1 = GPS time */
} ubx_payload_tx_cfg_rate_t;

/* Tx CFG-CFG */
typedef struct {
	uint32_t	clearMask;	/**< Clear settings */
	uint32_t	saveMask;	/**< Save settings */
	uint32_t	loadMask;	/**< Load settings */
	uint8_t		deviceMask; /**< Storage devices to apply this top */
} ubx_payload_tx_cfg_cfg_t;

/* Tx CFG-NAV5 */
typedef struct {
	uint16_t	mask;
	uint8_t		dynModel;	/**< Dynamic Platform model: 0 Portable, 2 Stationary, 3 Pedestrian, 4 Automotive, 5 Sea, 6 Airborne <1g, 7 Airborne <2g, 8 Airborne <4g */
	uint8_t		fixMode;	/**< Position Fixing Mode: 1 2D only, 2 3D only, 3 Auto 2D/3D */
	int32_t		fixedAlt;
	uint32_t	fixedAltVar;
	int8_t		minElev;
	uint8_t		drLimit;
	uint16_t	pDop;
	uint16_t	tDop;
	uint16_t	pAcc;
	uint16_t	tAcc;
	uint8_t		staticHoldThresh;
	uint8_t		dgpsTimeOut;
	uint8_t		cnoThreshNumSVs;	/**< (ubx7+ only, else 0) */
	uint8_t		cnoThresh;		/**< (ubx7+ only, else 0) */
	uint16_t	reserved;
	uint16_t	staticHoldMaxDist;	/**< (ubx8+ only, else 0) */
	uint8_t		utcStandard;		/**< (ubx8+ only, else 0) */
	uint8_t		reserved3;
	uint32_t	reserved4;
} ubx_payload_tx_cfg_nav5_t;

/* tx cfg-sbas */
typedef struct {
	uint8_t		mode;
	uint8_t		usage;
	uint8_t		maxSBAS;
	uint8_t		scanmode2;
	uint32_t	scanmode1;
} ubx_payload_tx_cfg_sbas_t;

/* Tx CFG-MSG */
typedef struct {
	union {
		uint16_t	msg;
		struct {
			uint8_t	msgClass;
			uint8_t	msgID;
		};
	};
	uint8_t rate;
} ubx_payload_tx_cfg_msg_t;

/* CFG-TMODE3 ublox 8 (protocol version >= 20) */
typedef struct {
	uint8_t     version;
	uint8_t     reserved1;
	uint16_t    flags;
	int32_t     ecefXOrLat;
	int32_t     ecefYOrLon;
	int32_t     ecefZOrAlt;
	int8_t      ecefXOrLatHP;
	int8_t      ecefYOrLonHP;
	int8_t      ecefZOrAltHP;
	uint8_t     reserved2;
	uint32_t    fixedPosAcc;
	uint32_t    svinMinDur;
	uint32_t    svinAccLimit;
	uint8_t     reserved3[8];
} ubx_payload_tx_cfg_tmode3_t;

/* General message and payload buffer union */
typedef union {
	ubx_payload_rx_nav_pvt_t		payload_rx_nav_pvt;
	ubx_payload_rx_nav_posllh_t		payload_rx_nav_posllh;
	ubx_payload_rx_nav_sol_t		payload_rx_nav_sol;
	ubx_payload_rx_nav_dop_t		payload_rx_nav_dop;
	ubx_payload_rx_nav_timeutc_t		payload_rx_nav_timeutc;
	ubx_payload_rx_nav_svinfo_part1_t	payload_rx_nav_svinfo_part1;
	ubx_payload_rx_nav_svinfo_part2_t	payload_rx_nav_svinfo_part2;
	ubx_payload_rx_nav_svin_t		payload_rx_nav_svin;
	ubx_payload_rx_nav_velned_t		payload_rx_nav_velned;
	ubx_payload_rx_mon_hw_ubx6_t		payload_rx_mon_hw_ubx6;
	ubx_payload_rx_mon_hw_ubx7_t		payload_rx_mon_hw_ubx7;
	ubx_payload_rx_mon_ver_part1_t		payload_rx_mon_ver_part1;
	ubx_payload_rx_mon_ver_part2_t		payload_rx_mon_ver_part2;
	ubx_payload_rx_ack_ack_t		payload_rx_ack_ack;
	ubx_payload_rx_ack_nak_t		payload_rx_ack_nak;
	ubx_payload_tx_cfg_prt_t		payload_tx_cfg_prt;
	ubx_payload_tx_cfg_rate_t		payload_tx_cfg_rate;
	ubx_payload_tx_cfg_nav5_t		payload_tx_cfg_nav5;
	ubx_payload_tx_cfg_sbas_t		payload_tx_cfg_sbas;
	ubx_payload_tx_cfg_msg_t		payload_tx_cfg_msg;
	ubx_payload_tx_cfg_tmode3_t		payload_tx_cfg_tmode3;
	ubx_payload_tx_cfg_cfg_t		payload_tx_cfg_cfg;
} ubx_buf_t;

#pragma pack(pop)
/*** END OF u-blox protocol binary message and payload definitions ***/

/* Decoder state */
typedef enum {
	UBX_DECODE_SYNC1 = 0,
	UBX_DECODE_SYNC2,
	UBX_DECODE_CLASS,
	UBX_DECODE_ID,
	UBX_DECODE_LENGTH1,
	UBX_DECODE_LENGTH2,
	UBX_DECODE_PAYLOAD,
	UBX_DECODE_CHKSUM1,
	UBX_DECODE_CHKSUM2,

	UBX_DECODE_RTCM3
} ubx_decode_state_t;

/* Rx message state */
typedef enum {
	UBX_RXMSG_IGNORE = 0,
	UBX_RXMSG_HANDLE,
	UBX_RXMSG_DISABLE,
	UBX_RXMSG_ERROR_LENGTH
} ubx_rxmsg_state_t;

/* ACK state */
typedef enum {
	UBX_ACK_IDLE = 0,
	UBX_ACK_WAITING,
	UBX_ACK_GOT_ACK,
	UBX_ACK_GOT_NAK
} ubx_ack_state_t;

enum class GPSCallbackType {
	/**
	 * Read data from device. This is a blocking operation with a timeout.
	 * data1: points to a buffer to be written to. The first sizeof(int) bytes contain the
	 *        timeout in ms when calling the method.
	 * data2: buffer length in bytes. Less bytes than this can be read.
	 * return: num read bytes, 0 on timeout (the method can actually also return 0 before
	 *         the timeout happens).
	 */
	readDeviceData = 0,

	/**
	 * Write data to device
	 * data1: data to be written
	 * data2: number of bytes to write
	 * return: num written bytes
	 */
	writeDeviceData,

	/**
	 * set Baudrate
	 * data1: ignored
	 * data2: baudrate
	 * return: 0 on success
	 */
	setBaudrate,

	/**
	 * Got an RTCM message from the device.
	 * data1: pointer to the message
	 * data2: message length
	 * return: ignored
	 */
	gotRTCMMessage,

	/**
	 * message about current survey-in status
	 * data1: points to a SurveyInStatus struct
	 * data2: ignored
	 * return: ignored
	 */
	surveyInStatus,

	/**
	 * can be used to set the current clock accurately
	 * data1: pointer to a timespec struct
	 * data2: ignored
	 * return: ignored
	 */
	setClock,
};

/** Callback function for platform-specific stuff.
 * data1 and data2 depend on type and user is the custom user-supplied argument.
 * @return <0 on error, >=0 on success (depending on type)
 */
typedef int (*GPSCallbackPtr)(GPSCallbackType type, void *data1, int data2, void *user);

struct SurveyInStatus {
	uint32_t mean_accuracy;       /**< [mm] */
	uint32_t duration;            /**< [s] */
	uint8_t flags;                /**< bit 0: valid, bit 1: active */
};

// TODO: this number seems wrong
#define GPS_EPOCH_SECS ((time_t)1234567890ULL)

class UBXM8P
{
public:
    /**
     * For setting the mode between Base or Rover
     */
    enum class OutputMode {
        GPS = 0,    // Normal GPS output
        RTCM        // request RTCM output. For base station
    };

    /**
	 * Constructor - WIP
     * @param gpscallbackptr callback
     * @param callback_user user
     * @param veh_gps_position_s gps_position
     * @param satellite_info_s satellite_info
     * @param dynamic_model 0 Portable, 2 Stationary, 3 Pedestrian, 4 Automotive
	 * @return <0 on error, otherwise a bitset:
	 *         bit 0 set: got gps position update
	 *         bit 1 set: got satellite info update
	 */
    UBXM8P(struct vehicle_gps_position_s *gps_position, 
			uint8_t dynamic_model = 0);

    ~UBXM8P();

    /**
	 * Receive and process data
	 * @param timeout [ms]
	 * @return <0 on error, otherwise a bitset:
	 *         bit 0 set: got gps position update
	 *         bit 1 set: got satellite info update
	 */
    int receive(unsigned timeout);

    /**
	 * Configure the device
	 * @param baud Input and output parameter: if set to 0, the baudrate will be automatically detected and set to
	 *             the detected baudrate. If not 0, a fixed baudrate is used.
	 * @return 0 on success, <0 otherwise
	 */
    int configure(unsigned &baud, OutputMode output_mode);

    /**
	 * set survey-in specs for RTK base station setup (for finding an accurate base station position
	 * by averaging the position measurements over time).
	 * @param survey_in_acc_limit minimum accuracy in 0.1mm
	 * @param survey_in_min_dur minimum duration in seconds
	 */
    void setSurveySpecs(uint32_t survey_in_acc_limit, uint32_t survey_in_min_dur);

    /**
	 * set the Baudrate
	 * @param baudrate
	 * @return 0 on success, <0 otherwise
	 */
    int setBaudrate(int baudrate);

    /**
	 * Send a message
	 * @return true on success, false on write error (errno set)
	 */
    bool sendMessage(const uint16_t msg, const uint8_t *payload, const uint16_t length);

    /**
	 * Calculate & add checksum for given buffer
	 */
	void calcChecksum(const uint8_t *buffer, const uint16_t length, ubx_checksum_t *checksum);

	/**
	 * set survey-in specs for RTK base station setup (for finding an accurate base station position
	 * by averaging the position measurements over time).
	 * @param survey_in_acc_limit minimum accuracy in 0.1mm
	 * @param survey_in_min_dur minimum duration in seconds
	 */
	void setSurveyInSpecs(uint32_t survey_in_acc_limit, uint32_t survey_in_min_dur);

protected:
    /**
	 * write to the device
	 * @param buf
	 * @param buf_length
	 * @return num written bytes, -1 on error
	 */
	int write(const void *buf, int buf_length)
	{
		printf("Writing - THIS IS NOT RIGHT WITHOUT CALLBACK\n");
		//return _callback(GPSCallbackType::writeDeviceData, (void *)buf, buf_length, _callback_user);
		return buf_length;
	}

    GPSCallbackPtr _callback{nullptr};
    void *_callback_user{};

    /**
	 * read from device
	 * @param buf: pointer to read buffer
	 * @param buf_length: size of read buffer
	 * @param timeout: timeout in ms
	 * @return: 0 for nothing read, or poll timed out
	 *	    < 0 for error
	 *	    > 0 number of bytes read
	 */
	int read(uint8_t *buf, int buf_length, int timeout)
	{
		*((int *)buf) = timeout;
		//return _callback(GPSCallbackType::readDeviceData, buf, buf_length, _callback_user);
		printf("Read - THIS IS NOT RIGHT WITHOUT CALLBACK");
	}

    /** got an RTCM message from the device */
	void gotRTCMMessage(uint8_t *buf, int buf_length)
	{
		_callback(GPSCallbackType::gotRTCMMessage, buf, buf_length, _callback_user);
	}

	void setClock(timespec &t)
	{
		_callback(GPSCallbackType::setClock, &t, 0, _callback_user);
	}

	void surveyInStatus(SurveyInStatus &status)
	{
		_callback(GPSCallbackType::surveyInStatus, &status, 0, _callback_user);
	}

private:
	/**
	 * Calculate FNV1 hash
	 */
	uint32_t fnv1_32_str(uint8_t *str, uint32_t hval);

	uint64_t _last_timestamp_time{0};
	uint32_t _ubx_version{0};
    uint32_t _survey_in_acc_limit;
	uint32_t _survey_in_min_dur;
    uint16_t _ack_waiting_msg{0};
    uint16_t _rx_msg{};
    uint16_t _rx_payload_length{};
	uint16_t _rx_payload_index{};
    uint8_t	_rx_ck_a{};
	uint8_t	_rx_ck_b{};
	uint8_t _rate_count_lat_lon{};
	uint8_t _rate_count_vel{};
	// ublox Dynamic platform model default 0: portable
	uint8_t _dyn_model{0};
	
	struct vehicle_gps_position_s *_gps_position {nullptr};
	struct satellite_info_s *_satellite_info {nullptr};
    ubx_decode_state_t	_decode_state{};
    ubx_ack_state_t		_ack_state{UBX_ACK_IDLE};
	ubx_rxmsg_state_t	_rx_state{UBX_RXMSG_IGNORE};
	ubx_buf_t			_buf{};

    bool _configured{false};    // Configuration status
    bool _got_posllh{false};
	bool _got_velned{false};
	bool _use_nav_pvt{false};

    
    gps_abstime		_disable_cmd_last{0};
    OutputMode _output_mode{OutputMode::GPS};
    
    RTCMParsing	*_rtcm_parsing{nullptr};

	/**
	 * Start or restart the survey-in procees. This is only used in RTCM ouput mode.
	 * It will be called automatically after configuring.
	 * @return 0 on success, <0 on error
	 */
	int restartSurveyIn();

    /**
	 * Wait for message acknowledge
	 */
	int waitForAck(const uint16_t msg, const unsigned timeout, const bool report);

    /**
	 * Parse the binary UBX packet
	 */
	int parseChar(const uint8_t b);

    /**
	 * Reset the parse state machine for a fresh start
	 */
	void decodeInit(void);

    /**
	 * While parsing add every byte (except the sync bytes) to the checksum
	 */
	void addByteToChecksum(const uint8_t);

	/**
	 * Configure message rate
	 * @return true on success, false on write error
	 */
	bool configureMessageRate(const uint16_t msg, const uint8_t rate);

    /**
	 * Start payload rx
	 */
	int payloadRxInit(void);

    /**
	 * Finish payload rx
	 */
	int payloadRxDone(void);

    /**
	 * Add payload rx byte
	 */
	int payloadRxAdd(const uint8_t b);
	int payloadRxAddNavSvinfo(const uint8_t b);
	int payloadRxAddMonVer(const uint8_t b);

	/**
	 * combines the configure_message_rate & wait_for_ack calls
	 * @return true on success
	 */
	inline bool configureMessageRateAndAck(uint16_t msg, uint8_t rate, bool report_ack_error = false);

 };

 #endif /* UBXM8P_H_ */
