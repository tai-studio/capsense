/*******************************************************************************
* File Name: SCB_1_EZI2C.c
* Version 1.20
*
* Description:
*  This file provides the source code to the API for the SCB Component in
*  EZI2C mode.
*
* Note:
*
*******************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SCB_1_PVT.h"
#include "SCB_1_EZI2C_PVT.h"


/***************************************
*      EZI2C Private Vars
***************************************/

volatile uint8 SCB_1_curStatus; /* Status byte */
uint8 SCB_1_fsmState;           /* FSM state   */

/* Variables intended to be used with Buffer1: Primary slave address */
volatile uint8 * SCB_1_dataBuffer1; /* Pointer to data buffer1 */
uint16 SCB_1_bufSizeBuf1; /* Size of buffer1 in bytes      */
uint16 SCB_1_protectBuf1; /* Start index of write protected area buffer1 */
uint16 SCB_1_offsetBuf1; /* Current offset within buffer1 */
uint16 SCB_1_indexBuf1;  /* Current index within buffer1  */

#if(SCB_1_SECONDARY_ADDRESS_ENABLE_CONST)
    uint8 SCB_1_addrBuf1; /* Primary slave address. Used for software comparison   */
    uint8 SCB_1_addrBuf2; /* Secondary slave address. Used for software comparison */

    /* Variables intended to be used with Buffer1: Primary slave address */
    volatile uint8 * SCB_1_dataBuffer2; /* Pointer to data buffer2 */
    uint16 SCB_1_bufSizeBuf2; /* Size of buffer2 in bytes      */
    uint16 SCB_1_protectBuf2; /* Start index of write protected area buffer 2 */
    uint16 SCB_1_offsetBuf2; /* Current offset within buffer2 */
    uint16 SCB_1_indexBuf2;  /* Current index within buffer2  */
#endif /* (SCB_1_SECONDARY_ADDRESS_ENABLE_CONST) */


/***************************************
*      EZI2C Private Functions
***************************************/

#if(SCB_1_SECONDARY_ADDRESS_ENABLE_CONST)
    static uint32 SCB_1_EzI2CUpdateRxMatchReg(uint32 addr1, uint32 addr2);
#endif /* (SCB_1_SECONDARY_ADDRESS_ENABLE_CONST) */

