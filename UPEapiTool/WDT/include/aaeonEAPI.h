// aaeonEAPI.h : main header file for the aaeonEAPI DLL
//

#pragma once

#if !defined(_WINRT_DLL) && !defined(WINDOWS_UWP)
#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif //#ifndef _WINRT_DLL

#include "resource.h"		// main symbols
#endif	//#if !defined(_WINRT_DLL) && !defined(WINDOWS_UWP)

#include "EAPIStatusCode.h"

#include <stdint.h> //uint64_t

#if !defined(_WINRT_DLL) && !defined(WINDOWS_UWP)
// CaaeonEAPIApp
// See aaeonEAPI.cpp for the implementation of this class
//

class CaaeonEAPIApp : public CWinApp
{
public:
	CaaeonEAPIApp();

	// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
#endif	//#if !defined(_WINRT_DLL) && !defined(WINDOWS_UWP)

extern "C" {
	/***********************************************************************/
	/* __IN
	* Arg Type		  | Characteristics
	* ----------------+-----------------------------------------------------
	* Immediate value | Input value that must be specified and is essential
	*				  | to function operation.
	*				  |
	* Pointer		  | Valid pointer to initialized buffer/variable.
	*				  |
	*/
#ifndef __IN
# define __IN
#endif

	/* __INOPT
	* Arg Type		  | Characteristics
	* ----------------+-----------------------------------------------------
	* Pointer		  | Valid pointer to initialized buffer/variable, or
	*				  | NULL Pointer.
	*				  | Note: refer to function specification for specifics.
	*/
#ifndef __INOPT
# define __INOPT
#endif

	/* __OUT
	* Arg Type		  | Characteristics
	* ----------------+-----------------------------------------------------
	* Pointer		  | Valid pointer to destination buffer/variable
	*/
#ifndef __OUT
# define __OUT
#endif

	/* __OUTOPT
	* Arg Type		  | Characteristics
	* ----------------+-----------------------------------------------------
	* Pointer		  | Valid pointer to destination buffer/variable, or
	*				  | NULL Pointer.
	*				  | Note: refer to function specification for specifics.
	*/
#ifndef __OUTOPT
# define __OUTOPT
#endif

	/* __INOUT
	* Arg Type		  | Characteristics
	* ----------------+-----------------------------------------------------
	* Pointer		  | Valid pointer to initialized buffer/variable
	*				  | Contents of buffer/variable updated before return.
	*/
#ifndef __INOUT
# define __INOUT
#endif

#ifndef EAPI_CALLTYPE
# define EAPI_CALLTYPE __declspec(dllexport)
#endif

	typedef unsigned __int8	 uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;

#ifndef EAPI_UINT32_C
# define EAPI_UINT8_C(x)	((uint8_t)(x))
# define EAPI_UINT16_C(x)	((uint16_t)(x))
# define EAPI_UINT32_C(x)	((uint32_t)(x))
#endif
	/***********************************************************************/
	/* All version numbers
	* +=========+===============================+
	* | Bits	| Descriptions					|
	* +=========+===============================+
	* | 24 - 31 | Version Number				|
	* +---------+-------------------------------+
	* | 16 - 23 | Revision Number				|
	* +---------+-------------------------------+
	* | 0 - 15	| Build Number					|
	* +=========+===============================+
	*/
#define EAPI_VER_MASK_VER		EAPI_UINT32_C(0xFF000000)
#define EAPI_VER_MASK_REV		EAPI_UINT32_C(0x00FF0000)
#define EAPI_VER_MASK_BUILD		EAPI_UINT32_C(0x0000FFFF)
#define EAPI_VER_GET_VER(x)		EAPI_UINT8_C (((x)>>24)&UINT8_MAX )
#define EAPI_VER_GET_REV(x)		EAPI_UINT8_C (((x)>>16)&UINT8_MAX )
#define EAPI_VER_GET_BUILD(x)	EAPI_UINT16_C(((x)>> 0)&UINT16_MAX)
#define EAPI_VER_CREATE(Version, Revision, Build) (	EAPI_UINT32_C((((Version )&UINT8_MAX )<<24)|(((Revision)&UINT8_MAX )<<16)|(((Build )&UINT16_MAX)<< 0)))

	/* Embedded API Standard Revision */
#define EAPI_VER		1
#define EAPI_REVISION	0
#define EAPI_VERSION	EAPI_VER_CREATE(EAPI_VER, EAPI_REVISION, 0)
	/***********************************************************************/

	/* Embedded Application System Interface */
	/*
	* EApi Types
	*/
	typedef uint32_t EApiStatus_t;
	typedef uint32_t EApiId_t;

	/*-------------------------------Library-------------------------------*/
	/*
	* EApiLibInitialize
	*
	* Condition								 | Return Values
	* ---------------------------------------+------------------------------
	* Library Already initialized			 | EAPI_STATUS_INITIALIZED
	* Common Error							 | Common Error Code
	* Else									 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@brief 	Should be called before
	*			calling any other API
	*			function is called
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiLibInitialize(void);	/* Should be called before
									* calling any other API
									* function is called
									*/
									/*
									* EApiLibUnInitialize
									*
									* Condition								 | Return Values
									* ---------------------------------------+------------------------------
									* Library Uninitialized					 | EAPI_STATUS_NOT_INITIALIZED
									* Common Error							 | Common Error Code
									* Else									 | EAPI_STATUS_SUCCESS
									*/
									/**
									*	@brief 	Should be called before
									*			program exit
									*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiLibUnInitialize(void); /* Should be called before
								   * program exit
								   */
								   /*
								   * Plug and Play Identifiers
								   */
								   /*
								   * The following MACROS are for Creating OEM IDs
								   * OEM ID macros should be named
								   * EAPI_[PNPID]_ID_[TYPE]_[NAME]
								   * E.G.
								   * EAPI_PMG_ID_BOARD_CUSTOMERID
								   */
#define EAPI_BYTE_SWAP_W(a) EAPI_UINT16_C((((a)<<8)&0xFF00)|(((a)>>8)&0x00FF))
#define EAPI_CREATE_PNPID(a, b, c) EAPI_BYTE_SWAP_W(((( a -'A'+1)&0x1F)<<10)|((( b -'A'+1)&0x1F)<< 5)|((( c -'A'+1)&0x1F)<< 0))
#define EAPI_CREATE_CUST_ID(a, b, c, Id) EAPI_UINT32_C((0xF<<28)|(EAPI_CREATE_PNPID(a, b, c)<<12)|(Id&0xFFF))
#define EAPI_PNPID_PICMG EAPI_CREATE_PNPID('P', 'M', 'G')
								   /* PICMG Should register this. */
								   /*
								   *
								   * B O A R D I N F O M A T I O N S T R I N G S
								   *
								   */
								   /* IDS */
#define EAPI_ID_BOARD_MANUFACTURER_STR	EAPI_UINT32_C(0) 
								   /* Board Manufacturer
								   * Name String
								   */
#define EAPI_ID_BOARD_NAME_STR			EAPI_UINT32_C(1) /* Board Name String */
#define EAPI_ID_BOARD_REVISION_STR		EAPI_UINT32_C(2) /* Board Name String */
#define EAPI_ID_BOARD_SERIAL_STR		EAPI_UINT32_C(3) /* Board Serial Number String */
#define EAPI_ID_BOARD_BIOS_REVISION_STR EAPI_UINT32_C(4) 
								   /* Board Bios Revision
								   * String
								   */
#define EAPI_ID_BOARD_HW_REVISION_STR	EAPI_UINT32_C(5) 
								   /* Board Hardware
								   * Revision String
								   */
#define EAPI_ID_BOARD_PLATFORM_TYPE_STR	EAPI_UINT32_C(6) 
								   /* Platform ID
								   * (ETX, COM Express,
								   * etc...)
								   */
#define EAPI_ID_EC_REVISION_STR EAPI_UINT32_C(7)
								   /* EC Revision
								   *
								   *
								   */

								   /*
								   * EApiBoardGetStringA
								   *
								   * Condition								 | Return Values
								   * ---------------------------------------+------------------------------
								   * Library Uninitialized					 | EAPI_STATUS_NOT_INITIALIZED
								   * pBufLen==NULL							 | EAPI_STATUS_INVALID_PARAMETER
								   * pBufLen!=NULL&&*pBufLen&&pBuffer==NULL | EAPI_STATUS_INVALID_PARAMETER
								   * unknown Id							 | EAPI_STATUS_UNSUPPORTED
								   * strlen(Id)+1>*pBufLen					 | EAPI_STATUS_MORE_DATA
								   * Common Error							 | Common Error Code
								   * Else									 | EAPI_STATUS_SUCCESS
								   */
								   /**
								   *	@param Id:			Name Id: EAPI_ID_BOARD_NAME_STR
								   *	@param Id:			Name Id: EAPI_ID_BOARD_SERIAL_STR
								   *	@param Id:          Name Id: EAPI_ID_BOARD_MANUFACTURER_STR
								   *	@param Id:			Name Id: EAPI_ID_BOARD_BIOS_REVISION_STR
								   *	@param Id:			Name Id: EAPI_ID_EC_REVISION_STR
								   *	@retval *pBuffer:	Destination pBuffer
								   *	@param  *pBufLen:	pBuffer Length
								   *	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
								   *	@return EAPI_STATUS_INVALID_PARAMETER: pBufLen==NULL/pBufLen!=NULL&&*pBufLen&&pBuffer==NULL
								   *	@return EAPI_STATUS_UNSUPPORTED: unknown Id
								   *	@return EAPI_STATUS_MORE_DATA: strlen(Id)+1>*pBufLen

								   *	@return EAPI_STATUS_SUCCESS: Else
								   */
	EApiStatus_t
		EAPI_CALLTYPE
		EApiBoardGetStringA(
			__IN EApiId_t Id,		/* Name Id */
			__OUT char *pBuffer,	/* Destination pBuffer */
			__INOUT uint32_t *pBufLen	/* pBuffer Length */
		);

	/*
	*
	* B O A R D I N F O M A T I O N V A L U E S
	*
	*/
	/* IDS */
#define EAPI_ID_GET_EAPI_SPEC_VERSION			EAPI_UINT32_C(0)	
	/* EAPI Specification
	* Revision I.E. The
	* EAPI Spec Version
	* Bits 31-24, Revision
	* 23-16, 15-0 always 0
	* Used to implement
	* this interface
	*/
#define EAPI_ID_BOARD_BOOT_COUNTER_VAL			EAPI_UINT32_C(1)	/* Units = Boots */
#define EAPI_ID_BOARD_RUNNING_TIME_METER_VAL	EAPI_UINT32_C(2)	/* Units = Minutes */
#define EAPI_ID_BOARD_PNPID_VAL					EAPI_UINT32_C(3)	
	/* Encoded PNP ID
	* Format
	* (Compressed ASCII)
	*/
#define EAPI_ID_BOARD_PLATFORM_REV_VAL			EAPI_UINT32_C(4)	
	/* Platform Revision
	* I.E. The PICMG Spec
	* Version Bits 31-24,
	* Revision 23-16,
	* 15-0 always 0
	*/
#define EAPI_ID_AONCUS_HISAFE_FUCTION			EAPI_UINT32_C(5)	
	/* Check function need
	* to turn on or off
	*/

#define EAPI_ID_BOARD_DRIVER_VERSION_VAL	EAPI_UINT32_C(0x10000)	
	/* Vendor Specific
	* (Optional)
	*/
#define EAPI_ID_BOARD_LIB_VERSION_VAL		EAPI_UINT32_C(0x10001)	
	/* Vendor Specific
	* (Optional)
	*/

#define EAPI_ID_HWMON_CPU_TEMP		EAPI_UINT32_C(0x20000) /* 0.1 Kelvins */
#define EAPI_ID_HWMON_CHIPSET_TEMP	EAPI_UINT32_C(0x20001) /* 0.1 Kelvins */
#define EAPI_ID_HWMON_SYSTEM_TEMP	EAPI_UINT32_C(0x20002) /* 0.1 Kelvins */

#define EAPI_KELVINS_OFFSET 2731
#define EAPI_ENCODE_CELCIUS(Celsius) EAPI_UINT32_C((((Celsius)*10))+EAPI_KELVINS_OFFSET)
#define EAPI_DECODE_CELCIUS(Celsius) ((Celsius)- EAPI_KELVINS_OFFSET)/10

#define EAPI_ID_HWMON_VOLTAGE_VCORE	EAPI_UINT32_C(0x21004) /* millivolts */
#define EAPI_ID_HWMON_VOLTAGE_2V5	EAPI_UINT32_C(0x21008) /* millivolts */
#define EAPI_ID_HWMON_VOLTAGE_3V3	EAPI_UINT32_C(0x2100C) /* millivolts */
#define EAPI_ID_HWMON_VOLTAGE_VBAT	EAPI_UINT32_C(0x21010) /* millivolts */
#define EAPI_ID_HWMON_VOLTAGE_5V	EAPI_UINT32_C(0x21014) /* millivolts */
#define EAPI_ID_HWMON_VOLTAGE_5VSB	EAPI_UINT32_C(0x21018) /* millivolts */
#define EAPI_ID_HWMON_VOLTAGE_12V	EAPI_UINT32_C(0x2101C) /* millivolts */
#define EAPI_ID_HWMON_VOLTAGE_DIMM	EAPI_UINT32_C(0x21020) /* millivolts */
#define EAPI_ID_HWMON_VOLTAGE_3VSB  EAPI_UINT32_C(0x21024) /* millivolts */		
#define EAPI_ID_HWMON_FAN_CPU		EAPI_UINT32_C(0x22000) /* RPM */
#define EAPI_ID_HWMON_FAN_CHIPSET	EAPI_UINT32_C(0x22001) /* RPM */
#define EAPI_ID_HWMON_FAN_SYSTEM	EAPI_UINT32_C(0x22002) /* RPM */
#define EAPI_ID_AONCUS_HISAFE_HWMON_GET_BASEADDR	EAPI_UINT32_C(0x23000)

	/*
	* EApiBoardGetValue
	*
	* Condition								 | Return Values
	* ---------------------------------------+------------------------------
	* Library Uninitialized					 | EAPI_STATUS_NOT_INITIALIZED
	* pValue==NULL							 | EAPI_STATUS_INVALID_PARAMETER
	* unknown Id							 | EAPI_STATUS_UNSUPPORTED
	* Common Error							 | Common Error Code
	* Else									 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@param  Id:			Value Id: EAPI_ID_HWMON_FAN_CPU
	*	@param  Id:			Value Id: EAPI_ID_HWMON_FAN_CHIPSET
	*	@param  Id:			Value Id: EAPI_ID_HWMON_FAN_SYSTEM

	*	@param  Id:			Value Id: EAPI_ID_HWMON_CPU_TEMP
	*	@param  Id:			Value Id: EAPI_ID_HWMON_CHIPSET_TEMP
	*	@param  Id:			Value Id: EAPI_ID_HWMON_SYSTEM_TEMP

	*	@param  Id:			Value Id: EAPI_ID_HWMON_VOLTAGE_VCORE
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_2V5
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_3V3
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_VBAT
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_5V
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_5VSB
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_12V
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_DIMM
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_3VSB

	*	@param  Id:			Value Id: EAPI_ID_BOARD_DRIVER_VERSION_VAL
	*	@param  Id:			Value Id: EAPI_ID_AONCUS_HISAFE_FUCTION
	*	@retval	*pValue:	Return Value
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: pValue==NULL
	*	@return EAPI_STATUS_UNSUPPORTED: unknown Id

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiBoardGetValue(
			__IN EApiId_t Id, /* Value Id */
			__OUT uint32_t *pValue /* Return Value */
		);

