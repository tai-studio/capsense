/*******************************************************************************
* File Name: CapSense_SMS.c
* Version 2.0
*
* Description:
*  This file provides the source code of wrapper between CapSense CSD component 
*  and Auto Tuning library.
*
* Note:
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CapSense_SMS.h"
#include "CapSense_PVT.h"

#include "cytypes.h"

#define CapSense_PRS_CORRECTION_DIVIDER (4u)
#define CapSense_MAX_RESOLUTION         (65535u)
#define CapSense_SENSITIVITY_DIVIDER    (10u)

#if (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO)

#if (0u != CapSense_IS_M0S8PERI_BLOCK)
    /*  Dividers are not chained */ 
	#if(CapSense_PRS_OPTIONS != CapSense__PRS_NONE)
		#define CapSense_SENSETIVITY_FACTOR (1228u)
	#else
		#define CapSense_SENSETIVITY_FACTOR (2456u)
	#endif /* (CapSense_PRS_OPTIONS == CapSense__PRS_NONE) */
#else
    /*  Dividers are chained */
	#if(CapSense_PRS_OPTIONS != CapSense__PRS_NONE)
		#define CapSense_SENSETIVITY_FACTOR (1228u/CapSense_CALIBRATION_MD)
	#else
		#define CapSense_SENSETIVITY_FACTOR (2456u/CapSense_CALIBRATION_MD)
	#endif /* (CapSense_PRS_OPTIONS == CapSense__PRS_NONE) */
#endif /* (0u != CapSense_IS_M0S8PERI_BLOCK) */


#if(CapSense_PRS_OPTIONS != CapSense__PRS_NONE)
	extern uint8 CapSense_prescalersTuningDone;
#endif /* (CapSense_PRS_OPTIONS == CapSense__PRS_NONE) */


/*  Real Charge Divider values */
#if (0u != CapSense_IS_M0S8PERI_BLOCK)
    /*  Dividers are not chained */
	#if (CYDEV_BCLK__HFCLK__MHZ >= 12u)
		const uint8 prescalerTable[] = {
			1u,     1u,     1u,    2u,    2u,    2u,    2u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u
		};

	#elif (CYDEV_BCLK__HFCLK__MHZ >= 6u)
		const uint8 prescalerTable[] = {
			1u,     1u,     1u,    1u,    1u,    1u,    1u,    2u,    2u,    2u,    2u,    2u,    2u,    2u,    2u,    2u
		};
	#else
		const uint8 prescalerTable[] = {
			1u,     1u,     1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u
		};
	#endif /* (CYDEV_BCLK__HFCLK__MHZ > 12u) */
#else
    /*  Dividers are chained */ 
	#if (CYDEV_BCLK__HFCLK__MHZ > 24u)
		const uint8 prescalerTable[] = {
			2u,     2u,     2u,    4u,    4u,    4u,    4u,    8u,    8u,    8u,    8u,    8u,    8u,    8u,    8u,    8u
		};

	#elif (CYDEV_BCLK__HFCLK__MHZ >12u)
		const uint8 prescalerTable[] = {
			1u,     1u,     1u,    2u,    2u,    2u,    2u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u
		};
	#else   /* (CYDEV_BCLK__HFCLK__MHZ > 12u) */
		const uint8 prescalerTable[] = {
			1u,     1u,     1u,    1u,    1u,    1u,    1u,    2u,    2u,    2u,    2u,    2u,    2u,    2u,    2u,    2u
		};
	#endif /* (CYDEV_BCLK__HFCLK__MHZ > 24u) */
#endif /* (0u != CapSense_IS_M0S8PERI_BLOCK) */


extern uint16 CapSense_sensorRaw[CapSense_TOTAL_SENSOR_COUNT];
extern uint8 CapSense_sensorSignal[CapSense_TOTAL_SENSOR_COUNT];

extern uint32 CapSense_widgetResolution[CapSense_RESOLUTIONS_TBL_SIZE];
extern const uint8 CapSense_widgetNumber[CapSense_TOTAL_SENSOR_COUNT];

