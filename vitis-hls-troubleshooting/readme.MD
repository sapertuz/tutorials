# Vitis HLS Troubleshooting Log

## Key Learnings and Common Issues

### 1. **Read the Warnings!**
Even if your code synthesizes successfully, warnings often indicate areas for improvement.

- **Out-of-bounds access warnings**: These can lead to incorrect results and must be addressed immediately.
- **Latency and pipeline warnings**: Often hint at suboptimal performance or potential deadlocks.

> **Example:**
> If you see a warning like:
> ```
> WARNING: Possible out-of-bounds memory access at line X
> ```
> Double-check your loop indices and memory accesses to ensure they stay within valid bounds.

---

### 2. **Ensure Proper Loop Termination with `#pragma HLS DATAFLOW`**
Using `#pragma HLS DATAFLOW` can greatly improve performance by allowing parallel execution of different functions, but:

- If loop termination conditions are incorrect, the design may **hang and stop processing new data**.
- Always verify that your loops will eventually terminate in **both simulation and synthesis**.

> **Example:**
> If processing hangs after the first batch of data, double-check termination conditions:
> ```cpp
> for (int i = 0; i < N; i++) {
>     #pragma HLS PIPELINE II=1
>     process(data[i]);
> }
> ```
> Ensure `N` is properly defined and that loop control logic is correct.

- When testing on **hardware (FPGA board)**, always run more than **one batch of data sequentially** to confirm continued operation.

---

### 3. **Watch Out for Timing Violations**
Timing violations can prevent the design from processing multiple data batches properly.

- In **DATAFLOW** designs, consider **increasing stream buffer sizes**.
- In **PIPELINE** designs, adjusting the **II (Initiation Interval)** can help resolve timing issues.

> **Example Fix for DATAFLOW Timing Violations:**
> ```cpp
> #pragma HLS STREAM variable=data depth=64
> ```
>
> **Example Fix for PIPELINE Timing Violations:**
> ```cpp
> #pragma HLS PIPELINE II=4
> ```
> Increasing `II` allows more flexibility for scheduling operations within timing constraints.

---

### 4. **Handling Designs That Only Process One Batch at a Time**
Some designs are too complex to handle multiple batches simultaneously. If that is acceptable, you can add an external reset mechanism to reinitialize the core automatically.

A **return stream signal** can be used to reset the core after processing completes:

```cpp
void reset_proc(hls::stream<ap_uint<1>> &return_stream_cond, ap_uint<1> &reset_out) {
    volatile int ii = 0;
    volatile char reset_out_tmp = 0;
    volatile char flag = 0;

    #pragma HLS reset variable = reset_out_tmp
    #pragma HLS reset variable = flag
    #pragma HLS reset variable = ii

    return_loop:
    while (true) {
        #pragma HLS PIPELINE II = 1
        if (!return_stream_cond.empty()) {
            ap_uint<1> return_val = return_stream_cond.read();
            if (return_val == 1) {
                flag = 1;
            }
        } else {
            if (flag == 1) {
                ii++;
                reset_out_tmp = (ii >> 8) & 0x1;
            }
            reset_out = reset_out_tmp;
            if (reset_out_tmp == 1) {
                ii = 0;
                flag = 0;
                reset_out_tmp = 0;
                break;
            }
        }
    }
}
```

- The `reset_out` signal is an external **ap_none port**.
- If your design uses **active-low reset**, you **must** connect it to a NOT gate in the **IP Integrator**.
- **Important:** Simply changing the value in HLS **won't work** because all signals are reset to **zero** during synthesis.
