/** \file aduc/aduc7026.h
 * \brief Bit defines and memory mapped registers for ADUC7026
 *
 * \author Copyright (C) 2011 samplemaker
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 *
 * \defgroup aduc_MMR Memory mapped register defines for ADUC bare silicon
 * \ingroup ADUC
 *
 * @{
 */


#ifndef aduc7026_H
#define aduc7026_H


#include "defs.h"


/** ADUC memory mapped registers base adress (offset)
 *
 *  MMR's starting from 0xFFFFFFFF ending at 0xFFFF0000
 *
 */
#define __MMRLO_BASE 0xFFFF0000
#define __MMRHI_BASE 0xFFFFF000


/** PLL and power control / ADC BANDGAP
 *
 */
#define _PLLSTA         0x0400 /* undocumented, unknown size */
#define _POWKEY1        0x0404
#define _POWCON         0x0408
#define _POWKEY2        0x040C
#define _PLLKEY1        0x0410
#define _PLLCON         0x0414
#define _PLLKEY2        0x0418
#define _PSMCON         0x0440
#define _CMPCON         0x0444
#define _REFCON         0x048C

#define PLLSTA          _MMR_08(_PLLSTA, __MMRLO_BASE)
#define POWKEY1         _MMR_16(_POWKEY1, __MMRLO_BASE)
#define POWCON          _MMR_16(_POWCON, __MMRLO_BASE)
#define POWKEY2         _MMR_16(_POWKEY2, __MMRLO_BASE)
#define PLLKEY1         _MMR_16(_PLLKEY1, __MMRLO_BASE)
#define PLLCON          _MMR_08(_PLLCON, __MMRLO_BASE)
#define PLLKEY2         _MMR_16(_PLLKEY2, __MMRLO_BASE)
#define PSMCON          _MMR_16(_PSMCON, __MMRLO_BASE)
#define CMPCON          _MMR_16(_CMPCON, __MMRLO_BASE)
#define REFCON          _MMR_08(_REFCON, __MMRLO_BASE)


#define REF_BANDGAP_ENABLE      0
#define PLL_OSEL                5
#define PLL_MDCLK               0
#define POW_PC                  4
#define POW_CD                  0


/** General purpose intput and output (GPIO)
 *
 */
#define _GP0CON         0x0400
#define _GP1CON         0x0404
#define _GP2CON         0x0408
#define _GP3CON         0x040C
#define _GP4CON         0x0410
#define _GP0DAT         0x0420
#define _GP0SET         0x0424
#define _GP0CLR         0x0428
#define _GP0PAR         0x042C
#define _GP1DAT         0x0430
#define _GP1SET         0x0434
#define _GP1CLR         0x0438
#define _GP1PAR         0x043C
#define _GP2DAT         0x0440
#define _GP2SET         0x0444
#define _GP2CLR         0x0448
#define _GP3DAT         0x0450
#define _GP3SET         0x0454
#define _GP3CLR         0x0458
#define _GP4DAT         0x0460
#define _GP4SET         0x0464
#define _GP4CLR         0x0468

#define GP0CON          _MMR_32(_GP0CON, __MMRHI_BASE)
#define GP1CON          _MMR_32(_GP1CON, __MMRHI_BASE)
#define GP2CON          _MMR_32(_GP2CON, __MMRHI_BASE)
#define GP3CON          _MMR_32(_GP3CON, __MMRHI_BASE)
#define GP4CON          _MMR_32(_GP4CON, __MMRHI_BASE)
#define GP0DAT          _MMR_32(_GP0DAT, __MMRHI_BASE)
#define GP0SET          _MMR_32(_GP0SET, __MMRHI_BASE)
#define GP0CLR          _MMR_32(_GP0CLR, __MMRHI_BASE)
#define GP0PAR          _MMR_32(_GP0PAR, __MMRHI_BASE)
#define GP1DAT          _MMR_32(_GP1DAT, __MMRHI_BASE)
#define GP1SET          _MMR_32(_GP1SET, __MMRHI_BASE)
#define GP1CLR          _MMR_32(_GP1CLR, __MMRHI_BASE)
#define GP1PAR          _MMR_32(_GP1PAR, __MMRHI_BASE)
#define GP2DAT          _MMR_32(_GP2DAT, __MMRHI_BASE)
#define GP2SET          _MMR_32(_GP2SET, __MMRHI_BASE)
#define GP2CLR          _MMR_32(_GP2CLR, __MMRHI_BASE)
#define GP3DAT          _MMR_32(_GP3DAT, __MMRHI_BASE)
#define GP3SET          _MMR_32(_GP3SET, __MMRHI_BASE)
#define GP3CLR          _MMR_32(_GP3CLR, __MMRHI_BASE)
#define GP4DAT          _MMR_32(_GP4DAT, __MMRHI_BASE)
#define GP4SET          _MMR_32(_GP4SET, __MMRHI_BASE)
#define GP4CLR          _MMR_32(_GP4CLR, __MMRHI_BASE)


