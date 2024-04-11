/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA431 (N)DRNG
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */

#include "sx_trng.h"
#include "cryptolib_def.h"
#include "sx_memcpy.h"
#include "ba431_config.h"
#include "ba414ep_config.h"
#include <string.h>
#include "sx_errors.h"


static bool rng_startup_failed = false;
static bool rng_needs_startup_chk = false;

#ifndef UNITTESTS

/**
* @brief Verify the conditioning function of the BA431 against test patterns.
* @return CRYPTOLIB_SUCCESS if successful CRYPTOLIB_CRYPTO_ERR otherwise
*/
static uint32_t sx_trng_conditioning_test(void)
{
   const uint32_t test_data[16]     = {0xE1BCC06B, 0x9199452A, 0x1A7434E1, 0x25199E7F,
                                       0x578A2DAE, 0x9CAC031E, 0xAC6FB79E, 0x518EAF45,
                                       0x461CC830, 0x11E45CA3, 0x19C1FBE5, 0xEF520A1A,
                                       0x45249FF6, 0x179B4FDF, 0x7B412BAD, 0x10376CE6};
   const uint32_t known_answer[4]   = {0xA1CAF13F, 0x09AC1F68,0x30CA0E12, 0xA7E18675};
   const uint32_t test_key[4]       = {0x16157E2B, 0xA6D2AE28,0x8815F7AB, 0x3C4FCF09};

   uint32_t i;
   uint32_t error = 0;

   /*Soft reset*/
   ba431_softreset();

   /*Enable test mode*/
   ba431_set_nb128bitblocks(4);
   ba431_set_cond_test_mode();

   /*Write key*/
   ba431_write_conditioning_key(test_key);

   /*Write test data input*/
   for(i=0;i<sizeof(test_data)/4;i++)
      ba431_write_test_data(test_data[i]);

   /*Wait for conditioning test to complete --> wait for return data to appear in FIFO*/
   while(ba431_read_fifolevel()<4);

   /*Clear control register*/
   ba431_clear_ctrl();

   /*Compare results to known answer*/
   for(i=0;i<sizeof(known_answer)/4;i++){
      error |= ba431_read_fifovalue() ^ known_answer[i];
   }
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   return CRYPTOLIB_SUCCESS;
}

/**
* @brief Poll for the end of the BA431 startup tests and check result.
* @return CRYPTOLIB_SUCCESS if successful CRYPTOLIB_CRYPTO_ERR otherwise
*/
static uint32_t sx_trng_wait_startup(void)
{
   uint32_t ba431_status;
   ba431_state_t ba431_state;

   /*Wait until startup is finished*/
   do{
      ba431_status = ba431_read_status();
      ba431_state = (ba431_state_t) (ba431_status & BA431_STAT_MASK_STATE);
   }while((ba431_state == BA431_STATE_RESET) || (ba431_state == BA431_STATE_STARTUP));

   /*Check startup test result*/
   if(ba431_status & BA431_STAT_MASK_STARTUP_FAIL){
      return CRYPTOLIB_CRYPTO_ERR;
   }

   return CRYPTOLIB_SUCCESS;
}


static const block_t random_blk = {(uint8_t *) ADDR_BA431_FIFO, 0, BLOCK_S_CONST_ADDR};

void sx_trng_get_rand_blk(block_t dest)
{
#if AIS31_ENABLED
   if (ba431_read_status() & BA431_STAT_MASK_PREALM_INT) {
      CRYPTOLIB_PRINTF("Preliminary noise alarm detected.\n");
      sx_trng_apply_soft_reset();
   }
#endif

   if (rng_needs_startup_chk) {
      sx_trng_wait_startup();
      rng_needs_startup_chk = false;
   }

   memcpy_blk(dest, random_blk, dest.len);

}


void sx_trng_get_rand(uint8_t * dst, uint32_t size)
{
    sx_trng_get_rand_blk(block_t_convert(dst, size));
}

#endif

