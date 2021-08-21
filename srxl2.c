/*
 * srxl2.c
 *
 *  Created on: Aug 15, 2021
 *      Author: Neil
 */
#include "srxl2.h"



// TODO: in the future, look into MSP 432's CRC hardware
uint16_t Crc16(uint16_t crc, uint8_t data)
{
    int i;
    crc = crc ^ ((uint16_t) data << 8);
    for(i = 0; i < 8; ++i)
    {
        if(crc & 0x8000)
        {
            crc = (crc << 1) ^ 0x1021;
        }
        else
        {
            crc = crc << 1;
        }
    }
    return crc;
}


bool verifyPacket(uint8_t* buf, int length)
{
    uint8_t i;
    uint16_t computedCRC = 0;
    for(i = 0; i < length-2; ++i)
    {
        computedCRC = Crc16(computedCRC, buf[i]);
    }
    uint16_t rxCRC = ((uint16_t) buf[length-2] << 8) | buf[length-1];

    return computedCRC = rxCRC;
}

// ========= Create Message Functions
void CreateHandshake(SrxlHandshake_t*  packet)
{
    packet->header.srxlID = SPEKTRUM_SRXL_ID;
    packet->header.packetType = SRXL_HANDSHAKE_ID;
    packet->header.length = SRXL_HANDSHAKE_LENGTH;

    // TODO: make lower nibble 0 if annoucing presence
    packet->srcDevID = SRXL_SRC_ID;
    packet->destDevID = 0;
    packet->priority = 10;
    // TODO: look up if higher baud rate is supported
    // for MSP 432
    packet->baud_rate = 0;
    // TODO: if later want to fancy stuff, change this
    packet->info = 0;
    packet->uid = SRXL_HANDSHAKE_UID;
}



// ========== Processing Functions

/**
 * TODO: might want to handle switching of baud rate
 *
 */
bool ProcessHandshake(uint8_t* packet)
{
    SrxlHandshake_t* handshake = (SrxlHandshake_t*) packet;
    uint8_t dest_id = handshake->destDevID;

    // now write our response
    if(dest_id == 0xFF)
    {
        // commands everyone to set their baud rate.
    }
    if(dest_id == SRXL_SRC_ID)
    {

    }
}




static inline void ProcessMessage(uint8_t type, uint8_t* packetBuff)
{
    switch(type)
    {
    case SRXL_HANDSHAKE_ID:
        ProcessHandshake(packetBuff);
        break;
    }
}

// TODO: all data values are in little endian. verify that MSP 432 is little endian
void ProcessPackets(UART_Handle uart)
{
    uint8_t        value;
    uint8_t        packetBuffer[SRXL_MAX_BUFFER_SIZE];

    while (1) {
        int_fast32_t bytesRead = UART_read(uart, &value, 1);

        if(bytesRead > 0 && value == SPEKTRUM_SRXL_ID)
        {
            uint8_t type, length;
            UART_read(uart, &type, 1);
            UART_read(uart, &length, 1);

            // next we will read the entire packet and store it in
            // a buffer
            packetBuffer[0] = SPEKTRUM_SRXL_ID;
            packetBuffer[1] = type;
            packetBuffer[2] = length;

            // TODO: handle case where UART_read does not read all bytes
            UART_read(uart, &packetBuffer[3], length-3);

            if(verifyPacket(packetBuffer, length))
            {
                printf("hi");
            }
        }
        //UART_write(print, &value, 1);
        //UART_write(uart, &value, 1);
    }
}