extern const uint8 CapSense_rawDataIndex[CapSense_WIDGETS_TBL_SIZE];
extern const uint8 CapSense_numberOfSensors[CapSense_WIDGETS_TBL_SIZE];

extern uint8 CapSense_modulationIDAC[CapSense_TOTAL_SENSOR_COUNT];
extern uint8 CapSense_compensationIDAC[CapSense_TOTAL_SENSOR_COUNT];

extern uint8 CapSense_senseClkDividerVal[CapSense_TOTAL_SCANSLOT_COUNT];
extern uint8 CapSense_sampleClkDividerVal[CapSense_TOTAL_SCANSLOT_COUNT];

extern uint8 CapSense_fingerThreshold[CapSense_THRESHOLD_TBL_SIZE];
extern uint8 CapSense_noiseThreshold[CapSense_THRESHOLD_TBL_SIZE];
extern uint8 CapSense_hysteresis[CapSense_WIDGET_PARAM_TBL_SIZE];
extern uint8 CapSense_negativeNoiseThreshold[CapSense_THRESHOLD_TBL_SIZE];

uint8 noiseEnvelopeTbl[CapSense_TOTAL_SENSOR_COUNT];
uint8 runningDifferenceTbl[CapSense_TOTAL_SENSOR_COUNT];
uint8 signRegisterTbl[CapSense_TOTAL_SENSOR_COUNT];
uint16 sampleMinTbl[CapSense_TOTAL_SENSOR_COUNT];
uint16 sampleMaxTbl[CapSense_TOTAL_SENSOR_COUNT];
uint16 previousSampleTbl[CapSense_TOTAL_SENSOR_COUNT];
uint8 kValueTbl[CapSense_TOTAL_SENSOR_COUNT];   
uint8 scanSpeedTbl[((CapSense_TOTAL_SENSOR_COUNT - 1u) / 8u) + 1u];

uint8 SMS_LIB_SensorSensitivity[] = {
    2u, 2u, 2u, 2u, 
};

CapSense_CONFIG_TYPE_P4_v2_0 CapSense_config;
CapSense_CONFIG_TYPE_POINTERS_P4_v2_0 const CapSense_configPointers = {
    CapSense_modulationIDAC,
    CapSense_compensationIDAC,
     SMS_LIB_SensorSensitivity,
    CapSense_senseClkDividerVal,
    CapSense_sampleClkDividerVal,
    CapSense_widgetNumber,
    CapSense_widgetResolution,
	(const uint8  *)CapSense_rawDataIndex,
	(const uint8  *)CapSense_numberOfSensors,
    &CapSense_GetSensorRaw,
    &CapSense_PreScan,
    &CapSense_ReadSensorRaw,
    &CapSense_GetBitValue,
	&CapSense_SetBitValue
};


