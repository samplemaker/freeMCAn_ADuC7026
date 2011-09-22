/** \file ./aduc7026.h
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
 * \defgroup ADUC
 * \ingroup ADUC
 *
 * @{
 */

#ifndef aduc7026_H
#define aduc7026_H

#include "defs.h"


/** Various bitmasks
 *
 *
 */
#define MASK_00000            0
#define MASK_00001            1
#define MASK_10000           16

#define MASK_0000             0
#define MASK_0001             1
#define MASK_0010             2
#define MASK_0011             3
#define MASK_0100             4
#define MASK_0101             5
#define MASK_0110             6
#define MASK_0111             7
#define MASK_1000             8
#define MASK_1001             9
#define MASK_1010             10
#define MASK_1011             11
#define MASK_1100             12
#define MASK_1101             13
#define MASK_1110             14
#define MASK_1111             15

#define MASK_000              0
#define MASK_001              1
#define MASK_010              2
#define MASK_011              3
#define MASK_100              4
#define MASK_101              5
#define MASK_110              6
#define MASK_111              7

#define MASK_00               0
#define MASK_01               1
#define MASK_10               2
#define MASK_11               3


/* ARM7TDMI specific defines
 * 
 */

/** Current program status register (cpsr) bit definitions and mode identifiers
 *
 * CPSR_c = [I F T M4 M3 M2 M1 M0]
 *
 * See: ARM7TDMI Technical Reference Manual p. 61
 *
 */

/** IRQ Mask
 */
#define I_FLAG  0x80
/** FIQ Mask
 */
#define F_FLAG  0x40
/** Arm <> Thumb state Mask
 */
#define T_FLAG  0x20
/** User Mode Mask
 */
#define M_USR   0x10
/** Fast Interrupt Mode Mask
 */
#define M_FIQ   0x11
/** Interrupt Mode Mask
 */
#define M_IRQ   0x12
/** Supervisor Mode Mask
 */
#define M_SVC   0x13
/** Abort Mode Mask
 */
#define M_ABT   0x17
/** Undefined Mode Mask
 */
#define M_UND   0x1B
/** System Mode Mask
 */
#define M_SYS   0x1F


/* ADUC7026 specific defines
 * 
 */

/** ADUC memory mapped registers base adress (offset)
 *
 *  MMR's starting from 0xFFFFFFFF ending at 0xFFFF0000
 *
 */
#define __MMR_BASE 0xFFFF0000


/** Defines for PLL and power control / ADC BANDGAP
 *
 * Access granted by special access procedure:
 *
 *   PLLKEY1 = 0xAA;
 *   PLLCON = tbd
 *   PLLKEY2 = 0x55;
 *   POWKEY1 = 0x01;
 *   POWCON = tbd
 *   POWKEY2 = 0xF4;
 */
#define POWKEY1   _MMR_SIZE_16(0x0404)
#define POWCON    _MMR_SIZE_16(0x0408)
#define POWKEY2   _MMR_SIZE_16(0x040C)
#define PLLKEY1   _MMR_SIZE_16(0x0410)
#define PLLCON    _MMR_SIZE_08(0x0414)
#define PLLKEY2   _MMR_SIZE_16(0x0418)
#define PSMCON    _MMR_SIZE_16(0x0440)
#define CMPCON    _MMR_SIZE_16(0x0444)
#define REFCON    _MMR_SIZE_08(0x048C)

#define REF_BANDGAP_ENABLE    0
#define PLL_OSEL              5
#define PLL_MDCLK             0
#define POW_PC0               4
#define POW_PC1               5
#define POW_PC2               6
#define POW_CD0               0
#define POW_CD1               1
#define POW_CD2               2


/** Defines for GPIO module
 *
 *
 */
#define GP0CON    _MMR_SIZE_32(0xF400)
#define GP1CON    _MMR_SIZE_32(0xF404)
#define GP2CON    _MMR_SIZE_32(0xF408)
#define GP3CON    _MMR_SIZE_32(0xF40C)
#define GP4CON    _MMR_SIZE_32(0xF410)
#define GP0DAT    _MMR_SIZE_32(0xF420)
#define GP0SET    _MMR_SIZE_32(0xF424)
#define GP0CLR    _MMR_SIZE_32(0xF428)
#define GP0PAR    _MMR_SIZE_32(0xF42C)
#define GP1DAT    _MMR_SIZE_32(0xF430)
#define GP1SET    _MMR_SIZE_32(0xF434)
#define GP1CLR    _MMR_SIZE_32(0xF438)
#define GP1PAR    _MMR_SIZE_32(0xF43C)
#define GP2DAT    _MMR_SIZE_32(0xF440)
#define GP2SET    _MMR_SIZE_32(0xF444)
#define GP2CLR    _MMR_SIZE_32(0xF448)
#define GP3DAT    _MMR_SIZE_32(0xF450)
#define GP3SET    _MMR_SIZE_32(0xF454)
#define GP3CLR    _MMR_SIZE_32(0xF458)
#define GP4DAT    _MMR_SIZE_32(0xF460)
#define GP4SET    _MMR_SIZE_32(0xF464)
#define GP4CLR    _MMR_SIZE_32(0xF468)