	/*
	*
	* I 2 C
	*
	*/
	/* IDs */
#define EAPI_ID_I2C_EXTERNAL			EAPI_UINT32_C(0) /* Baseboard first I2C Interface */
#define EAPI_ID_I2C_LVDS_1				EAPI_UINT32_C(1) /* LVDS/EDP 1 Interface (optional) */
#define EAPI_ID_I2C_LVDS_2				EAPI_UINT32_C(2) /* LVDS/EDP 2 Interface (optional) */
#define EAPI_ID_AONCUS_I2C_EXTERNAL_2	EAPI_UINT32_C(3) /* Baseboard 2nd I2C Interface */
#define EAPI_ID_AONCUS_I2C_EXTERNAL_3	EAPI_UINT32_C(4) /* Baseboard 3rd I2C Interface */
#define EAPI_ID_AONCUS_I2C_EXTERNAL_4	EAPI_UINT32_C(5) /* Baseboard 4rd I2C Interface */
#define EAPI_ID_AONCUS_I2C_EXTERNAL_5	EAPI_UINT32_C(6) /* Baseboard 5rd I2C Interface */
#define EAPI_ID_AONCUS_I2C_EXTERNAL_6	EAPI_UINT32_C(7) /* Baseboard 6rd I2C Interface */
#define EAPI_ID_AONCUS_I2C_EXTERNAL_7	EAPI_UINT32_C(8) /* Baseboard 7rd I2C Interface */
#define EAPI_ID_AONCUS_I2C_EXTERNAL_8	EAPI_UINT32_C(9) /* Baseboard 8rd I2C Interface */
#define EAPI_ID_AONCUS_SMBUS_EXTERNAL_1	EAPI_UINT32_C(10) /* Baseboard first SMBUS Interface */
#define EAPI_ID_AONCUS_SMBUS_EXTERNAL_2	EAPI_UINT32_C(11) /* Baseboard 2nd SMBUS Interface */
#define EAPI_ID_AONCUS_SMBUS_EXTERNAL_3	EAPI_UINT32_C(12) /* Baseboard 3rd SMBUS Interface */