#define GP_SELECT_FUNCTION_Px0  0
#define GP_SELECT_FUNCTION_Px1  4
#define GP_SELECT_FUNCTION_Px2  8
#define GP_SELECT_FUNCTION_Px3  12
#define GP_SELECT_FUNCTION_Px4  16
#define GP_SELECT_FUNCTION_Px5  20
#define GP_SELECT_FUNCTION_Px6  24
#define GP_SELECT_FUNCTION_Px7  28
#define GP_DATA_DIRECTION_Px0   24
#define GP_DATA_DIRECTION_Px1   25
#define GP_DATA_DIRECTION_Px2   26
#define GP_DATA_DIRECTION_Px3   27
#define GP_DATA_DIRECTION_Px4   28
#define GP_DATA_DIRECTION_Px5   29
#define GP_DATA_DIRECTION_Px6   30
#define GP_DATA_DIRECTION_Px7   31
#define GP_DATA_OUTPUT_Px0      16
#define GP_DATA_OUTPUT_Px1      17
#define GP_DATA_OUTPUT_Px2      18
#define GP_DATA_OUTPUT_Px3      19
#define GP_DATA_OUTPUT_Px4      20
#define GP_DATA_OUTPUT_Px5      21
#define GP_DATA_OUTPUT_Px6      22
#define GP_DATA_OUTPUT_Px7      23
#define GP_DATA_INPUT_Px0       0
#define GP_DATA_INPUT_Px1       1
#define GP_DATA_INPUT_Px2       2
#define GP_DATA_INPUT_Px3       3
#define GP_DATA_INPUT_Px4       4
#define GP_DATA_INPUT_Px5       5
#define GP_DATA_INPUT_Px6       6
#define GP_DATA_INPUT_Px7       7
#define GP_PAR_PULL_UP_Px0      0
#define GP_PAR_PULL_UP_Px1      4
#define GP_PAR_PULL_UP_Px2      8
#define GP_PAR_PULL_UP_Px3      12
#define GP_PAR_PULL_UP_Px4      16
#define GP_PAR_PULL_UP_Px5      20
#define GP_PAR_PULL_UP_Px6      24
#define GP_PAR_PULL_UP_Px7      28


/** TIMER 0 .. 3
 *
 */
#define _T0LD           0x0300
#define _T0VAL          0x0304
#define _T0CON          0x0308
#define _T0CLRI         0x030C

#define T0LD            _MMR_16(_T0LD, __MMRLO_BASE)
#define T0VAL           _MMR_16(_T0VAL, __MMRLO_BASE)
#define T0CON           _MMR_16(_T0CON, __MMRLO_BASE)
#define T0CLRI          _MMR_08(_T0CLRI, __MMRLO_BASE)


#define TIMER0_ENABLE           7
#define TIMER0_MODE             6
#define TIMER0_PRESCALER        2


#define _T1LD           0x0320
#define _T1VAL          0x0324
#define _T1CON          0x0328
#define _T1CLRI         0x032C
#define _T1CAP          0x0330

#define T1LD            _MMR_32(_T1LD, __MMRLO_BASE)
#define T1VAL           _MMR_32(_T1VAL, __MMRLO_BASE)
#define T1CON           _MMR_32(_T1CON, __MMRLO_BASE)
#define T1CLRI          _MMR_08(_T1CLRI, __MMRLO_BASE)
#define T1CAP           _MMR_32(_T1CAP, __MMRLO_BASE)


