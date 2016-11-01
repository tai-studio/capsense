/*******************************************************************************
* File Name: CapSense_TunerHelper.c
* Version 2.0
*
* Description:
*  This file provides the source code of the Tuner helper APIs for the CapSense CSD 
*  component.
*
* Note:
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CapSense_TunerHelper.h"

#if (CapSense_TUNER_API_GENERATE)
    #include "SCB_1.h"
    #include "SCB_1_EZI2C.h"
    volatile CapSense_MAILBOXES CapSense_mailboxesComm;
    extern uint8 CapSense_sensorOnMask[(((CapSense_TOTAL_SENSOR_COUNT - 1u) / 8u) + 1u)];
#endif  /* (CapSense_TUNER_API_GENERATE) */


/*******************************************************************************
* Function Name: CapSense_TunerStart
********************************************************************************
*
* Summary:
*  Initializes the CapSense CSD component and EzI2C communication component to use
*  a mailbox data structure for communication with the Tuner GUI.
*  Start the scanning, after initialization is complete.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  None
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_TunerStart(void)
{
    #if (CapSense_TUNER_API_GENERATE)
        
        /* Init mbx and quick check */
        CapSense_InitMailbox(&CapSense_mailboxesComm.csdMailbox);
        CapSense_mailboxesComm.numMailBoxes = CapSense_DEFAULT_MAILBOXES_NUMBER;
        
        /* Start CapSense and baselines */
        CapSense_Start();
        
        /* Initialize baselines */ 
        CapSense_InitializeEnabledBaselines();
        CapSense_InitializeEnabledBaselines();
        
        /* Start EzI2C, clears buf pointers */
        SCB_1_Start();
        
        /* Setup EzI2C buffers */
        SCB_1_EzI2CSetBuffer1(sizeof(CapSense_mailboxesComm), sizeof(CapSense_mailboxesComm),
                                        (volatile uint8 *) &CapSense_mailboxesComm);
        
        /* Starts scan all enabled sensors */
        CapSense_ScanEnabledWidgets();
    
    #endif  /* (CapSense_TUNER_API_GENERATE) */
}


/*******************************************************************************
* Function Name: CapSense_TunerComm
********************************************************************************
*
* Summary:
*  This function is blocking. It waits till scanning loop is completed and applies
*  the new parameters from the Tuner GUI if available (the manual tuning mode only). Updates
*  the enabled baselines and state of widgets. Waits while the Tuner GUI reports that 
*  the mailbox content  could be modified. Then loads the report data into the outbox 
*  and sets a busy flag. Starts a new scanning loop.
*  
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  None
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_TunerComm(void)
{
    #if (CapSense_TUNER_API_GENERATE)
        if (0u == CapSense_IsBusy())
        {   
            /* Apply new settings */
            #if (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL)
                CapSense_ReadMessage(&CapSense_mailboxesComm.csdMailbox);
            #endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL) */

            /* Update all baselines and process all widgets */
            CapSense_UpdateEnabledBaselines();
            CapSense_ProcessAllWidgets(&CapSense_mailboxesComm.csdMailbox.outbox);
            CapSense_PostMessage(&CapSense_mailboxesComm.csdMailbox);

            /* Enable EZI2C interrupts, after scan complete */
            SCB_1_EnableInt();

            while((CapSense_mailboxesComm.csdMailbox.type != CapSense_TYPE_ID) || \
			      (0u != (SCB_1_EzI2CGetActivity() & SCB_1_EZI2C_STATUS_BUSY))){;}
            
            /* Disable EZI2C interrupts, while scanning */
            SCB_1_DisableInt();
            
            /* Start scan all sensors */
            CapSense_ScanEnabledWidgets();
        }
    #endif /* (CapSense_TUNER_API_GENERATE) */

}