#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)

    /***************************************
    *  Config Structure Initialization
    ***************************************/

    /* Constant configuration of EZI2C */
    const SCB_1_EZI2C_INIT_STRUCT SCB_1_configEzI2C =
    {
        SCB_1_EZI2C_CLOCK_STRETCHING,
        SCB_1_EZI2C_MEDIAN_FILTER_ENABLE,
        SCB_1_EZI2C_NUMBER_OF_ADDRESSES,
        SCB_1_EZI2C_PRIMARY_SLAVE_ADDRESS,
        SCB_1_EZI2C_SECONDARY_SLAVE_ADDRESS,
        SCB_1_EZI2C_SUB_ADDRESS_SIZE,
        SCB_1_EZI2C_WAKE_ENABLE
    };

    /*******************************************************************************
    * Function Name: SCB_1_EzI2CInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for the EZI2C operation.
    *
    * Parameters:
    *  config:  Pointer to a structure that contains the following ordered list of
    *           fields. These fields match the selections available in the
    *           customizer.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void SCB_1_EzI2CInit(const SCB_1_EZI2C_INIT_STRUCT *config)
    {
        if(NULL == config)
        {
            CYASSERT(0u != 0u); /* Halt execution due to invalid function parameter */
        }
        else
        {
            /* Configure pins */
            SCB_1_SetPins(SCB_1_SCB_MODE_EZI2C, SCB_1_DUMMY_PARAM,
                                                                      SCB_1_DUMMY_PARAM);

            /* Store internal configuration */
            SCB_1_scbMode       = (uint8) SCB_1_SCB_MODE_EZI2C;
            SCB_1_scbEnableWake = (uint8) config->enableWake;
            SCB_1_scbEnableIntr = (uint8) SCB_1_SCB_IRQ_INTERNAL;

            SCB_1_numberOfAddr  = (uint8) config->numberOfAddresses;
            SCB_1_subAddrSize   = (uint8) config->subAddrSize;

            /* Configure I2C interface */
            SCB_1_CTRL_REG     = SCB_1_GET_CTRL_ADDR_ACCEPT(config->numberOfAddresses) |
                                            SCB_1_GET_CTRL_EC_AM_MODE (config->enableWake);

            SCB_1_I2C_CTRL_REG = SCB_1_EZI2C_CTRL;

        #if(SCB_1_CY_SCBIP_V0)
            /* Adjust SDA filter settings. Ticket ID#150521 */
            SCB_1_SET_I2C_CFG_SDA_FILT_TRIM(SCB_1_EC_AM_I2C_CFG_SDA_FILT_TRIM);
        #endif /* (SCB_1_CY_SCBIP_V0) */

            /* Configure RX direction */
            SCB_1_RX_CTRL_REG      = SCB_1_GET_RX_CTRL_MEDIAN(config->enableMedianFilter) |
                                                SCB_1_EZI2C_RX_CTRL;
            SCB_1_RX_FIFO_CTRL_REG = SCB_1_CLEAR_REG;

            /* Set default address and mask */
            if(SCB_1_EZI2C_PRIMARY_ADDRESS == config->numberOfAddresses)
            {
                SCB_1_RX_MATCH_REG = SCB_1_EzI2CUpdateRxMatchReg(config->primarySlaveAddr,
                                                                                       config->primarySlaveAddr);
            }
            else
            {
                SCB_1_RX_MATCH_REG = SCB_1_EzI2CUpdateRxMatchReg(config->primarySlaveAddr,
                                                                                       config->secondarySlaveAddr);
            }

            /* Configure TX direction */
            SCB_1_TX_CTRL_REG      = SCB_1_EZI2C_TX_CTRL;
            SCB_1_TX_FIFO_CTRL_REG = ((0u != (config->enableClockStretch)) ?
                                                 SCB_1_CLEAR_REG : SCB_1_EZI2C_TX_FIFO_CTRL);

            /* Configure interrupt sources */
        #if(SCB_1_CY_SCBIP_V0)
           SCB_1_INTR_SPI_EC_MASK_REG = SCB_1_NO_INTR_SOURCES;
        #endif /* (SCB_1_CY_SCBIP_V0) */

            SCB_1_INTR_I2C_EC_MASK_REG = SCB_1_NO_INTR_SOURCES;
            SCB_1_INTR_MASTER_MASK_REG = SCB_1_NO_INTR_SOURCES;
            SCB_1_INTR_SLAVE_MASK_REG  = SCB_1_EZI2C_INTR_SLAVE_MASK;
            SCB_1_INTR_TX_MASK_REG     = SCB_1_NO_INTR_SOURCES;

            /* Configure interrupt with EZI2C handler but do not enable it */
            CyIntDisable    (SCB_1_ISR_NUMBER);
            CyIntSetPriority(SCB_1_ISR_NUMBER, SCB_1_ISR_PRIORITY);
            (void) CyIntSetVector(SCB_1_ISR_NUMBER, (0u != (config->enableClockStretch)) ?
                                                                      (&SCB_1_EZI2C_STRETCH_ISR) :
                                                                      (&SCB_1_EZI2C_NO_STRETCH_ISR));

            if(0u != (config->enableClockStretch))
            {
                /* Configure interrupt sources */
                SCB_1_INTR_SLAVE_MASK_REG |= SCB_1_INTR_SLAVE_I2C_ADDR_MATCH;
                SCB_1_INTR_RX_MASK_REG     = SCB_1_NO_INTR_SOURCES;
            }
            else
            {
                /* Enable Auto ACK/NACK features */
                SCB_1_I2C_CTRL_REG |= SCB_1_EZI2C_CTRL_AUTO;

                /* Configure RX interrupt source */
                SCB_1_INTR_SLAVE_MASK_REG |= SCB_1_INTR_SLAVE_I2C_START;
                SCB_1_INTR_RX_MASK_REG     = SCB_1_INTR_RX_NOT_EMPTY;
            }

            /* Configure global variables */
            SCB_1_fsmState = SCB_1_EZI2C_FSM_IDLE;

            SCB_1_curStatus  = 0u;
            SCB_1_indexBuf1  = 0u;
            SCB_1_offsetBuf1 = 0u;
            SCB_1_indexBuf2  = 0u;
            SCB_1_offsetBuf2 = 0u;

            SCB_1_addrBuf1 = (uint8) config->primarySlaveAddr;
            SCB_1_addrBuf2 = (uint8) config->secondarySlaveAddr;
        }
    }

