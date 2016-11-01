/*******************************************************************************
* File Name: SCB_1_EZI2C.h
* Version 1.20
*
* Description:
*  This file provides constants and parameter values for the SCB Component in
*  EZI2C mode.
*
* Note:
*
********************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_EZI2C_SCB_1_H)
#define CY_SCB_EZI2C_SCB_1_H

#include "SCB_1.h"


/***************************************
*   Initial Parameter Constants
****************************************/

#define SCB_1_EZI2C_CLOCK_STRETCHING         (1u)
#define SCB_1_EZI2C_MEDIAN_FILTER_ENABLE     (1u)
#define SCB_1_EZI2C_NUMBER_OF_ADDRESSES      (0u)
#define SCB_1_EZI2C_PRIMARY_SLAVE_ADDRESS    (8u)
#define SCB_1_EZI2C_SECONDARY_SLAVE_ADDRESS  (9u)
#define SCB_1_EZI2C_SUB_ADDRESS_SIZE         (1u)
#define SCB_1_EZI2C_WAKE_ENABLE              (0u)
#define SCB_1_EZI2C_DATA_RATE                (400u)
#define SCB_1_EZI2C_SLAVE_ADDRESS_MASK       (254u)


/***************************************
*  Conditional Compilation Parameters
****************************************/

#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)

    #define SCB_1_SUB_ADDRESS_SIZE16             (0u != SCB_1_subAddrSize)
    #define SCB_1_SECONDARY_ADDRESS_ENABLE       (0u != SCB_1_numberOfAddr)

    #define SCB_1_EZI2C_EC_AM_ENABLE         (0u != (SCB_1_CTRL_REG & \
                                                                SCB_1_CTRL_EC_AM_MODE))
    #define SCB_1_EZI2C_SCL_STRETCH_ENABLE   (0u != (SCB_1_GetI2CExtClkInterruptSourceMasked() & \
                                                                SCB_1_INTR_SLAVE_I2C_ADDR_MATCH))
    #define SCB_1_EZI2C_SCL_STRETCH_DISABLE       (!SCB_1_EZI2C_SCL_STRETCH_ENABLE)

    #define SCB_1_SECONDARY_ADDRESS_ENABLE_CONST  (1u)
    #define SCB_1_SUB_ADDRESS_SIZE16_CONST        (1u)
    #define SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST  (1u)
    #define SCB_1_EZI2C_SCL_STRETCH_DISABLE_CONST (1u)
    #define SCB_1_EZI2C_WAKE_ENABLE_CONST         (1u)

#else

    #define SCB_1_SUB_ADDRESS_SIZE16         (0u != SCB_1_EZI2C_SUB_ADDRESS_SIZE)
    #define SCB_1_SUB_ADDRESS_SIZE16_CONST   (SCB_1_SUB_ADDRESS_SIZE16)

    #define SCB_1_SECONDARY_ADDRESS_ENABLE        (0u != SCB_1_EZI2C_NUMBER_OF_ADDRESSES)
    #define SCB_1_SECONDARY_ADDRESS_ENABLE_CONST  (SCB_1_SECONDARY_ADDRESS_ENABLE)

    #define SCB_1_EZI2C_SCL_STRETCH_ENABLE        (0u != SCB_1_EZI2C_CLOCK_STRETCHING)
    #define SCB_1_EZI2C_SCL_STRETCH_DISABLE       (!SCB_1_EZI2C_SCL_STRETCH_ENABLE)
    #define SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST  (SCB_1_EZI2C_SCL_STRETCH_ENABLE)
    #define SCB_1_EZI2C_SCL_STRETCH_DISABLE_CONST (SCB_1_EZI2C_SCL_STRETCH_DISABLE)

    #define SCB_1_EZI2C_WAKE_ENABLE_CONST         (0u != SCB_1_EZI2C_WAKE_ENABLE)
    #define SCB_1_EZI2C_EC_AM_ENABLE              (0u != SCB_1_EZI2C_WAKE_ENABLE)

#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */

#define SCB_1_EZI2C_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SCB_1) || \
                                                   (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

/***************************************
*       Type Definitions
***************************************/

typedef struct
{
    uint32 enableClockStretch;
    uint32 enableMedianFilter;
    uint32 numberOfAddresses;
    uint32 primarySlaveAddr;
    uint32 secondarySlaveAddr;
    uint32 subAddrSize;
    uint32 enableWake;
} SCB_1_EZI2C_INIT_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)
    void SCB_1_EzI2CInit(const SCB_1_EZI2C_INIT_STRUCT *config);
#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */

uint32 SCB_1_EzI2CGetActivity(void);