#if (CapSense_TUNER_API_GENERATE)
    /*******************************************************************************
    * Function Name: CapSense_ProcessAllWidgets
    ********************************************************************************
    *
    * Summary:
    *  Calls the required functions to update all the widgets' state:
    *   - CapSense_GetCentroidPos() - calls only if linear sliders are
    *     available.
    *   - CapSense_GetRadialCentroidPos() - calls only if the  radial slider is
    *     available.
    *   - CapSense_GetTouchCentroidPos() - calls only if the  touchpad slider 
    *     available.
    *   - CapSense_CheckIsAnyWidgetActive();
    *  The results of operations are copied to OUTBOX.
    *   
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CapSense_OUTBOX outbox - the structure which is used as an output 
    *  buffer to report data to the Tuner GUI.
    *  Update fields:
    *    - position[];
    *    - OnMask[];
	*
	* Global Variables:
	*  None
	*
	* Side Effects:
	*  None
	*
    *******************************************************************************/
    void CapSense_ProcessAllWidgets(volatile CapSense_OUTBOX *outbox)
    {
        uint8 i = 0u;
		#if (CapSense_TOTAL_MATRIX_BUTTONS_COUNT)
        	uint8 mbPosition[2u];
		#endif /* (CapSense_TOTAL_MATRIX_BUTTONS_COUNT) */

        #if (CapSense_TOTAL_TOUCH_PADS_COUNT)
            uint16 pos[2];
        #endif  /* (CapSense_TOTAL_TOUCH_PADS_COUNT) */
        
        #if ( (CapSense_TOTAL_RADIAL_SLIDERS_COUNT) || (CapSense_TOTAL_TOUCH_PADS_COUNT) || \
              (CapSense_TOTAL_MATRIX_BUTTONS_COUNT) )
            uint8 widgetCnt;
        #endif  /* ((CapSense_TOTAL_RADIAL_SLIDERS_COUNT) || (CapSense_TOTAL_TOUCH_PADS_COUNT)) || 
                *   (CapSense_TOTAL_MATRIX_BUTTONS_COUNT)
                */
        
        /* Calculate widget with centroids */
        #if (CapSense_TOTAL_LINEAR_SLIDERS_COUNT)
            for(; i < CapSense_TOTAL_LINEAR_SLIDERS_COUNT; i++)
            {
                outbox->position[i] = (uint16)CapSense_GetCentroidPos((uint32)i);
            }
        #endif /* (CapSense_TOTAL_LINEAR_SLIDERS_COUNT) */
        
        #if (CapSense_TOTAL_RADIAL_SLIDERS_COUNT)
            widgetCnt = i;
            for(; i < (widgetCnt + CapSense_TOTAL_RADIAL_SLIDERS_COUNT); i++)
            {
                outbox->position[i] = (uint16)CapSense_GetRadialCentroidPos((uint32)i);
            }
        #endif /* (CapSense_TOTAL_RADIAL_SLIDERS_COUNT) */
        
        #if (CapSense_TOTAL_TOUCH_PADS_COUNT)
            widgetCnt = i;
            for(; i < (widgetCnt + (CapSense_TOTAL_TOUCH_PADS_COUNT * 2u)); i=i+2u)
            {
                if(CapSense_GetTouchCentroidPos((uint32)i, pos) == 0u)
                {
                    outbox->position[i] = 0xFFFFu;
                    outbox->position[i+1u] = 0xFFFFu;
                }
                else
                {
                    outbox->position[i] = (uint16) pos[0u];
                    outbox->position[i+1u] =(uint16) pos[1u];
                }
            }
        #endif /* (CapSense_TOTAL_TOUCH_PADS_COUNT) */

        #if (CapSense_TOTAL_MATRIX_BUTTONS_COUNT)
            i += CapSense_TOTAL_BUTTONS_COUNT;
            widgetCnt = 0u;
            for(; widgetCnt < (CapSense_TOTAL_MATRIX_BUTTONS_COUNT * 2u); widgetCnt += 2u)
            {
                if(CapSense_GetMatrixButtonPos((uint32)i, mbPosition) == 0u)
                {
                    outbox->mb_position[widgetCnt] = 0xFFu;
                    outbox->mb_position[widgetCnt+1u] = 0xFFu;
                }
				else
				{
					outbox->mb_position[widgetCnt] = mbPosition[0u];
					outbox->mb_position[widgetCnt+1u] = mbPosition[1u];
				}
                i += 2u;
            }
        #endif /* (CapSense_TOTAL_MATRIX_BUTTONS_COUNT) */

        /* Update On/Off State */
        (void)CapSense_CheckIsAnyWidgetActive();

        /* Copy OnMask */
        for(i=0u; i < CapSense_TOTAL_SENSOR_MASK_COUNT; i++)
        {
            outbox->onMask[i]  = CapSense_sensorOnMask[i];
        }
    }


#endif /* (CapSense_TUNER_API_GENERATE) */


/* [] END OF FILE */
