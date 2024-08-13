/**
 * \file IfxSrc_regdef.h
 * \brief
 * \copyright Copyright (c) 2014 Infineon Technologies AG. All rights reserved.
 *
 * Version: TC23XADAS_UM_V1.0P1.R0
 * Specification: tc23xadas_um_sfrs_MCSFR.xml (Revision: UM_V1.0p1)
 * MAY BE CHANGED BY USER [yes/no]: No
 *
 *                                 IMPORTANT NOTICE
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * \defgroup IfxLld_Src Src
 * \ingroup IfxLld
 * 
 * \defgroup IfxLld_Src_Bitfields Bitfields
 * \ingroup IfxLld_Src
 * 
 * \defgroup IfxLld_Src_union Union
 * \ingroup IfxLld_Src
 * 
 * \defgroup IfxLld_Src_struct Struct
 * \ingroup IfxLld_Src
 * 
 */
#ifndef IFXSRC_REGDEF_H
#define IFXSRC_REGDEF_H 1
/******************************************************************************/
#include "Ifx_TypesReg.h"
/******************************************************************************/
/** \addtogroup IfxLld_Src_Bitfields
 * \{  */

/** \\brief  Service request register */
typedef struct _Ifx_SRC_SRCR_Bits
{
    unsigned int SRPN:8;                    /**< \brief [7:0] Service Request Priority Number (rw) */
    unsigned int reserved_8:2;              /**< \brief \internal Reserved */
    unsigned int SRE:1;                     /**< \brief [10:10] Service Request Enable (rw) */
    unsigned int TOS:1;                     /**< \brief [11:11] Type of Service Control (rw) */
    unsigned int reserved_12:4;             /**< \brief \internal Reserved */
    unsigned int ECC:5;                     /**< \brief [20:16] ECC (rwh) */
    unsigned int reserved_21:3;             /**< \brief \internal Reserved */
    unsigned int SRR:1;                     /**< \brief [24:24] Service Request Flag (rh) */
    unsigned int CLRR:1;                    /**< \brief [25:25] Request Clear Bit (w) */
    unsigned int SETR:1;                    /**< \brief [26:26] Request Set Bit (w) */
    unsigned int IOV:1;                     /**< \brief [27:27] Interrupt Trigger Overflow Bit (rh) */
    unsigned int IOVCLR:1;                  /**< \brief [28:28] Interrupt Trigger Overflow Clear Bit (w) */
    unsigned int SWS:1;                     /**< \brief [29:29] SW Sticky Bit (rh) */
    unsigned int SWSCLR:1;                  /**< \brief [30:30] SW Sticky Clear Bit (w) */
    unsigned int reserved_31:1;             /**< \brief \internal Reserved */
} Ifx_SRC_SRCR_Bits;
/** \}  */
/******************************************************************************/
/******************************************************************************/
/** \addtogroup IfxLld_Src_union
 * \{  */

/** \\brief  Service request register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SRC_SRCR_Bits B;
} Ifx_SRC_SRCR;
/** \}  */
/******************************************************************************/
/******************************************************************************/
/** \addtogroup IfxLld_Src_struct
 * \{  */
/******************************************************************************/
/** \name Object L2
 * \{  */

/** \\brief  ASCLIN Service requests */
typedef volatile struct _Ifx_SRC_ASCLIN
{
    Ifx_SRC_SRCR TX;                        /**< \brief 0, ASCLIN Transmit Service Request */
    Ifx_SRC_SRCR RX;                        /**< \brief 4, ASCLIN Receive Service Request */
    Ifx_SRC_SRCR ERR;                       /**< \brief 8, ASCLIN Error Service Request */
} Ifx_SRC_ASCLIN;

/** \\brief  SPB Service requests */
typedef volatile struct _Ifx_SRC_BCUSPB
{
    Ifx_SRC_SRCR SBSRC;                     /**< \brief 0, Bus Control Unit SPB Service Request */
} Ifx_SRC_BCUSPB;

/** \\brief  CAN Service requests */
typedef volatile struct _Ifx_SRC_CAN
{
    Ifx_SRC_SRCR INT[16];                   /**< \brief 0, MULTICAN Service Request */
} Ifx_SRC_CAN;

/** \\brief  CAN Service requests */
typedef volatile struct _Ifx_SRC_CAN1
{
    Ifx_SRC_SRCR INT[8];                    /**< \brief 0, MULTICAN1 Service Request */
} Ifx_SRC_CAN1;

