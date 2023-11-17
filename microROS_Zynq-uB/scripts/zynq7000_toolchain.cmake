# User defined variables
set(VITIS_PATH "/tools/Xilinx/Vitis/2023.1")
set(CMAKE_SYSTEM_PROCESSOR processing_system7_0)

# Generic
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_ASM_COMPILER_WORKS TRUE)

# Set the path to your C99 and C++ compilers
set(CMAKE_C_COMPILER ${VITIS_PATH}/gnu/aarch32/lin/gcc-arm-none-eabi/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${VITIS_PATH}/gnu/aarch32/lin/gcc-arm-none-eabi/bin/arm-none-eabi-g++)

set(CMAKE_C_COMPILER_WORKS 1 CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER_WORKS 1 CACHE INTERNAL "")

# Set your building flags
set(FLAGS "-O2 -ffunction-sections -fdata-sections -fno-exceptions -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -nostdlib -mthumb-interwork --param max-inline-insns-single=500 -DF_CPU=1000000000L -D'RCUTILS_LOG_MIN_SEVERITY=RCUTILS_LOG_MIN_SEVERITY_NONE'" CACHE STRING "" FORCE)

set(CMAKE_C_FLAGS_INIT "-std=c11 ${FLAGS} -DCLOCK_MONOTONIC=0 -D'__attribute__(x)='" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_INIT "-std=c++11 ${FLAGS} -fno-rtti -DCLOCK_MONOTONIC=0 -D'__attribute__(x)='" CACHE STRING "" FORCE)

set(__BIG_ENDIAN__ 0)