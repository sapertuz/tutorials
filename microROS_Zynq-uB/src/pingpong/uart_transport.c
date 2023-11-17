/*------------------By: Roy Kislev and Michael Grenader----------------------*/

/***************************** Include Files *********************************/
#include "uart_transport.h"

/***************************** Constant Definitions***************************/
#define UART_DEVICE_ID    XPAR_XUARTPS_NUM_INSTANCES-1

#define UART_BUFFER_SIZE 2048

#define MICROROS_TRANSPORTS_FRAMING_MODE 1
#define MICROROS_TRANSPORTS_PACKET_MODE 1

/***************************** Function Prototypes *********************************/

int UartPsSelfTestExample(u16 DeviceId);


/***************************** Variable Definitions *********************************/

XUartPs Uart_Ps;  /* The instance of the UART Driver */

static u8 UART_RecvBuffer[UART_BUFFER_SIZE];

bool vitis_transport_open(struct uxrCustomTransport* transport){
	int Status;

	Status = UartPsSelfTestExample(UART_DEVICE_ID);
	if (Status != XST_SUCCESS){
		return false;
	}
	return true;
}

bool vitis_transport_close(struct uxrCustomTransport * transport){

    return true;
}


size_t vitis_transport_write(struct uxrCustomTransport* transport, const uint8_t* buf, size_t len, uint8_t * err){
	u32 SentCount = 0;
	u32 LoopCount = 0;
	/* Block sending the buffer. */
	// if Sent nothing return 0, otherwise return the length of buf
	SentCount = XUartPs_Send(&Uart_Ps, (uint8_t*)buf, len);
	while(XUartPs_IsSending(&Uart_Ps)){
		LoopCount++;
	}
	if (SentCount != len){
		return 0;
	}
	return len;
}

size_t vitis_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err){
	/* Block Receiving the buffer. */

	unsigned int receivedCount = 0;
	unsigned int receive_bytes = 1;

	while( receivedCount < len && receive_bytes != 0){
		receive_bytes = XUartPs_Recv(&Uart_Ps, &UART_RecvBuffer[receivedCount], (len-receivedCount));
		receivedCount += receive_bytes;
	}

	unsigned int index;
	for(index=0; index<receivedCount; index++){
		buf[index] = UART_RecvBuffer[index];
	}

	return receivedCount;
}


int UartPsSelfTestExample(u16 DeviceId)
{
	int Status;
	XUartPs_Config *Config;

	/*
	 * Initialize the UART driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config = XUartPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_Ps, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Perform a self-test to check hardware build. */
	Status = XUartPs_SelfTest(&Uart_Ps);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