/** \\brief  CCU6 Service requests */
typedef volatile struct _Ifx_SRC_CCU6
{
    Ifx_SRC_SRCR SR0;                       /**< \brief 0, CCU6 Service Request 0 */
    Ifx_SRC_SRCR SR1;                       /**< \brief 4, CCU6 Service Request 1 */
    Ifx_SRC_SRCR SR2;                       /**< \brief 8, CCU6 Service Request 2 */
    Ifx_SRC_SRCR SR3;                       /**< \brief C, CCU6 Service Request 3 */
} Ifx_SRC_CCU6;

/** \\brief  CERBERUS Service requests */
typedef volatile struct _Ifx_SRC_CERBERUS
{
    Ifx_SRC_SRCR SR[2];                     /**< \brief 0, Cerberus Service Request */
} Ifx_SRC_CERBERUS;

/** \\brief  CPU Service requests */
typedef volatile struct _Ifx_SRC_CPU
{
    Ifx_SRC_SRCR SBSRC;                     /**< \brief 0, CPUSoftware Breakpoint Service Request */
    unsigned char reserved_4[28];           /**< \brief 4, \internal Reserved */
} Ifx_SRC_CPU;

/** \\brief  DMA Service requests */
typedef volatile struct _Ifx_SRC_DMA
{
    Ifx_SRC_SRCR ERR;                       /**< \brief 0, DMA Error Service Request */
    unsigned char reserved_4[12];           /**< \brief 4, \internal Reserved */
    Ifx_SRC_SRCR CH[16];                    /**< \brief 10, DMA Channel Service Request */
} Ifx_SRC_DMA;

/** \\brief  EMEM Service requests */
typedef volatile struct _Ifx_SRC_EMEM
{
    Ifx_SRC_SRCR SR;                        /**< \brief 0, Emulation Memory Service Request */
} Ifx_SRC_EMEM;

/** \\brief  ERAY Service requests */
typedef volatile struct _Ifx_SRC_ERAY
{
    Ifx_SRC_SRCR INT[2];                    /**< \brief 0, E-RAY Service Request */
    Ifx_SRC_SRCR TINT[2];                   /**< \brief 8, E-RAY Timer Interrupt Service Request */
    Ifx_SRC_SRCR NDAT[2];                   /**< \brief 10, E-RAY New Data Service Request */
    Ifx_SRC_SRCR MBSC[2];                   /**< \brief 18, E-RAY Message Buffer Status Changed Service Request */
    Ifx_SRC_SRCR OBUSY;                     /**< \brief 20, E-RAY Output Buffer Busy Service Request */
    Ifx_SRC_SRCR IBUSY;                     /**< \brief 24, E-RAY Input Buffer Busy Service Request */
    unsigned char reserved_28[40];          /**< \brief 28, \internal Reserved */
} Ifx_SRC_ERAY;

/** \\brief  ETH Service requests */
typedef volatile struct _Ifx_SRC_ETH
{
    Ifx_SRC_SRCR SR;                        /**< \brief 0, Ethernet Service Request */
} Ifx_SRC_ETH;

/** \\brief  EVR Service requests */
typedef volatile struct _Ifx_SRC_EVR
{
    Ifx_SRC_SRCR WUT;                       /**< \brief 0, EVR Wake Up Timer Service Request */
    Ifx_SRC_SRCR SCDC;                      /**< \brief 4, EVR Supply Service Request */
} Ifx_SRC_EVR;

/** \\brief  FFT Service requests */
typedef volatile struct _Ifx_SRC_FFT
{
    Ifx_SRC_SRCR DONE;                      /**< \brief 0, FFT Done Service Request */
    Ifx_SRC_SRCR ERR;                       /**< \brief 4, FFT Error Service Request */
    Ifx_SRC_SRCR RFS;                       /**< \brief 8, FFT Ready For Start Service Request */
} Ifx_SRC_FFT;

/** \\brief  GPSR Service requests */
typedef volatile struct _Ifx_SRC_GPSR
{
    Ifx_SRC_SRCR SR0;                       /**< \brief 0, General Purpose Service Request 0 */
    Ifx_SRC_SRCR SR1;                       /**< \brief 4, General Purpose Service Request 1 */
    Ifx_SRC_SRCR SR2;                       /**< \brief 8, General Purpose Service Request 2 */
    Ifx_SRC_SRCR SR3;                       /**< \brief C, General Purpose Service Request 3 */
    unsigned char reserved_10[1520];        /**< \brief 10, \internal Reserved */
} Ifx_SRC_GPSR;

