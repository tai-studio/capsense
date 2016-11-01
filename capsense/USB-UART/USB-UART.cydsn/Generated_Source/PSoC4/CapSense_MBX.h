/*******************************************************************************
* File Name: CapSense_MBX.h
* Version 2.0
*
* Description:
*  This file provides constants and structure declarations for the tuner 
*  communication for the CapSense CSD component.
*
* Note:
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CAPSENSE_CSD_MBX_CapSense_H)
#define CY_CAPSENSE_CSD_MBX_CapSense_H

#include "CapSense.h"
#include "CapSense_PVT.h"
#include "CapSense_CSHL.h"


/***************************************
*   Condition compilation parameters
***************************************/

/* Check Sum to identify data */
#define CapSense_CHECK_SUM      (3706u)

#define CapSense_TOTAL_SENSOR_MASK_COUNT     (((CapSense_TOTAL_SENSOR_COUNT - 1u) / 8u) + 1u)

#if defined(__GNUC__) || defined(__ARMCC_VERSION)
    #define CapSense_PACKED_STRUCT_GNUC_ARMCC __attribute__ ((packed))
#else
    #define CapSense_PACKED_STRUCT_GNUC_ARMCC
#endif /* (defined(__GNUC__) || defined(__ARMCC_VERSION)) */

#if defined(__ICCARM__)
	#define CapSense_PACKED_STRUCT_IAR __packed
#else
	#define CapSense_PACKED_STRUCT_IAR
#endif /* (__ICCARM__) */	

#if defined(__GNUC__)
	#define CapSense_NO_STRICT_VOLATILE __attribute__((optimize("-fno-strict-volatile-bitfields")))
#else
	#define CapSense_NO_STRICT_VOLATILE
#endif /* (defined(__GNUC__)) */

#define CapSense_WIDGET_CSHL_PARAMETERS_COUNT           (CapSense_TOTAL_WIDGET_COUNT + \
                                                                 CapSense_TOTAL_TOUCH_PADS_COUNT + \
                                                                 CapSense_TOTAL_MATRIX_BUTTONS_COUNT)

#define CapSense_WIDGET_RESOLUTION_PARAMETERS_COUNT     (CapSense_WIDGET_CSHL_PARAMETERS_COUNT + \
                                                                 CapSense_TOTAL_GENERICS_COUNT)


/***************************************
*      Type defines for mailboxes
***************************************/

/* Outbox structure definition */
typedef CapSense_PACKED_STRUCT_IAR struct
{
    #if (CapSense_TOTAL_CENTROIDS_COUNT)
        uint16   position[CapSense_TOTAL_CENTROIDS_COUNT];
    #endif  /* (CapSense_TOTAL_CENTROIDS_COUNT) */

    #if (CapSense_TOTAL_MATRIX_BUTTONS_COUNT)
        uint8   mb_position[CapSense_TOTAL_MATRIX_BUTTONS_COUNT * 2u];
    #endif /* (CapSense_TOTAL_MATRIX_BUTTONS_COUNT) */

    uint16   rawData[CapSense_TOTAL_SENSOR_COUNT];
    uint16   baseLine[CapSense_TOTAL_SENSOR_COUNT];

	uint8 signal[CapSense_TOTAL_SENSOR_COUNT];

    uint8    onMask[CapSense_TOTAL_SENSOR_MASK_COUNT];
    
    #if (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO)
		uint8   fingerThreshold[CapSense_WIDGET_CSHL_PARAMETERS_COUNT];
		uint8   noiseThreshold[CapSense_WIDGET_CSHL_PARAMETERS_COUNT];

        uint8   scanResolution[CapSense_WIDGET_RESOLUTION_PARAMETERS_COUNT];
        uint8   idac1Value[CapSense_TOTAL_SENSOR_COUNT];
        uint8   idac2Value[CapSense_TOTAL_SENSOR_COUNT];
		uint8   analogSwitchDivider[CapSense_TOTAL_SCANSLOT_COUNT];
        uint8   modulatorDivider[CapSense_TOTAL_SCANSLOT_COUNT];
    #endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO) */

    #if(CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL)
        uint8   noReadMsg;
    #endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL) */
    uint16  checkSum;
} CapSense_PACKED_STRUCT_GNUC_ARMCC CapSense_OUTBOX;

