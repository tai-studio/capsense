/*******************************************************************************
* File Name: SCB_1_PM.c
* Version 1.20
*
* Description:
*  This file provides the source code to the Power Management support for
*  the SCB Component.
*
* Note:
*
********************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SCB_1.h"
#include "SCB_1_PVT.h"

#if(SCB_1_SCB_MODE_I2C_INC)
    #include "SCB_1_I2C_PVT.h"
#endif /* (SCB_1_SCB_MODE_I2C_INC) */

#if(SCB_1_SCB_MODE_EZI2C_INC)
    #include "SCB_1_EZI2C_PVT.h"
#endif /* (SCB_1_SCB_MODE_EZI2C_INC) */

#if(SCB_1_SCB_MODE_SPI_INC || SCB_1_SCB_MODE_UART_INC)
    #include "SCB_1_SPI_UART_PVT.h"
#endif /* (SCB_1_SCB_MODE_SPI_INC || SCB_1_SCB_MODE_UART_INC) */


/***************************************
*   Backup Structure declaration
***************************************/

#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG || \
   (SCB_1_SCB_MODE_I2C_CONST_CFG   && (!SCB_1_I2C_WAKE_ENABLE_CONST))   || \
   (SCB_1_SCB_MODE_EZI2C_CONST_CFG && (!SCB_1_EZI2C_WAKE_ENABLE_CONST)) || \
   (SCB_1_SCB_MODE_SPI_CONST_CFG   && (!SCB_1_SPI_WAKE_ENABLE_CONST))   || \
   (SCB_1_SCB_MODE_UART_CONST_CFG  && (!SCB_1_UART_WAKE_ENABLE_CONST)))

    SCB_1_BACKUP_STRUCT SCB_1_backup =
    {
        0u, /* enableState */
    };
#endif


/*******************************************************************************
* Function Name: SCB_1_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component to enter Deep Sleep.
*  The “Enable wakeup from Sleep Mode” selection has an influence on
*  this function implementation.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SCB_1_Sleep(void)
{
#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)

    if(SCB_1_SCB_WAKE_ENABLE_CHECK)
    {
        if(SCB_1_SCB_MODE_I2C_RUNTM_CFG)
        {
            SCB_1_I2CSaveConfig();
        }
        else if(SCB_1_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            SCB_1_EzI2CSaveConfig();
        }
    #if(!SCB_1_CY_SCBIP_V1_I2C_ONLY)
        else if(SCB_1_SCB_MODE_SPI_RUNTM_CFG)
        {
            SCB_1_SpiSaveConfig();
        }
        else if(SCB_1_SCB_MODE_UART_RUNTM_CFG)
        {
            SCB_1_UartSaveConfig();
        }
    #endif /* (!SCB_1_CY_SCBIP_V1_I2C_ONLY) */
        else
        {
            /* Unknown mode: do nothing */
        }
    }
    else
    {
        SCB_1_backup.enableState = (uint8) SCB_1_GET_CTRL_ENABLED;

        if(0u != SCB_1_backup.enableState)
        {
            SCB_1_Stop();
        }
    }

#else

    #if (SCB_1_SCB_MODE_I2C_CONST_CFG && SCB_1_I2C_WAKE_ENABLE_CONST)
        SCB_1_I2CSaveConfig();

    #elif (SCB_1_SCB_MODE_EZI2C_CONST_CFG && SCB_1_EZI2C_WAKE_ENABLE_CONST)
        SCB_1_EzI2CSaveConfig();

    #elif (SCB_1_SCB_MODE_SPI_CONST_CFG && SCB_1_SPI_WAKE_ENABLE_CONST)
        SCB_1_SpiSaveConfig();

    #elif (SCB_1_SCB_MODE_UART_CONST_CFG && SCB_1_UART_WAKE_ENABLE_CONST)
        SCB_1_UartSaveConfig();

    #else

        SCB_1_backup.enableState = (uint8) SCB_1_GET_CTRL_ENABLED;

        if(0u != SCB_1_backup.enableState)
        {
            SCB_1_Stop();
        }

    #endif /* defined (SCB_1_SCB_MODE_I2C_CONST_CFG) && (SCB_1_I2C_WAKE_ENABLE_CONST) */

#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SCB_1_Wakeup
********************************************************************************
*
* Summary:
*  Prepares the component for the Active mode operation after exiting Deep Sleep.
*  The “Enable wakeup from Sleep Mode” option has an influence on this function
*  implementation.
*  This function should not be called after exiting Sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SCB_1_Wakeup(void)
{
#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)

    if(SCB_1_SCB_WAKE_ENABLE_CHECK)
    {
        if(SCB_1_SCB_MODE_I2C_RUNTM_CFG)
        {
            SCB_1_I2CRestoreConfig();
        }
        else if(SCB_1_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            SCB_1_EzI2CRestoreConfig();
        }
    #if(!SCB_1_CY_SCBIP_V1_I2C_ONLY)
        else if(SCB_1_SCB_MODE_SPI_RUNTM_CFG)
        {
            SCB_1_SpiRestoreConfig();
        }
        else if(SCB_1_SCB_MODE_UART_RUNTM_CFG)
        {
            SCB_1_UartRestoreConfig();
        }
    #endif /* (!SCB_1_CY_SCBIP_V1_I2C_ONLY) */
        else
        {
            /* Unknown mode: do nothing */
        }
    }
    else
    {
        if(0u != SCB_1_backup.enableState)
        {
            SCB_1_Enable();
        }
    }

#else

    #if (SCB_1_SCB_MODE_I2C_CONST_CFG  && SCB_1_I2C_WAKE_ENABLE_CONST)
        SCB_1_I2CRestoreConfig();

    #elif (SCB_1_SCB_MODE_EZI2C_CONST_CFG && SCB_1_EZI2C_WAKE_ENABLE_CONST)
        SCB_1_EzI2CRestoreConfig();

    #elif (SCB_1_SCB_MODE_SPI_CONST_CFG && SCB_1_SPI_WAKE_ENABLE_CONST)
        SCB_1_SpiRestoreConfig();

    #elif (SCB_1_SCB_MODE_UART_CONST_CFG && SCB_1_UART_WAKE_ENABLE_CONST)
        SCB_1_UartRestoreConfig();

    #else

        if(0u != SCB_1_backup.enableState)
        {
            SCB_1_Enable();
        }

    #endif /* (SCB_1_I2C_WAKE_ENABLE_CONST) */

#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/* [] END OF FILE */
