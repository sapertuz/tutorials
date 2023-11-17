#ifndef __UART_TRANSPORT_H__
#define __UART_TRANSPORT_H__

/*------------------By: Roy Kislev and Michael Grenader----------------------*/

/***************************** Include Files *********************************/
#include <uxr/client/transport.h>
#include <rmw_microxrcedds_c/config.h>

#include "xparameters.h"
#include "xuartps.h"

#include "stdio.h"
#include "stdbool.h"


/***************************** Function Prototypes *********************************/

int UartPsSelfTestExample(u16 DeviceId);

bool vitis_transport_open(struct uxrCustomTransport* transport);

bool vitis_transport_close(struct uxrCustomTransport * transport);


size_t vitis_transport_write(struct uxrCustomTransport* transport, const uint8_t* buf, size_t len, uint8_t * err);

size_t vitis_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

int UartPsSelfTestExample(u16 DeviceId);

#endif