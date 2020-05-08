/**
 * @file
 * @brief Defines BA431 Random generator registers and fields
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#include <stdint.h>
#include "cryptolib_def.h"

#ifndef BA431_CONFIG_H
#define BA431_CONFIG_H

#include <stdint.h>
#include "cryptolib_def.h"

/**
* @brief Structure that represent the register map of the BA431
*/
typedef struct BA431Regs_s
{
   volatile uint32_t ControlReg;       /**< (0x00) Control register */
   volatile uint32_t FIFOLevelReg;     /**< (0x04) FIFO Level: number of 32-bit
                                        * words of random in the FIFO */
   volatile uint32_t FIFOThresholdReg; /**< (0x08) FIFO threshold: number of
                                        * 32-bit words of random in the FIFO that
                                        * causes BA431 to wake up */
   volatile uint32_t FIFODepthReg;     /**< (0x0C) FIFO Depth  : maximum number
                                        * of 32-bit words in the FIFO */
   volatile uint32_t Key0Reg;          /**< (0x10) Key[0] */
   volatile uint32_t Key1Reg;          /**< (0x14) Key[1] */
   volatile uint32_t Key2Reg;          /**< (0x18) Key[2] */
   volatile uint32_t Key3Reg;          /**< (0x1C) Key[3] */
   volatile uint32_t TestDataReg;      /**< (0x20) Test Data register */
   volatile uint32_t RepThresReg;      /**< (0x24) Repetition Count Threshold */
   volatile uint32_t P1ThresReg;       /**< (0x28) Adaptive Proportion Threshold 1 (64-sample window) */
   volatile uint32_t P2ThresReg;       /**< (0x2C) Adaptive Proportion Threshold 2 (4096-sample window) */
   volatile uint32_t StatusReg;        /**< (0x30) Status register */
   volatile uint32_t InitWaitReg;      /**< (0x34) Initial Wait Counter value:
                                        * number of clock cycles to wait before
                                        * sampling data from the noise source */
   volatile uint32_t DisableOsc0Reg;   /**< (0x38) Disable bits for oscillators 0-31 */
   volatile uint32_t DisableOsc1Reg;   /**< (0x3C) Disable bits for oscillators 32-63 */
   volatile uint32_t SwOffTmrValReg;   /**< (0x40) Switch Off Timer value:
                                        * number of clock cycles to wait after
                                        * FIFO is full before going to sleep */
   volatile uint32_t ClkDivReg;        /**< (0x44) Clock Divider register:
                                        * determines sample frequency */
   volatile uint32_t AIS31Conf0Reg;    /**< (0x48) AIS31 Configuration register 0 */
   volatile uint32_t AIS31Conf1Reg;    /**< (0x4C) AIS31 Configuration register 1 */
   volatile uint32_t AIS31Conf2Reg;    /**< (0x50) AIS31 Configuration register 2 */
   volatile uint32_t AIS31StatusReg;   /**< (0x54) AIS31 Status register */
   volatile uint32_t HwConfig;         /**< (0x58) Hardware configuration register */
} BA431Regs_t;


#define BA431_REGS    ((struct BA431Regs_s *)     ADDR_BA431_REGS)
#define CRYPTOLIB_BA431_FIFO    ((volatile uint32_t *)      ADDR_BA431_FIFO)


/** @brief mask for ::BA431Regs_t.StatusReg TestData register busy*/
#define BA431_STAT_MASK_TESTDATA_BUSY     0x00000001
/** @brief mask for ::BA431Regs_t.StatusReg BA431 state*/
#define BA431_STAT_MASK_STATE             0x0000000E
/** @brief mask for ::BA431Regs_t.StatusReg repetition count test interrupt status */
#define BA431_STAT_MASK_REP_FAIL          0x00000010
/** @brief mask for ::BA431Regs_t.StatusReg adaptive proportion test (1024-sample window)
 * interrupt status */
#define BA431_STAT_MASK_PROP_FAIL         0x00000020
/** @brief mask for ::BA431Regs_t.StatusReg FIFO full interrupt status */
#define BA431_STAT_MASK_FULL_INT          0x00000080
/** @brief mask for ::BA431Regs_t.StatusReg AIS31 preliminary noise alarm status */
#define BA431_STAT_MASK_PREALM_INT        0x00000100
/** @brief mask for ::BA431Regs_t.StatusReg AIS31  noise alarm status */
#define BA431_STAT_MASK_ALM_INT           0x00000200
/** @brief mask for ::BA431Regs_t.StatusReg startup tests failure*/
#define BA431_STAT_MASK_STARTUP_FAIL      0x00000400