	/*
	* I2C Address Format
	*
	* L = Set to 0
	* H = Set to 1
	* X = Don't Care(Direction Bit)
	* 0-F Address Bit
	*
	* Bits 31-16 are Reserved and should be set to 0
	*
	* Bit Offset	  | F E D C B A 9 8 7 6 5 4 3 2 1 0
	* ----------------+--------------------------------
	* 7 Bit Address   | L L L L L L L L 6 5 4 3 2 1 0 X
	* 10 Bit Address  | H H H H L 9 8 X 7 6 5 4 3 2 1 0
	*
	* Examples where Don't Care bits set to 0
	*			Encoded Encoded
	* Address	7Bit	10Bit
	*	0x01	0x02	0xF001
	*	0x58	0xA0	0xF058
	*	0x59	0xA2	0xF059
	*	0x77	0xEE	0xF077
	*	0x3FF			0xF6FF
	*
	*/
#define EAPI_I2C_ENC_7BIT_ADDR(x) EAPI_UINT8_C(((x)&0x07F)<<1)
#define EAPI_I2C_DEC_7BIT_ADDR(x) EAPI_UINT8_C(((x)>>1)&0x07F)

	/*
	* EApiI2CGetBusCap
	*
	* Condition								 | Return Values
	* ---------------------------------------+------------------------------
	* Library Uninitialized					 | EAPI_STATUS_NOT_INITIALIZED
	* pMaxBlkLen==NULL						 | EAPI_STATUS_INVALID_PARAMETER
	* unknown Id							 | EAPI_STATUS_UNSUPPORTED
	* Common Error							 | Common Error Code
	* Else									 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@brief	Not support this function
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiI2CGetBusCap(
			__IN EApiId_t Id, /* I2C Bus Id */
			__OUT uint32_t *pMaxBlkLen /* Max Block Length Supported on this interface */
		);
	/*
	* EApiI2CWriteRead
	*
	* Condition								   | Return Values
	* -----------------------------------------+------------------------------
	* Library Uninitialized					   | EAPI_STATUS_NOT_INITIALIZED
	* (WriteBCnt>1)&&(pWBuffer==NULL)		   | EAPI_STATUS_INVALID_PARAMETER
	* (ReadBCnt>1)&&(pRBuffer==NULL)		   | EAPI_STATUS_INVALID_PARAMETER
	* (ReadBCnt>1)&&(RBufLen==0)			   | EAPI_STATUS_INVALID_PARAMETER
	* ((WriteBCnt==0)&&(ReadBCnt==0))		   | EAPI_STATUS_INVALID_PARAMETER
	* unknown Id							   | EAPI_STATUS_UNSUPPORTED
	* WriteBCnt>(pMaxBlkLen+1)				   | EAPI_STATUS_INVALID_BLOCK_LENGTH
	* ReadBCnt>(pMaxBlkLen+1)				   | EAPI_STATUS_INVALID_BLOCK_LENGTH
	* Bus Busy SDA/SDC low					   | EAPI_STATUS_BUSY_COLLISION
	* Arbitration Error/Collision Error		   | EAPI_STATUS_BUSY_COLLISION
	* On Write 1 write cycle				   |
	* SDA Remains low						   |
	* Timeout due to clock stretching		   | EAPI_STATUS_TIMEOUT
	* start<Addr Byte><W>Nak				   | EAPI_STATUS_NOT_FOUND
	* start<Addr Byte><R>Nak				   | EAPI_STATUS_NOT_FOUND
	* start<Addr Byte><W>Ack<Data Byte 1>Nak   | EAPI_STATUS_WRITE_ERROR
	* ReadBCnt>(RBufLen+1)					   | EAPI_STATUS_MORE_DATA
	* Common Error							   | Common Error Code
	* Else									   | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@brief	Not support this function
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiI2CWriteReadRaw(
			__IN EApiId_t Id, /* I2C Bus Id */
			__IN uint8_t Addr, /* Encoded 7Bit I2C Device Address */
			__INOPT void *pWBuffer, /* Write Data pBuffer */
			__IN uint32_t WriteBCnt, /* Number of Bytes to write plus 1 */
			__OUTOPT void *pRBuffer, /* Read Data pBuffer */
			__IN uint32_t RBufLen, /* Data pBuffer Length */
			__IN uint32_t ReadBCnt /* Number of Bytes to Read plus 1 */
		);