#else

    /*******************************************************************************
    * Function Name: SCB_1_EzI2CInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for the EZI2C operation.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void SCB_1_EzI2CInit(void)
    {
        /* Configure I2C interface */
        SCB_1_CTRL_REG     = SCB_1_EZI2C_DEFAULT_CTRL;
        SCB_1_I2C_CTRL_REG = SCB_1_EZI2C_DEFAULT_I2C_CTRL;

    #if(SCB_1_CY_SCBIP_V0)
        /* Adjust SDA filter settings. Ticket ID#150521 */
        SCB_1_SET_I2C_CFG_SDA_FILT_TRIM(SCB_1_EC_AM_I2C_CFG_SDA_FILT_TRIM);
    #endif /* (SCB_1_CY_SCBIP_V0) */

        /* Configure RX direction */
        SCB_1_RX_CTRL_REG      = SCB_1_EZI2C_DEFAULT_RX_CTRL;
        SCB_1_RX_FIFO_CTRL_REG = SCB_1_EZI2C_DEFAULT_RX_FIFO_CTRL;

        /* Set default address and mask */
        SCB_1_RX_MATCH_REG     = SCB_1_EZI2C_DEFAULT_RX_MATCH;

        /* Configure TX direction */
        SCB_1_TX_CTRL_REG      = SCB_1_EZI2C_DEFAULT_TX_CTRL;
        SCB_1_TX_FIFO_CTRL_REG = SCB_1_EZI2C_DEFAULT_TX_FIFO_CTRL;

        /* Configure interrupt with EZI2C handler but do not enable it */
    #if !defined (CY_EXTERNAL_INTERRUPT_CONFIG)
        CyIntDisable    (SCB_1_ISR_NUMBER);
        CyIntSetPriority(SCB_1_ISR_NUMBER, SCB_1_ISR_PRIORITY);

    #if(SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST)
        (void) CyIntSetVector(SCB_1_ISR_NUMBER, &SCB_1_EZI2C_STRETCH_ISR);
    #else
        (void) CyIntSetVector(SCB_1_ISR_NUMBER, &SCB_1_EZI2C_NO_STRETCH_ISR);
    #endif /* (SCB_1_EZI2C_SCL_STRETCH_ENABLE_CONST) */

    #endif /* !defined (CY_EXTERNAL_INTERRUPT_CONFIG) */

        /* Configure interrupt sources */
    #if(SCB_1_CY_SCBIP_V0)
        SCB_1_INTR_SPI_EC_MASK_REG = SCB_1_EZI2C_DEFAULT_INTR_SPI_EC_MASK;
    #endif /* (SCB_1_CY_SCBIP_V0) */

        SCB_1_INTR_I2C_EC_MASK_REG = SCB_1_EZI2C_DEFAULT_INTR_I2C_EC_MASK;
        SCB_1_INTR_SLAVE_MASK_REG  = SCB_1_EZI2C_DEFAULT_INTR_SLAVE_MASK;
        SCB_1_INTR_MASTER_MASK_REG = SCB_1_EZI2C_DEFAULT_INTR_MASTER_MASK;
        SCB_1_INTR_RX_MASK_REG     = SCB_1_EZI2C_DEFAULT_INTR_RX_MASK;
        SCB_1_INTR_TX_MASK_REG     = SCB_1_EZI2C_DEFAULT_INTR_TX_MASK;

        /* Configure global variables */
        SCB_1_fsmState = SCB_1_EZI2C_FSM_IDLE;

        SCB_1_curStatus  = 0u;
        SCB_1_indexBuf1  = 0u;
        SCB_1_offsetBuf1 = 0u;

    #if(SCB_1_SECONDARY_ADDRESS_ENABLE_CONST)
        SCB_1_indexBuf2  = 0u;
        SCB_1_offsetBuf2 = 0u;

        SCB_1_addrBuf1 = SCB_1_EZI2C_PRIMARY_SLAVE_ADDRESS;
        SCB_1_addrBuf2 = SCB_1_EZI2C_SECONDARY_SLAVE_ADDRESS;
    #endif /* (SCB_1_SECONDARY_ADDRESS_ENABLE_CONST) */
    }
