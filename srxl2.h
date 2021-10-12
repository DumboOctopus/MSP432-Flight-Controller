#ifndef SRXL2_H
#define SRXL2_H

#include <stdint.h>
#include <stdbool.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/PWM.h>


#include "half_duplex_uart.h"


#define PACKED __attribute__((packed))

//
// Note: some of this code was adapted from https://github.com/SpektrumRC/SRXL2/blob/master/Source/spm_srxl.h
//

// each packet starts with this character.
#define SPEKTRUM_SRXL_ID        (0xA6)
#define SRXL_MAX_BUFFER_SIZE    (80)

// id for certain
#define SRXL_CTRL_ID                (0xCD)
#define SRXL_BIND_INFO_ID       (0x41)
#define SRXL_HANDSHAKE_ID       (0x21)

#define SRXL_CHANNEL_DATA_CMD       0

// message lengths
#define SRXL_HANDSHAKE_LENGTH   14

// my src id
#define SRXL_SRC_ID                  (0x30)
// TODO: make this random at start up
#define SRXL_HANDSHAKE_UID              (uint32_t) 0x5d746cbe

#define FWD_PGM_MAX_DATA_SIZE   (64)
#define SRXL_MAX_CHANNELS       (32)

typedef struct
{
    uint8_t srxlID;     // Always 0xA6 for SRXL2
    uint8_t packetType;
    uint8_t length;

} __attribute__((packed)) SrxlHeader_t;


typedef struct
{
    SrxlHeader_t header;
    uint8_t        packetBuffer[SRXL_MAX_BUFFER_SIZE];
} __attribute__((packed)) SrxlPacket_t;


// Handshake
typedef struct
{
    SrxlHeader_t header;
    uint8_t      srcDevID;
    uint8_t      destDevID;
    uint8_t      priority;
    uint8_t      baudSupported;  // 0 = 115200, 1 = 400000 (See SRXL_BAUD_xxx definitions above)
    uint8_t      info;           // See SRXL_DEVINFO_xxx definitions above for defined bits
    uint32_t     uid;            // Unique/random id to allow detection of two devices on bus with same deviceID
} __attribute__((packed)) SrxlHandshake_t;

// VTX Data
typedef struct
{
    uint8_t band;       // VTX Band (0 = Fatshark, 1 = Raceband, 2 = E, 3 = B, 4 = A)
    uint8_t channel;    // VTX Channel (0-7)
    uint8_t pit;        // Pit/Race mode (0 = Race, 1 = Pit). Race = normal power, Pit = reduced power
    uint8_t power;      // VTX Power (0 = Off, 1 = 1mw to 14mW, 2 = 15mW to 25mW, 3 = 26mW to 99mW,
                        // 4 = 100mW to 299mW, 5 = 300mW to 600mW, 6 = 601mW+, 7 = manual control)
    uint16_t powerDec;  // VTX Power as a decimal 1mw/unit
    uint8_t region;     // Region (0 = USA, 1 = EU)
} PACKED SrxlVtxData_t;

// Forward Programming Data
typedef struct
{
    int8_t  rssi;       // Best RSSI while sending forward programming data
    uint8_t rfu[2];     // 0 for now -- used to word-align data
    uint8_t data[FWD_PGM_MAX_DATA_SIZE];
} PACKED SrxlFwdPgmData_t;

// Channel Data
typedef struct
{
    int8_t    rssi;         // Best RSSI when sending channel data, or dropout RSSI when sending failsafe data
    uint16_t  frameLosses;  // Total lost frames (or fade count when sent from Remote Rx to main Receiver)
    uint32_t  mask;         // Set bits indicate that channel data with the corresponding index is present
    uint16_t  values[32];   // Channel values, shifted to full 16-bit range (32768 = mid-scale); lowest 2 bits RFU
} PACKED SrxlChannelData_t;

// Control Data
typedef struct
{
    SrxlHeader_t header;
    uint8_t      cmd;
    uint8_t      replyID;
    union
    {
        SrxlChannelData_t channelData;    // Used for Channel Data and Failsafe Channel Data commands
        SrxlVtxData_t     vtxData;        // Used for VTX commands
        SrxlFwdPgmData_t  fpData;         // Used to pass forward programming data to an SRXL device
    };
} __attribute__((packed)) SrxlControlData_t;


typedef struct
{
    SrxlHeader_t header;
    uint8_t      request;
    uint8_t      deviceId;
    uint8_t      type;
    uint8_t      options;
    uint64_t     guid;
    uint32_t     uid;
} PACKED SrxlBindInfo_t;


void ProcessPackets(HalfDuplexUart_t hdu,  PWM_Handle pwm);

#endif