#define EApiI2CWriteRaw(Id, Addr, pBuffer, ByteCnt) \
			EApiI2CWriteReadRaw(Id, Addr, pBuffer, ByteCnt, NULL, 0, 0)
#define EApiI2CReadRaw(Id, Addr, pBuffer, BufLen, ByteCnt) \
			EApiI2CWriteReadRaw(Id, Addr, NULL, 0, pBuffer, BufLen, ByteCnt)
#define EAPI_I2C_ENC_10BIT_ADDR(x)	EAPI_UINT32_C(((x)&0xFF)|(((x)&0x0300)<<1)|0xF000)
#define EAPI_I2C_DEC_10BIT_ADDR(x)	EAPI_UINT32_C(((x)&0xFF)|(((x)>>1)&0x300))
#define EAPI_I2C_IS_10BIT_ADDR(x)	(((x)&0xF800)==0xF000)
#define EAPI_I2C_IS_7BIT_ADDR(x)	(!EAPI_I2C_IS_10BIT_ADDR(x))

	/*
	* I2C Transfer Types
	* Bits 31 & 30 Selects Command Type
	*
	* Transfer Type 1:
	* Address Format : 7Bit
	* Command Type : None
	* Data Direction : Write
	* Start<Addr Byte><W>Ack<Data Byte 1>Ack Stop
	*
	* Transfer Type 2:
	* Address Format : 7Bit
	* Command Type : None
	* Data Direction : Read
	* Start<Addr Byte><R>Ack<Data Byte 1>Nak Stop
	*
	* Transfer Type 3:
	* Address Format : 7Bit
	* Command Type : Standard
	* Data Direction : Write
	* Start<Addr Byte><W>Ack<CMD Byte>Ack<Data Byte 1>Ack Stop
	*
	* Transfer Type 4:
	* Address Format : 7Bit
	* Command Type : Standard
	* Data Direction : Read
	* Start<Addr Byte><W>Ack<CMD Byte>Ack
	* Start<Addr Byte><R>Ack<Data Byte 1>Nak Stop
	*
	* Transfer Type 5:
	* Address Format : 7Bit
	* Command Type : Extended
	* Data Direction : Write
	* Start<Addr Byte><W>Ack<CMD Byte MSB>Ack<CMD Byte LSB>Ack<Data Byte 1>Ack Stop
	*
	* Transfer Type 6:
	* Address Format : 7Bit
	* Command Type : Extended
	* Data Direction : Read
	* Start<Addr Byte><W>Ack<CMD Byte MSB>Ack<CMD Byte LSB>Ack
	* Start<Addr Byte><R>Ack<Data Byte 1>Nak Stop
	*
	* Transfer Type 7:
	* Address Format : 10Bit
	* Command Type : None
	* Data Direction : Write
	* Start<Addr Byte MSB><W>Ack<Addr Byte LSB>Ack<Data Byte 1>Ack Stop
	*
	* Transfer Type 8:
	* Address Format : 10Bit
	* Command Type : None
	* Data Direction : Read
	* Start<Addr Byte MSB><W>Ack<Addr Byte LSB>Ack
	* Start<Addr Byte MSB><R>Ack<Data Byte 1>Nak Stop
	*
	* Transfer Type 9:
	* Address Format : 10Bit
	* Command Type : Standard
	* Data Direction : Write
	* Start<Addr Byte MSB><W>Ack<Addr Byte LSB>Ack<CMD Byte>Ack<Data Byte 1>Ack Stop
	*
	* Transfer Type 10:
	* Address Format : 10Bit
	* Command Type : Standard
	* Data Direction : Read
	* Start<Addr Byte MSB><W>Ack<Addr Byte LSB>Ack<CMD Byte>Ack
	* Start<Addr Byte MSB><R>Ack<Data Byte 1>Nak Stop
	*
	* Transfer Type 11:
	* Address Format : 10Bit
	* Command Type : Extended
	* Data Direction : Write
	* Start<Addr Byte MSB><W>Ack<Addr Byte LSB>Ack<CMD Byte MSB>Ack<CMD Byte LSB>Ack<Data Byte1>Ack Stop
	*
	* Transfer Type 12:
	* Address Format : 10Bit
	* Command Type : Extended
	* Data Direction : Read
	* Start<Addr Byte MSB><W>Ack<Addr Byte LSB>Ack<CMD Byte MSB>Ack<CMD Byte LSB>Ack
	* Start<Addr Byte MSB><R>Ack<Data Byte 1>Nak Stop
	*
	*/
