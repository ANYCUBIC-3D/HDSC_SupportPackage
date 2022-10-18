/*******************************************************************************
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by HDSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 */
/******************************************************************************/
/** \file system_hc32f460.c
 **
 ** A detailed description is available at
 ** @link Hc32f460SystemGroup Hc32f460System description @endlink
 **
 **   - 2018-10-15  1.0  Zhangxl First version
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "system_hc32f460.h"
#include "hc32f460.h"

/**
 *******************************************************************************
 ** \addtogroup Hc32f460SystemGroup
 ******************************************************************************/

/*******************************************************************************
 * Global pre-processor symbols/macros ('define')
 ******************************************************************************/

//@{

/**
 ******************************************************************************
 ** System Clock Frequency (Core Clock) Variable according CMSIS
 ******************************************************************************/
 #if defined (__ICCARM__)                /*!< IAR Compiler */
  #ifndef __NO_INIT
    #define __NO_INIT                   __no_init
  #endif /* __NO_INIT */
#else                                  /*!< ARM Compiler */
  #ifndef __NO_INIT
    #define __NO_INIT
  #endif /* __NO_INIT */
#endif

__NO_INIT uint32_t HRC_VALUE;
__NO_INIT uint32_t SystemCoreClock;

/**
 ******************************************************************************
 ** \brief  Setup the microcontroller system. Initialize the System and update
 ** the SystemCoreClock variable.
 **
 ** \param  None
 ** \return None
 ******************************************************************************/
void SystemInit(void)
{
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 20) | (3UL << 22)); /* set CP10 and CP11 Full Access */
#endif

    SystemCoreClockUpdate();
}

void SystemCoreClockUpdate(void)  // Update SystemCoreClock variable
{
    uint8_t tmp = 0u;
    uint32_t plln = 19u, pllp = 1u, pllm = 0u, pllsource = 0u;

    /* Select proper HRC_VALUE according to ICG1.HRCFREQSEL bit */
    /* ICG1.HRCFREQSEL = '0' represent HRC_VALUE = 20000000UL   */
    /* ICG1.HRCFREQSEL = '1' represent HRC_VALUE = 16000000UL   */
    if (1UL == (HRC_FREQ_MON() & 1UL))
    {
        HRC_VALUE = HRC_16MHz_VALUE;
    }
    else
    {
        HRC_VALUE = HRC_20MHz_VALUE;
    }

    tmp = M4_SYSREG->CMU_CKSWR_f.CKSW;
    switch (tmp)
    {
        case 0x00:  /* use internal high speed RC */
            SystemCoreClock = HRC_VALUE;
            break;
        case 0x01:  /* use internal middle speed RC */
            SystemCoreClock = MRC_VALUE;
            break;
        case 0x02:  /* use internal low speed RC */
            SystemCoreClock = LRC_VALUE;
            break;
        case 0x03:  /* use external high speed OSC */
            SystemCoreClock = XTAL_VALUE;
            break;
        case 0x04:  /* use external low speed OSC */
            SystemCoreClock = XTAL32_VALUE;
            break;
        case 0x05:  /* use MPLL */
            /* PLLCLK = ((pllsrc / pllm) * plln) / pllp */
            pllsource = M4_SYSREG->CMU_PLLCFGR_f.PLLSRC;
            plln = M4_SYSREG->CMU_PLLCFGR_f.MPLLN;
            pllp = M4_SYSREG->CMU_PLLCFGR_f.MPLLP;
            pllm = M4_SYSREG->CMU_PLLCFGR_f.MPLLM;
            /* use exteranl high speed OSC as PLL source */
            if (0ul == pllsource)
            {
                SystemCoreClock = (XTAL_VALUE) / (pllm + 1ul) * (plln + 1ul) / (pllp + 1ul);
            }
            /* use interanl high RC as PLL source */
            else if (1ul == pllsource)
            {
                SystemCoreClock = (HRC_VALUE) / (pllm + 1ul) * (plln + 1ul) / (pllp + 1ul);
            }
            else
            {
                /* Reserved */
            }
            break;
    }
}

#if (defined (__CC_ARM) || defined (__CLANG_ARM)) || \
    (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
extern int32_t $Super$$main(void);
/* re-define main function: keil */
int $Sub$$main(void)
{
    SystemCoreClockUpdate();
    $Super$$main();
    return 0;
}
#endif

//@} // UsartGroup

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