/* Inbox structure definition */
#if (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL)
    typedef CapSense_PACKED_STRUCT_IAR struct
    {
        uint8   sensorIndex;
        uint8   idac1Settings;
        #if (CapSense_IDAC_CNT == 2u)
            uint8   idac2Settings;
        #endif /* ( (CapSense_IDAC_CNT == 2u) */

        uint8   resolution;
		uint8   fingerThreshold;
		uint8   noiseThreshold;
		uint8   hysteresis;

        uint8   debounce;
        uint8   analogSwitchDivider;
		uint8   modulatorDivider;
    } CapSense_PACKED_STRUCT_GNUC_ARMCC CapSense_INBOX;
    
#endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL) */
/* Mailbox structure definition */
typedef CapSense_PACKED_STRUCT_IAR struct
{
    uint8   type;               /* Must be first byte with values ranging from 0-119 */
    uint16  size;               /* Must be size of this data structure. Range between 0-127 */
    CapSense_OUTBOX  outbox;
    #if (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL)
        CapSense_INBOX inbox;
    #endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL) */
    
}CapSense_PACKED_STRUCT_GNUC_ARMCC CapSense_MAILBOX;


/* Mailboxes structure definition */
typedef CapSense_PACKED_STRUCT_IAR struct
{
    uint8   numMailBoxes;       /* This must always be first. Represents # of mailboxes */
    CapSense_MAILBOX    csdMailbox;
    
}CapSense_PACKED_STRUCT_GNUC_ARMCC CapSense_MAILBOXES;

/***************************************
*        Function Prototypes
***************************************/

CapSense_NO_STRICT_VOLATILE void CapSense_InitMailbox(volatile CapSense_MAILBOX *mbx);
CapSense_NO_STRICT_VOLATILE void CapSense_PostMessage(volatile CapSense_MAILBOX *mbx);

#if (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL)
    CapSense_NO_STRICT_VOLATILE void CapSense_ReadMessage(volatile CapSense_MAILBOX *mbx);
#endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL) */


/***************************************
*           API Constants        
***************************************/

/* Selected ID for this version of Tuner */
#define CapSense_TYPE_ID        (0x04u)

#define CapSense_BUSY_FLAG      (CapSense_TYPE_ID | 0x80u)
#define CapSense_HAVE_MSG       (sizeof(CapSense_MAILBOX) | 0x8000u)

extern uint8 CapSense_modulationIDAC[CapSense_TOTAL_SENSOR_COUNT];

#if (CapSense_IDAC_CNT == 2u)
    extern uint8 CapSense_compensationIDAC[CapSense_TOTAL_SENSOR_COUNT];
#endif /* (CapSense_IDAC_CNT == 2u) */

extern uint32 CapSense_widgetResolution[CapSense_RESOLUTIONS_TBL_SIZE];

#if (CapSense_TUNING_METHOD == CapSense__TUNING_MANUAL)
	extern uint8 CapSense_fingerThreshold[CapSense_THRESHOLD_TBL_SIZE];
	extern uint8 CapSense_noiseThreshold[CapSense_THRESHOLD_TBL_SIZE];
	extern uint8 CapSense_hysteresis[CapSense_WIDGET_PARAM_TBL_SIZE];
    extern uint8 CapSense_debounce[CapSense_WIDGET_PARAM_TBL_SIZE];
    
#elif (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO)
	extern uint8 CapSense_fingerThreshold[CapSense_THRESHOLD_TBL_SIZE];
	extern uint8 CapSense_noiseThreshold[CapSense_THRESHOLD_TBL_SIZE];
    extern uint8 CapSense_GetPrescaler(void);
#else
    /* No tuning */
#endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO) */

#if ( (CapSense_MULTIPLE_FREQUENCY_SET ) || \
      (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO) )
            extern uint8 CapSense_senseClkDividerVal[CapSense_TOTAL_SCANSLOT_COUNT];
			#if (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO)
				extern uint8 CapSense_sampleClkDividerVal;
			#else
				extern uint8 CapSense_sampleClkDividerVal[CapSense_TOTAL_SCANSLOT_COUNT];
			#endif  /* (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO) */
#else
            extern uint8 CapSense_senseClkDividerVal;
            extern uint8 CapSense_sampleClkDividerVal;
#endif /* ( (CapSense_MULTIPLE_FREQUENCY_SET) || \
       *    (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO) )
       */


#endif  /* (CY_CAPSENSE_CSD_MBX_CapSense_H) */


/* [] END OF FILE */
