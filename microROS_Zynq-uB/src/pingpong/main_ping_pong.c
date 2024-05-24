
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xuartps.h"

/* microRos includes */
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/int64.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/header.h>
#include <rosidl_runtime_c/string_functions.h>

#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <time.h>

/* Custom includes. */
#include "microros_time.h"
#include "uart_transport.h"
#include "custom_memory_manager.h"
#include "microros_allocators.h"

#define TIMER_ID	1
#define DELAY_10_SECONDS	10000UL
#define DELAY_1_SECOND		1000UL
#define DELAY_10_MS			10UL
#define TIMER_CHECK_THRESHOLD	9
/*-----------------------------------------------------------*/
#define UART_DEVICE_ID                  XPAR_XUARTPS_0_DEVICE_ID
#define THREAD_UROS_STACKSIZE 1000  // 12kb

#define MICROROS_TRANSPORTS_FRAMING_MODE 1
#define MICROROS_TRANSPORTS_PACKET_MODE 0
#define STRING_BUFFER_LEN 50

/*--------------------- CHECK FUNCTIONS ---------------------*/
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){xil_printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){xil_printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}


static void microros_thread_custom(void *pvParameters);
const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );
const TickType_t x10millisecond = pdMS_TO_TICKS( DELAY_10_MS );

/*-----------------------------------------------------------*/

static TaskHandle_t xHandle = NULL;

// micro-ROS app
rcl_publisher_t ping_publisher;
rcl_publisher_t pong_publisher;
rcl_subscription_t ping_subscriber;
rcl_subscription_t pong_subscriber;

std_msgs__msg__Header incoming_ping;
std_msgs__msg__Header outcoming_ping;
std_msgs__msg__Header incoming_pong;

rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

int device_id;
int seq_no;
int pong_count;

/*-------------------- CALLBACK FUNCTIONS -------------------*/
void ping_timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
	RCLC_UNUSED(last_call_time);

	if (timer != NULL) {
		seq_no = rand();
		rosidl_runtime_c__String__init(&outcoming_ping.frame_id);
		sprintf(outcoming_ping.frame_id.data, "%d_%d", seq_no, device_id);
		outcoming_ping.frame_id.size = strlen(outcoming_ping.frame_id.data);

		// Fill the message timestamp
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		outcoming_ping.stamp.sec = ts.tv_sec;
		outcoming_ping.stamp.nanosec = ts.tv_nsec;

		// Reset the pong count and publish the ping message
		pong_count = 0;
		RCSOFTCHECK(rcl_publish(&ping_publisher, (const void*)&outcoming_ping, NULL));
		// xil_printf("Ping send seq %s\n", outcoming_ping.frame_id.data);

	}
}

void ping_subscription_callback(const void * msgin)
{
	const std_msgs__msg__Header * msg = (const std_msgs__msg__Header *)msgin;

	// Dont pong my own pings
	if(strcmp(outcoming_ping.frame_id.data, msg->frame_id.data) != 0){
		// xil_printf("Ping received with seq %s. Answering.\n", msg->frame_id.data);
		RCSOFTCHECK(rcl_publish(&pong_publisher, (const void*)msg, NULL));
	}
}

void pong_subscription_callback(const void * msgin)
{
	const std_msgs__msg__Header * msg = (const std_msgs__msg__Header *)msgin;

	if(strcmp(outcoming_ping.frame_id.data, msg->frame_id.data) == 0) {
		pong_count++;
		// xil_printf("Pong for seq %s (%d)\n", msg->frame_id.data, pong_count);
	}
}



int main( void )
{
	xTaskCreate( microros_thread_custom,
				 ( const char * ) "GB",
				 THREAD_UROS_STACKSIZE,
				 NULL,
				 tskIDLE_PRIORITY + 1,
				 &xHandle );

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}
/*----------------------------------------------------*/
static void microros_thread_custom( void *pvParameters )
{
	const TickType_t delay = pdMS_TO_TICKS( DELAY_1_SECOND );

	// Transport
	rmw_uros_set_custom_transport(
		MICROROS_TRANSPORTS_FRAMING_MODE, // Framing enable here
		(void *) NULL, //transport->args
		vitis_transport_open,
		vitis_transport_close,
		vitis_transport_write,
		vitis_transport_read);


		 rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
		 freeRTOS_allocator.allocate = custom_allocate;
		 freeRTOS_allocator.deallocate = custom_deallocate;
		 freeRTOS_allocator.reallocate = custom_reallocate;
		 freeRTOS_allocator.zero_allocate =  custom_zero_allocate;

		 bool status = rcutils_set_default_allocator(&freeRTOS_allocator);
		 if (!status) {
		 	xil_printf("Error on default allocators (line %d)\n", __LINE__);
		 	return;
		 }

		allocator = rcl_get_default_allocator();

		// create init_options
		rcl_ret_t rclc_status = rclc_support_init(&support, 0, NULL, &allocator);
		RCCHECK(rclc_status);

		// create node
		rclc_status = rclc_node_init_default(&node, "pingpong_node", "", &support);
		RCCHECK(rclc_status);

		// Create a reliable ping publisher
		rclc_status = rclc_publisher_init_default(&ping_publisher, &node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header), "/microROS/ping");
		RCCHECK(rclc_status);

		// Create a reliable pong publisher
		rclc_status = rclc_publisher_init_best_effort(&pong_publisher, &node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header), "/microROS/pong");
		RCCHECK(rclc_status);

		// Create a best effort ping subscriber
		rclc_status = rclc_subscription_init_best_effort(&ping_subscriber, &node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header), "/microROS/ping");
		RCCHECK(rclc_status);

		// Create a best effort pong subscriber
		rclc_status = rclc_subscription_init_best_effort(&pong_subscriber, &node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header), "/microROS/pong");
		RCCHECK(rclc_status);

		// Create and allocate the pingpong messages
		char outcoming_ping_buffer[STRING_BUFFER_LEN];
		outcoming_ping.frame_id.data = outcoming_ping_buffer;
		outcoming_ping.frame_id.capacity = STRING_BUFFER_LEN;

		char incoming_ping_buffer[STRING_BUFFER_LEN];
		incoming_ping.frame_id.data = incoming_ping_buffer;
		incoming_ping.frame_id.capacity = STRING_BUFFER_LEN;

		char incoming_pong_buffer[STRING_BUFFER_LEN];
		incoming_pong.frame_id.data = incoming_pong_buffer;
		incoming_pong.frame_id.capacity = STRING_BUFFER_LEN;

		device_id = rand();

		// Create a 5 seconds ping timer timer,
		rcl_timer_t timer;
		RCCHECK(rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(5000), ping_timer_callback));

		// Create executor
		rclc_executor_t executor;
		RCCHECK(rclc_executor_init(&executor, &support.context, 3, &allocator));
		RCCHECK(rclc_executor_add_timer(&executor, &timer));
		RCCHECK(rclc_executor_add_subscription(&executor, &ping_subscriber, &incoming_ping, &ping_subscription_callback, ON_NEW_DATA));
		RCCHECK(rclc_executor_add_subscription(&executor, &pong_subscriber, &incoming_pong,	&pong_subscription_callback, ON_NEW_DATA));

		while(1){
			rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1000));
			vTaskDelay(delay);
		}

		// Free resources
		RCCHECK(rcl_publisher_fini(&ping_publisher, &node));
		RCCHECK(rcl_publisher_fini(&pong_publisher, &node));
		RCCHECK(rcl_subscription_fini(&ping_subscriber, &node));
		RCCHECK(rcl_subscription_fini(&pong_subscriber, &node));
		RCCHECK(rcl_node_fini(&node));
}
