#include <stdint.h>
#include "k82f_hal.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_port.h"


/* Similar trace info at:
  https://mcuoneclipse.com/2016/11/05/tutorial-getting-etm-instruction-trace-with-nxp-kinetis-arm-cortex-m4f/
  
  The K82F doesn't have the ETF or MCM register, so just turn on outputs is enough to be tracin' in the USA.
*/

static void KinetisTrace_ConfigureGPIO(void) {
    uint32_t value;

    #define PORT_PCR_DSE_ENABLE       (1<<6)  /* Port Configuration Register, Drive Strength Enable (DSE) bit */
    #define PORT_PCR_MUX_ALTERNATE_4  (4<<8) /* Port Configuration Register, Alternate 4 function (mux as trace pin) */
    #define PORT_PCR_CONFIG_FOR_TRACE (PORT_PCR_DSE_ENABLE|PORT_PCR_DSE_ENABLE|PORT_PCR_MUX_ALTERNATE_4) /* for trace, mux it with function 5 and high drive strength */

    /* check and enable clocking of PORTE */
    value = SIM->SCGC5; /* read SIM_SCGC5 at 0x40048038 */
    if ((value & (1<<13)) == 0) { /* Bit13 in SCGC5 is the PortE clock gate control bit. Clock not already enabled? */
        SIM->SCGC5 |= (1<<13);    /* Enabling clock gate for Port E */
    }
    value = SIM->SOPT2; /* SIM_SOPT2 at 0x40048004 */
    if ((value&(1<<12))==0) { /* Bit 12 enables the trace clock. Is the debug trace clock not already enabled? */
        SIM->SOPT2 |= (1<<12); /* Debug trace clock select = Core/system clock */
    }

    CLOCK_EnableClock(kCLOCK_PortA); //enable port a clock
    PORT_SetPinMux(PORTA, 16, kPORT_MuxAlt4);
    PORT_SetPinMux(PORTA, 15, kPORT_MuxAlt4);
    PORT_SetPinMux(PORTA, 14, kPORT_MuxAlt4);
    PORT_SetPinMux(PORTA, 13, kPORT_MuxAlt4);
    PORT_SetPinMux(PORTA, 12, kPORT_MuxAlt4);


}

void etmtrace_enable(void)
{
    KinetisTrace_ConfigureGPIO();
}
