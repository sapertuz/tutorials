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
    source <PATH-TO-XILINX>/Vitis/<VITIS-VERSION>/settings64.sh
    ```

6. Run the command (Note: you will need to replace the directory paths in the command with their appropriate location - `<any-output-install-directory>` and `<path-to-opencv-directory>`):

    ```bash
    cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=<any-output-install-directory>/install -D WITH_V4L=ON -D OPENCV_EXTRA_MODULES_PATH=<path-to-opencv-directory>/source_contrib/modules -DBUILD_TESTS=OFF -DBUILD_ZLIB=ON -DBUILD_JPEG=ON -DWITH_JPEG=ON -DWITH_PNG=ON -DBUILD_EXAMPLES=OFF -DINSTALL_C_EXAMPLES=OFF -DINSTALL_PYTHON_EXAMPLES=OFF -DWITH_OPENEXR=OFF -DBUILD_OPENEXR=OFF -D CMAKE_CXX_COMPILER=$XILINX_HLS/tps/lnx64/gcc-6.2.0/bin/g++ ..
    ```

7. Run the command:
    ```bash
    make all -j8
    make install
    ```

8. Set the following env variables on the terminal when you want to open vitis:
    ```bash
    export OPENCV_INCLUDE=/home/user/bin/opencv/install/include/opencv4
    export OPENCV_LIB=/home/user/bin/opencv/install/lib
    export LD_LIBRARY_PATH=/home/user/bin/opencv/install/lib:$LD_LIBRARY_PATH
    ```

    then, source and open Vitis:
    ```bash
    source <PATH-TO-XILINX>/Vitis/<VITIS-VERSION>/settings64.sh
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