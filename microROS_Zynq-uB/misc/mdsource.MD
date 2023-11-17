I want to make a tutorial on how to generate a the microros library for Xilinx devices (Zynq and microblaze) with UART custom transport.

First the tutorial should have an instruction about microros and why its useful for embedded systems and specifically FPGA-SoCs.

## Get microros base from docker
Then, the first section should e about using docker to get the microros base system `docker pull microros/base:foxy`.  NOTE: it is important to use foxy version of microros because on later versions the build is not correct for some reason.

after getting the base docker image of microros, it should be followed the first steps of microros

(Taken from https://micro.ros.org/docs/tutorials/core/first_application_linux/)
Once you have a ROS 2 installation in the computer, follow these steps to install the micro-ROS build system:

```sh
# Source the ROS 2 installation
source /opt/ros/$ROS_DISTRO/setup.bash

# Create a workspace and download the micro-ROS tools
mkdir microros_ws
cd microros_ws
git clone -b $ROS_DISTRO https://github.com/micro-ROS/micro_ros_setup.git src/micro_ros_setup

# Update dependencies using rosdep
sudo apt update && rosdep update
rosdep install --from-paths src --ignore-src -y

# Install pip
sudo apt-get install python3-pip

# Build micro-ROS tools and source them
colcon build
source install/local_setup.bash
```

# build the microros library
Following is (taken from https://micro.ros.org/docs/tutorials/advanced/create_custom_static_library/)
Once your micro-ROS workspace is created and the `micro_ros_setup` tool is installed, we are going to prepare the micro-ROS environment:

```sh
ros2 run micro_ros_setup create_firmware_ws.sh generate_lib
```

Once all the packages are downloaded, we need to create a couple of files in order to crosscompile a custom static library and a set of header files. These files are available on the `misc` folder of this tutorial.

`Zynq7000_toolchain.cmake` and `Zynq7000_colcon.meta` for Zynq Devices and `microblaze_toolchain.cmake` and `microblaze_colcon.meta`.

It is very important to replace the variable `$vivado_path` on the cmake file and point it to the instalation folder of vivado (e.g., `/tools/Xilinx/Vitis/2023.1`)

Once you have both files ready, just run the build step in the micro-ROS build system:
```sh
ros2 run micro_ros_setup build_firmware.sh $(full-path-to-cmake) $(full-path-to-cmake)
```

after waiting for the build you will see some `stderr` but you can ignore them.
Once the build finishes you will have a precompiled static library with all the micro-ROS functionality in firmware/build/libmicroros.a and you will have all the required headers for your application in firmware/build/include.

# make your vivado design and make the vitis workspace

-------------
Placeholder for Vivado Design for Zynq and microros diagram
-------------

explain this is the example vivado project for the ping ping talker we will make. there is nothing special, just a nnormal Zynq system.

after generating the bitstream create your vitis project:
        ChatGPT, create a minitutorial from exporting the xsa all the way to generate the platform 

# create application and configure the builder

Explain that now we will implement a ping pong example using the provided code. 

This example showcases a micro-ROS node with two publisher-subscriber pairs associated with a ping and a pong topics, respectively. The node sends a ping package with a unique identifier, using a ping publisher. If the ping subscriber receives a ping from an external node, the pong publisher responds to the incoming ping with a pong. To test that this logic is correctly functioning, we implement communication with a ROS 2 node that:

* `Listens to the topics published by the ping subscriber.
* `Publishes a fake_ping package, that is received by the micro-ROS ping subscriber. As a consequence, the pong publisher on the micro-ROS application will publish a pong, to signal that it received the fake_ping correctly.

The diagram below clarifies the communication flow between these entities:

-------------
Placeholder pingpong app diagram
-------------
It was build considering the microros tutorial for it: https://micro.ros.org/docs/tutorials/advanced/create_custom_transports/

highlight that the custom transport code is included in src folder of this tutorial and that it simply uses the uart available on the zynq

now create minitutorial with steps on creating an application project in vitis using the platform created on the previous step and adding all sources on the `src` folder (make sure to indicate to make softlinks so there is not a project local copy of the code). it must be with freeRTOS and after that give some instructions on how to configure the bsp so you give at least 10kB of stack to freeRTOS

then add the previously built  .a library following this steps:
- Adding the the linker settings and the include headers: Project Properties > C/C++ Build / Settings > x gcc compiler > Directories > - Include Paths - Add path to directory             _projectpath_/libmicroros/include
    - Project Properties > C/C++ Build / Settings > x gcc linker> Libraries> Libraries - Add microros
    - Project Properties > C/C++ Build / Settings > x gcc linker> Libraries> Library search path - Add path to directory libmicroros

-------------
Placeholder image of the vitis promt pf the previous steps
-------------

Also Adding the Include Path to the compiler:

-------------
Placeholder image of the vitis promt pf the previous step
-------------

Build everything 

# Create agent and test everything 
Compile and flash the board with thethe ping pong example

Following the steps from: https://micro.ros.org/docs/tutorials/core/first_application_rtos/freertos/

## Running the micro-ROS app
At this point, you have both the client and the agent correctly installed.

To give micro-ROS access to the ROS 2 dataspace, you just need to run the agent:

```sh
# Run a micro-ROS agent
ros2 run micro_ros_agent micro_ros_agent serial --dev [device] -v 6
```
TIP: the device name shoud be somthing like: /dev/tty/USB1

## Run debug the microros application from vitis
put here the steps to debug the created ping pong application

## Testing the micro-ROS app
At this point, the micro-ROS app is built and flashed and the board is connected to a micro-ROS agent. We now want to check that everything is working.

Open a new command line. We are going to listen to the ping topic with ROS 2 to check whether the micro-ROS Ping Pong node is correctly publishing the expected pings:

```sh
source /opt/ros/$ROS_DISTRO/setup.bash
# Subscribe to micro-ROS ping topic
ros2 topic echo /microROS/ping
```

You should see the topic messages published by the Ping Pong node every 5 seconds:

```shell
user@user:~$ ros2 topic echo /microROS/ping
stamp:
  sec: 20
  nanosec: 867000000
frame_id: '1344887256_1085377743'
---
stamp:
  sec: 25
  nanosec: 942000000
frame_id: '730417256_1085377743'
---
```

At this point, we know that our micro-ROS app is publishing pings. Let’s check if it also answers to someone else’s pings. If this works, it’ll publish a pong.

So, first of all, let’s subscribe with ROS 2 to the pong topic from a new shell (notice that initially we don’t expect to receive any pong, since none has been sent yet):

```sh
source /opt/ros/$ROS_DISTRO/setup.bash

# Subscribe to micro-ROS pong topic
ros2 topic echo /microROS/pong
```
And now, let’s publish a fake_ping with ROS 2 from yet another command line:

```sh
source /opt/ros/$ROS_DISTRO/setup.bash

# Send a fake ping
ros2 topic pub --once /microROS/ping std_msgs/msg/Header '{frame_id: "fake_ping"}'
```

Now, we should see this fake_ping in the ping subscriber console, along with the board’s pings:
```shell
user@user:~$ ros2 topic echo /microROS/ping
stamp:
  sec: 0
  nanosec: 0
frame_id: fake_ping
---
stamp:
  sec: 305
  nanosec: 973000000
frame_id: '451230256_1085377743'
---
stamp:
  sec: 310
  nanosec: 957000000
frame_id: '2084670932_1085377743'
---
```

Also, we expect that, because of having received the fake_ping, the micro-ROS pong publisher will answer with a pong. As a consequence, in the pong subscriber console, we should see the board’s answer to our fake_ping:
```sh
user@user:~$ ros2 topic echo /microROS/pong
stamp:
  sec: 0
  nanosec: 0
frame_id: fake_ping
---
```

