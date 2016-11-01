/*******************************************************************************
* File Name: CapSense_MBX.c
* Version 2.0
*
* Description:
*  This file provides the source code of Tuner communication APIs for the 
*  CapSense CSD component.
*
* Note:
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CapSense_MBX.h"

extern uint8 CapSense_sensorOnMask[(((CapSense_TOTAL_SENSOR_COUNT - 1u) / 8u) + 1u)];
extern uint16 CapSense_sensorBaseline[CapSense_TOTAL_SENSOR_COUNT];
extern uint8 CapSense_sensorSignal[CapSense_TOTAL_SENSOR_COUNT];
extern uint8 CapSense_sensorBaselineLow[CapSense_TOTAL_SENSOR_COUNT];

#if (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL)
    /*******************************************************************************
    * Function Name: CapSense_InitParams
    ********************************************************************************
    *
    * Summary:
    *  Configures the component parameters to match the parameters in the inbox.
    *  Used only in the manual tuning mode to apply new scanning parameters from the Tuner 
    *  GUI.
    *
    * Parameters:
    *  inbox:  Pointer to Inbox structure in RAM.
    *
    * Return:
    *  None. Contents of the structure are not modified.
    * 
    * Global Variables:
    *  CapSense_idacSettings[] - used to store the idac value for all the sensors.
    *  CapSense_fingerThreshold[] - used to store the finger threshold for all 
    *  the widgets.
    *  CapSense_noiseThreshold[] - used to store the noise threshold for all 
    *  the widgets.
    *  CapSense_hysteresis[] - used to store the finger threshold for all 
    *  the widgets.
    *  CapSense_debounce[] - used to store the finger threshold for all the 
    *  widgets.
    *  CapSense_INBOX inbox - the structure which is used as an input buffer
    *  for the parameters from the Tuner GUI. Used only in the manual tuning mode.
	*
	* Side Effects:
	*  None
	* 
    *******************************************************************************/
    static void CapSense_InitParams(volatile const CapSense_INBOX *inbox);
    static void CapSense_InitParams(volatile const CapSense_INBOX *inbox)
    {
        /* Define widget sensor belongs to */
        uint8 sensor = inbox->sensorIndex;
        uint8 widget = CapSense_widgetNumber[sensor];
        
        /* Scanning parameters */
        CapSense_modulationIDAC[sensor] = inbox->idac1Settings; 

        #if (CapSense_IDAC_CNT == 2u)
            CapSense_compensationIDAC[sensor] = inbox->idac2Settings;
        #endif /* ( CapSense_IDAC_CNT == 2u ) */

        CapSense_widgetResolution[widget] = (uint32)((uint32)inbox->resolution << 
                                                    CapSense_MSB_RESOLUTION_OFFSET);
                                                    
        CapSense_widgetResolution[widget] |= CapSense_RESOLUTION_8_BITS;                                            

        #if (CapSense_TOTAL_GENERICS_COUNT)
            /* Exclude generic widget */
            if(widget < CapSense_END_OF_WIDGETS_INDEX)
            {
        #endif  /* CapSense_TOTAL_GENERICS_COUNT */
        
            /* High level parameters */
            CapSense_fingerThreshold[widget] = inbox->fingerThreshold;
            CapSense_noiseThreshold[widget]  = inbox->noiseThreshold;
            
            CapSense_hysteresis[widget] = inbox->hysteresis;
            CapSense_debounce[widget]   = inbox->debounce;
        
        #if (CapSense_TOTAL_GENERICS_COUNT)
            /* Exclude generic widget */
            }
        #endif  /* CapSense_TOTAL_GENERICS_COUNT */

        #if ( CapSense_MULTIPLE_FREQUENCY_SET )
        
            CapSense_senseClkDividerVal[sensor] = inbox->analogSwitchDivider;
            CapSense_sampleClkDividerVal[sensor] = inbox->modulatorDivider;
        #else
            CapSense_senseClkDividerVal = inbox->analogSwitchDivider;
            CapSense_sampleClkDividerVal = inbox->modulatorDivider;
        #endif /* ( CapSense_MULTIPLE_FREQUENCY_SET ) */

        /* Re-Init baselines */
        CapSense_InitializeAllBaselines();
    }
#endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL) */


/*******************************************************************************
* Function Name: CapSense_InitMailbox
********************************************************************************
*
* Summary:
*  Initializes parameters of the mailbox structure.
*  Sets the type and size of the mailbox structure. 
*  Sets a check sum to check by the Tuner GUI and noReadMsg flag to indicate that this 
*  is the first communication packet.
*
* Parameters:
*  mbx:  Pointer to Mailbox structure in RAM.
*
* Return:
*  None. Modifies the contents of mbx and mbx->outbox.
*
* Global Variables:
*  CapSense_MAILBOXES - the structure which incorporates two fields: 
*    - numMailBoxes (number of mailboxes);
*    - CapSense_MAILBOX csdMailbox;
*  CapSense_MAILBOX - the structure which incorporates a status output and
*  input buffer fields and CapSense_OUTBOX and CapSense_INBOX.
*    - type (used as the outbox read status of the Tuner GUI);
*    - size (used as the inbox apply status of the component);
*    - CapSense_OUTBOX outbox - the structure which is used as an output 
*      buffer to report data to the Tuner GUI.
*    - CapSense_INBOX inbox - the structure which is used as an input buffer
*      for the parameters from the Tuner GUI. Used only in the manual tuning mode.
*
* Side Effects:
*  None
* 
*******************************************************************************/
void CapSense_InitMailbox(volatile CapSense_MAILBOX *mbx)
{
    /* Restore TYPE_ID (clear busy flag) to indicate "action complete" */
    mbx->type = CapSense_TYPE_ID;
    /* Restore default value - clear "have_msg" */
    mbx->size = sizeof(CapSense_MAILBOX);
    
    /* Additional fields for async and reset handling */
    #if(CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL)
        mbx->outbox.noReadMsg = 1u;
    #endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL) */
    
    mbx->outbox.checkSum = (uint16)(CapSense_CHECK_SUM);
}


/*******************************************************************************
* Function Name:  CapSense_PostMessage
********************************************************************************
*
* Summary:
*  This blocking function waits while the Tuner GUI reports that the mailbox content
*  could be modified (clears CapSense_BUSY_FLAG). Then loads the report 
*  data to the outbox and sets a busy flag.
*  In the manual tuning mode the report data is:
*    - raw data, baseline, signal;
*  In the auto tuning mode the added data is:
*    - fingerThreshold;
*    - noiseThreshold;
*    - scanResolution;
*    - idacValue;
*    - prescaler.
*
* Parameters:
*  mbx:  Pointer to Mailbox structure in RAM.
*
* Return:
*  None. Modifies the contents of mbx->outbox.
*
* Global Variables:
*  CapSense_MAILBOXES - the structure which incorporates two fields: 
*    - numMailBoxes (number of mailboxes);
*    - CapSense_MAILBOX csdMailbox;
*  CapSense_MAILBOX - the structure which incorporates the status output and
*  input buffer fields and CapSense_OUTBOX and CapSense_INBOX.
*    - type (used as the  outbox read status of the Tuner GUI);
*    - size (used as the inbox apply status of the component);
*    - CapSense_OUTBOX outbox - the structure which is used as an output 
*      buffer to report data to the Tuner GUI.
*    - CapSense_INBOX inbox - the structure which is used as an input buffer
*      for parameters from the Tuner GUI. Used only in the manual tuning mode.
*  CapSense_lowLevelTuningDone - used to notify the Tuner GUI that 
*  tuning of the scanning parameters is done. The scanning parameters in the mailbox
*  will not be updated after clearing it.
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_PostMessage(volatile CapSense_MAILBOX *mbx)
{
    uint8 i;

    /* Check busy flag */
    while (mbx->type != CapSense_TYPE_ID){;}

    /* Copy all data - Raw, Base, Signal, OnMask */
    for(i=0u; i < CapSense_TOTAL_SENSOR_COUNT; i++)
    {
        mbx->outbox.rawData[i]  = CapSense_sensorRaw[i];
        mbx->outbox.baseLine[i] = CapSense_sensorBaseline[i];
        #if (CapSense_SIGNAL_SIZE == CapSense_SIGNAL_SIZE_UINT8)
            mbx->outbox.signal[i]   = (uint8)CapSense_sensorSignal[i];
        #else
            mbx->outbox.signal[i]   = (uint16)CapSense_sensorSignal[i];
        #endif  /* (CapSense_SIGNAL_SIZE == CapSense_SIGNAL_SIZE_UINT8) */
    }
        
    /* AutoTuning - need to copy all High Level parameters */
    #if (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO)
        
        /* Parameters are changed in run time */
        for(i=0u; i < CapSense_WIDGET_CSHL_PARAMETERS_COUNT; i++)
        {
            mbx->outbox.fingerThreshold[i] = CapSense_fingerThreshold[i];
            mbx->outbox.noiseThreshold[i]  = CapSense_noiseThreshold[i];
        }
        
        /* Widget resolution, take to account TP and MB */
        for(i=0u; i < CapSense_WIDGET_RESOLUTION_PARAMETERS_COUNT; i++)
        {
            mbx->outbox.scanResolution[i]  = (uint8)(CapSense_widgetResolution[i] >> CapSense_MSB_RESOLUTION_OFFSET);
        }

        /* Copy tuned idac values */
        for(i=0u; i < CapSense_TOTAL_SENSOR_COUNT; i++)
        {
            mbx->outbox.idac1Value[i] = CapSense_modulationIDAC[i];
			#if (CapSense_IDAC_CNT == 2u)
            	mbx->outbox.idac2Value[i] = CapSense_compensationIDAC[i];
			#endif /* (CapSense_IDAC_CNT == 2u) */	
        }
            
        /* Pass parameters only once */
        CapSense_lowLevelTuningDone = 0u;
    #endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL) */

    /* Copy scan parameters */
    #if ( CapSense_TUNING_METHOD == CapSense__TUNING_AUTO )
        for(i=0u; i < CapSense_TOTAL_SCANSLOT_COUNT; i++)
        {
            mbx->outbox.analogSwitchDivider[i] = CapSense_senseClkDividerVal[i];
            mbx->outbox.modulatorDivider[i] = CapSense_sampleClkDividerVal;
        }
    #endif /* (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO) */

    /* Set busy flag */
    mbx->type = CapSense_BUSY_FLAG;
}


