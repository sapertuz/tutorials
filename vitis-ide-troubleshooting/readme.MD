# Troubleshooting Vitis 2024.2 Launch Issues on Ubuntu 24.04

## Problem Description

When launching Vitis 2024.2 (`vitis -g`) on Ubuntu 24.04, the IDE fails to open, but no error messages are displayed. Instead, the command runs silently, making debugging difficult.

To identify the issue, we need to modify the Vitis launch script to allow errors to be displayed.

## Step 1: Enabling Debugging in the Vitis Script

By default, the Vitis script suppresses all error messages by redirecting output to `/dev/null`.

### How to Remove Output Redirection
1. Locate the Vitis launch script (vitis command is just a wrapper script):
    ```bash
    nano /tools/Xilinx/Vitis/2024.2/bin/vitis
    ```

2. Find the following line (inside the --gui case handling):
    ```bash
    $XILINX_VITIS/ide/electron-app/lnx64/vitis-ide --no-sandbox --log-level=debug ${analyzeArgs[@]} > /dev/null 2>&1 &
    ```

3. Modify it to remove redirection:
    ```bash
    $XILINX_VITIS/ide/electron-app/lnx64/vitis-ide --no-sandbox --log-level=debug ${analyzeArgs[@]} &
    ```

4. Save and exit (Ctrl + X, then Y, then Enter).
5. Run Vitis again:
    ```bash
    vitis -g
    ```

Now, any errors will be visible in the terminal.

## Step 2: Fixing libstdc++.so.6 Version Errors

After enabling debugging, the following error appears:

```bash
/tools/Xilinx/Vitis/2024.2/ide/electron-app/lnx64/vitis-ide: /tools/Xilinx/Vitis/2024.2/lib/lnx64.o/Ubuntu/libstdc++.so.6: version `GLIBCXX_3.4.26' not found (required by /lib/x86_64-linux-gnu/libgallium-24.2.8-1ubuntu1~24.04.1.so)
```

This means Vitis is trying to use an outdated version of libstdc++.so.6, which lacks required symbols (GLIBCXX_3.4.26, GLIBCXX_3.4.29, etc.).

### Solution: Use System libstdc++.so.6 Instead of Vitis’ Bundled Version

1. Backup the Old Library
    ```bash
    mv /tools/Xilinx/Vitis/2024.2/lib/lnx64.o/Ubuntu/libstdc++.so.6 \ 
       /tools/Xilinx/Vitis/2024.2/lib/lnx64.o/Ubuntu/libstdc++.so.6.bak
    ```

2. Create a Symbolic Link to the System Version
    ```bash
    ln -s /usr/lib/x86_64-linux-gnu/libstdc++.so.6 \
          /tools/Xilinx/Vitis/2024.2/lib/lnx64.o/Ubuntu/libstdc++.so.6
    ```

3. Verify the Fix

   Run:
    ```bash
    vitis -g
    ```

It should now open successfully.

### Alternative Temporary Fix: Override LD_LIBRARY_PATH

If you don’t want to modify Vitis’ installation, you can override the library path at runtime:
    ```bash
    export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
    vitis -g
    ```

However, this needs to be done every time before launching Vitis.

### Why This Works
1. Step 1 (Disabling Output Redirection): Allowed error messages to be displayed.
2. Step 2 (Fixing libstdc++.so.6): Ensured Vitis loads the correct system libraries instead of its outdated version.