#define EAPI_I2C_STD_CMD		EAPI_UINT32_C(0<<30)
#define EAPI_I2C_EXT_CMD		EAPI_UINT32_C(2<<30)
#define EAPI_I2C_NO_CMD			EAPI_UINT32_C(1<<30)
#define EAPI_I2C_CMD_TYPE_MASK	EAPI_UINT32_C(3<<30)

#define EAPI_I2C_ENC_STD_CMD(x)	EAPI_UINT32_C(((x)&0xFF)|EAPI_I2C_STD_CMD)
#define EAPI_I2C_ENC_EXT_CMD(x)	EAPI_UINT32_C(((x)&0xFFFF)|EAPI_I2C_EXT_CMD)
#define EAPI_I2C_IS_EXT_CMD(x) (EAPI_UINT32_C((x)&(EAPI_I2C_CMD_TYPE_MASK))==EAPI_I2C_EXT_CMD)
#define EAPI_I2C_IS_STD_CMD(x) (EAPI_UINT32_C((x)&(EAPI_I2C_CMD_TYPE_MASK))==EAPI_I2C_STD_CMD)
#define EAPI_I2C_IS_NO_CMD(x) (EAPI_UINT32_C((x)&(EAPI_I2C_CMD_TYPE_MASK))==EAPI_I2C_NO_CMD)

	/*
	* EApiI2CReadTransfer
	* Addr Byte 1 Below Designates Addr MSB in a 10bit address transfer and
	* the complete address in an 7bit address transfer.
	*
	* Condition								   | Return Values
	* -----------------------------------------+------------------------------
	* Library Uninitialized					   | EAPI_STATUS_NOT_INITIALIZED
	* pBuffer==NULL							   | EAPI_STATUS_INVALID_PARAMETER
	* BufLen==0								   | EAPI_STATUS_INVALID_PARAMETER
	* ByteCnt==0							   | EAPI_STATUS_INVALID_PARAMETER
	* unknown Id							   | EAPI_STATUS_UNSUPPORTED
	* ByteCnt>pMaxBlkLen					   | EAPI_STATUS_INVALID_BLOCK_LENGTH
	* Bus Busy SDA/SDC low					   | EAPI_STATUS_BUSY_COLLISION
	* Arbitration Error/Collision Error		   | EAPI_STATUS_BUSY_COLLISION
	* On Write 1 write cycle				   |
	* SDA Remains low						   |
	* Time-out due to clock stretching		   | EAPI_STATUS_TIMEOUT
	* start<Addr Byte 1><W>Nak				   | EAPI_STATUS_NOT_FOUND
	* start<Addr Byte 1><R>Nak				   | EAPI_STATUS_NOT_FOUND
	* start<Addr Byte 1><W>Ack<Addr Byte 2>Nak | EAPI_STATUS_WRITE_ERROR or
	*										   | EAPI_STATUS_NOT_FOUND
	* start<Addr Byte 1><W>Ack<CMD Byte 1>Nak  | EAPI_STATUS_WRITE_ERROR
	* start<Addr Byte 1><W>Ack<Data Byte 1>Nak | EAPI_STATUS_WRITE_ERROR
	* ByteCnt>BufLen						   | EAPI_STATUS_MORE_DATA
	* Common Error							   | Common Error Code
	* Else									   | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@param	Id:			I2C Bus Id: EAPI_ID_I2C_EXTERNAL
	*	@param	Id:			I2C Bus Id: EAPI_ID_SMBUS_EXTERNAL
	*	@param	Addr:		Encoded 7/10Bit I2C Device Address
	*	@param	Cmd:		I2C Command/Offset
	*	@retval	*pBuffer:	Transfer Data pBuffer
	*	@param	BufLen		Data pBuffer Length: Byte=1; Word=2
	*	@param  ByteCnt		Byte Count to read:	Byte=1; Word=2
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: pBuffer==NULL/BufLen==0/ByteCnt==0
	*	@return EAPI_STATUS_UNSUPPORTED: unknown Id
	*	@return EAPI_STATUS_INVALID_BLOCK_LENGTH: ByteCnt>pMaxBlkLen
	*	@return EAPI_STATUS_BUSY_COLLISION: Bus Busy SDA/SDC low/Arbitration Error/Collision Error
	*	@return EAPI_STATUS_TIMEOUT: Time-out due to clock stretching
	*	@return EAPI_STATUS_NOT_FOUND: start<Addr Byte 1><R>Nak

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiI2CReadTransfer(
			__IN EApiId_t Id, /* I2C Bus Id */
			__IN uint32_t Addr, /* Encoded 7/10Bit I2C Device Address */
			__IN uint32_t Cmd, /* I2C Command/Offset */
			__OUT void *pBuffer, /* Transfer Data pBuffer */
			__IN uint32_t BufLen, /* Data pBuffer Length */
			__IN uint32_t ByteCnt /* Byte Count to read */
		);
	/*
	* EApiI2CWriteTransfer
	* Addr Byte 1 Below Designates Addr MSB in a 10bit address transfer and
	* the complete address in an 7bit address transfer.
	*
	* Condition								   | Return Values
	* -----------------------------------------+------------------------------
	* Library Uninitialized					   | EAPI_STATUS_NOT_INITIALIZED
	* pBuffer==NULL							   | EAPI_STATUS_INVALID_PARAMETER
	* ByteCnt==0							   | EAPI_STATUS_INVALID_PARAMETER
	* unknown Id							   | EAPI_STATUS_UNSUPPORTED
	* ByteCnt+(overhead)>pMaxBlkLen			   | EAPI_STATUS_INVALID_BLOCK_LENGTH
	* Bus Busy SDA/SDC low					   | EAPI_STATUS_BUSY_COLLISION
	* Arbitration Error/Collision Error		   | EAPI_STATUS_BUSY_COLLISION
	* On Write 1 write cycle				   |
	* SDA Remains low						   |
	* Time-out due to clock stretching		   | EAPI_STATUS_TIMEOUT
	* start<Addr Byte 1><W>Nak				   | EAPI_STATUS_NOT_FOUND
	* start<Addr Byte 1><R>Nak				   | EAPI_STATUS_NOT_FOUND
	* start<Addr Byte 1><W>Ack<Addr Byte 2>Nak | EAPI_STATUS_WRITE_ERROR or
	*										   | EAPI_STATUS_NOT_FOUND
	* start<Addr Byte 1><W>Ack<CMD Byte 1>Nak  | EAPI_STATUS_WRITE_ERROR
	* start<Addr Byte 1><W>Ack<Data Byte 1>Nak | EAPI_STATUS_WRITE_ERROR
	* Common Error							   | Common Error Code
	* Else									   | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@param	Id:			I2C Bus Id: EAPI_ID_I2C_EXTERNAL
	*	@param	Id:			I2C Bus Id: EAPI_ID_SMBUS_EXTERNAL
	*	@param	Addr:		Encoded 7/10Bit I2C Device Address
	*	@param	Cmd:		I2C Command/Offset
	*	@param	*pBuffer	Transfer Data pBuffer
	*	@param  ByteCnt		Byte Count to write: Byte=1; Word=2
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: pBuffer==NULL/BufLen==0/ByteCnt==0
	*	@return EAPI_STATUS_UNSUPPORTED: unknown Id
	*	@return EAPI_STATUS_INVALID_BLOCK_LENGTH: ByteCnt+(overhead)>pMaxBlkLen
	*	@return EAPI_STATUS_BUSY_COLLISION: Bus Busy SDA/SDC low/Arbitration Error/Collision Error
	*	@return EAPI_STATUS_TIMEOUT: Time-out due to clock stretching
	*	@return EAPI_STATUS_NOT_FOUND: start<Addr Byte 1><W>Nak
	*	@return EAPI_STATUS_WRITE_ERROR: start<Addr Byte 1><W>Ack<Addr Byte 2>Nak/<CMD Byte 1>Nak/<Data Byte 1>Nak

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiI2CWriteTransfer(
			__IN EApiId_t Id, /* I2C Bus Id */
			__IN uint32_t Addr, /* Encoded 7/10Bit I2C Device Address */
			__IN uint32_t Cmd, /* I2C Command/Offset;  Seq&Page Mode-->ByteCnt*/
			__IN void *pBuffer, /* Transfer Data pBuffer */
			__IN uint32_t ByteCnt /* Byte Count to write */
		);
	/*
	* I2C Probe Types
	*
	* Probe Type 1:
	* Address Format : 7Bit
	* Start<Addr Byte><W>Ack Stop
	*
	* Probe Type 2:
	* Address Format : 10Bit
	* Start<Addr Byte MSB><W>Ack<Addr Byte LSB>Ack Stop
	*
	*/
	/*
	* EApiI2CProbeDevice
	*
	* Condition									   | Return Values
	* ---------------------------------------------+------------------------------
	* Library Uninitialized						   | EAPI_STATUS_NOT_INITIALIZED
	* Bus Busy SDA/SDC low						   | EAPI_STATUS_BUSY_COLLISION
	* Arbitration Error/Collision Error			   | EAPI_STATUS_BUSY_COLLISION
	* On Write 1 write cycle					   |
	* SDA Remains low							   |
	* Time-out due to clock stretching			   | EAPI_STATUS_TIMEOUT
	*											   |
	* 7Bit Address								   |
	* start<Addr Byte><W>Nak					   | EAPI_STATUS_NOT_FOUND
	*											   |
	* 10Bit Address								   |
	* start<Addr Byte MSB><W>Nak				   | EAPI_STATUS_NOT_FOUND
	* start<Addr Byte MSB><W>Ack<Addr Byte LSB>Nak | EAPI_STATUS_NOT_FOUND
	*											   |
	* Common Error								   | Common Error Code
	* Else										   | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@brief	Not support this function
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiI2CProbeDevice(
			__IN EApiId_t Id, /* I2C Bus Id */
			__IN uint32_t Addr /* Encoded 7/10Bit I2C Device Address */
		);
	
	EApiStatus_t
		EAPI_CALLTYPE
		EApiI2CSetSpeed(
			__IN EApiId_t Id, /* I2C Bus Id */
			__IN uint32_t Speed /* I2C Speed Setting */
		);
	/*
	*
	* G P I O
	*
	*/
	/* IDs */
	/*
	* Individual ID Per GPIO Mapping
	*/