#define GP_SELECT_FUNCTION_Px0 0
#define GP_SELECT_FUNCTION_Px1 4
#define GP_SELECT_FUNCTION_Px2 8
#define GP_SELECT_FUNCTION_Px3 12
#define GP_SELECT_FUNCTION_Px4 16
#define GP_SELECT_FUNCTION_Px5 20
#define GP_SELECT_FUNCTION_Px6 24
#define GP_SELECT_FUNCTION_Px7 28
#define GP_DATA_DIRECTION_Px0  24
#define GP_DATA_DIRECTION_Px1  25
#define GP_DATA_DIRECTION_Px2  26
#define GP_DATA_DIRECTION_Px3  27
#define GP_DATA_DIRECTION_Px4  28
#define GP_DATA_DIRECTION_Px5  29
#define GP_DATA_DIRECTION_Px6  30
#define GP_DATA_DIRECTION_Px7  31
#define GP_DATA_OUTPUT_Px0     16
#define GP_DATA_OUTPUT_Px1     17
#define GP_DATA_OUTPUT_Px2     18
#define GP_DATA_OUTPUT_Px3     19
#define GP_DATA_OUTPUT_Px4     20
#define GP_DATA_OUTPUT_Px5     21
#define GP_DATA_OUTPUT_Px6     22
#define GP_DATA_OUTPUT_Px7     23
#define GP_DATA_INPUT_Px0      0
#define GP_DATA_INPUT_Px1      1
#define GP_DATA_INPUT_Px2      2
#define GP_DATA_INPUT_Px3      3
#define GP_DATA_INPUT_Px4      4
#define GP_DATA_INPUT_Px5      5
#define GP_DATA_INPUT_Px6      6
#define GP_DATA_INPUT_Px7      7
#define GP_PAR_PULL_UP_Px0     0
#define GP_PAR_PULL_UP_Px1     4
#define GP_PAR_PULL_UP_Px2     8
#define GP_PAR_PULL_UP_Px3     12
#define GP_PAR_PULL_UP_Px4     16
#define GP_PAR_PULL_UP_Px5     20
#define GP_PAR_PULL_UP_Px6     24
#define GP_PAR_PULL_UP_Px7     28


/** Defines for TIMER module
 *
 */
#define T0LD      _MMR_SIZE_16(0x0300)
#define T0VAL     _MMR_SIZE_16(0x0304)
#define T0CON     _MMR_SIZE_16(0x0308)
#define T0CLRI    _MMR_SIZE_08(0x030C)

#define TIMER0_ENABLE        7
#define TIMER0_MODE          6
/* Set the clockdivision factor */
#define TIMER0_PRESCALER     2


#define T1LD      _MMR_SIZE_32(0x0320)
#define T1VAL     _MMR_SIZE_32(0x0324)
#define T1CON     _MMR_SIZE_16(0x0328)
#define T1CLRI    _MMR_SIZE_08(0x032C)
#define T1CAP     _MMR_SIZE_32(0x0330)

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

#define T2LD      _MMR_SIZE_32(0x0340)
#define T2VAL     _MMR_SIZE_32(0x0344)
#define T2CON     _MMR_SIZE_16(0x0348)
#define T2CLRI    _MMR_SIZE_08(0x034C)

#define TIMER2_COUNT_DIR     8
#define TIMER2_ENABLE        7
#define TIMER2_MODE          6
#define TIMER2_FORMAT0       4
#define TIMER2_FORMAT1       5
#define TIMER2_CLKSOURCE     9
#define TIMER2_EXT_XTAL      0
#define TIMER2_INT_OSC       2
#define TIMER2_CORE_CLK      3
#define TIMER2_PRESCALER     0

#define T3LD      _MMR_SIZE_16(0x0360)
#define T3VAL     _MMR_SIZE_16(0x0364)
#define T3CON     _MMR_SIZE_16(0x0368)
#define T3CLRI    _MMR_SIZE_08(0x036C)

