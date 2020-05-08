#include "sam4l_hal.h"
#include "sam4l_aes.h"

#define GENCLK_SRC_CLK_CPU 7

/**
 * \brief Initialize the AES module.
 *
 * \param dev_inst    Device structure pointer.
 * \param aesa         Base address of the AESA instance.
 * \param cfg         Pointer to AES configuration.
 *
 * \retval true if the initialization was successful.
 * \retval false if initialization failed.
 */
void HW_AES128_Init(void)
{
	periclk_aesa_init();
	SCIF->SCIF_GCCTRL[AESA_GCLK_NUM].SCIF_GCCTRL = SCIF_GCCTRL_OSCSEL(GENCLK_SRC_CLK_CPU) |  SCIF_GCCTRL_CEN;

    /* AES Enable */
    AESA->AESA_CTRL = AESA_CTRL_ENABLE | AESA_CTRL_NEWMSG; /* Enable, auto-accept new messages */
	
	//Use with debugger to check PARAMETER register value
	//volatile uint32_t param = AESA->AESA_PARAMETER;
	
    /* AES Mode */
    //AESA->AESA_MODE = AESA_MODE_ENCRYPT | (AESA_MODE_CTYPE(0x0F)); /* Encrypt Mode, with all countermeasures */    
	AESA->AESA_MODE = AESA_MODE_ENCRYPT; /* Encrypt Mode, without countermeasures */   
    
    /* Setup random seed for countermeasures to work */
    AESA->AESA_DRNGSEED = 0xDEADBEEF; //A very random number        
}

void HW_AES128_LoadKey(uint8_t * key)
{
    AESA->AESA_KEY[0].AESA_KEY = *(WoReg *)(key + 0);
    AESA->AESA_KEY[1].AESA_KEY = *(WoReg *)(key + 4);
    AESA->AESA_KEY[2].AESA_KEY = *(WoReg *)(key + 8);
    AESA->AESA_KEY[3].AESA_KEY = *(WoReg *)(key + 12);
}


void HW_AES128_Enc_pretrigger(uint8_t* pt)
{
    AESA->AESA_DATABUFPTR = 0; /* Auto-intecremented */
    AESA->AESA_IDATA = *(WoReg *)(pt + 0);
    AESA->AESA_IDATA = *(WoReg *)(pt + 4);
    AESA->AESA_IDATA = *(WoReg *)(pt + 8);
}

void HW_AES128_Enc(uint8_t * pt)
{
    AESA->AESA_IDATA = *(WoReg *)(pt + 12);
	
	/* Wait */
	while((AESA->AESA_SR & AESA_SR_ODATARDY) == 0);
}

void HW_AES128_Enc_posttrigger(uint8_t* pt)
{
	AESA->AESA_DATABUFPTR = 0;
    *(WoReg *)(pt + 0) = AESA->AESA_ODATA;
	*(WoReg *)(pt + 4) = AESA->AESA_ODATA;
	*(WoReg *)(pt + 8) = AESA->AESA_ODATA;
	*(WoReg *)(pt + 12) = AESA->AESA_ODATA;
}