/** \\brief  GPT12 Service requests */
typedef volatile struct _Ifx_SRC_GPT12
{
    Ifx_SRC_SRCR CIRQ;                      /**< \brief 0, GPT12 CAPREL Service Request */
    Ifx_SRC_SRCR T2;                        /**< \brief 4, GPT12 T2 Overflow/Underflow Service Request */
    Ifx_SRC_SRCR T3;                        /**< \brief 8, GPT12 T3 Overflow/Underflow Service Request */
    Ifx_SRC_SRCR T4;                        /**< \brief C, GPT12 T4 Overflow/Underflow Service Request */
    Ifx_SRC_SRCR T5;                        /**< \brief 10, GPT12 T5 Overflow/Underflow Service Request */
    Ifx_SRC_SRCR T6;                        /**< \brief 14, GPT12 T6 Overflow/Underflow Service Request */
    unsigned char reserved_18[24];          /**< \brief 18, \internal Reserved */
} Ifx_SRC_GPT12;

/** \\brief  GTM Service requests */
typedef volatile struct _Ifx_SRC_GTM
{
    Ifx_SRC_SRCR AEIIRQ;                    /**< \brief 0, GTM AEI Shared Service Request */
    unsigned char reserved_4[364];          /**< \brief 4, \internal Reserved */
    Ifx_SRC_SRCR ERR;                       /**< \brief 170, GTM Error Service Request */
    unsigned char reserved_174[12];         /**< \brief 174, \internal Reserved */
    Ifx_SRC_SRCR TIM[1][8];                 /**< \brief 180, GTM TIM Shared Service Request */
    unsigned char reserved_1A0[992];        /**< \brief 1A0, \internal Reserved */
    Ifx_SRC_SRCR TOM[2][8];                 /**< \brief 580, GTM TOM Shared Service Request */
} Ifx_SRC_GTM;

/** \\brief  HSM Service requests */
typedef volatile struct _Ifx_SRC_HSM
{
    Ifx_SRC_SRCR HSM[2];                    /**< \brief 0, HSM Service Request */
} Ifx_SRC_HSM;

/** \\brief  LMU Service requests */
typedef volatile struct _Ifx_SRC_LMU
{
    Ifx_SRC_SRCR SR;                        /**< \brief 0, LMU Service Request */
} Ifx_SRC_LMU;

/** \\brief  PMU Service requests */
typedef volatile struct _Ifx_SRC_PMU
{
    Ifx_SRC_SRCR SR;                        /**< \brief 0, PMU  Service Request */
} Ifx_SRC_PMU;

/** \\brief  QSPI Service requests */
typedef volatile struct _Ifx_SRC_QSPI
{
    Ifx_SRC_SRCR TX;                        /**< \brief 0, QSPI Transmit Service Request */
    Ifx_SRC_SRCR RX;                        /**< \brief 4, QSPI Receive Service Request */
    Ifx_SRC_SRCR ERR;                       /**< \brief 8, QSPI Error Service Request */
    Ifx_SRC_SRCR PT;                        /**< \brief C, QSPI Phase Transition Service Request */
    Ifx_SRC_SRCR HC;                        /**< \brief 10, QSPI High Speed Capture Service Request */
    Ifx_SRC_SRCR U;                         /**< \brief 14, QSPI User Defined Service Request */
} Ifx_SRC_QSPI;

/** \\brief  SCU Service requests */
typedef volatile struct _Ifx_SRC_SCU
{
    Ifx_SRC_SRCR DTS;                       /**< \brief 0, SCU DTS Busy Service Request */
    Ifx_SRC_SRCR ERU[4];                    /**< \brief 4, SCU ERU Service Request */
} Ifx_SRC_SCU;

/** \\brief  SENT Service requests */
typedef volatile struct _Ifx_SRC_SENT
{
    Ifx_SRC_SRCR SR[4];                     /**< \brief 0, SENT TRIG Service Request */
} Ifx_SRC_SENT;

/** \\brief  SMU Service requests */
typedef volatile struct _Ifx_SRC_SMU
{
    Ifx_SRC_SRCR SR[3];                     /**< \brief 0, SMU Service Request */
} Ifx_SRC_SMU;