#define EAPI_GPIO_GPIO_ID(GPIO_NUM)	EAPI_UINT32_C(GPIO_NUM)
#define EAPI_GPIO_GPIO_BITMASK		EAPI_UINT32_C(1)
#define EAPI_ID_GPIO_GPIO00			EAPI_GPIO_GPIO_ID(0) /* (Optional) */
#define EAPI_ID_GPIO_GPIO01			EAPI_GPIO_GPIO_ID(1) /* (Optional) */
#define EAPI_ID_GPIO_GPIO02			EAPI_GPIO_GPIO_ID(2) /* (Optional) */
#define EAPI_ID_GPIO_GPIO03			EAPI_GPIO_GPIO_ID(3) /* (Optional) */
#define EAPI_ID_GPIO_GPIO04			EAPI_GPIO_GPIO_ID(4) /* (Optional) */
#define EAPI_ID_GPIO_GPIO05			EAPI_GPIO_GPIO_ID(5) /* (Optional) */
#define EAPI_ID_GPIO_GPIO06			EAPI_GPIO_GPIO_ID(6) /* (Optional) */
#define EAPI_ID_GPIO_GPIO07			EAPI_GPIO_GPIO_ID(7) /* (Optional) */

	/*
	* Multiple GPIOs Per ID Mapping
	*/
#define EAPI_GPIO_BANK_ID(GPIO_NUM)		EAPI_UINT32_C(0x10000|((GPIO_NUM)>>5))
#define EAPI_GPIO_BANK_MASK(GPIO_NUM)	EAPI_UINT32_C((1<<((GPIO_NUM)&0x1F))
#define EAPI_GPIO_BANK_TEST_STATE(GPIO_NUM, TState, TValue) \
								(((TValue>>((GPIO_NUM)&0x1F))&1)==(TState))
#define EAPI_ID_GPIO_BANK00 EAPI_GPIO_BANK_ID( 0) /* GPIOs 0 - 31 (optional) */
#define EAPI_ID_GPIO_BANK01 EAPI_GPIO_BANK_ID(32) /* GPIOs 32 - 63 (optional) */
#define EAPI_ID_GPIO_BANK02 EAPI_GPIO_BANK_ID(64) /* GPIOs 64 - 95 (optional) */

	/* Bit mask Bit States */
#define EAPI_GPIO_BITMASK_SELECT	1
#define EAPI_GPIO_BITMASK_NOSELECT	0
	/* Levels */
#define EAPI_GPIO_LOW				0
#define EAPI_GPIO_HIGH				1
	/* Directions */