#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */


/*******************************************************************************
* Function Name: SCB_1_EzI2CStop
********************************************************************************
*
* Summary:
*  Resets the EZI2C FSM into a default state and disables TX interrupt sources.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SCB_1_EzI2CStop(void)
{
    /* Disable TX interrupt sources in order not to  cause the false interrupts trigger.
    * The incoming transaction will enable an appropriate TX interrupt.
    */
    SCB_1_SetTxInterruptMode(SCB_1_NO_INTR_SOURCES);

#if(SCB_1_CY_SCBIP_V0)
    /* Clear pending interrupt as after the block is disabled, the TX FIFO becomes empty and
    * the block does not gate the interrupt sources when disabled.
    */
    SCB_1_ClearPendingInt();
#endif /* (SCB_1_CY_SCBIP_V0) */

    SCB_1_fsmState = SCB_1_EZI2C_FSM_IDLE;
}


/*******************************************************************************
* Function Name: SCB_1_EzI2CGetActivity
********************************************************************************
*
* Summary:
*  Returns the EZI2C slave status.
*  The read, write and error status flags reset to zero after this function call.
*  The busy status flag is cleared when the transaction intended for the EZ
*  I2C slave completes.
*
* Parameters:
*  None
*
* Return:
*  Returns the status of the EZI2C Slave activity.
*
*******************************************************************************/
uint32 SCB_1_EzI2CGetActivity(void)
{
    uint32 status;
    uint32 slaveBusy;

    slaveBusy = 0u;

    SCB_1_DisableInt();  /* Lock from interruption */

    status = SCB_1_curStatus;

    /* Relay on address match event from HW as bus busy status */
#if(SCB_1_CY_SCBIP_V0)

    #if(SCB_1_EZI2C_SCL_STRETCH_DISABLE)
    {
        slaveBusy = (uint32) SCB_1_CHECK_INTR_SLAVE(SCB_1_INTR_SLAVE_I2C_ADDR_MATCH);
    }
    #endif
#else
    #if(SCB_1_EZI2C_SCL_STRETCH_DISABLE)
    {
        #if(SCB_1_EZI2C_EC_AM_ENABLE)
        {
            slaveBusy = (uint32) SCB_1_CHECK_INTR_I2C_EC(SCB_1_INTR_I2C_EC_WAKE_UP);
        }
        #else
        {
            slaveBusy = (uint32) SCB_1_CHECK_INTR_SLAVE(SCB_1_INTR_SLAVE_I2C_ADDR_MATCH);
        }
        #endif
    }
    #endif
#endif /* (SCB_1_CY_SCBIP_V0) */

    if(0u != slaveBusy)
    {
        status |= SCB_1_EZI2C_STATUS_BUSY;
    }

    SCB_1_curStatus &= ((uint8) ~SCB_1_EZI2C_CLEAR_STATUS);

    SCB_1_EnableInt();   /* Release lock */

    return(status);
}


/*******************************************************************************
* Function Name: SCB_1_EzI2CSetAddress1
********************************************************************************
*
* Summary:
*  Sets the primary I2C slave address.
*
* Parameters:
*  address: I2C slave address for the secondary device.
*           This address is the 7-bit right-justified slave address and does
*           not include the R/W bit.
*           This value may be any address between 0 and 127 (0x00 to 0x7F).
*
* Return:
*  None
*
*******************************************************************************/
void SCB_1_EzI2CSetAddress1(uint32 address)
{
    #if(SCB_1_SECONDARY_ADDRESS_ENABLE)
    {
        SCB_1_addrBuf1 = (uint8) address;

        SCB_1_RX_MATCH_REG = SCB_1_EzI2CUpdateRxMatchReg(address,
                                                                               (uint32) SCB_1_addrBuf2);
    }
    #else
    {
        uint32 matchReg;

        matchReg = SCB_1_RX_MATCH_REG;

        matchReg &= ((uint32) ~SCB_1_RX_MATCH_ADDR_MASK);            /* Clear address bits */
        matchReg |= ((uint32)  SCB_1_GET_I2C_8BIT_ADDRESS(address)); /* Set mask */

        SCB_1_RX_MATCH_REG = matchReg;
    }
    #endif
}