void   SCB_1_EzI2CSetAddress1(uint32 address);
uint32 SCB_1_EzI2CGetAddress1(void);
void   SCB_1_EzI2CSetBuffer1(uint32 bufSize, uint32 rwBoundary, volatile uint8 * buffer);
void   SCB_1_EzI2CSetReadBoundaryBuffer1(uint32 rwBoundary);

#if(SCB_1_SECONDARY_ADDRESS_ENABLE_CONST)
    void   SCB_1_EzI2CSetAddress2(uint32 address);
    uint32 SCB_1_EzI2CGetAddress2(void);
    void   SCB_1_EzI2CSetBuffer2(uint32 bufSize, uint32 rwBoundary, volatile uint8 * buffer);
    void   SCB_1_EzI2CSetReadBoundaryBuffer2(uint32 rwBoundary);
#endif /* (SCB_1_SECONDARY_ADDRESS_ENABLE_CONST) */

#if(SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST)
    CY_ISR_PROTO(SCB_1_EZI2C_STRETCH_ISR);
#endif /* (SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST) */

#if(SCB_1_EZI2C_SCL_STRETCH_DISABLE_CONST)
    CY_ISR_PROTO(SCB_1_EZI2C_NO_STRETCH_ISR);
#endif /* (SCB_1_EZI2C_SCL_STRETCH_DISABLE) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_1_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void SCB_1_EZI2CCyBtldrCommStart(void);
    void SCB_1_EZI2CCyBtldrCommStop (void);
    void SCB_1_EZI2CCyBtldrCommReset(void);
    cystatus SCB_1_EZI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SCB_1_EZI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_1_EZI2C_BTLDR_COMM_ENABLED) */


/***************************************
*            API Constants
***************************************/

/* Configuration structure constants */
#define SCB_1_EZI2C_ONE_ADDRESS      (0u)
#define SCB_1_EZI2C_TWO_ADDRESSES    (1u)

#define SCB_1_EZI2C_PRIMARY_ADDRESS  (0u)
#define SCB_1_EZI2C_BOTH_ADDRESSES   (1u)

#define SCB_1_EZI2C_SUB_ADDR8_BITS   (0u)
#define SCB_1_EZI2C_SUB_ADDR16_BITS  (1u)

/* SCB_1_EzI2CGetActivity() return constants */
#define SCB_1_EZI2C_STATUS_SECOND_OFFSET (1u)
#define SCB_1_EZI2C_STATUS_READ1     ((uint32) (SCB_1_INTR_SLAVE_I2C_NACK))        /* Bit [1]   */
#define SCB_1_EZI2C_STATUS_WRITE1    ((uint32) (SCB_1_INTR_SLAVE_I2C_WRITE_STOP))  /* Bit [3]   */
#define SCB_1_EZI2C_STATUS_READ2     ((uint32) (SCB_1_INTR_SLAVE_I2C_NACK >> \
                                                           SCB_1_EZI2C_STATUS_SECOND_OFFSET)) /* Bit [0]   */
#define SCB_1_EZI2C_STATUS_WRITE2    ((uint32) (SCB_1_INTR_SLAVE_I2C_WRITE_STOP >> \
                                                           SCB_1_EZI2C_STATUS_SECOND_OFFSET)) /* Bit [2]   */
#define SCB_1_EZI2C_STATUS_ERR       ((uint32) (0x10u))                                       /* Bit [4]   */
#define SCB_1_EZI2C_STATUS_BUSY      ((uint32) (0x20u))                                       /* Bit [5]   */
#define SCB_1_EZI2C_CLEAR_STATUS     ((uint32) (0x1Fu))                                       /* Bits [0-4]*/
#define SCB_1_EZI2C_CMPLT_INTR_MASK  ((uint32) (SCB_1_INTR_SLAVE_I2C_NACK | \
                                                           SCB_1_INTR_SLAVE_I2C_WRITE_STOP))


/***************************************
*     Vars with External Linkage
***************************************/

#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)
    extern const SCB_1_EZI2C_INIT_STRUCT SCB_1_configEzI2C;
#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*           FSM states
***************************************/

/* Returns to master when there is no data to transmit */
#define SCB_1_EZI2C_OVFL_RETURN  (0xFFu)

/* States of EZI2C Slave FSM */
#define SCB_1_EZI2C_FSM_OFFSET_HI8 ((uint8) (0x02u)) /* Idle state for 1 addr: waits for high byte offset */
#define SCB_1_EZI2C_FSM_OFFSET_LO8 ((uint8) (0x00u)) /* Idle state for 2 addr: waits for low byte offset  */
#define SCB_1_EZI2C_FSM_BYTE_WRITE ((uint8) (0x01u)) /* Data byte write state: byte by byte mode          */
#define SCB_1_EZI2C_FSM_WAIT_STOP  ((uint8) (0x03u)) /* Discards written bytes as they do not match write
                                                                   criteria */