#define EAPI_GPIO_INPUT				1
#define EAPI_GPIO_OUTPUT			0

	/*
	* EApiGPIOGetDirectionCaps
	*
	* Condition							 | Return Values
	* -----------------------------------+------------------------------
	* Library Uninitialized				 | EAPI_STATUS_NOT_INITIALIZED
	* ((pOutputs==NULL)&&(pInputs==NULL))| EAPI_STATUS_INVALID_PARAMETER
	* Unsupported Id					 | EAPI_STATUS_UNSUPPORTED
	* Common Error						 | Common Error Code
	* Else								 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@brief	Not support this function
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiGPIOGetDirectionCaps(
			__IN EApiId_t Id, /* GPIO Id */
			__OUTOPT uint32_t *pInputs, /* Supported GPIO Input Bit Mask */
			__OUTOPT uint32_t *pOutputs /* Supported GPIO Output Bit Mask */
		);
	/*
	* EApiGPIOGetDirection
	*
	* Condition							 | Return Values
	* -----------------------------------+------------------------------
	* Library Uninitialized				 | EAPI_STATUS_NOT_INITIALIZED
	* Bitmask==0						 | EAPI_STATUS_INVALID_PARAMETER
	* Unsupported Id					 | EAPI_STATUS_UNSUPPORTED
	* (Bitmask&~(pInputs|pOutputs))		 | EAPI_STATUS_INVALID_BITMASK
	* Common Error						 | Common Error Code
	* Else								 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@param	Id:				GPIO Id: EAPI_GPIO_GPIO_ID(GPIO_NUM)
	*	@param	Bitmask:		Bit mask of Affected Bits: 0xFFFFFFFF
	*	@retval	*pDirection:	Current Direction
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: Bitmask==0
	*	@return EAPI_STATUS_UNSUPPORTED: Unsupported Id
	*	@return EAPI_STATUS_INVALID_BITMASK: (Bitmask&~(pInputs|pOutputs))

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiGPIOGetDirection(
			__IN EApiId_t Id, /* GPIO Id */
			__IN uint32_t Bitmask, /* Bit mask of Affected Bits */
			__OUT uint32_t *pDirection /* Current Direction */
		);
	/*
	* EApiGPIOSetDirection
	*
	* Condition							 | Return Values
	* -----------------------------------+------------------------------
	* Library Uninitialized				 | EAPI_STATUS_NOT_INITIALIZED
	* Bitmask==0						 | EAPI_STATUS_INVALID_PARAMETER
	* Unsupported Id					 | EAPI_STATUS_UNSUPPORTED
	* (Bitmask&~(pInputs|pOutputs))		 | EAPI_STATUS_INVALID_BITMASK
	* (Bitmask&pDirection)&~pInputs		 | EAPI_STATUS_INVALID_DIRECTION
	* (Bitmask&~pDirection)&~pOutputs	 | EAPI_STATUS_INVALID_DIRECTION
	* Common Error						 | Common Error Code
	* Else								 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@param	Id:				GPIO Id: EAPI_GPIO_GPIO_ID(GPIO_NUM)
	*	@param	Bitmask:		Bit mask of Affected Bits: 0xFFFFFFFF
	*	@param	Direction:		Direction
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: Bitmask==0
	*	@return EAPI_STATUS_UNSUPPORTED: Unsupported Id
	*	@return EAPI_STATUS_INVALID_BITMASK: (Bitmask&~(pInputs|pOutputs))
	*	@return EAPI_STATUS_INVALID_DIRECTION: (Bitmask&pDirection)&~pInputs/(Bitmask&~pDirection)&~pOutputs

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiGPIOSetDirection(
			__IN EApiId_t Id, /* GPIO Id */
			__IN uint32_t Bitmask, /* Bit mask of Affected Bits */
			__IN uint32_t Direction /* Direction */
		);
	/*
	* EApiGPIOGetLevel
	*
	* Condition							 | Return Values
	* -----------------------------------+------------------------------
	* Library Uninitialized				 | EAPI_STATUS_NOT_INITIALIZED
	* Bitmask==0						 | EAPI_STATUS_INVALID_PARAMETER
	* Unsupported Id					 | EAPI_STATUS_UNSUPPORTED
	* (Bitmask&~(pInputs|pOutputs))		 | EAPI_STATUS_INVALID_BITMASK
	* Common Error						 | Common Error Code
	* Else								 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@param	Id:				GPIO Id: EAPI_GPIO_GPIO_ID(GPIO_NUM)
	*	@param	Bitmask:		Bit mask of Affected Bits: 0xFFFFFFFF
	*	@retval	*pLevel:		Current Level
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: Bitmask==0
	*	@return EAPI_STATUS_UNSUPPORTED: Unsupported Id
	*	@return EAPI_STATUS_INVALID_BITMASK: (Bitmask&~(pInputs|pOutputs))

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiGPIOGetLevel(
			__IN EApiId_t Id, /* GPIO Id */
			__IN uint32_t Bitmask, /* Bit mask of Affected Bits */
			__OUT uint32_t *pLevel /* Current Level */
		);
	/*
	* EApiGPIOSetLevel
	*
	* Condition							 | Return Values
	* -----------------------------------+------------------------------
	* Library Uninitialized				 | EAPI_STATUS_NOT_INITIALIZED
	* Bitmask==0						 | EAPI_STATUS_INVALID_PARAMETER
	* Unsupported Id					 | EAPI_STATUS_UNSUPPORTED
	* Common Error						 | Common Error Code
	* Else								 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@param	Id:				GPIO Id: EAPI_GPIO_GPIO_ID(GPIO_NUM)
	*	@param	Bitmask:		Bit mask of Affected Bits: 0xFFFFFFFF
	*	@param	Level:			Level
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: Bitmask==0
	*	@return EAPI_STATUS_UNSUPPORTED: Unsupported Id

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiGPIOSetLevel(
			__IN EApiId_t Id, /* GPIO Id */
			__IN uint32_t Bitmask, /* Bit mask of Affected Bits */
			__IN uint32_t Level /* Level */
		);

	/*
	*
	* AAEON Customize Function
	*
	*/

	/**
	*	@param	Id:				GPIO Id: EAPI_GPIO_GPIO_ID(GPIO_NUM)
	*	@retval	*PinCount:		Pin Count
	*	@retval	*pDioDisable:	Pin Enable or Disable 1:Disable; 0:Enable
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: Bitmask==0
	*	@return EAPI_STATUS_UNSUPPORTED: Unsupported Id
	*	@return EAPI_STATUS_INVALID_BITMASK: (Bitmask&~(pInputs|pOutputs))

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiGPIOGetCaps(
			__IN EApiId_t Id, /* GPIO Id */
			__OUTOPT uint32_t *PinCount, /* PinCount */
			__OUTOPT uint32_t *pDioDisable /* Pin Enable or Disable: 1:Disable; 0:Enable */
		);


	/**
	*	@param  Id:			Value Id: EAPI_ID_HWMON_FAN_CPU
	*	@param  Id:			Value Id: EAPI_ID_HWMON_FAN_CHIPSET
	*	@param  Id:			Value Id: EAPI_ID_HWMON_FAN_SYSTEM

	*	@param  Id:			Value Id: EAPI_ID_HWMON_CPU_TEMP
	*	@param  Id:			Value Id: EAPI_ID_HWMON_CHIPSET_TEMP
	*	@param  Id:			Value Id: EAPI_ID_HWMON_SYSTEM_TEMP

	*	@param  Id:			Value Id: EAPI_ID_HWMON_VOLTAGE_VCORE
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_2V5
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_3V3
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_VBAT
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_5V
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_5VSB
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_12V
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_DIMM
	*	@param  Id:			Value Id:EAPI_ID_HWMON_VOLTAGE_3VSB

	*	@param  Id:			Value Id: EAPI_ID_BOARD_DRIVER_VERSION_VAL
	*	@param  Id:			Value Id: EAPI_ID_AONCUS_HISAFE_FUCTION
	*	@retval	*pFanEnable:	FanEnable 1:Enable; 0:Disable
	*	@retval	*pTempEnable:	TempEnable 1:Enable; 0:Disable
	*	@retval	*pVoltEnable:	VoltEnable 1:Enable; 0:Disable
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_UNSUPPORTED: unknown Id

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiHWMONGetCaps(
			__IN EApiId_t Id, /* HWMON Id */
			__OUT uint32_t *pFanEnable,
			__OUT uint32_t *pTempEnable,
			__OUT uint32_t *pVoltEnable
		);


	/*
	*
	* P W M
	*
	*/
	/* IDS */