/** @brief value of ::BA431Regs_t.ControlReg to enable NDRNG*/
#define BA431_CTRL_NDRNG_ENABLE           0x00000001
/** @brief value of ::BA431Regs_t.ControlReg to enable pseudo-random generator
 * for simulation*/
#define BA431_CTRL_SYNC_MODE              0x00000002
/** @brief value of ::BA431Regs_t.ControlReg to enable conditioning function test mode*/
#define BA431_CTRL_TEST_ENABLE            0x00000004
/** @brief value of ::BA431Regs_t.ControlReg to bypass conditioning function*/
#define BA431_CTRL_COND_BYPASS            0x00000008
/** @brief value of ::BA431Regs_t.ControlReg to enable interrupt for repetition
 * count test failure*/
#define BA431_CTRL_INT_REP_EN             0x00000010
/** @brief value of ::BA431Regs_t.ControlReg to enable interrupt for adaptive
 * proportion test failure (1024-sample window)*/
#define BA431_CTRL_INT_PROP_EN            0x00000020
/** @brief value of ::BA431Regs_t.ControlReg to enable interrupt for FIFO full*/
#define BA431_CTRL_INT_FIFOFULL_EN        0x00000080
/** @brief value of ::BA431Regs_t.ControlReg to do software reset*/
#define BA431_CTRL_SOFTRESET              0x00000100
/** @brief value of ::BA431Regs_t.ControlReg to enable interrupt for AIS31 preliminary noise alarm*/
#define BA431_CTRL_INT_PREALM_EN          0x00000200
/** @brief value of ::BA431Regs_t.ControlReg to enable interrupt for AIS31 noise alarm*/
#define BA431_CTRL_INT_ALM_EN             0x00000400
/** @brief value of ::BA431Regs_t.ControlReg to force oscillators to run when FIFO is full*/
#define BA431_CTRL_FORCE_RUN              0x00000800
/** @brief value of ::BA431Regs_t.ControlReg to bypass NIST startup test*/
#define BA431_CTRL_BYPASS_HEALTH_TEST     0x00001000
/** @brief value of ::BA431Regs_t.ControlReg to bypass AIS31 startup test*/
#define BA431_CTRL_BYPASS_AIS31_TEST      0x00002000
/** @brief value of ::BA431Regs_t.ControlReg to use conditioning as input to the
 * health test module instead of the noise source */
#define BA431_CTRL_HEALTH_TEST_SEL        0x00004000
/** @brief value of ::BA431Regs_t.ControlReg to use conditioning as input to the
 * AIS31 test module instead of the noise source */
#define BA431_CTRL_AIS31_TEST_SEL         0x00008000
/** @brief mask for ::BA431Regs_t.ControlReg, for the setting that controls the
 * amount of 128-bit blocks of data that are used as input to the conditioning function*/
#define BA431_CTRL_MASK_NB_128BIT_BLOCKS  0x000F0000
/** @brief offset for ::BA431Regs_t.ControlReg, for the setting that controls the
 * amount of 128-bit blocks of data that are used as input to the conditioning function*/
#define BA431_CTRL_OFFSET_NB_128BIT_BLOCKS 16

/* Macros to be used on the HwConfig register of the BA431 TRNG */
#define BA431_HW_CONF_NUM_RINGS_LSB                         0
#define BA431_HW_CONF_NUM_RINGS_MASK                        (0xFF<<BA431_HW_CONF_NUM_RINGS_LSB)
#define BA431_HW_CONF_AIS31_INCLUDED_LSB                    8
#define BA431_HW_CONF_AIS31_INCLUDED_MASK                   (1L<<BA431_HW_CONF_AIS31_INCLUDED_LSB)
#define BA431_HW_CONF_AIS31_PARAM_ACCESS_SUPPORTED_LSB      9
#define BA431_HW_CONF_AIS31_PARAM_ACCESS_SUPPORTED_MASK     (1L<<BA431_HW_CONF_AIS31_PARAM_ACCESS_SUPPORTED_LSB)

#define BA431_HW_CFG (const uint32_t)(BA431_REGS->HwConfig)


/**
* @brief Enumeration of possible BA431 states.
*/
typedef enum ba431_state_e
{
BA431_STATE_RESET       = 0x00000000,    /**< Reset */
BA431_STATE_STARTUP     = 0x00000002,    /**< Start-up */
BA431_STATE_FIFOFULLON  = 0x00000004,    /**< FIFO full - on */
BA431_STATE_FIFOFULLOFF = 0x00000006,    /**< FIFO full - off */
BA431_STATE_RUNNING     = 0x00000008,    /**< Running */
BA431_STATE_ERROR       = 0x0000000A     /**< Error */
} ba431_state_t;


