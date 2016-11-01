/*******************************************************************************
* File Name: SCB_1_scl.h  
* Version 2.0
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_SCB_1_scl_H) /* Pins SCB_1_scl_H */
#define CY_PINS_SCB_1_scl_H

#include "cytypes.h"
#include "cyfitter.h"
#include "SCB_1_scl_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    SCB_1_scl_Write(uint8 value) ;
void    SCB_1_scl_SetDriveMode(uint8 mode) ;
uint8   SCB_1_scl_ReadDataReg(void) ;
uint8   SCB_1_scl_Read(void) ;
uint8   SCB_1_scl_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define SCB_1_scl_DRIVE_MODE_BITS        (3)
#define SCB_1_scl_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - SCB_1_scl_DRIVE_MODE_BITS))
#define SCB_1_scl_DRIVE_MODE_SHIFT       (0x00u)
#define SCB_1_scl_DRIVE_MODE_MASK        (0x07u << SCB_1_scl_DRIVE_MODE_SHIFT)

#define SCB_1_scl_DM_ALG_HIZ         (0x00u << SCB_1_scl_DRIVE_MODE_SHIFT)
#define SCB_1_scl_DM_DIG_HIZ         (0x01u << SCB_1_scl_DRIVE_MODE_SHIFT)
#define SCB_1_scl_DM_RES_UP          (0x02u << SCB_1_scl_DRIVE_MODE_SHIFT)
#define SCB_1_scl_DM_RES_DWN         (0x03u << SCB_1_scl_DRIVE_MODE_SHIFT)
#define SCB_1_scl_DM_OD_LO           (0x04u << SCB_1_scl_DRIVE_MODE_SHIFT)
#define SCB_1_scl_DM_OD_HI           (0x05u << SCB_1_scl_DRIVE_MODE_SHIFT)
#define SCB_1_scl_DM_STRONG          (0x06u << SCB_1_scl_DRIVE_MODE_SHIFT)
#define SCB_1_scl_DM_RES_UPDWN       (0x07u << SCB_1_scl_DRIVE_MODE_SHIFT)

/* Digital Port Constants */
#define SCB_1_scl_MASK               SCB_1_scl__MASK
#define SCB_1_scl_SHIFT              SCB_1_scl__SHIFT
#define SCB_1_scl_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SCB_1_scl_PS                     (* (reg32 *) SCB_1_scl__PS)
/* Port Configuration */
#define SCB_1_scl_PC                     (* (reg32 *) SCB_1_scl__PC)
/* Data Register */
#define SCB_1_scl_DR                     (* (reg32 *) SCB_1_scl__DR)
/* Input Buffer Disable Override */
#define SCB_1_scl_INP_DIS                (* (reg32 *) SCB_1_scl__PC2)


#if defined(SCB_1_scl__INTSTAT)  /* Interrupt Registers */

    #define SCB_1_scl_INTSTAT                (* (reg32 *) SCB_1_scl__INTSTAT)

#endif /* Interrupt Registers */

#endif /* End Pins SCB_1_scl_H */


/* [] END OF FILE */
