/*! \file system.h
 *  \brief Basic system control API definition
 *
 *  A simple API providing general system control functions like
 *  - PLL control
 *  - interrupt enable/disable
 *  - access protection enable/disable
 *  - software reset
 *  - power management
 *
 *  \autor TGL
 *
 *  \version
 *    08.08.2010  initial version
 *    13.09.2010  GetExtClock function added
 *
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/*! \brief Check if cache is enabled
 */
     int SYSTEM_IsCacheEnabled(void);

/*! \brief Enable/disable cache
 */
     void SYSTEM_EnaDisCache(int Enable);

/*   0,1,2 ... core WDT
 *   3     ... safety WDT
 */
     void SYSTEM_EnableProtectionExt(int Sel);
     void SYSTEM_DisableProtectionExt(int Sel);

     void SYSTEM_EnableSecProtection(void);
     void SYSTEM_DisableSecProtection(void);

     unsigned long SYSTEM_GetStmClock(void);

     unsigned long SYSTEM_GetCanClock(void);
/*! \brief System initialisation
 *
 *  Do basic system initialisation like
 *  - PLL setup
 */
void SYSTEM_Init(void);

/*! \brief Get external clock frequency
 *
 *  Return external clock frequency. Usually this is the system's
 *  crystal or oscillator frequency.
 *  \return External clock frequency, unit Hz
 */
unsigned long SYSTEM_GetExtClock(void);

/*! \brief Get CPU clock frequency
 *
 *  Return CPU clock frequency. Usually this is the core frequency.
 *  \return CPU clock frequency, unit Hz
 */
unsigned long SYSTEM_GetCpuClock(void);

/*! \brief Get system clock frequency
 *
 *  Return system clock frequency. Usually this is the peripheral frequency.
 *  \return System clock frequency, unit Hz
 */
unsigned long SYSTEM_GetSysClock(void);

/*! \brief Globally enable interrupts
 */
void SYSTEM_EnableInterrupts(void);

/*! \brief Globally disable interrupts
 */
void SYSTEM_DisableInterrupts(void);

/*! \brief Globally enable access protection
 *
 *  This function is optional. If the architecture doesn't support access
 *  protection this function does nothing.
 */
void SYSTEM_EnableProtection(void);

/*! \brief Globally disable access protection
 *
 *  This function is optional. If the architecture doesn't support access
 *  protection this function does nothing.
 */
void SYSTEM_DisableProtection(void);

/*! \brief Execute software reset
 */
int SYSTEM_Reset(void);

/*! \brief Execute Idle instruction
 */
int SYSTEM_Idle(void);

/*! \brief Execute power down function
 */
int SYSTEM_Sleep(void);

/*! \brief Debug break system
 */
void SYSTEM_DbgBreak(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SYSTEM_H__ */
