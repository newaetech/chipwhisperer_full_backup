
`ifdef USE_VENDOR_PRIMITIVES

`define MAC16_PRIMITIVE				mac16_artix7
`define ADDER32_PRIMITIVE			adder32_artix7
`define ADDER47_PRIMITIVE			adder47_artix7
`define SUBTRACTOR32_PRIMITIVE	subtractor32_artix7

`else

`define MAC16_PRIMITIVE				mac16_generic
`define ADDER32_PRIMITIVE			adder32_generic
`define ADDER47_PRIMITIVE			adder47_generic
`define SUBTRACTOR32_PRIMITIVE	subtractor32_generic

`endif