/**
* @brief Returns random word from FIFO
* @return 32-bit random value
*/
uint32_t ba431_read_fifovalue(void);


/**
* @brief Returns the current BA431 fifolevel
* @return fifolevel value
*/
uint32_t ba431_read_fifolevel(void);


/**
* @brief Returns the current BA431 status
* @return BA431 status
*/
uint32_t ba431_read_status(void);

/**
* @brief Write an user-defined value to the BA431 control register
* @return BA431 ControlReg
*/
void ba431_write_controlreg(uint32_t control);

/**
* @brief Returns the current BA431 ControlReg
* @return BA431 ControlReg
*/
uint32_t ba431_read_controlreg(void);

/**
 * @brief Read the fifo level at which BA431 should wake up
 * @return threshold fifo value
 */
uint32_t ba431_read_fifo_wakeup_threshold();

/**
* @brief Writes the fifo level at which BA431 should wake up
* @param fifo_threshold
*/
void ba431_write_fifo_wakeup_threshold(uint32_t fifo_threshold);

/**
* @brief Read the current number of cycles before the ring will switch off
*        when the fifo becomes full.
* @return the timer values (expressed in cycles)
*/
uint32_t ba431_read_switch_off_timer();

/**
* @brief Writes switch off timer value
* @param sw_off_timer_val
*/
void ba431_write_switch_off_timer(uint32_t sw_off_timer_val);

/**
 * @brief Read the current clock divider (Fsampling = Fclock / ClkDiv + 1)
 * @return the current clock divider value
 */
uint32_t ba431_read_clk_div();

/**
* @brief Writes clock divider for the sampling frequency
* @param clk_div
*/
void ba431_write_clk_div(uint8_t clk_div);

/**
 * @brief Read the number of cycles to wait before sampling the noise source
 * @return the number of cycles
 */
uint32_t ba431_read_init_wait();

/**
* @brief Writes number of clock cycles to wait before sampling data from the noise source
* @param init_wait_val
*/
void ba431_write_init_wait(uint32_t init_wait_val);


/**
* @brief Clear BA431 control register
*/
void ba431_clear_ctrl(void);


/**
* @brief Enable BA431 configured as NDRNG source
*/
void ba431_enable_ndrng(void);


/**
* @brief Disable BA431 configured as NDRNG source
*/
void ba431_disable_ndrng(void);


/**
* @brief Enable BA431 configured in test mode
*/
void ba431_enable_test_mode(void);


/**
* @brief Disable BA431 configured in test mode
*/
void ba431_disable_test_mode(void);


/**
* @brief Configure BA431 for conditioning test
*/
void ba431_set_cond_test_mode(void);


/**
* @brief Enable BA431 IRQ for health test failures
*/
void ba431_enable_health_test_irq(void);


/**
* @brief Disable BA431 IRQ for healths test failures
*/
void ba431_disable_health_test_irq(void);


/**
* @brief Enable BA431 IRQ when FIFO is full
*/
void ba431_enable_fifofull_irq(void);


/**
* @brief Disable BA431 IRQ when FIFO is full
*/
void ba431_disable_fifofull_irq(void);


/**
* @brief Software reset of the BA431
*/
void ba431_softreset(void);


/**
* @brief Enable BA431 IRQ for AIS31 test failures
*/
void ba431_enable_AIS31_test_irq(void);


/**
* @brief Force oscillators to run even when FIFO is full
*/
void ba431_force_run(void);


/**
* @brief Disable BA431 IRQ for AIS31 test failures
*/
void ba431_disable_AIS31_test_irq(void);

/**
* @brief Set length of input data of AES-CBCMAC post-processing
*/
void ba431_set_nb128bitblocks(uint8_t nb128bitblocks);

/**
 * @brief Read the current conditioning key
 * @param key pointer to an array of 4 32b words
 */
void ba431_read_conditioning_key(uint32_t *key);

/**
* @brief BA431 write conditioning key
* @param key points to 4x32-bit words containing the key to use
*/
void ba431_write_conditioning_key(const uint32_t * key);

/**
* @brief Write test data to BA431
* @param test_data word to write
*/
void ba431_write_test_data(uint32_t test_data);

/**
* @brief Get the current state of BA431
*/
ba431_state_t ba431_get_state(void);

#endif /* BA431_CONFIG_H */