/** \\brief  STM Service requests */
typedef volatile struct _Ifx_SRC_STM
{
    Ifx_SRC_SRCR SR0;                       /**< \brief 0, System Timer  Service Request 0 */
    Ifx_SRC_SRCR SR1;                       /**< \brief 4, System Timer  Service Request 1 */
    unsigned char reserved_8[88];           /**< \brief 8, \internal Reserved */
} Ifx_SRC_STM;

/** \\brief  VADCCG Service requests */
typedef volatile struct _Ifx_SRC_VADCCG
{
    Ifx_SRC_SRCR SR0;                       /**< \brief 0, VADC Common Group  Service Request 0 */
    Ifx_SRC_SRCR SR1;                       /**< \brief 4, VADC Common Group  Service Request 1 */
    Ifx_SRC_SRCR SR2;                       /**< \brief 8, VADC Common Group  Service Request 2 */
    Ifx_SRC_SRCR SR3;                       /**< \brief C, VADC Common Group  Service Request 3 */
    unsigned char reserved_10[304];         /**< \brief 10, \internal Reserved */
} Ifx_SRC_VADCCG;

/** \\brief  VADCG Service requests */
typedef volatile struct _Ifx_SRC_VADCG
{
    Ifx_SRC_SRCR SR0;                       /**< \brief 0, VADC Group  Service Request 0 */
    Ifx_SRC_SRCR SR1;                       /**< \brief 4, VADC Group  Service Request 1 */
    Ifx_SRC_SRCR SR2;                       /**< \brief 8, VADC Group  Service Request 2 */
    Ifx_SRC_SRCR SR3;                       /**< \brief C, VADC Group  Service Request 3 */
} Ifx_SRC_VADCG;

/** \\brief  XBAR Service requests */
typedef volatile struct _Ifx_SRC_XBAR
{
    Ifx_SRC_SRCR SRC;                       /**< \brief 0, XBAR_SRI Service Request */
} Ifx_SRC_XBAR;
/** \}  */
/******************************************************************************/
/** \}  */
/******************************************************************************/
/******************************************************************************/
/** \addtogroup IfxLld_Src_struct
 * \{  */
/******************************************************************************/
/** \name Object L1
 * \{  */

/** \\brief  ASCLIN Service requests */
typedef volatile struct _Ifx_SRC_GASCLIN
{
    Ifx_SRC_ASCLIN ASCLIN[2];               /**< \brief 0, ASCLIN Service requests */
} Ifx_SRC_GASCLIN;

/** \\brief  BCU Service requests */
typedef volatile struct _Ifx_SRC_GBCU
{
    Ifx_SRC_BCUSPB SPB;                     /**< \brief 0, SPB Service requests */
} Ifx_SRC_GBCU;

/** \\brief  CAN Service requests */
typedef volatile struct _Ifx_SRC_GCAN
{
    Ifx_SRC_CAN CAN[1];                     /**< \brief 0, CAN Service requests */
    Ifx_SRC_CAN1 CAN1[1];                   /**< \brief 40, CAN Service requests */
} Ifx_SRC_GCAN;

/** \\brief  CCU6 Service requests */
typedef volatile struct _Ifx_SRC_GCCU6
{
    Ifx_SRC_CCU6 CCU6[2];                   /**< \brief 0, CCU6 Service requests */
} Ifx_SRC_GCCU6;

/** \\brief  CERBERUS Service requests */
typedef volatile struct _Ifx_SRC_GCERBERUS
{
    Ifx_SRC_CERBERUS CERBERUS;              /**< \brief 0, CERBERUS Service requests */
} Ifx_SRC_GCERBERUS;

/** \\brief  CPU Service requests */
typedef volatile struct _Ifx_SRC_GCPU
{
    Ifx_SRC_CPU CPU[1];                     /**< \brief 0, CPU Service requests */
} Ifx_SRC_GCPU;

/** \\brief  DMA Service requests */
typedef volatile struct _Ifx_SRC_GDMA
{
    Ifx_SRC_DMA DMA[1];                     /**< \brief 0, DMA Service requests */
} Ifx_SRC_GDMA;

/** \\brief  EMEM Service requests */
typedef volatile struct _Ifx_SRC_GEMEM
{
    Ifx_SRC_EMEM EMEM[1];                   /**< \brief 0, EMEM Service requests */
} Ifx_SRC_GEMEM;

/** \\brief  ERAY Service requests */
typedef volatile struct _Ifx_SRC_GERAY
{
    Ifx_SRC_ERAY ERAY[1];                   /**< \brief 0, ERAY Service requests */
} Ifx_SRC_GERAY;

