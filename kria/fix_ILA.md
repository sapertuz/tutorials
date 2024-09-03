# Ubuntu 22.04 hangs when trying to connect to KV260 via Vivado Hardware Manager

This happens when using the kria and you also want to use the System ILA, or JTAG, throught the Vivado Hardware Manager.

The bug makes the system completely crash once you trigger the System ILA and run the HW application using, for example, PYNQ.

I made this tutorial based on [this post](https://support.xilinx.com/s/question/0D54U00005u8v4nSAA/ubuntu-2204-hangs-when-trying-to-connect-to-kv260-via-vivado-hardware-manager?language=en_US&t=1725355909975&searchQuery) and using [these instructions](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/2363129857/Getting+Started+with+Certified+Ubuntu+22.04+LTS+for+Xilinx+Devices#%5BinlineExtension%5DChanging-the-Kernel-bootargs-Used-By-U-Boot).

Currently, the work around this issue consist in adding "cpuidle.off=1" to bootargs.Bootargs is defined in "boot.scr.uimg" file in sd card first partition. 

# Solution 1: Working
1. Extract the sd-card from the kria and mount it on your PC. `cd` to the first partition (the `system-boot` partition) and run:
```bash
s tail -c+73 < boot.scr.uimg > out
```

2. This command will generate a file named `out`. Open it with a text editor and, on line `103` change the `bootargs` from
```shell
      setenv bootargs "${bootargs} earlycon console=ttyPS1,115200 console=tty1 clk_ignore_unused uio_pdrv_genirq.of_id=generic-uio xilinx_tsn_ep.st_pcp=4"
```
to 
```shell
      setenv bootargs "${bootargs} earlycon console=ttyPS1,115200 console=tty1 clk_ignore_unused uio_pdrv_genirq.of_id=generic-uio xilinx_tsn_ep.st_pcp=4 cpuidle.off=1"
```

3. Now, still on the `system-boot` partition, run this command to generate the boot image:
```bash
mkimage -C none -A arm -T script -d out boot.scr.uimg
```

4. Plug the sd-card back into the kria and boot. Verify the bootargs with
```bash
ubuntu@kria:~$ cat /proc/cmdline
 root=LABEL=writable rootwait earlycon console=ttyPS1,115200 console=tty1 clk_ignore_unused uio_pdrv_genirq.of_id=generic-uio xilinx_tsn_ep.st_pcp=4 cpuidle.off=1 cma=1000M quiet splash
```

# Solution 2: Doesnt work for now, but should be preferred

You can configure U-Boot to pass these arguments automatically.  You can use the flash-kernel utility and update the bootargs in `/etc/default/flash-kernel`. Here is the how-to:

1. There are two variables you can set in this file to custom the kernel command line: `LINUX_KERNEL_CMDLINE="<args added to end of default bootargs from u-boot env>"` and
`LINUX_KERNEL_CMDLINE_DEFAULTS="<args before the default bootargs from u-boot env>"`

```bash
ubuntu@kria:~$ export LINUX_KERNEL_CMDLINE="cpuidle.off=1"
```

2. After updating the Linux kernel boot arguments, run the flash-kernel utility to update the bootargs passed the Linux kernel on the next reboot. 

```bash
ubuntu@kria:~$ sudo flash-kernel

flash-kernel: installing version 5.15.0-1031-xilinx-zynqmp
Couldn't find DTB  on the following paths: /etc/flash-kernel/dtbs /usr/lib/linux-image-5.15.0-1031-xilinx-zynqmp /lib/firmware/5.15.0-1031-xilinx-zynqmp/device-tree/
Generating u-boot image... done.
Taking backup of image.fit.
Installing new image.fit.
Generating boot script u-boot image... done.
Taking backup of boot.scr.uimg.
Installing new boot.scr.uimg.

```

3. After reboot verify the bootargs
```bash
cat /proc/cmdline
```

# Note 

The problem with this solution is that ubuntu will only be able to use 3 cores of the kria instead of 4 (hence, less performance on the PS). 

The problem is that the hardware manager needs one core to work, and currently, ubuntu has no dynamic disabling of cores while the OS is running. the solution is disabling one core so the ILA can use it for debugging... ideally, once everything is working (i.e., there is no more need to use the ILA), the bootloader should be set back to what it was.