uint32_t sx_trng_init_part1(uint32_t cond_test_en)
{
   if(cond_test_en) {
      /*Conditioning function test*/
      uint32_t status = sx_trng_conditioning_test();
      if(status){
         rng_startup_failed = true;
         return CRYPTOLIB_CRYPTO_ERR;
      }
   }

   /*Soft reset*/
   ba431_softreset();

   /*Program powerdown and clock settings*/
   ba431_write_fifo_wakeup_threshold(RNG_FIFO_WAKEUP_LVL);
   if (RNG_OFF_TIMER_VAL<0) {
      ba431_force_run();
   } else {
      ba431_write_switch_off_timer(RNG_OFF_TIMER_VAL);
   }
   ba431_write_clk_div(RNG_CLKDIV);
   ba431_write_init_wait(RNG_INIT_WAIT_VAL);

   /*Configure conditioning test*/
   ba431_set_nb128bitblocks(RNG_NB_128BIT_BLOCKS);

   /*Enable NDRNG*/
   ba431_enable_ndrng();

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_trng_init_part2(void)
{
   uint32_t key[4];

   /*Check startup tests*/
   uint32_t status = sx_trng_wait_startup();
   if (status) {
      rng_startup_failed = true;
      return CRYPTOLIB_CRYPTO_ERR;
   }

   /*Program random key for the conditioning function*/
   key[0] = ba431_read_fifovalue();
   key[1] = ba431_read_fifovalue();
   key[2] = ba431_read_fifovalue();
   key[3] = ba431_read_fifovalue();
   ba431_write_conditioning_key(key);

   /*Soft reset to flush FIFO*/
   sx_trng_apply_soft_reset();

   /*Enable interrupts for health tests (repetition and adaptive proportion tests) & AIS31 test failures */
   ba431_enable_health_test_irq();
#if AIS31_ENABLED
   ba431_enable_AIS31_test_irq();
#endif

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_trng_init(uint32_t cond_test_en)
{
   uint32_t status = sx_trng_init_part1(cond_test_en);
   if (status)
      return status;

   return sx_trng_init_part2();
}

bool sx_trng_startup_failed(void)
{
   return rng_startup_failed;
}

void sx_trng_set_startup_chk_flag(void)
{
   rng_needs_startup_chk = true;
}

bool sx_trng_critical_raised(void)
{
   uint32_t status = ba431_read_status();
   uint32_t critical_mask = BA431_STAT_MASK_REP_FAIL | BA431_STAT_MASK_PROP_FAIL;
#if AIS31_ENABLED
   critical_mask |= BA431_STAT_MASK_ALM_INT;
#endif
   return status & critical_mask;
}

void sx_trng_apply_soft_reset(void)
{
   ba431_softreset();
   sx_trng_set_startup_chk_flag();
}

uint32_t sx_trng_get_word(void)
{
   uint32_t rand = 0;
   sx_trng_get_rand((uint8_t*)&rand, sizeof(rand));
   return rand;
}

void sx_trng_fill_blk(void *param, block_t result)
{
   (void)param;
   sx_trng_get_rand_blk(result);
}

bool sx_trng_is_error_detected(void)
{
   return ba431_get_state() == BA431_STATE_ERROR;
}

void sx_trng_trigger_failure(void)
{
   //Disable TRNG rings
   ba431_disable_ndrng();
   //Enable test mode
   ba431_enable_test_mode();
   //Send zeros to testdata register until all continuous tests fail
   while (1) {
      ba431_write_test_data(0);
      //Poll for continuous test failure (PROP will always be the last to fail)
      if (ba431_read_status() & BA431_STAT_MASK_PROP_FAIL) {
         break;
      }
   }
   //Restore settings
   ba431_disable_test_mode();
   ba431_enable_ndrng();
}

uint32_t sx_trng_trigger_test_failure(sx_trng_test_t test)
{
   uint32_t ctrl_reg, size, i;

   /*Set test mode*/
   switch (test) {
   case TRNG_REP_TEST:
      ctrl_reg = BA431_CTRL_TEST_ENABLE | BA431_CTRL_INT_REP_EN;
      size = 8;
      break;
   case TRNG_PROP_TEST:
      ctrl_reg = BA431_CTRL_TEST_ENABLE | BA431_CTRL_INT_PROP_EN;
      size = 64;
      break;
#if AIS31_ENABLED
   case TRNG_PREALM_TEST:
      ctrl_reg = BA431_CTRL_TEST_ENABLE | BA431_CTRL_INT_PREALM_EN;
      size = 8;
      break;
   case TRNG_ALM_TEST:
      ctrl_reg = BA431_CTRL_TEST_ENABLE | BA431_CTRL_INT_ALM_EN;
      size = 2048;
      break;
#endif
   default:
      return CRYPTOLIB_INVALID_PARAM;
      break;
   }

   /*Disable TRNG rings and health tests*/
   ba431_disable_ndrng();
   ba431_disable_health_test_irq();
#if AIS31_ENABLED
   ba431_disable_AIS31_test_irq();
#endif
   /*Set test mode and the health test to be checked*/
   BA431_REGS->ControlReg |= ctrl_reg;

#if AIS31_ENABLED
   /*Make sure that startup test is passed for AIS31.
    *That requires 4 truly random words to be present in the FIFO.
    *Writing them here.*/
   if ((test == TRNG_PREALM_TEST) || (test == TRNG_ALM_TEST)) {
      ba431_write_test_data(0x6EA3EE32);
      ba431_write_test_data(0x507E68A4);
      ba431_write_test_data(0x4D61C5C5);
      ba431_write_test_data(0xAD2C4AD8);
   }
#endif

   /*Trigger error*/
   for (i = 0; i < size; i++) {
      ba431_write_test_data(0);
   }

   /*Restore settings*/
   BA431_REGS->ControlReg &= ~ctrl_reg;
   ba431_enable_ndrng();
   ba431_enable_health_test_irq();
#if AIS31_ENABLED
   ba431_enable_AIS31_test_irq();
#endif

   return CRYPTOLIB_SUCCESS;
}

void sx_trng_save_state(struct sx_trng_internal_state *copy)
{
   copy->control = ba431_read_controlreg();
   ba431_read_conditioning_key(copy->key);
   copy->wakeup_lvl =  ba431_read_fifo_wakeup_threshold();
   copy->swoff_timer = ba431_read_switch_off_timer();
   copy->clk_divider = ba431_read_clk_div();
   copy->init_wait_cnt = ba431_read_init_wait();
}

void sx_trng_restore_state(struct sx_trng_internal_state *source)
{
   ba431_write_init_wait(source->init_wait_cnt);
   ba431_write_clk_div(source->clk_divider);
   ba431_write_switch_off_timer(source->swoff_timer);
   ba431_write_fifo_wakeup_threshold(source->wakeup_lvl);
   ba431_write_conditioning_key(source->key);
   ba431_write_controlreg(source->control);
}
