// Note: adapted from openadc/hdl/hdl/registers.v; also includes registers
// specific to CW-Pro.
// This isn't the actual file used in building the CW-Lite/Pro bitfiles, but
// since there is no need to rebuild those bitfiles, it'll do.
`define GAIN_ADDR    	0
`define SETTINGS_ADDR  1
`define STATUS_ADDR    2
`define ADCREAD_ADDR   3
`define ECHO_ADDR      4
`define EXTFREQ_ADDR   5	 
`define ADVCLOCK_ADDR  6
`define SYSTEMCLK_ADDR 7
`define ADCFREQ_ADDR   8	 
`define PHASE_ADDR     9 	 
`define VERSION_ADDR	10
`define DECIMATE_ADDR  15
`define SAMPLES_ADDR   16
`define PRESAMPLES_ADDR 17
`define RETSAMPLES_ADDR 18
`define TRIGGER_DUR_ADDR 20
`define OFFSET_ADDR    26
`define REGISTER_VERSION 1
`define CW_EXTCLK_ADDR		38
`define CW_TRIGSRC_ADDR		39
`define CW_TRIGMOD_ADDR		40
`define CW_IOROUTE_ADDR    55
`define GLITCHCYCLES_CNT   19
`define CLOCKGLITCH_SETTINGS	51
`define CLOCKGLITCH_OFFSET    25
`define CLOCKGLITCH_OFFSET_LEN 4
`define RECONFIG_REG 52
`define SAD_STATUSCFG_ADDR 53
`define SAD_REFDATA_ADDR   54
`define GLITCH_RECONFIG_RB_ADDR 56
`define IODECODETRIG_CFG_ADDR  57
`define IODECODETRIG_DATA_ADDR 58
`define CW_IOREAD_ADDR       59