#define TIMER1_CAPTURE_ENABLE   17
#define TIMER1_CAPTURE_EVENT    12
#define TIMER1_CLKSOURCE        9
#define TIMER1_CORE_CLK         0
#define TIMER1_EXT_XTAL         1
#define TIMER1_P10_RISING_EDGE  2
#define TIMER1_P06_RISING_EDGE  3
#define TIMER1_COUNT_DIR        8
#define TIMER1_ENABLE           7
#define TIMER1_MODE             6
#define TIMER1_FORMAT           4
#define TIMER1_PRESCALER        0


#define _T2LD           0x0340
#define _T2VAL          0x0344
#define _T2CON          0x0348
#define _T2CLRI         0x034C

#define T2LD            _MMR_32(_T2LD, __MMRLO_BASE)
#define T2VAL           _MMR_32(_T2VAL, __MMRLO_BASE)
#define T2CON           _MMR_16(_T2CON, __MMRLO_BASE)
#define T2CLRI          _MMR_08(_T2CLRI, __MMRLO_BASE)


#define TIMER2_COUNT_DIR        8
#define TIMER2_ENABLE           7
#define TIMER2_MODE             6
#define TIMER2_FORMAT0          4
#define TIMER2_FORMAT1          5
#define TIMER2_CLKSOURCE        9
#define TIMER2_EXT_XTAL         0
#define TIMER2_INT_OSC          2
#define TIMER2_CORE_CLK         3
#define TIMER2_PRESCALER        0


#define _T3LD           0x0360
#define _T3VAL          0x0364
#define _T3CON          0x0368
#define _T3CLRI         0x036C

#define T3LD            _MMR_16(_T3LD, __MMRLO_BASE)
#define T3VAL           _MMR_16(_T3VAL, __MMRLO_BASE)
#define T3CON           _MMR_16(_T3CON, __MMRLO_BASE)
#define T3CLRI          _MMR_08(_T3CLRI, __MMRLO_BASE)


#define TIMER3_COUNT_DIR        8
#define TIMER3_ENABLE           7
#define TIMER3_MODE             6
#define TIMER3_WDT_ENABLE       5
#define TIMER3_SECURE           4
#define TIMER3_IRQ_ENABLE       1
#define TIMER3_PRESCALER        2


/** Interrupt controller
 *
 */
#define _IRQSTA         0x0000
#define _IRQSIG         0x0004
#define _IRQEN          0x0008
#define _IRQCLR         0x000C
#define _SWICFG         0x0010
#define _FIQSTA         0x0100
#define _FIQSIG         0x0104
#define _FIQEN          0x0108
#define _FIQCLR         0x010C

#define IRQSTA          _MMR_32(_IRQSTA, __MMRLO_BASE)
#define IRQSIG          _MMR_32(_IRQSIG, __MMRLO_BASE)
#define IRQEN           _MMR_32(_IRQEN, __MMRLO_BASE)
#define IRQCLR          _MMR_32(_IRQCLR, __MMRLO_BASE)
#define SWICFG          _MMR_32(_SWICFG, __MMRLO_BASE)
#define FIQSTA          _MMR_32(_FIQSTA, __MMRLO_BASE)
#define FIQSIG          _MMR_32(_FIQSIG, __MMRLO_BASE)
#define FIQEN           _MMR_32(_FIQEN, __MMRLO_BASE)
#define FIQCLR          _MMR_32(_FIQCLR, __MMRLO_BASE)


#define INT_ALL                 0
#define INT_SWI                 1
#define INT_TIMER0              2
#define INT_TIMER1              3
#define INT_WAKEUP_TIMER2       4
#define INT_WATCHDOG_TIMER3     5
#define INT_FLASH_CONTROL       6
#define INT_ADC_CHANNEL         7
#define INT_PLL_LOCK            8
#define INT_I2C0_SLAVE          9
#define INT_I2C0_MASTER         10
#define INT_I2C1_MASTER         11
#define INT_SPI_SLAVE           12
#define INT_SPI_MASTER          13
#define INT_UART                14
#define INT_EXTERNAL_IRQ0       15
#define INT_COMPARATOR          16
#define INT_PSM                 17
#define INT_EXTERNAL_IRQ1       18
#define INT_PLA_IRQ0            19
#define INT_PLA_IRQ1            20
#define INT_EXTERNAL_IRQ2       21
#define INT_EXTERNAL_IRQ3       22
#define INT_PWM_TRIP            23


/** Communication (UART)
 *
 */
