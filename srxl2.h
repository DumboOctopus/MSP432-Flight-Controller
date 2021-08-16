#ifndef SRXL2_H
#define SRXL2_H

#include <stdint.h>
#include <stdbool.h>
#include <ti/drivers/UART.h>


//
// Note: some of this code was adapted from https://github.com/SpektrumRC/SRXL2/blob/master/Source/spm_srxl.h
//

// each packet starts with this character.
#define SPEKTRUM_SRXL_ID        (0xA6)
#define SRXL_MAX_BUFFER_SIZE    (80)

// id for certain
#define SRXL_CTRL_ID                (0xCD)
#define SRXL_HANDSHAKE_ID       (0x21)

// Handshake
typedef struct SrxlHandshake
{
    uint8_t     srcDevID;
    uint8_t     destDevID;
    uint8_t     priority;
    uint8_t     baudSupported;  // 0 = 115200, 1 = 400000 (See SRXL_BAUD_xxx definitions above)
    uint8_t     info;           // See SRXL_DEVINFO_xxx definitions above for defined bits
    uint32_t    uid;            // Unique/random id to allow detection of two devices on bus with same deviceID
} __attribute__((packed)) SrxlHandshake;


// this will be used for adding packets to the queue
// we use union
typedef struct srxl_packet_t
{
    uint8_t srxlID;     // Always 0xA6 for SRXL2
    uint8_t packetType;
    uint8_t length;

} srxl_packet_t;


void ProcessPackets(UART_Handle uart);

#endif
