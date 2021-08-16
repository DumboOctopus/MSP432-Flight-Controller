#ifndef MAIN_H
#define MAIN_H

#include <ti/drivers/UART.h>

typedef struct TaskArg {
    QueueHandle_t packetQueue;
    UART_Handle print;
} TaskArg;


/* Prototypes*/
void createMainThread(TaskArg *arg);
void createUartThread(TaskArg *arg);
UART_Handle createDebugUartHandle();


#endif