#define _COMTX          0x0700
#define _COMRX          0x0700
#define _COMIEN0        0x0704
#define _COMIID0        0x0708
#define _COMCON0        0x070C
#define _COMCON1        0x0710
#define _COMSTA0        0x0714
#define _COMSTA1        0x0718
#define _COMSCR         0x071C
#define _COMIEN1        0x0720
#define _COMIID1        0x0724
#define _COMADR         0x0728
#define _COMDIV0        0x0700
#define _COMDIV1        0x0704
#define _COMDIV2        0x072C

#define COMTX           _MMR_08(_COMTX, __MMRLO_BASE)
#define COMRX           _MMR_08(_COMRX, __MMRLO_BASE)
#define COMIEN0         _MMR_08(_COMIEN0, __MMRLO_BASE)
#define COMIID0         _MMR_08(_COMIID0, __MMRLO_BASE)
#define COMCON0         _MMR_08(_COMCON0, __MMRLO_BASE)
#define COMCON1         _MMR_08(_COMCON1, __MMRLO_BASE)
#define COMSTA0         _MMR_08(_COMSTA0, __MMRLO_BASE)
#define COMSTA1         _MMR_08(_COMSTA1, __MMRLO_BASE)
#define COMSCR          _MMR_08(_COMSCR, __MMRLO_BASE)
#define COMIEN1         _MMR_08(_COMIEN1, __MMRLO_BASE)
#define COMIID1         _MMR_08(_COMIID1, __MMRLO_BASE)
#define COMADR          _MMR_08(_COMADR, __MMRLO_BASE)
#define COMDIV0         _MMR_08(_COMDIV0, __MMRLO_BASE)
#define COMDIV1         _MMR_08(_COMDIV1, __MMRLO_BASE)
#define COMDIV2         _MMR_16(_COMDIV2, __MMRLO_BASE)


#define UART_DLAB               7
#define UART_BRK                6
#define UART_SP                 5
#define UART_EPS                4
#define UART_PEN                3
#define UART_STOP               2
#define UART_WLS                0
#define UART_TEMT               6
#define UART_THRE               5
#define UART_BI                 4
#define UART_FE                 3
#define UART_PE                 2
#define UART_OE                 1
#define UART_DR                 0
#define UART_LOOPBACK           4
#define UART_FBEN               15
#define UART_FBN                0
#define UART_FBM                11


/** Reset
 *
 */
#define _REMAP          0x0220
#define _RSTSTA         0x0230
#define _RSTCLR         0x0234

#define REMAP           _MMR_08(_REMAP, __MMRLO_BASE)
#define RSTSTA          _MMR_08(_RSTSTA, __MMRLO_BASE)
#define RSTCLR          _MMR_08(_RSTCLR, __MMRLO_BASE)


#define REMAP_SRAM              0
#define RST_SOFTRST             2
#define RST_WDTRST              1
#define RST_POWRONRST           0


/** Analog digital converter (ADC)
 *
 */
#define _ADCCON         0x0500
#define _ADCCP          0x0504
#define _ADCCN          0x0508
#define _ADCSTA         0x050C
#define _ADCDAT         0x0510
#define _ADCRST         0x0514
#define _ADCGN          0x0530
#define _ADCOF          0x0534

#define ADCCON          _MMR_16(_ADCCON, __MMRLO_BASE)
#define ADCCP           _MMR_08(_ADCCP, __MMRLO_BASE)
#define ADCCN           _MMR_08(_ADCCN, __MMRLO_BASE)
#define ADCSTA          _MMR_08(_ADCSTA, __MMRLO_BASE)
#define ADCDAT          _MMR_32(_ADCDAT, __MMRLO_BASE)
#define ADCRST          _MMR_08(_ADCRST, __MMRLO_BASE)
#define ADCGN           _MMR_16(_ADCGN, __MMRLO_BASE)
#define ADCOF           _MMR_16(_ADCOF, __MMRLO_BASE)


#define ADC_CLOCK_SPEED         10
#define ADC_ACQUISITION_TIME    8
#define ADC_ENABLE_CONVERION    7
#define ADC_ENABLE_ADCBUSY      6
#define ADC_POWER_CONTROL       5
#define ADC_CONVERSION_MODE     3
#define ADC_TRIGGER_SOURCE      0
#define ADC_NCHANNEL_SELECTION  0
#define ADC_PCHANNEL_SELECTION  0


/** FLASH
 *
 */