#define TIMER3_COUNT_DIR     8
#define TIMER3_ENABLE        7
#define TIMER3_MODE          6
#define TIMER3_WDT_ENABLE    5
#define TIMER3_SECURE        4
#define TIMER3_IRQ_ENABLE    1
/* Set the clockdivision factor */
#define TIMER3_PRESCALER     2


/** Defines for interrupt controler Module
 *
 */
#define IRQSTA    _MMR_SIZE_32(0x0000)
#define IRQSIG    _MMR_SIZE_32(0x0004)
#define IRQEN     _MMR_SIZE_32(0x0008)
#define IRQCLR    _MMR_SIZE_32(0x000C)
#define SWICFG    _MMR_SIZE_32(0x0010)
#define FIQSTA    _MMR_SIZE_32(0x0100)
#define FIQSIG    _MMR_SIZE_32(0x0104)
#define FIQEN     _MMR_SIZE_32(0x0108)
#define FIQCLR    _MMR_SIZE_32(0x010C)

#define INT_ALL              0
#define INT_SWI              1
#define INT_TIMER0           2
#define INT_TIMER1           3
#define INT_WAKEUP_TIMER2    4
#define INT_WATCHDOG_TIMER3  5
#define INT_FLASH_CONTROL    6
#define INT_ADC_CHANNEL      7
#define INT_PLL_LOCK         8
#define INT_I2C0_SLAVE       9
#define INT_I2C0_MASTER      10
#define INT_I2C1_MASTER      11
#define INT_SPI_SLAVE        12
#define INT_SPI_MASTER       13
#define INT_UART             14
#define INT_EXTERNAL_IRQ0    15
#define INT_COMPARATOR       16
#define INT_PSM              17
#define INT_EXTERNAL_IRQ1    18
#define INT_PLA_IRQ0         19
#define INT_PLA_IRQ1         20
#define INT_EXTERNAL_IRQ2    21
#define INT_EXTERNAL_IRQ3    22
#define INT_PWM_TRIP         23


/** Defines for UART Modul
 *
 */
#define COMTX     _MMR_SIZE_08(0x0700)
#define COMRX     _MMR_SIZE_08(0x0700)
#define COMIEN0   _MMR_SIZE_08(0x0704)
#define COMIID0   _MMR_SIZE_08(0x0708)
#define COMCON0   _MMR_SIZE_08(0x070C)
#define COMCON1   _MMR_SIZE_08(0x0710)
#define COMSTA0   _MMR_SIZE_08(0x0714)
#define COMSTA1   _MMR_SIZE_08(0x0718)
#define COMSCR    _MMR_SIZE_08(0x071C)
#define COMIEN1   _MMR_SIZE_08(0x0720)
#define COMIID1   _MMR_SIZE_08(0x0724)
#define COMADR    _MMR_SIZE_08(0x0728)
#define COMDIV0   _MMR_SIZE_08(0x0700)
#define COMDIV1   _MMR_SIZE_08(0x0704)
#define COMDIV2   _MMR_SIZE_16(0x072C)

#define UART_DLAB            7
#define UART_BRK             6
#define UART_SP              5
#define UART_EPS             4
#define UART_PEN             3
#define UART_STOP            2
#define UART_WLS0            0
#define UART_WLS1            1
#define UART_TEMT            6
#define UART_THRE            5
#define UART_BI              4
#define UART_FE              3
#define UART_PE              2
#define UART_OE              1
#define UART_DR              0
#define UART_LOOPBACK        4
#define UART_FBEN            15
#define UART_FBN             0
#define UART_FBM0            11
#define UART_FBM1            12


/** Defines for Reset Modul
 *
 */
#define REMAP     _MMR_SIZE_08(0x0220)
#define RSTSTA    _MMR_SIZE_08(0x0230)
#define RSTCLR    _MMR_SIZE_08(0x0234)

#define REMAP_SRAM           0

#define RST_SOFTRST          2
#define RST_WDTRST           1
#define RST_POWRONRST        0



/** Defines for ADC Modul
 *
 */
#define ADCCON    _MMR_SIZE_16(0x0500)
#define ADCCP     _MMR_SIZE_08(0x0504)
#define ADCCN     _MMR_SIZE_08(0x0508)
#define ADCSTA    _MMR_SIZE_08(0x050C)
#define ADCDAT    _MMR_SIZE_32(0x0510)
#define ADCRST    _MMR_SIZE_08(0x0514)
#define ADCGN     _MMR_SIZE_16(0x0530)
#define ADCOF     _MMR_SIZE_16(0x0534)