#define SCB_1_EZI2C_FSM_WRITE_MASK ((uint8) (0x01u)) /* Incorporates write states: EZI2C_FSM_BYTE_WRITE and
                                                                   EZI2C_FSM_WAIT_STOP  */

#define SCB_1_EZI2C_FSM_IDLE     ((SCB_1_SUB_ADDRESS_SIZE16) ?      \
                                                (SCB_1_EZI2C_FSM_OFFSET_HI8) : \
                                                (SCB_1_EZI2C_FSM_OFFSET_LO8))

#define SCB_1_EZI2C_CONTINUE_TRANSFER    (1u)
#define SCB_1_EZI2C_COMPLETE_TRANSFER    (0u)

#define SCB_1_EZI2C_NACK_RECEIVED_ADDRESS  (0u)
#define SCB_1_EZI2C_ACK_RECEIVED_ADDRESS   (1u)

#define SCB_1_EZI2C_ACTIVE_ADDRESS1  (0u)
#define SCB_1_EZI2C_ACTIVE_ADDRESS2  (1u)


/***************************************
*       Macro Definitions
***************************************/

/* Access to global variables */
#if(SCB_1_SECONDARY_ADDRESS_ENABLE_CONST)

    #define SCB_1_EZI2C_UPDATE_LOC_STATUS(activeAddress, locStatus) \
            do{ \
                (locStatus) >>= (activeAddress); \
            }while(0)

    #define SCB_1_EZI2C_GET_INDEX(activeAddress) \
            ((SCB_1_EZI2C_ACTIVE_ADDRESS1 == (activeAddress)) ? \
                ((uint32) SCB_1_indexBuf1) :                    \
                ((uint32) SCB_1_indexBuf2))

    #define SCB_1_EZI2C_GET_OFFSET(activeAddress) \
            ((SCB_1_EZI2C_ACTIVE_ADDRESS1 == (activeAddress)) ? \
                ((uint32) SCB_1_offsetBuf1) :                   \
                ((uint32) SCB_1_offsetBuf2))

    #define SCB_1_EZI2C_SET_INDEX(activeAddress, locIndex) \
            do{ \
                if(SCB_1_EZI2C_ACTIVE_ADDRESS1 == (activeAddress)) \
                {    \
                    SCB_1_indexBuf1 = (uint16) (locIndex); \
                }    \
                else \
                {    \
                    SCB_1_indexBuf2 = (uint16) (locIndex); \
                }    \
            }while(0)

    #define SCB_1_EZI2C_SET_OFFSET(activeAddress, locOffset) \
            do{ \
                if(SCB_1_EZI2C_ACTIVE_ADDRESS1 == (activeAddress)) \
                {    \
                    SCB_1_offsetBuf1 = (uint16) (locOffset); \
                }    \
                else \
                {    \
                    SCB_1_offsetBuf2 = (uint16) (locOffset); \
                }    \
            }while(0)
#else
    #define SCB_1_EZI2C_UPDATE_LOC_STATUS(activeAddress, locStatus)  do{ /* Empty*/ }while(0)

    #define SCB_1_EZI2C_GET_INDEX(activeAddress)  ((uint32) (SCB_1_indexBuf1))

    #define SCB_1_EZI2C_GET_OFFSET(activeAddress) ((uint32) (SCB_1_offsetBuf1))

    #define SCB_1_EZI2C_SET_INDEX(activeAddress, locIndex) \
            do{ \
                SCB_1_indexBuf1 = (uint16) (locIndex); \
            }while(0)

    #define SCB_1_EZI2C_SET_OFFSET(activeAddress, locOffset) \
            do{ \
                SCB_1_offsetBuf1 = (uint16) (locOffset); \
            }while(0)

#endif  /* (SCB_1_SUB_ADDRESS_SIZE16_CONST) */


/***************************************
*      Common Register Settings
***************************************/

#define SCB_1_CTRL_EZI2C     (SCB_1_CTRL_MODE_I2C)

#define SCB_1_EZI2C_CTRL     (SCB_1_I2C_CTRL_S_GENERAL_IGNORE | \
                                         SCB_1_I2C_CTRL_SLAVE_MODE)

#define SCB_1_EZI2C_CTRL_AUTO    (SCB_1_I2C_CTRL_S_READY_ADDR_ACK      | \
                                             SCB_1_I2C_CTRL_S_READY_DATA_ACK      | \
                                             SCB_1_I2C_CTRL_S_NOT_READY_ADDR_NACK | \
                                             SCB_1_I2C_CTRL_S_NOT_READY_DATA_NACK)