#define _FEESTA         0x0800
#define _FEEMOD         0x0804
#define _FEECON         0x0808
#define _FEEDAT         0x080C
#define _FEEADR         0x0810
#define _FEESIGN        0x0818
#define _FEEPRO         0x081C
#define _FEEHIDE        0x0820

#define FEESTA          _MMR_08(_FEESTA, __MMRHI_BASE)
#define FEEMOD          _MMR_16(_FEEMOD, __MMRHI_BASE)
#define FEECON          _MMR_08(_FEECON, __MMRHI_BASE)
#define FEEDAT          _MMR_16(_FEEDAT, __MMRHI_BASE)
#define FEEADR          _MMR_16(_FEEADR, __MMRHI_BASE)
#define FEESIGN         _MMR_32(_FEESIGN, __MMRHI_BASE)
#define FEEPRO          _MMR_32(_FEEPRO, __MMRHI_BASE)
#define FEEHIDE         _MMR_32(_FEEHIDE, __MMRHI_BASE)


#define FEE_CONTROLLER_BUSY     2
#define FEE_FAIL                1
#define FEE_PASS                0
#define FEE_EW_PROTECTION       3
#define FEE_INT_ENABLE          3
#define FEE_CMD_SINGLE_READ     0x01
#define FEE_CMD_SINGLE_WRITE    0x02
#define FEE_CMD_SINGLE_VERIFY   0x04
#define FEE_CMD_SINGLE_ERASE    0x05
#define FEE_CMD_MASS_ERASE      0x06
#define FEE_CMD_PING            0x0F


/** Digital analog converter (DAC)
 *
 */
#define _DAC0CON        0x0600
#define _DAC0DAT        0x0604
#define _DAC1CON        0x0608
#define _DAC1DAT        0x060C
#define _DAC2CON        0x0610
#define _DAC2DAT        0x0614
#define _DAC3CON        0x0618
#define _DAC3DAT        0x061C

#define DAC0CON         _MMR_08(_DAC0CON, __MMRLO_BASE)
#define DAC0DAT         _MMR_32(_DAC0DAT, __MMRLO_BASE)
#define DAC1CON         _MMR_08(_DAC1CON, __MMRLO_BASE)
#define DAC1DAT         _MMR_32(_DAC1DAT, __MMRLO_BASE)
#define DAC2CON         _MMR_08(_DAC2CON, __MMRLO_BASE)
#define DAC2DAT         _MMR_32(_DAC2DAT, __MMRLO_BASE)
#define DAC3CON         _MMR_08(_DAC3CON, __MMRLO_BASE)
#define DAC3DAT         _MMR_32(_DAC3DAT, __MMRLO_BASE)


#define DAC_DACCLK              5
#define DAC_DACCLR              4
#define DAC_POWER_DOWN          0
#define DAC_DATA                16


/** Pulse width modulation (PWM)
 *
 */
#define _PWMCON         0x0C00
#define _PWMSTA         0x0C04
#define _PWMCFG         0x0C10
#define _PWMEN          0x0C20
#define _PWMDAT0        0x0C08 /* switching period     */
#define _PWMDAT1        0x0C0C /* dead time            */
#define _PWMDAT2        0x0C24 /* PWM sync pulse width */
#define _PWMCH0         0x0C14
#define _PWMCH1         0x0C18
#define _PWMCH2         0x0C1C

#define PWMCON          _MMR_16(_PWMCON, __MMRHI_BASE)
#define PWMSTA          _MMR_16(_PWMSTA, __MMRHI_BASE)
#define PWMCFG          _MMR_16(_PWMCFG, __MMRHI_BASE)
#define PWMEN           _MMR_16(_PWMEN, __MMRHI_BASE)
#define PWMDAT0         _MMR_16(_PWMDAT0, __MMRHI_BASE)
#define PWMDAT1         _MMR_16(_PWMDAT1, __MMRHI_BASE)
#define PWMDAT2         _MMR_16(_PWMDAT2, __MMRHI_BASE)
#define PWMCH0          _MMR_16(_PWMCH0, __MMRHI_BASE)
#define PWMCH1          _MMR_16(_PWMCH1, __MMRHI_BASE)
#define PWMCH2          _MMR_16(_PWMCH2, __MMRHI_BASE)