/*******************************************************************************
* Function Name: SCB_1_EzI2CGetAddress1
********************************************************************************
*
* Summary:
*  Gets the primary I2C slave 7-bit address.
*
* Parameters:
*  None
*
* Return:
*  Returns I2C slave address for the primary device.
*  This address is the 7-bit right-justified slave address and does not
*  include the R/W bit.
*  This value may be any address between 0 and 127 (0x00 to 0x7F).
*
*******************************************************************************/
uint32 SCB_1_EzI2CGetAddress1(void)
{
    uint32 address;

    #if(SCB_1_SECONDARY_ADDRESS_ENABLE)
    {
        address = (uint32) SCB_1_addrBuf1;
    }
    #else
    {
        address = (SCB_1_GET_RX_MATCH_ADDR(SCB_1_RX_MATCH_REG) >>
                   SCB_1_I2C_SLAVE_ADDR_POS);
    }
    #endif

    return(address);
}


/*******************************************************************************
* Function Name: SCB_1_EzI2CSetBuffer1
********************************************************************************
*
* Summary:
*  Sets up the data buffer to be exposed to the I2C master on a primary slave
*  address request.
*
* Parameters:
*  bufSize: Size of the buffer in bytes.
*  rwBoundary: Sets how many bytes are writeable in the beginning of the buffer.
*              This value must be less than or equal to the buffer size.
*              Data located at offset rwBoundry and greater are read only.
*  buffer: Pointer to the data buffer.
*
* Return:
*  None
*
*******************************************************************************/
void SCB_1_EzI2CSetBuffer1(uint32 bufSize, uint32 rwBoundary, volatile uint8 * buffer)
{
    if(NULL != buffer)
    {
        SCB_1_dataBuffer1 =  buffer;
        SCB_1_bufSizeBuf1 = (uint16) bufSize;
        SCB_1_protectBuf1 = (uint16) rwBoundary;
    }
}


/*******************************************************************************
* Function Name: SCB_1_EzI2CSetReadBoundaryBuffer1
********************************************************************************
*
* Summary:
*  Sets the read only boundary in the data buffer to be exposed to the I2C
*  master on a primary slave address request.
*
* Parameters:
*  rwBoundry: Sets how many bytes are writeable in the beginning of the buffer.
*             This value must be less than or equal to the buffer size.
*             Data located at offset rwBoundry and greater are read only.
*
* Return:
*  None
*
*******************************************************************************/
void SCB_1_EzI2CSetReadBoundaryBuffer1(uint32 rwBoundary)
{
    SCB_1_protectBuf1 = (uint16) rwBoundary;
}