/*******************************************************************************
* Function Name: CapSense_AutoTune
********************************************************************************
*
* Summary:
*  Provides the tuning procedure for all sensors.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*   CapSense_config: the structure which contains configuration parameters
*   CapSense_senseClkDividerVal[]: used to store the Analog Switch 
*   dividers for each sensor.
*   CapSense_senseClkDividerVal[]: used to store the 
*    Analog Switch divider for all sensors.
*   CapSense_prescalersTuningDone - used to notify the Tuner GUI that 
*   the pre-scalers tuning  is done.
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_AutoTune(void)
{
	uint32 curSensor;
    uint32 widget;

	CapSense_config.totalScanslotsNum = CapSense_TOTAL_SCANSLOT_COUNT;
	CapSense_config.totalWidgetsNum = CapSense_END_OF_WIDGETS_INDEX;
	CapSense_config.totalSensorsNum = CapSense_TOTAL_SENSOR_COUNT;
	
	CapSense_config.hfclkFreq_MHz = CYDEV_BCLK__HFCLK__MHZ;
	CapSense_config.sensetivitySeed = CapSense_SENSETIVITY_FACTOR;
	
    CapSense_config.pointers = &CapSense_configPointers;       
    
	CapSense_SetAnalogSwitchesSrcDirect();
    
    for(widget = 0u; widget < CapSense_config.totalWidgetsNum; widget++)
    {
        CapSense_widgetResolution[widget] = CapSense_CALIBRATION_RESOLUTION;
    }
	

	for(curSensor = 0u; curSensor < CapSense_TOTAL_SCANSLOT_COUNT; curSensor++)
    {
		CapSense_senseClkDividerVal[curSensor] = CapSense_CALIBRATION_ASD;
		CapSense_sampleClkDividerVal[curSensor] = CapSense_CALIBRATION_MD;
	}
    
    CapSense_DisableBaselineIDAC();

    CalibrateSensors_P4_v2_0(&CapSense_config, CapSense_CAL_RAW_COUNT);
	CapSense_NormalizeWidgets(CapSense_END_OF_WIDGETS_INDEX, CapSense_modulationIDAC);
	
    TunePrescalers_P4_v2_0(&CapSense_config);
	
	#if(CapSense_PRS_OPTIONS != CapSense__PRS_NONE)
		CapSense_prescalersTuningDone = 1u;
	#endif /* (CapSense_PRS_OPTIONS == CapSense__PRS_NONE) */
       
    CalibrateSensors_P4_v2_0(&CapSense_config, CapSense_CAL_RAW_COUNT);
	CapSense_NormalizeWidgets(CapSense_END_OF_WIDGETS_INDEX, CapSense_modulationIDAC);
    TuneSensitivity_P4_v2_0(&CapSense_config);
	
	CapSense_EnableBaselineIDAC(&CapSense_config);
    
    for(curSensor = 0u; curSensor < CapSense_TOTAL_SCANSLOT_COUNT; curSensor++)
    {
        CapSense_UpdateThresholds(curSensor);
    }
}


/*******************************************************************************
* Function Name: CapSense_UpdateThresholds
********************************************************************************
*
* Summary:
*  The API updates the Finger Threshold, Hysteresis, Noise Threshold, and 
*  Negative Noise Threshold in the AutoTuning (Smartsense) Mode.
*
* Parameters:
*  sensor: sensor number.
*
* Return:
*  None
*
* Global Variables:
*  CapSense_widgetNumber[] - contains widget Number for given sensor.
*  0 through 7 (sensor 0 is bit 0, sensor 1 is bit 1).
*  For other sensors the array element equals to 255. 
*  CapSense_numberOfSensors[widget] - Number of sensors in the widget.
*   CapSense_fingerThreshold[] - contains the level of signal for each sensor
*   that determines if a finger present on the sensor.
*   CapSense_negativeNoiseThreshold[] - negative Noise Threshold
*   CapSense_hysteresis[] - the array with hysteresis values.
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_UpdateThresholds(uint32 sensor)
{
	uint8 fingerThreshold;
	uint32 widget;
	
	widget = CapSense_widgetNumber[sensor];

	if(0u != CapSense_FLEXIBLE_THRESHOLDS_EN)
	{
	    fingerThreshold = CapSense_fingerThreshold[widget];
        /* Update Threshold manually (flexible) */   
		CapSense_noiseThreshold[widget] = (uint8)(fingerThreshold >> 1u);
		CapSense_negativeNoiseThreshold[widget] = (uint8)(fingerThreshold >> 1u);
		CapSense_hysteresis[widget] = (uint8)(fingerThreshold >> 3u);
	}
	else
	{
		/* Calculate Finger Threshold and Noise Threshold with Smartsense (automatic) */ 	
		CalculateThresholds_P4_v2_0(&CapSense_config, (uint8)sensor, CapSense_sensorRaw, (uint8)widget, CapSense_fingerThreshold, CapSense_noiseThreshold);
		
	    fingerThreshold = CapSense_fingerThreshold[widget];
		
	    /* Update Threshold based on calculated with Smartsense (automatic) */
		CapSense_negativeNoiseThreshold[widget] = CapSense_noiseThreshold[widget];
        
		if(fingerThreshold < CapSense_THRESHOLD_1)
		{
			CapSense_hysteresis[widget] = (uint8)(fingerThreshold >> 3u);
		}
		else if(fingerThreshold <  CapSense_THRESHOLD_2)
		{
			CapSense_hysteresis[widget] = (uint8)(fingerThreshold >> 4u);
		}
		else if(fingerThreshold <  CapSense_THRESHOLD_3)
		{
			CapSense_hysteresis[widget] = (uint8)(fingerThreshold >> 5u);
		}
		else if(fingerThreshold <  CapSense_THRESHOLD_4)
		{
			CapSense_hysteresis[widget] = (uint8)(fingerThreshold >> 6u);
		}
		else
		{
			CapSense_hysteresis[widget] = 0u;
		}
	}
}


