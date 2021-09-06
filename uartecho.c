/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * green wire: transmit
 * blue wire: recieve
 *
 */

/*
 *  ======== uartecho.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* Driver configuration */
#include "ti_drivers_config.h"

// internal dependencies
#include "srxl2.h"
#include "half_duplex_uart.h"

UART_Handle debugUart();
UART_Handle initRadioUart();

/*
 *  ======== mainThread ========
 */
void mainThread(void *arg0)
{
    UART_Handle uart;
    //UART_Handle print;

    /* Call driver init functions */
    GPIO_init();
    UART_init();

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    GPIO_setConfig(CONFIG_GPIO_UART_CTRL, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);

    GPIO_write(CONFIG_GPIO_UART_CTRL, 1);

    //print = debugUart();
    uart = initRadioUart();

    //UART_write(print, echoPrompt, sizeof(echoPrompt));
    HalfDuplexUart_t hdu = {
       .uart= uart,
       .gpio= CONFIG_GPIO_UART_CTRL // TODO: change
    };


    /* Loop forever echoing */
    ProcessPackets(hdu);
}

UART_Handle initRadioUart()
{
    UART_Handle uart;
    UART_Params uartParams;
    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_1, &uartParams);
    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }
    return uart;
}

/**
 *
 * Assumes the UART_init() has already been called
 * Example
 *   UART_Handle uart = debugUart()
 *   UART_write(uart, echoPrompt, sizeof(echoPrompt));
 *
 */
UART_Handle debugUart(){

   UART_Handle uart;
   UART_Params uartParams;

   /* Create a UART with data processing off. */
   UART_Params_init(&uartParams);
   uartParams.writeDataMode = UART_DATA_BINARY;
   uartParams.readDataMode = UART_DATA_BINARY;
   uartParams.readReturnMode = UART_RETURN_FULL;
   uartParams.readEcho = UART_ECHO_OFF;
   uartParams.baudRate = 115200;


   uart = UART_open(CONFIG_UART_0, &uartParams);

   if (uart == NULL) {
       /* UART_open() failed */
       while (1);
   }
   return uart;

}
