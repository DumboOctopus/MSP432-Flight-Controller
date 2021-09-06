/*
 * srxl2.c
 *
 *  Created on: Aug 15, 2021
 *      Author: Neil
 */
#include "srxl2.h"
#include "half_duplex_uart.h"


uint16_t channelData[SRXL_MAX_CHANNELS] = {0};
uint8_t recieverId = 0;

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


void addCRC(uint8_t* buf, int maxLength) {
    uint16_t computedCRC = 0;
    size_t i;

    // last two bytes are for crc
    for(i = 0; i < maxLength -2; ++i){
        computedCRC = Crc16(computedCRC, buf[i]);
    }

    // upper byte because big endian
    // TODO: use FreeRTOS_htons
    buf[maxLength-2] = (uint8_t) (computedCRC >> 8);
    buf[maxLength-1] = (uint8_t) (computedCRC & 0xff);

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

// ========= Create Message Functions ==========
void CreateHandshake(SrxlHandshake_t* packet)
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
    packet->baudSupported = 0;
    // TODO: if later want to fancy stuff, change this
    packet->info = 0;
    packet->uid = SRXL_HANDSHAKE_UID;

    addCRC((uint8_t*) packet, SRXL_HANDSHAKE_LENGTH);
}

void CreateBindInfoRequest(SrxlBindInfo_t* bindInfo)
{
    bindInfo->header.srxlID = SPEKTRUM_SRXL_ID;
    bindInfo->header.packetType = SRXL_BIND_INFO_ID;
    bindInfo->header.length = 21;


    bindInfo->request = 0xB5;
    bindInfo->deviceId = 0x21;

    addCRC((uint8_t*) bindInfo, 21);
}



// ========== Processing Functions ===========

/**
 * TODO: might want to handle switching of baud rate
 *
 */
bool ProcessHandshake(SrxlPacket_t* packet)
{
    SrxlHandshake_t* handshake = (SrxlHandshake_t*) packet;
    uint8_t dest_id = handshake->destDevID;
    recieverId = handshake->srcDevID;

    // now write our response
    if(dest_id == 0xFF)
    {
        // commands everyone to set their baud rate.
        CreateHandshake(handshake);
        return true;
    }
    if(dest_id == SRXL_SRC_ID)
    {
        CreateHandshake(handshake);
        return true;
    }

    return false;
}

static bool ProcessCtrlData(SrxlPacket_t* packet)
{
    size_t i;
    SrxlControlData_t* ctrlData = (SrxlControlData_t*) packet;
    uint16_t* chnData;

    switch(ctrlData->cmd)
    {
    case SRXL_CHANNEL_DATA_CMD:
        // 0=thrust, 1=ailerons (343,1024, 1705) 2=elavator, 3=rudder, 5=ax1
        chnData = &(ctrlData->channelData.values[0]);
        for(i =0; i < SRXL_MAX_CHANNELS; i++)
        {
            if( ( (1 << i ) & ctrlData->channelData.mask ) != 0)
            {
                uint16_t actualData = *chnData >> 5;
                chnData++; // move to next data point.
                if(channelData[i] != actualData)
                {
                    channelData[i] = actualData;
                }
            }
        }
        break;
    }



    return false;
}

static bool ProcessMessage(SrxlPacket_t* packet)
{
    bool reply = false;


    switch(packet->header.packetType)
    {
    case SRXL_HANDSHAKE_ID:
        reply = ProcessHandshake(packet);
        break;
    case SRXL_BIND_INFO_ID:
        break;
    case SRXL_CTRL_ID:
        ProcessCtrlData(packet);
//        CreateBindInfoRequest(packet);
//        reply=true;
        break;

    }

    return reply;
}

// ======================== non static functions =================//

void Init(UART_Handle uart)
{

}


// TODO: all data values are in little endian. verify that MSP 432 is little endian
void ProcessPackets(HalfDuplexUart_t hdu)
{
    SrxlPacket_t   packet;

    while (1) {
        int_fast32_t bytesRead = HduRead(hdu, &packet.header.srxlID, 1);

        if(bytesRead > 0 && packet.header.srxlID == SPEKTRUM_SRXL_ID)
        {
            HduRead(hdu, &packet.header.packetType, 1);
            HduRead(hdu, &packet.header.length, 1);

            // next we will read the entire packet and store it in
            // a buffer

            // TODO: handle case where UART_read does not read all bytes
            HduRead(
                hdu, 
                packet.packetBuffer, 
                packet.header.length-sizeof(packet.header)
            );

            if(verifyPacket( (void*) &packet, packet.header.length))
            {
                bool reply = ProcessMessage(&packet);
                if(reply)
                {
                    HduWrite(hdu, &packet, packet.header.length);
                }
            }
        }
    }
}
