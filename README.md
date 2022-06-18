# RTL generator

  Generator of SystemC, VHDL and SystemVerilog projects.

## Main Idea

  River CPU and peripheries libraries have different implementation versions
on both RTL languages VHDL and System Verilog. I am using 
SystemC implementation as the main reference model. To support consistence 
of all these implementations with the reference SystemC I am goind to use
this RTL generator.

## Code generator requirements:

- Preserve project structure and files
- Support commentaries in the same way as in the current manual implementation
- Dynamically track global and local configuration parameters to compute their values
- Preserve text names of configuration parameters in the signal bus declarations

## Example System Verilog:

    // Code commentary
    localparam int CFG_PARAM1 = 128;
    localparam int CFG_PARAM2 = 64;

    logic [CFG_PARAM1-1:0] A;
    logic [CFG_PARAM1-CFG_PARAM2-1:0] B;

## Example System C:

    // Code commentary
    static const int CFG_PARAM1 = 128;
    static const int CFG_PARAM2 = 64;

    sc_biguint<CFG_PARAM1> A;
    sc_uint<CFG_PARAM1-CFG_PARAM2> B;

<b> Generator should preserve parameters names and automatically detect
bus width to properly <i>sc_uint</i>/<i>sc_biguint</i>.</b>