#define PWM_SYNCSEL             4
#define PWM_EXTSYNC             3
#define PWM_DBL                 2
#define PWM_SYNC_EN             1
#define PWM_EN                  0
#define PWM_PWMSYNCINT          9
#define PWM_PWMTRIPINT          8
#define PWM_PWMTRIP             3
#define PWM_PWMPHASE            0
#define PWM_CHOPLO              9
#define PWM_CHOPHI              8
#define PWM_GDCLK               0
#define PWM_0H0L_XOVR           8
#define PWM_1H1L_XOVR           7
#define PWM_2H2L_XOVR           6
#define PWM_0L_EN               5
#define PWM_0H_EN               4
#define PWM_1L_EN               3
#define PWM_1H_EN               2
#define PWM_2L_EN               1
#define PWM_2H_EN               0


/** Programmable logic array (PLA)
 *
 */
#define _PLAELM0        0x0B00
#define _PLAELM1        0x0B04
#define _PLAELM2        0x0B08
#define _PLAELM3        0x0B0C
#define _PLAELM4        0x0B10
#define _PLAELM5        0x0B14
#define _PLAELM6        0x0B18
#define _PLAELM7        0x0B1C
#define _PLAELM8        0x0B20
#define _PLAELM9        0x0B24
#define _PLAELM10       0x0B28
#define _PLAELM11       0x0B2C
#define _PLAELM12       0x0B30
#define _PLAELM13       0x0B34
#define _PLAELM14       0x0B38
#define _PLAELM15       0x0B3C
#define _PLACLK         0x0B40
#define _PLAIRQ         0x0B44
#define _PLAADC         0x0B48
#define _PLADIN         0x0B4C
#define _PLADOUT        0x0B50
#define _PLALCK         0x0B54

#define PLAELM0         _MMR_16(_PLAELM0, __MMRLO_BASE)
#define PLAELM1         _MMR_16(_PLAELM1, __MMRLO_BASE)
#define PLAELM2         _MMR_16(_PLAELM2, __MMRLO_BASE)
#define PLAELM3         _MMR_16(_PLAELM3, __MMRLO_BASE)
#define PLAELM4         _MMR_16(_PLAELM4, __MMRLO_BASE)
#define PLAELM5         _MMR_16(_PLAELM5, __MMRLO_BASE)
#define PLAELM6         _MMR_16(_PLAELM6, __MMRLO_BASE)
#define PLAELM7         _MMR_16(_PLAELM7, __MMRLO_BASE)
#define PLAELM8         _MMR_16(_PLAELM8, __MMRLO_BASE)
#define PLAELM9         _MMR_16(_PLAELM9, __MMRLO_BASE)
#define PLAELM10        _MMR_16(_PLAELM10, __MMRLO_BASE)
#define PLAELM11        _MMR_16(_PLAELM11, __MMRLO_BASE)
#define PLAELM12        _MMR_16(_PLAELM12, __MMRLO_BASE)
#define PLAELM13        _MMR_16(_PLAELM13, __MMRLO_BASE)
#define PLAELM14        _MMR_16(_PLAELM14, __MMRLO_BASE)
#define PLAELM15        _MMR_16(_PLAELM15, __MMRLO_BASE)
#define PLACLK          _MMR_08(_PLACLK, __MMRLO_BASE)
#define PLAIRQ          _MMR_32(_PLAIRQ, __MMRLO_BASE)
#define PLAADC          _MMR_32(_PLAADC, __MMRLO_BASE)
#define PLADIN          _MMR_32(_PLADIN, __MMRLO_BASE)
#define PLADOUT         _MMR_32(_PLADOUT, __MMRLO_BASE)
#define PLALCK          _MMR_08(_PLALCK, __MMRLO_BASE)


#define PLA_BLOCK1_CLK_SRC      4
#define PLA_BLOCK0_CLK_SRC      0
#define PLA_ADC_CONV_START      4
#define PLA_ADC_CONV_SRC        0
#define PLA_MUX0_CONTROL        9
#define PLA_MUX1_CONTROL        7
#define PLA_MUX2_CONTROL        6
#define PLA_MUX3_CONTROL        5
#define PLA_MUX4_CONTROL        0
#define PLA_LOOKUP_TABLE        1
#define PLADIN_ELEMENT_0        0
#define PLADIN_ELEMENT_1        1
#define PLADIN_ELEMENT_2        2


/** @} */


#endif  /* !ADUC7026_H */