#define SCB_1_EZI2C_RX_CTRL  ((SCB_1_FIFO_SIZE - 1u)   | \
                                          SCB_1_RX_CTRL_MSB_FIRST | \
                                          SCB_1_RX_CTRL_ENABLED)

#define SCB_1_EZI2C_TX_FIFO_CTRL (2u)
#define SCB_1_TX_LOAD_SIZE       (2u)

#define SCB_1_EZI2C_TX_CTRL  ((SCB_1_FIFO_SIZE - 1u)   | \
                                          SCB_1_TX_CTRL_MSB_FIRST | \
                                          SCB_1_TX_CTRL_ENABLED)

#define SCB_1_EZI2C_INTR_SLAVE_MASK  (SCB_1_INTR_SLAVE_I2C_BUS_ERROR | \
                                                 SCB_1_INTR_SLAVE_I2C_ARB_LOST  | \
                                                 SCB_1_INTR_SLAVE_I2C_STOP)

#define SCB_1_INTR_SLAVE_COMPLETE    (SCB_1_INTR_SLAVE_I2C_STOP | \
                                                 SCB_1_INTR_SLAVE_I2C_NACK | \
                                                 SCB_1_INTR_SLAVE_I2C_WRITE_STOP)


/***************************************
*    Initialization Register Settings
***************************************/

#if(SCB_1_SCB_MODE_EZI2C_CONST_CFG)

    #define SCB_1_EZI2C_DEFAULT_CTRL \
                                (SCB_1_GET_CTRL_ADDR_ACCEPT(SCB_1_EZI2C_NUMBER_OF_ADDRESSES) | \
                                 SCB_1_GET_CTRL_EC_AM_MODE (SCB_1_EZI2C_WAKE_ENABLE))

    #if(SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST)
        #define SCB_1_EZI2C_DEFAULT_I2C_CTRL (SCB_1_EZI2C_CTRL)
    #else
        #define SCB_1_EZI2C_DEFAULT_I2C_CTRL (SCB_1_EZI2C_CTRL_AUTO | SCB_1_EZI2C_CTRL)
    #endif /* (SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST) */


    #define SCB_1_EZI2C_DEFAULT_RX_MATCH \
                                (SCB_1_GET_I2C_8BIT_ADDRESS(SCB_1_EZI2C_PRIMARY_SLAVE_ADDRESS) | \
                                 SCB_1_GET_RX_MATCH_MASK   (SCB_1_EZI2C_SLAVE_ADDRESS_MASK))

    #define SCB_1_EZI2C_DEFAULT_RX_CTRL \
                                (SCB_1_GET_RX_CTRL_MEDIAN(SCB_1_EZI2C_MEDIAN_FILTER_ENABLE) | \
                                 SCB_1_EZI2C_RX_CTRL)

    #define SCB_1_EZI2C_DEFAULT_TX_CTRL  (SCB_1_EZI2C_TX_CTRL)

    #define SCB_1_EZI2C_DEFAULT_RX_FIFO_CTRL (0u)
    #if(SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST)
        #define SCB_1_EZI2C_DEFAULT_TX_FIFO_CTRL (0u)
    #else
        #define SCB_1_EZI2C_DEFAULT_TX_FIFO_CTRL (2u)
    #endif /* (SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST) */

    /* Interrupt sources */
    #define SCB_1_EZI2C_DEFAULT_INTR_I2C_EC_MASK (SCB_1_NO_INTR_SOURCES)
    #define SCB_1_EZI2C_DEFAULT_INTR_SPI_EC_MASK (SCB_1_NO_INTR_SOURCES)
    #define SCB_1_EZI2C_DEFAULT_INTR_MASTER_MASK (SCB_1_NO_INTR_SOURCES)
    #define SCB_1_EZI2C_DEFAULT_INTR_TX_MASK     (SCB_1_NO_INTR_SOURCES)

    #if(SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST)
        #define SCB_1_EZI2C_DEFAULT_INTR_RX_MASK     (SCB_1_NO_INTR_SOURCES)
        #define SCB_1_EZI2C_DEFAULT_INTR_SLAVE_MASK  (SCB_1_INTR_SLAVE_I2C_ADDR_MATCH | \
                                                                 SCB_1_EZI2C_INTR_SLAVE_MASK)
    #else
        #define SCB_1_EZI2C_DEFAULT_INTR_RX_MASK     (SCB_1_INTR_RX_NOT_EMPTY)
        #define SCB_1_EZI2C_DEFAULT_INTR_SLAVE_MASK  (SCB_1_INTR_SLAVE_I2C_START | \
                                                                 SCB_1_EZI2C_INTR_SLAVE_MASK)
    #endif /* (SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST) */

#endif /* (SCB_1_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* (CY_SCB_EZI2C_SCB_1_H) */


/* [] END OF FILE */