/** \\brief  ETH Service requests */
typedef volatile struct _Ifx_SRC_GETH
{
    Ifx_SRC_ETH ETH[1];                     /**< \brief 0, ETH Service requests */
} Ifx_SRC_GETH;

/** \\brief  EVR Service requests */
typedef volatile struct _Ifx_SRC_GEVR
{
    Ifx_SRC_EVR EVR[1];                     /**< \brief 0, EVR Service requests */
} Ifx_SRC_GEVR;

/** \\brief  FFT Service requests */
typedef volatile struct _Ifx_SRC_GFFT
{
    Ifx_SRC_FFT FFT[1];                     /**< \brief 0, FFT Service requests */
} Ifx_SRC_GFFT;

/** \\brief  GPSR Service requests */
typedef volatile struct _Ifx_SRC_GGPSR
{
    Ifx_SRC_GPSR GPSR[1];                   /**< \brief 0, GPSR Service requests */
} Ifx_SRC_GGPSR;

/** \\brief  GPT12 Service requests */
typedef volatile struct _Ifx_SRC_GGPT12
{
    Ifx_SRC_GPT12 GPT12[1];                 /**< \brief 0, GPT12 Service requests */
} Ifx_SRC_GGPT12;

/** \\brief  GTM Service requests */
typedef volatile struct _Ifx_SRC_GGTM
{
    Ifx_SRC_GTM GTM[1];                     /**< \brief 0, GTM Service requests */
} Ifx_SRC_GGTM;

/** \\brief  HSM Service requests */
typedef volatile struct _Ifx_SRC_GHSM
{
    Ifx_SRC_HSM HSM[1];                     /**< \brief 0, HSM Service requests */
} Ifx_SRC_GHSM;

/** \\brief  LMU Service requests */
typedef volatile struct _Ifx_SRC_GLMU
{
    Ifx_SRC_LMU LMU[1];                     /**< \brief 0, LMU Service requests */
} Ifx_SRC_GLMU;

/** \\brief  PMU Service requests */
typedef volatile struct _Ifx_SRC_GPMU
{
    Ifx_SRC_PMU PMU[2];                     /**< \brief 0, PMU Service requests */
} Ifx_SRC_GPMU;

/** \\brief  QSPI Service requests */
typedef volatile struct _Ifx_SRC_GQSPI
{
    Ifx_SRC_QSPI QSPI[4];                   /**< \brief 0, QSPI Service requests */
} Ifx_SRC_GQSPI;

/** \\brief  SCU Service requests */
typedef volatile struct _Ifx_SRC_GSCU
{
    Ifx_SRC_SCU SCU;                        /**< \brief 0, SCU Service requests */
} Ifx_SRC_GSCU;

/** \\brief  SENT Service requests */
typedef volatile struct _Ifx_SRC_GSENT
{
    Ifx_SRC_SENT SENT[1];                   /**< \brief 0, SENT Service requests */
} Ifx_SRC_GSENT;

/** \\brief  SMU Service requests */
typedef volatile struct _Ifx_SRC_GSMU
{
    Ifx_SRC_SMU SMU[1];                     /**< \brief 0, SMU Service requests */
} Ifx_SRC_GSMU;

/** \\brief  STM Service requests */
typedef volatile struct _Ifx_SRC_GSTM
{
    Ifx_SRC_STM STM[1];                     /**< \brief 0, STM Service requests */
} Ifx_SRC_GSTM;

/** \\brief  VADC Service requests */
typedef volatile struct _Ifx_SRC_GVADC
{
    Ifx_SRC_VADCG G[4];                     /**< \brief 0, VADCG Service requests */
    unsigned char reserved_40[224];         /**< \brief 40, \internal Reserved */
    Ifx_SRC_VADCCG CG[1];                   /**< \brief 120, VADCCG Service requests */
} Ifx_SRC_GVADC;

/** \\brief  XBAR Service requests */
typedef volatile struct _Ifx_SRC_GXBAR
{
    Ifx_SRC_XBAR XBAR;                      /**< \brief 0, XBAR Service requests */
} Ifx_SRC_GXBAR;
/** \}  */
/******************************************************************************/
/** \}  */
/******************************************************************************/
/******************************************************************************/
/** \addtogroup IfxLld_Src_struct
 * \{  */
/******************************************************************************/
/** \name Object L0
 * \{  */