#define ADC_CLOCK_SPEED        10
#define ADC_ACQUISITION_TIME    8
#define ADC_ENABLE_CONVERION    7
#define ADC_ENABLE_ADCBUSY      6
#define ADC_POWER_CONTROL       5
#define ADC_CONVERSION_MODE     3
#define ADC_TRIGGER_SOURCE      0


#define ADC_NCHANNEL_SELECTION  0
#define ADC_PCHANNEL_SELECTION  0


/** Defines for FLASH Modul
 *
 */
#define FEESTA    _MMR_SIZE_08(0xF800)
#define FEEMOD    _MMR_SIZE_16(0xF804)
#define FEECON    _MMR_SIZE_08(0xF808)
#define FEEDAT    _MMR_SIZE_16(0xF80C)
#define FEEADR    _MMR_SIZE_16(0xF810)
#define FEESIGN   _MMR_SIZE_32(0xF818)
#define FEEPRO    _MMR_SIZE_32(0xF81C)
#define FEEHIDE   _MMR_SIZE_32(0xF820)

#define FEE_CONTROLLER_BUSY   2
#define FEE_FAIL              1
#define FEE_PASS              0
#define FEE_EW_PROTECTION     3
#define FEE_INT_ENABLE        3

#define FEE_CMD_SINGLE_READ   0x01
#define FEE_CMD_SINGLE_WRITE  0x02
#define FEE_CMD_SINGLE_VERIFY 0x04
#define FEE_CMD_SINGLE_ERASE  0x05
#define FEE_CMD_MASS_ERASE    0x06
#define FEE_CMD_PING          0x0F


/** Defines for DAC Modul
 *
 */
#define DAC0CON   _MMR_SIZE_08(0x0600)
#define DAC0DAT   _MMR_SIZE_32(0x0604)
#define DAC1CON   _MMR_SIZE_08(0x0608)
#define DAC1DAT   _MMR_SIZE_32(0x060C)
#define DAC2CON   _MMR_SIZE_08(0x0610)
#define DAC2DAT   _MMR_SIZE_32(0x0614)
#define DAC3CON   _MMR_SIZE_08(0x0618)
#define DAC3DAT   _MMR_SIZE_32(0x061C)

#define DAC_DACCLK            5
#define DAC_DACCLR            4
#define DAC_POWER_DOWN        0
#define DAC_DATA              16


/** Defines for Programmable logic array
 *
 */
#define PLAELM0   _MMR_SIZE_16(0x0B00)
#define PLAELM1   _MMR_SIZE_16(0x0B04)
#define PLAELM2   _MMR_SIZE_16(0x0B08)
#define PLAELM3   _MMR_SIZE_16(0x0B0C)
#define PLAELM4   _MMR_SIZE_16(0x0B10)
#define PLAELM5   _MMR_SIZE_16(0x0B14)
#define PLAELM6   _MMR_SIZE_16(0x0B18)
#define PLAELM7   _MMR_SIZE_16(0x0B1C)
#define PLAELM8   _MMR_SIZE_16(0x0B20)
#define PLAELM9   _MMR_SIZE_16(0x0B24)
#define PLAELM10  _MMR_SIZE_16(0x0B28)
#define PLAELM11  _MMR_SIZE_16(0x0B2C)
#define PLAELM12  _MMR_SIZE_16(0x0B30)
#define PLAELM13  _MMR_SIZE_16(0x0B34)
#define PLAELM14  _MMR_SIZE_16(0x0B38)
#define PLAELM15  _MMR_SIZE_16(0x0B3C)
#define PLACLK    _MMR_SIZE_08(0x0B40)
#define PLAIRQ    _MMR_SIZE_32(0x0B44)
#define PLAADC    _MMR_SIZE_32(0x0B48)
#define PLADIN    _MMR_SIZE_32(0x0B4C)
#define PLADOUT   _MMR_SIZE_32(0x0B50)
#define PLALCK    _MMR_SIZE_08(0x0B54)

#define PLA_BLOCK1_CLK_SRC    4
#define PLA_BLOCK0_CLK_SRC    0
#define PLA_ADC_CONV_START    4
#define PLA_ADC_CONV_SRC      0
#define PLA_MUX0_CONTROL      9
#define PLA_MUX1_CONTROL      7
#define PLA_MUX2_CONTROL      6
#define PLA_MUX3_CONTROL      5
#define PLA_MUX4_CONTROL      0
#define PLA_LOOKUP_TABLE      1

#define PLADIN_ELEMENT_0      0
#define PLADIN_ELEMENT_1      1
#define PLADIN_ELEMENT_2      2


/** @} */


#endif  /* !ADUC7026_H */