/*******************************************************************************
* Function Name: CapSense_NormalizeWidgets
********************************************************************************
*
* Summary:
*  This API aligns all the parameters of the widget to the maximum parameter.
*
* Parameters:
*  uint32 widgetsNum: Number of widgets.
*  uint8 *datsPrt: pointer ro array with widget parameters.	
*
* Return:
*  None
*
* Global Variables:
*  CapSense_numberOfSensors[] - Number of sensors in the widget.
*  CapSense_rawDataIndex[currentWidget] - Contains the  1st sensor
*  position in the widget.
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_NormalizeWidgets(uint32 widgetsNum, uint8 *dataPtr)
{
	uint32 currentWidget;
	uint32 currentSensor;
	uint32 lastSensor;
	uint32 sensorsPerWidget;
	uint32 maxPerWidget;

	for(currentWidget = 0u; currentWidget < widgetsNum; currentWidget++)
	{
		maxPerWidget = 0u;
		
		sensorsPerWidget = CapSense_numberOfSensors[currentWidget];
		currentSensor = CapSense_rawDataIndex[currentWidget];
		lastSensor = currentSensor + sensorsPerWidget;
		
		while(currentSensor < lastSensor)
		{
			if(maxPerWidget < dataPtr[currentSensor])
			{
				maxPerWidget = dataPtr[currentSensor];
			}
			currentSensor++;
		}
		
		currentSensor = CapSense_rawDataIndex[currentWidget];
		
		while(currentSensor < lastSensor)
		{
			dataPtr[currentSensor] = (uint8)maxPerWidget;
			currentSensor++;
		}
	}
}

/*******************************************************************************
* Function Name: CapSense_SetAnalogSwitchesSrcDirect
********************************************************************************
*
* Summary:
*  This API switches the charge clock source to prescaler. 
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
void CapSense_SetAnalogSwitchesSrcDirect(void)
{
	CapSense_CSD_CFG_REG &= ~(CapSense_CSD_CFG_PRS_SELECT);
}


/*******************************************************************************
* Function Name: CapSense_DisableBaselineIDAC
********************************************************************************
*
* Summary:
*  The API disables the Compensation IDAC. 
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
void CapSense_DisableBaselineIDAC(void)
{
	uint32 newRegValue;
    
    newRegValue = CapSense_CSD_IDAC_REG;
    
    newRegValue &= ~(CapSense_CSD_IDAC1_MODE_MASK | CapSense_CSD_IDAC2_MODE_MASK);
    newRegValue |= CapSense_CSD_IDAC1_MODE_VARIABLE;
    
	CapSense_CSD_IDAC_REG = newRegValue;
}


/*******************************************************************************
* Function Name: CapSense_EnableBaselineIDAC
********************************************************************************
*
* Summary:
*  The API enables the Compensation IDAC. 
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  CapSense_compensationIDAC[] - used to store a 7-bit idac value for all the sensors.
*
* Side Effects:
*  None
*
*******************************************************************************/
void  CapSense_EnableBaselineIDAC(const CapSense_CONFIG_TYPE_P4_v2_0 *config)
{
    uint32 curSensor;
    uint32 newRegValue;

    for(curSensor = 0u; curSensor < config->totalScanslotsNum; curSensor++)
    {
		CapSense_compensationIDAC[curSensor] = CapSense_modulationIDAC[curSensor] / 2u;
	    CapSense_modulationIDAC[curSensor] = (CapSense_modulationIDAC[curSensor] + 1u) / 2u;
    }
	
	CyIntDisable(CapSense_ISR_NUMBER);
	
	newRegValue = CapSense_CSD_IDAC_REG;
    newRegValue &= ~(CapSense_CSD_IDAC1_MODE_MASK | CapSense_CSD_IDAC2_MODE_MASK);
    newRegValue |= (CapSense_CSD_IDAC1_MODE_VARIABLE | CapSense_CSD_IDAC2_MODE_FIXED);
    CapSense_CSD_IDAC_REG = newRegValue;
	
	CyIntEnable(CapSense_ISR_NUMBER);
}