#if(SCB_1_SECONDARY_ADDRESS_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: SCB_1_EzI2CUpdateRxMatchReg
    ********************************************************************************
    *
    * Summary:
    *  Returns the value of the RX MATCH register for addr1 and addr2. The addr1 is
    *  accepted as the primary address and it is written to RX_MATCH.ADDRESS
    *  (addr1 << 0x01).
    *  The RX_MATCH.MASK is set as follow: addr1 and addr2 equal bits set to 1
    *  otherwise 0.
    *
    * Parameters:
    *  addr1: I2C slave address for the primary device.
    *  addr2: I2C slave address for the secondary device.
    *         This address is the 7-bit right-justified slave address and does
    *         not include the R/W bit.
    *         This value may be any address between 0 and 127 (0x00 to 0x7F).
    *
    * Return:
    *  Value of RX MATCH register.
    *
    *******************************************************************************/
    static uint32 SCB_1_EzI2CUpdateRxMatchReg(uint32 addr1, uint32 addr2)
    {
        uint32 matchReg;

        matchReg  = ~(addr1 ^ addr2); /* If (addr1 and addr2) bit matches - mask bit equal 1, in other case 0 */

        matchReg  = (uint32) (SCB_1_GET_I2C_8BIT_ADDRESS(matchReg) << SCB_1_RX_MATCH_MASK_POS);
        matchReg |= SCB_1_GET_I2C_8BIT_ADDRESS(addr1);

        return(matchReg);
    }

    /*******************************************************************************
    * Function Name: SCB_1_EzI2CSetAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the secondary I2C slave address.
    *
    * Parameters:
    *  address: I2C slave address for the secondary device.
    *           This address is the 7-bit right-justified slave address and does
    *           not include the R/W bit.
    *           This value may be any address between 0 and 127 (0x00 to 0x7F).
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void SCB_1_EzI2CSetAddress2(uint32 address)
    {
        SCB_1_addrBuf2 = (uint8) address;

        SCB_1_RX_MATCH_REG = SCB_1_EzI2CUpdateRxMatchReg((uint32) SCB_1_addrBuf1,
                                                                               address);
    }


    /*******************************************************************************
    * Function Name: SCB_1_EzI2CGetAddress2
    ********************************************************************************
    *
    * Summary:
    *  Gets secondary the I2C slave 7-bit address.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Returns the I2C slave address for the secondary device.
    *  This address is a 7-bit right-justified slave address and does not
    *  include the R/W bit.
    *  This value may be any address between 0 and 127 (0x00 to 0x7F).
    *
    *******************************************************************************/
    uint32 SCB_1_EzI2CGetAddress2(void)
    {
        return((uint32) SCB_1_addrBuf2);
    }


    /*******************************************************************************
    * Function Name: SCB_1_EzI2CSetBuffer2
    ********************************************************************************
    *
    * Summary:
    *  Sets up the data buffer to be exposed to the I2C master on a secondary slave
    *  address request.
    *
    * Parameters:
    *  bufSize: Size of the buffer in bytes.
    *  rwBoundary: Sets how many bytes are writeable in the beginning of the buffer.
    *              This value must be less than or equal to the buffer size.
    *              Data located at offset rwBoundry and greater are read only.
    *  buffer: Pointer to the data buffer.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void SCB_1_EzI2CSetBuffer2(uint32 bufSize, uint32 rwBoundary, volatile uint8 * buffer)
    {
        if(NULL != buffer)
        {
            SCB_1_dataBuffer2 =  buffer;
            SCB_1_bufSizeBuf2 = (uint16) bufSize;
            SCB_1_protectBuf2 = (uint16) rwBoundary;
        }
    }


    /*******************************************************************************
    * Function Name: SCB_1_EzI2CSetReadBoundaryBuffer2
    ********************************************************************************
    *
    * Summary:
    *  Sets the read only boundary in the data buffer to be exposed to the I2C
    *  master on a secondary address request.
    *
    * Parameters:
    *  rwBoundary: Sets how many bytes are writeable at the beginning of the buffer.
    *              This value must be less than or equal to the buffer size.
    *              Data located at offset rwBoundry and greater are read only.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void SCB_1_EzI2CSetReadBoundaryBuffer2(uint32 rwBoundary)
    {
        SCB_1_protectBuf2 = (uint16) rwBoundary;
    }

#endif /* (SCB_1_SECONDARY_ADDRESS_ENABLE_CONST) */


