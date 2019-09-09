/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2013 Semtech-Cycleo

Description:
    Host specific functions to address the LoRa concentrator registers through
    a SPI interface.
    Single-byte read/write and burst read/write.
    Does not handle pagination.
    Could be used with multiple SPI ports in parallel (explicit file descriptor)

License: Revised BSD License, see LICENSE.TXT file include in the project
Maintainer: Sylvain Miermont
*/


/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */
#include <ppltasks.h>
#include <collection.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cwctype>

#include "loragw_spi.h"
#include "loragw_hal.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Devices::Spi;

class wexception
{
public:
	explicit wexception(const std::wstring &msg) : msg_(msg) { }
	virtual ~wexception() { /*empty*/ }

	virtual const wchar_t *wwhat() const
	{
		return msg_.c_str();
	}

private:
	std::wstring msg_;
};

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#if DEBUG_SPI == 1
    #define DEBUG_MSG(str)               fprintf(stderr, str)
	#define DEBUG_PRINTF(fmt, ...)		 fprintf(stderr,fmt, ##__VA_ARGS__)
    #define CHECK_NULL(a)                if(a==NULL){fprintf(stderr,"%s:%d: ERROR: NULL POINTER AS ARGUMENT\n", __FUNCTION__, __LINE__);return LGW_SPI_ERROR;}
#else
    #define DEBUG_MSG(str)
    //#define DEBUG_PRINTF(fmt, args...)
	#define DEBUG_PRINTF(fmt, ...)
    #define CHECK_NULL(a)                if(a==NULL){return LGW_SPI_ERROR;}
#endif

extern SpiDevice^ lgw_spi_target;
/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

#define READ_ACCESS     0x00
#define WRITE_ACCESS    0x80
//#define SPI_SPEED       8000000
//#define SPI_DEV_PATH    "/dev/spidev0.0"
//#define SPI_DEV_PATH    "/dev/spidev32766.0"

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ------------------------------------------ */