/*******************************************************************************
* Function Name: CapSense_SetSensitivity
********************************************************************************
*
* Summary:
*  This API sets the sensitivity value for the sensor. The sensitivity 
*  value is used during the auto-tuning algorithm executed as part of the CapSense_Start API.
*  This API is called by the application layer prior to calling the CapSense_Start API. 
*  Calling this API after execution of CapSense_Start API has no effect. 
*
* Parameters: 
*  sensor  Sensor index 
*  data    Sensitivity of the sensor. Possible values are below
*  1 - 0.1pf sensitivity
*  2 - 0.2pf sensitivity
*  3 - 0.3pf sensitivity
*  4 - 0.4pf sensitivity
*  All other values, set sensitivity to 0.4pf
*
*  Return Value:
*   None
*
* Global Variables:
* SMS_LIB_SensorSensitivity[] - This 8-bits array contains Sensitivity in the LSB nibble
*                                and Noise Reset Counter value in the MSB nibble.      
*
* Side Effects: 
*  None
*
*******************************************************************************/
void CapSense_SetSensitivity(uint32 sensor, uint32 value)
{
	if(value > 4u)
	{
		value = 4u;
	}
	
    /* Clear SensorSensitivity value in LSB nibble */
	SMS_LIB_SensorSensitivity[sensor] &= (uint8)~CapSense_SENSITIVITY_MASK;
	/* Set SensorSensitivity value in LSB nibble */
	SMS_LIB_SensorSensitivity[sensor] |= (uint8)value;
}


/*******************************************************************************
* Function Name: CapSense_GetSensitivityCoefficient
********************************************************************************
*
* Summary:
*  This API returns the K coefficient for the appropriate sensor. 
*
* Parameters: 
*  sensor:  Sensor index 
*
*  Return Value:
*   K - value for the appropriate sensor.
*
* Global Variables:
*  kValueTbl[] - This 8-bits array contains the K value for each sensor.
*
* Side Effects: 
*  None
*
*******************************************************************************/
uint8 CapSense_GetSensitivityCoefficient(uint32 sensor)
{
	return (kValueTbl[sensor]);
}


/*******************************************************************************
* Function Name: CapSense_GetNoiseEnvelope
********************************************************************************
*
* Summary:
*  This function returns the noise value of a
*  component.
*
* Parameters:
*  sensor - Sensor number.
*
* Return:
*  The noise envelope value of the sensor
*  indicated by argument.
*
* Global Variables:
*  baNoiseEnvelope[] - array with Noise Envelope.
*
* Side Effects:
*  None
*
*******************************************************************************/
uint16 CapSense_GetNoiseEnvelope(uint32 sensor)
{
    return((uint16)((uint16)noiseEnvelopeTbl[sensor] << 1u) + 1u);
}


/*******************************************************************************
* Function Name: CapSense_GetNormalizedDiffCountData
********************************************************************************
*
* Summary:
*  This API returns normalized difference count data.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  This API returns a normalized count value of the sensor indicated by the 
*  argument. 
*
* Global Variables:
*  CapSense_sensorSignal[] - array with difference counts for sensors
* 
* Side Effects:
*  None
*
*******************************************************************************/
uint16 CapSense_GetNormalizedDiffCountData(uint32 sensor)
{
	return (uint16)(((uint32)CapSense_sensorSignal[sensor] << 7u) / kValueTbl[sensor]);
}

#endif /* (CapSense_TUNING_METHOD == CapSense__TUNING_AUTO) */

/* [] END OF FILE */
