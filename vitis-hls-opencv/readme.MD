# Vitis - Compiling and Installing OpenCV to Simulate 

## IMPORTANT
- On Ubuntu 22.04
- Based on this [Xilinx Post](https://support.xilinx.com/s/article/Vitis-Libraries-Compiling-and-Installing-OpenCV?language=en_US)

## Prerequisites:
- Vitis and XRT installed
- OpenCL package installed
- cmake version >3.5 
    - **(Recommended)** Install Newest version with this [tutorial, option A](https://askubuntu.com/questions/355565/how-do-i-install-the-latest-version-of-cmake-from-the-command-line). 
        - **NOTE:** you may need to reopen the terminal.
        - **NOTE2:** on the step to copy the key, make sure to copy the missing key on your terminal and not the one on the tutorial.
    - Modify `<path-to-repo>/source/cmake/OpenCVMinDepVersions.cmake` and change it to your cmake version

## Tutorial
1. Create a directory "source" and clone opencv-4.4.0 into it.
    ```bash
    git clone https://github.com/opencv/opencv --branch 4.4.0 source
    ```

2. Create a directory "source_contrib" and clone opencv-4.4.0-contrib into it.
    ```bash
    git clone https://github.com/opencv/opencv_contrib --branch 4.4.0 source_contrib
    ```

3. cd into 'source', and create 'build' dir and cd into it.
    ```bash
    cd source
    mkdir build
    cd build
    ```

4. Run the command:
    ```bash
    export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:$LIBRARY_PATH
    ```

5. Set up the Vitis tools via the settings64 sh script in install.
    ```bash
    source $MY_PATH_TO_XILINX/Vitis/$MY_VITIS_VERSION/settings64.sh
    ```

6. Run the command (Note: you will need to replace the directory paths in the command with their appropriate location - `$MY_OPENCV_INSTALL_DIR` and `$MY_OPENCV_DIR`):

    ```bash
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=$MY_OPENCV_INSTALL_DIR \
      -D OPENCV_EXTRA_MODULES_PATH=$MY_OPENCV_DIR/source_contrib/modules \
      -DWITH_V4L=ON \
      -DBUILD_TESTS=OFF \
      -DBUILD_ZLIB=ON \
      -DBUILD_JPEG=ON \
      -DWITH_JPEG=ON \
      -DWITH_PNG=ON \
      -DBUILD_EXAMPLES=OFF \
      -DINSTALL_C_EXAMPLES=OFF \
      -DINSTALL_PYTHON_EXAMPLES=OFF \
      -DWITH_OPENEXR=OFF \
      -DBUILD_OPENEXR=OFF \
      -D CMAKE_CXX_COMPILER=$XILINX_HLS/tps/lnx64/gcc-6.2.0/bin/g++ \
      ..
    ```

7. Run the command:
    ```bash
    cmake all -j8
    make install
    ```

8. Set the following env variables on the terminal when you want to open vitis:
    ```bash
    export OPENCV_INCLUDE=$MY_OPENCV_INSTALL_DIR/include/opencv4
    export OPENCV_LIB=$MY_OPENCV_INSTALL_DIR/lib
    export LD_LIBRARY_PATH=$MY_OPENCV_INSTALL_DIR/lib:$LD_LIBRARY_PATH
    ```

    then, source and open Vitis:
    ```bash
    source $MY_PATH_TO_XILINX/Vitis/$MY_VITIS_VERSION/settings64.sh
    vitis
    ```

9. On your Vitis project/example `hls_config.cfg` file, configure the flags to:
    ```
    tb.file_cflags=<testbench-file>.cpp,-I$OPENCV_INCLUDE
    
    csim.ldflags=-L $OPENCV_LIB -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_flann -lopencv_features2d
    ```

10. Your code should compile now :)

## Common Issues:

### No libidn11
```bash
~$ cd /tmp 
/tmp $ wget http://mirrors.kernel.org/ubuntu/pool/main/libi/libidn/libidn11_1.33-2.2ubuntu2_amd64.deb
$ sudo apt install ./libidn11_1.33-2.2ubuntu2_amd64.deb
```

### ASM error
```bash
sudo apt install gcc-multilib
```

## Bonus: 

### Building on cross-compile aarch64 platform

If you are on Parallels Ubuntu VM on a MacOS host with Apple Silicon (aarch64) chip, you may need to create a CMake toolchain file that explicitly sets the target architecture to x86_64 and uses the Vitis HLS toolchain for both C and C++ compilation. Replace **Step 6** on the tutorial with:

1. Create a file, e.g., `x86_64_toolchain.cmake`:
```cmake
# x86_64_toolchain.cmake

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify the cross-compiler paths
set(CMAKE_C_COMPILER $XILINX_HLS/tps/lnx64/gcc-6.2.0/bin/gcc)
set(CMAKE_CXX_COMPILER $XILINX_HLS/tps/lnx64/gcc-6.2.0/bin/g++)

# Specify the target architecture flags
set(CMAKE_C_FLAGS "-m64")
set(CMAKE_CXX_FLAGS "-m64")

# Specify the linker flags for x86_64
set(CMAKE_EXE_LINKER_FLAGS "-m64")
set(CMAKE_SHARED_LINKER_FLAGS "-m64")
```

2. Run CMake with the Toolchain File
```bash
cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=$MY_OPENCV_INSTALL_DIR \
      -D OPENCV_EXTRA_MODULES_PATH=$MY_OPENCV_DIR/source_contrib/modules \
      -DWITH_V4L=ON \
      -DBUILD_TESTS=OFF \
      -DBUILD_ZLIB=ON \
      -DBUILD_JPEG=ON \
      -DWITH_JPEG=ON \
      -DWITH_PNG=ON \
      -DBUILD_EXAMPLES=OFF \
      -DINSTALL_C_EXAMPLES=OFF \
      -DINSTALL_PYTHON_EXAMPLES=OFF \
      -DWITH_OPENEXR=OFF \
      -DBUILD_OPENEXR=OFF \
      -DCMAKE_TOOLCHAIN_FILE=../x86_64_toolchain.cmake \
      ..
```

## Build only core modules for Ubuntu 24.04

Due to compatibility issues with certain libraries and C++ standards on Ubuntu 24.04, you may encounter linking errors during the OpenCV build process, particularly related to the `libLerc.so` library. This workaround simplifies the build by focusing only on the core modules you need, thus reducing dependency complications.

The linking issues encountered on Ubuntu 24.04 are primarily due to the updated standard library and changes in the C++ ABI (Application Binary Interface). By building only the core modules, you minimize the dependencies and avoid complications with modules that may have unresolved references or incompatibilities with newer libraries, streamlining the installation process for your specific use case.

1. The environment variables should now be:
```bash
export LIBRARY_PATH=$VITIS_HLS/tps/lnx64/gcc-6.2.0/lib64/:/usr/lib/x86_64-linux-gnu/:$LIBRARY_PATH
export LD_LIBRARY_PATH=$VITIS_HLS/tps/lnx64/gcc-6.2.0/lib64/libstdc++.so.6.0.22
```

2. The `cmake` command should now be

```bash
cmake -D BUILD_LIST=core,highgui,imgcodecs,imgproc,features2d,flann \
      -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=$MY_OPENCV_INSTALL_DIR \
      -D OPENCV_EXTRA_MODULES_PATH=$MY_OPENCV_DIR/source_contrib/modules \
      -DWITH_V4L=ON \
      -DBUILD_TESTS=OFF \
      -DBUILD_ZLIB=ON \
      -DBUILD_JPEG=ON \
      -DWITH_JPEG=ON \
      -DWITH_PNG=ON \
      -DBUILD_EXAMPLES=OFF \
      -DINSTALL_C_EXAMPLES=OFF \
      -DINSTALL_PYTHON_EXAMPLES=OFF \
      -DWITH_OPENEXR=OFF \
      -DBUILD_OPENEXR=OFF \
      -D CMAKE_CXX_COMPILER=$XILINX_HLS/tps/lnx64/gcc-6.2.0/bin/g++ \
      ..
```