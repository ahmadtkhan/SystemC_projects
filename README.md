Two projects written using SystemC. 

1. **Hardware Accelerator**
  - Implements a 8x8 multiplier using 1 bit carry propagate adders and carry save adders. The idea is to delay carry propagation by saving carry bits for later instead of immediately propagating carries, enabling fast multi-operand addition by compressing many operands into sum and carry, which are added in the last row/step using CPA.
2. **Image Encoder/Decoder**
  - Takes a bmp image, divides it into blocks, applies forward discrete transform function, transforms the image into distinct values using quantization table, and applies a zig-zag sequence. To improve system perforamce, FDCT and IDCT are written as a separate hardware module and a cosine table is transferred to the modules to keep the hardware area
minimized.