#if(SCB_1_EZI2C_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: SCB_1_EzI2CSaveConfig
    ********************************************************************************
    *
    * Summary:
    *  Clock stretching is  enabled: Disables INTR_I2C_EC.WAKE_UP interrupt source.
    *  It triggers on the slave address match.
    *  Clock stretching is disabled: waits until the I2C slave becomes free and
    *  disables the block to perform reconfiguration from the active mode operation
    *  to deep sleep with wake up on the address match: enables the INTR_I2C_EC.WAKE_UP
    *  interrupt source and disables the INTR_S and INTR_TX interrupt sources.
    *  The block is disabled before reconfiguration and enabled when
    *  it is completed.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void SCB_1_EzI2CSaveConfig(void)
    {
    #if(SCB_1_CY_SCBIP_V0)

        #if(SCB_1_EZI2C_SCL_STRETCH_ENABLE)
        {
            /* Enable wakeup interrupt source on address match */
            SCB_1_SetI2CExtClkInterruptMode(SCB_1_INTR_I2C_EC_WAKE_UP);
        }
        #endif

    #else
        uint8 enableInt;

        enableInt = (uint8) SCB_1_INTR_I2C_EC_WAKE_UP;

        #if(SCB_1_EZI2C_SCL_STRETCH_ENABLE)
        {
            /* Enable interrupt source to wakeup the device */
            SCB_1_SetI2CExtClkInterruptMode(enableInt);
        }
        #else
        {
            for(;;) /* Wait for end of transaction intended to slave */
            {
                if(0u == (SCB_1_GetI2CExtClkInterruptSource() & SCB_1_INTR_I2C_EC_WAKE_UP))
                {
                    enableInt = CyEnterCriticalSection();

                    if(0u == (SCB_1_GetI2CExtClkInterruptSource() & SCB_1_INTR_I2C_EC_WAKE_UP))
                    {
                        /* Attempts to set NACK command before disable block */
                        SCB_1_I2C_SLAVE_GENERATE_NACK;

                        if(0u == (SCB_1_GetI2CExtClkInterruptSource() & SCB_1_INTR_I2C_EC_WAKE_UP))
                        {
                            /* NACK command was set before. It is safe to disable block */
                            SCB_1_CTRL_REG &= (uint32) ~SCB_1_CTRL_ENABLED;
                            SCB_1_DisableInt();

                            CyExitCriticalSection(enableInt);
                            break;
                        }
                        else
                        {
                            /* Clear NACK command to prevent data NACK */
                            SCB_1_I2C_SLAVE_CLEAR_NACK;
                        }
                    }

                    CyExitCriticalSection(enableInt);
                }
            }

            /* Disable all active mode interrupt sources */
            SCB_1_SetTxInterruptMode(SCB_1_NO_INTR_SOURCES);
            SCB_1_SetSlaveInterruptMode(SCB_1_NO_INTR_SOURCES);
            SCB_1_ClearPendingInt();
            SCB_1_EnableInt();

            /* Enable wakeup interrupt on address match */
            SCB_1_SetI2CExtClkInterruptMode(SCB_1_INTR_I2C_EC_WAKE_UP);

            enableInt = CyEnterCriticalSection();

            SCB_1_CTRL_REG |= (uint32) SCB_1_CTRL_ENABLED;
            SCB_1_I2C_SLAVE_GENERATE_NACK;

            CyExitCriticalSection(enableInt);
        }
        #endif
    #endif /* (SCB_1_CY_SCBIP_V0) */
    }


    /*******************************************************************************
    * Function Name: SCB_1_EzI2CRestoreConfig
    ********************************************************************************
    *
    * Summary:
    *  Clock stretching is  enabled: Disables the INTR_I2C_EC.WAKE_UP interrupt source.
    *  Clock stretching is disabled: Reconfigures the EZI2C component from deep sleep
    *  (wake up on the address match) to active operation: disables the INTR_I2C_EC.WAKE_UP
    *  interrupt source and restores the INTR_S. interrupt sources to operate in
    *  the active mode. The block is disabled before reconfiguration and enabled when
    *  it is completed.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void SCB_1_EzI2CRestoreConfig(void)
    {
    #if(SCB_1_CY_SCBIP_V0)

        #if(SCB_1_EZI2C_SCL_STRETCH_ENABLE)
        {
            /* Disable wakeup interrupt on address match */
            SCB_1_SetI2CExtClkInterruptMode(SCB_1_NO_INTR_SOURCES);
        }
        #endif

    #else

        #if(SCB_1_EZI2C_SCL_STRETCH_ENABLE)
        {
            /* Disables wakeup interrupt source on address match */
            SCB_1_SetI2CExtClkInterruptMode(SCB_1_NO_INTR_SOURCES);
        }
        #else
        {
            /* NACK will be driven on the bus by wakeup or NACK command.
            * It is safe to disable block to restore active mode configuration.
            */
            SCB_1_CTRL_REG &= (uint32) ~SCB_1_CTRL_ENABLED;

            /* Restore active mode interrupt sources */
            SCB_1_SetI2CExtClkInterruptMode(SCB_1_NO_INTR_SOURCES);
            SCB_1_SetSlaveInterruptMode(SCB_1_EZI2C_INTR_SLAVE_MASK |
                                                   SCB_1_INTR_SLAVE_I2C_START);
            SCB_1_ClearPendingInt();

            SCB_1_CTRL_REG |= (uint32) SCB_1_CTRL_ENABLED;
        }
        #endif

    #endif /* (SCB_1_CY_SCBIP_V0) */
    }
#endif /* (SCB_1_EZI2C_WAKE_ENABLE_CONST) */


/* [] END OF FILE */