#define EAPI_ID_PWM_1 EAPI_UINT32_C(0)
#define EAPI_ID_PWM_2 EAPI_UINT32_C(1)
#define EAPI_ID_PWM_3 EAPI_UINT32_C(2)

	/*
	* EApiPwmGetValue
	*
	* Condition								 | Return Values
	* ---------------------------------------+------------------------------
	* Library Uninitialized					 | EAPI_STATUS_NOT_INITIALIZED
	* pPWMBaseUnitInt==NULL ||				 |
	* pPWMBaseUnitFrac==NULL ||				 |
	* pPWMDutyCycle==NULL					 | EAPI_STATUS_INVALID_PARAMETER
	* unknown Id							 | EAPI_STATUS_UNSUPPORTED
	* Common Error							 | Common Error Code
	* Else									 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@param  Id:		PWM Id: EAPI_ID_PWM_1
	*	@param  Id:		PWM Id: EAPI_ID_PWM_2
	*	@param  Id:		PWM Id: EAPI_ID_PWM_3
	*	@retval *pPWMBaseUnitInt:	Get the integer portion of Base Unit
	*	@retval *pPWMBaseUnitFrac:	Get the fractional portion of Base Unit
	*	@retval *pPWMDutyCycle:	Get the duty cycle
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: pPWMBaseUnitInt==NULL or pPWMBaseUnitFrac==NULL or pPWMDutyCycle==NULL
	*	@return EAPI_STATUS_UNSUPPORTED: unknown Id

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiPwmGetValue(
			__IN EApiId_t Id, /* PWM Id */
			__OUT uint32_t *pPWMBaseUnitInt, /* PWM Base Unit Integer */
			__OUT uint32_t *pPWMBaseUnitFrac, /* PWM Base Unit Integer */
			__OUT uint32_t *pPWMDutyCycle /* PWM Base Unit Integer */
		);
	/*
	* EApiPwmSetValue
	*
	* Condition								 | Return Values
	* ---------------------------------------+------------------------------
	* Library Uninitialized					 | EAPI_STATUS_NOT_INITIALIZED
	* unknown Id							 | EAPI_STATUS_UNSUPPORTED
	* Common Error							 | Common Error Code
	* Else									 | EAPI_STATUS_SUCCESS
	*/
	/**
	*	@param  Id:		PWM Id: EAPI_ID_PWM_1
	*	@param  Id:		PWM Id: EAPI_ID_PWM_2
	*	@param  Id:		PWM Id: EAPI_ID_PWM_3
	*	@retval PWMBaseUnitInt:	Set the integer portion of Base Unit
	*	@retval PWMBaseUnitFrac: Set the fractional portion of Base Unit
	*	@retval PWMDutyCycle: Set the duty cycle
	*	@return EAPI_STATUS_NOT_INITIALIZED: Library Uninitialized
	*	@return EAPI_STATUS_INVALID_PARAMETER: Bright>EAPI_BACKLIGHT_SET_BRIGHTEST
	*	@return EAPI_STATUS_UNSUPPORTED: unknown Id

	*	@return EAPI_STATUS_SUCCESS: Else
	*/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiPwmSetValue(
			__IN EApiId_t Id, /* PWM Id */
			__IN uint32_t PWMBaseUnitInt, /* PWM Base Unit Integer */
			__IN uint32_t PWMBaseUnitFrac, /* PWM Base Unit Integer */
			__IN uint32_t PWMDutyCycle /* PWM Base Unit Integer */
		);
	
	/****************************************************************************/
	EApiStatus_t
		EAPI_CALLTYPE
		EApiSpiSetSpeed(uint8_t Id, uint32_t speed);
	
	EApiStatus_t
		EAPI_CALLTYPE
		EApiSpiDataBitLength(uint8_t Id, uint8_t DataBitLen);

	EApiStatus_t
		EAPI_CALLTYPE
		EApiSpiReadRaw(uint8_t Id, uint32_t mode, uint8_t ss, uint8_t* pIn, uint32_t inLn, uint8_t* pOut, uint32_t outLen);

	EApiStatus_t
		EAPI_CALLTYPE
		EApiSpiWriteRaw(uint8_t Id, uint32_t mode, uint8_t ss, uint8_t* pIn, uint32_t len);

	EApiStatus_t
		EAPI_CALLTYPE
		EApiCpldReadWrite(
			__IN EApiId_t ReadWrite, /* 0=Write 1=Read */
			__IN uint32_t Address, /* CPLD Address */
			__IN uint64_t RegData, /* Data */
			__OUT uint64_t *DataOut /* PWM Base Unit Integer */
		);
	EApiStatus_t
		EAPI_CALLTYPE
		EApiWDTStart(
			__IN uint32_t timeout
		);
	EApiStatus_t
		EAPI_CALLTYPE
		EApiWDTStop(
		);
	EApiStatus_t
		EAPI_CALLTYPE
		EApiWDTReload(
		);
	EApiStatus_t
		EAPI_CALLTYPE
		EApiWDTGet(
			__OUT uint32_t *timeout
		);
}