/** \\brief  SRC object */
typedef volatile struct _Ifx_SRC
{
    Ifx_SRC_GCPU CPU;                       /**< \brief 0, CPU Service requests */
    Ifx_SRC_GEMEM EMEM;                     /**< \brief 20, EMEM Service requests */
    unsigned char reserved_24[28];          /**< \brief 24, \internal Reserved */
    Ifx_SRC_GBCU BCU;                       /**< \brief 40, BCU Service requests */
    unsigned char reserved_44[4];           /**< \brief 44, \internal Reserved */
    Ifx_SRC_GXBAR XBAR;                     /**< \brief 48, XBAR Service requests */
    unsigned char reserved_4C[4];           /**< \brief 4C, \internal Reserved */
    Ifx_SRC_GCERBERUS CERBERUS;             /**< \brief 50, CERBERUS Service requests */
    unsigned char reserved_58[40];          /**< \brief 58, \internal Reserved */
    Ifx_SRC_GASCLIN ASCLIN;                 /**< \brief 80, ASCLIN Service requests */
    unsigned char reserved_98[248];         /**< \brief 98, \internal Reserved */
    Ifx_SRC_GQSPI QSPI;                     /**< \brief 190, QSPI Service requests */
    unsigned char reserved_1F0[352];        /**< \brief 1F0, \internal Reserved */
    Ifx_SRC_GSENT SENT;                     /**< \brief 350, SENT Service requests */
    unsigned char reserved_360[192];        /**< \brief 360, \internal Reserved */
    Ifx_SRC_GCCU6 CCU6;                     /**< \brief 420, CCU6 Service requests */
    unsigned char reserved_440[32];         /**< \brief 440, \internal Reserved */
    Ifx_SRC_GGPT12 GPT12;                   /**< \brief 460, GPT12 Service requests */
    Ifx_SRC_GSTM STM;                       /**< \brief 490, STM Service requests */
    Ifx_SRC_GDMA DMA;                       /**< \brief 4F0, DMA Service requests */
    unsigned char reserved_540[944];        /**< \brief 540, \internal Reserved */
    Ifx_SRC_GETH ETH;                       /**< \brief 8F0, ETH Service requests */
    unsigned char reserved_8F4[12];         /**< \brief 8F4, \internal Reserved */
    Ifx_SRC_GCAN CAN;                       /**< \brief 900, CAN Service requests */
    unsigned char reserved_960[32];         /**< \brief 960, \internal Reserved */
    Ifx_SRC_GVADC VADC;                     /**< \brief 980, VADC Service requests */
    Ifx_SRC_GERAY ERAY;                     /**< \brief BE0, ERAY Service requests */
    Ifx_SRC_GPMU PMU;                       /**< \brief C30, PMU Service requests */
    unsigned char reserved_C38[136];        /**< \brief C38, \internal Reserved */
    Ifx_SRC_GHSM HSM;                       /**< \brief CC0, HSM Service requests */
    unsigned char reserved_CC8[8];          /**< \brief CC8, \internal Reserved */
    Ifx_SRC_GSCU SCU;                       /**< \brief CD0, SCU Service requests */
    unsigned char reserved_CE4[44];         /**< \brief CE4, \internal Reserved */
    Ifx_SRC_GSMU SMU;                       /**< \brief D10, SMU Service requests */
    unsigned char reserved_D1C[196];        /**< \brief D1C, \internal Reserved */
    Ifx_SRC_GLMU LMU;                       /**< \brief DE0, LMU Service requests */
    unsigned char reserved_DE4[460];        /**< \brief DE4, \internal Reserved */
    Ifx_SRC_GEVR EVR;                       /**< \brief FB0, EVR Service requests */
    unsigned char reserved_FB8[8];          /**< \brief FB8, \internal Reserved */
    Ifx_SRC_GFFT FFT;                       /**< \brief FC0, FFT Service requests */
    unsigned char reserved_FCC[52];         /**< \brief FCC, \internal Reserved */
    Ifx_SRC_GGPSR GPSR;                     /**< \brief 1000, GPSR Service requests */
    Ifx_SRC_GGTM GTM;                       /**< \brief 1600, GTM Service requests */
    unsigned char reserved_1BC0[1088];      /**< \brief 1BC0, \internal Reserved */
} Ifx_SRC;
/** \}  */
/******************************************************************************/
/** \}  */
/******************************************************************************/
/******************************************************************************/
#endif /* IFXSRC_REGDEF_H */