/* SPI initialization and configuration */
int lgw_spi_open(SpiDevice^ spi_target_ptr) {
	try {
		using namespace Windows::Devices::Enumeration;

		String^ aqs = SpiDevice::GetDeviceSelector(SPI_DEV_PATH);
		auto dis = concurrency::create_task(DeviceInformation::FindAllAsync(aqs)).get();
		if (dis->Size < 1)
		{
			DEBUG_PRINTF("DBG: %s device not found\n", SPI_DEV_PATH);
			return LGW_SPI_ERROR;
		}

		String^ id = dis->GetAt(0)->Id;

		auto settings = ref new SpiConnectionSettings(0);
		settings->Mode = SpiMode(0);
		settings->DataBitLength = 8;
		settings->ClockFrequency = SPI_SPEED;

		auto device = concurrency::create_task(SpiDevice::FromIdAsync(id, settings)).get();

		if (!device)
		{
			DEBUG_MSG("ERROR: SPI PORT FAILED TO OPEN\n");
			return LGW_SPI_ERROR;
		}
		lgw_spi_target = device;
		
		if(!spi_target_ptr)
			DEBUG_MSG("ERROR: spi_target_ptr NULL\n");
	}
	catch (const wexception& ex) {
		DEBUG_PRINTF("ERROR: what %s\n", ex.wwhat());
		return LGW_SPI_ERROR;
	}
	catch (Platform::Exception^ ex) {
		DEBUG_PRINTF("ERROR: data %s\n", ex->Message->Data());
		return LGW_SPI_ERROR;
	}

    return LGW_SPI_SUCCESS;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* SPI release */
int lgw_spi_close(SpiDevice^ spi_target) {

	return LGW_SPI_SUCCESS;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Simple write */
int lgw_spi_w(SpiDevice^ spi_target, uint8_t spi_mux_mode, uint8_t spi_mux_target, uint8_t address, uint8_t data) {
    uint8_t out_buf[3];
    uint8_t command_size;

    /* prepare frame to be sent */
    if (spi_mux_mode == LGW_SPI_MUX_MODE1) {
        out_buf[0] = spi_mux_target;
        out_buf[1] = WRITE_ACCESS | (address & 0x7F);
        out_buf[2] = data;
        command_size = 3;
    } else {
        out_buf[0] = WRITE_ACCESS | (address & 0x7F);
        out_buf[1] = data;
        command_size = 2;
    }

	/* check input variables */
	if ((address & 0x80) != 0) {
		DEBUG_MSG("WARNING: SPI address > 127\n");
	}

	/* I/O transaction */
	spi_target->Write(ArrayReference<BYTE>(out_buf, static_cast<unsigned int>(command_size)));
	return LGW_SPI_SUCCESS;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Simple read */
int lgw_spi_r(SpiDevice^ spi_target, uint8_t spi_mux_mode, uint8_t spi_mux_target, uint8_t address, uint8_t *data) {
    uint8_t out_buf[3];
    uint8_t command_size;
    uint8_t in_buf[ARRAY_SIZE(out_buf)];
  
    /* check input variables */
    if ((address & 0x80) != 0) {
        DEBUG_MSG("WARNING: SPI address > 127\n");
    }

    /* prepare frame to be sent */
    if (spi_mux_mode == LGW_SPI_MUX_MODE1) {
        out_buf[0] = spi_mux_target;
        out_buf[1] = READ_ACCESS | (address & 0x7F);
        out_buf[2] = 0x00;
        command_size = 2;
    } else {
        out_buf[0] = READ_ACCESS | (address & 0x7F);
        out_buf[1] = 0x00;
        command_size = 1;
    }

    /* I/O transaction */
	auto readBuf = ref new Array<BYTE>(1);
	spi_target->TransferSequential(ArrayReference<BYTE>(out_buf, static_cast<unsigned int>(command_size)), readBuf);
	/* determine return code */
	*data = readBuf->get(0);

	return LGW_SPI_SUCCESS;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Burst (multiple-byte) write */
int lgw_spi_wb(SpiDevice^ spi_target, uint8_t spi_mux_mode, uint8_t spi_mux_target, uint8_t address, uint8_t *data, uint16_t size) {
    auto command = ref new Array<BYTE>(spi_mux_mode?2:1+size);
    uint8_t command_size;
 
    /* check input parameters */
    if ((address & 0x80) != 0) {
        DEBUG_MSG("WARNING: SPI address > 127\n");
    }
    if (size == 0) {
        DEBUG_MSG("ERROR: BURST OF NULL LENGTH\n");
        return LGW_SPI_ERROR;
    }

    /* prepare command byte */
    if (spi_mux_mode == LGW_SPI_MUX_MODE1) {
        command[0] = spi_mux_target;
        command[1] = WRITE_ACCESS | (address & 0x7F);
		memcpy(command->Data+2, data, size);
        command_size = 2+size;
    } else {
        command[0] = WRITE_ACCESS | (address & 0x7F);
		memcpy(command->Data + 1, data, size);
        command_size = 1+size;
    }
 
    /* I/O transaction */
	spi_target->Write(command);
	return LGW_SPI_SUCCESS;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Burst (multiple-byte) read */
int lgw_spi_rb(SpiDevice^ spi_target, uint8_t spi_mux_mode, uint8_t spi_mux_target, uint8_t address, uint8_t *data, uint16_t size) {
    uint8_t command[2];
    uint8_t command_size;

    /* check input parameters */
    if ((address & 0x80) != 0) {
        DEBUG_MSG("WARNING: SPI address > 127\n");
    }
    if (size == 0) {
        DEBUG_MSG("ERROR: BURST OF NULL LENGTH\n");
        return LGW_SPI_ERROR;
    }

    /* prepare command byte */
    if (spi_mux_mode == LGW_SPI_MUX_MODE1) {
        command[0] = spi_mux_target;
        command[1] = READ_ACCESS | (address & 0x7F);
        command_size = 2;
    } else {
        command[0] = READ_ACCESS | (address & 0x7F);
        command_size = 1;
    }
 
    /* I/O transaction */
	auto readBuf = ref new Array<BYTE>(size);
	spi_target->TransferSequential(ArrayReference<BYTE>(command, static_cast<unsigned int>(command_size)), readBuf);
	memcpy(data, readBuf->Data, size);
	/* determine return code */
	return LGW_SPI_SUCCESS;
}

/* --- EOF ------------------------------------------------------------------ */