#if (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL)

    /*******************************************************************************
    * Function Name: CapSense_ReadMessage
    ********************************************************************************
    *
    * Summary:
    *  If CapSense_HAVE_MSG is found in the mailbox, the function initializes
    *	the component with parameters which are found in the inbox.
    *   Signal is DONE by overwriting the value in the mailbox size field.
    *  Only available in the manual tuning mode.
    *
    * Parameters:
    *  mbx:  Pointer to Mailbox structure in RAM.
    *
    * Return:
    *  None. Modifies the contents of mbx.
	*
    * Global Variables:
    *  CapSense_MAILBOXES - the structure which incorporates two fields: 
    *    - numMailBoxes (number of mailboxes);
    *    - CapSense_MAILBOX csdMailbox;
    *  CapSense_MAILBOX - the structure which incorporates the status output and
    *  input buffer fields and CapSense_OUTBOX and CapSense_INBOX.
    *    - type (used as the outbox read status of the Tuner GUI);
    *    - size (used as the inbox apply status of the component);
    *    - CapSense_OUTBOX outbox - the structure which is used as an output 
    *      buffer to report data to the Tuner GUI.
    *    - CapSense_INBOX inbox - the structure which is used as an input buffer
    *      for parameters from the Tuner GUI. Used only in the manual tuning mode.
	*
    * Side Effects: 
    *  Initializes component parameters if CapSense_HAVE_MSG is received.
    *
    *******************************************************************************/
    void CapSense_ReadMessage(volatile CapSense_MAILBOX *mbx)
    {
        volatile CapSense_INBOX *tmpInbox;
       
        /* Do we have a message waiting? */
        if ((mbx->size) == CapSense_HAVE_MSG)
        {
            tmpInbox = &(mbx->inbox);
            
            CapSense_InitParams(tmpInbox);
    
            /* Notify host/tuner that we have consumed message */
            mbx->size = sizeof(CapSense_MAILBOX);
            
            /* Default settings were changed */
            mbx->outbox.noReadMsg = 0u;
        }       
    }
#endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL) */


/* [] END OF FILE */
