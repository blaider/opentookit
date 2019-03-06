
#ifndef _BDAQADAM_H_
#define _BDAQADAM_H_

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#endif

/*!
* \file bdaqadamio.h
*
* For using IO Driver SDK, please include this file in your source code.
*/

#define IN
#define OUT

//#if !defined(_BDAQ_NO_NAMESPACE) && defined(__cplusplus)
//namespace Automation { 
//    namespace BDAQDIO {  
//#endif
typedef signed char    int8;
typedef signed short   int16;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef signed int		   int32;
typedef unsigned int	   uint32;
typedef signed long long   int64;
typedef unsigned long long uint64;
typedef void *			   HANDLE;

typedef void* HMODULE;
typedef void* HANDLE;
typedef bool   BOOL;

// **********************************************************
// Bionic DAQ types
// **********************************************************
#ifndef _BDAQ_TYPES_DEFINED
#define _BDAQ_TYPES_DEFINED

/** Module ID */
typedef enum tagModuleId{
	/** ADAM-5017 Module ID: 0x04  */   
	 MODULEID_5017    =    0x04,
	/** ADAM-5018 Module ID: 0x05  */   	 
	 MODULEID_5018    =    0x05,
	/** ADAM-5013A Module ID: 0x08 */   
	 MODULEID_5013A   =    0x08,
	/** ADAM-5013B Module ID: 0x09 */   	 
	 MODULEID_5013B   =    0x09,
	/** ADAM-5017H Module ID: 0x0C  */   
	 MODULEID_5017H   =    0x0C,
	/** ADAM-50137UH Module ID: 0x17  */   	 
	 MODULEID_5017UH  =    0x17,
	/** ADAM-5018P Module ID: 0x38  */   	 
	 MODULEID_5018P   =    0x38, //5017P
	 
	/** ADAM-5024Module ID  */   
	 MODULEID_5024    =    0x18,

	//DIO
	/** ADAM-5050 Module ID: 0x10  */   	
	 MODULEID_5050    =    0x10, //16DI/DO  
	//DI
	/** ADAM-5051 Module ID: 0x11   */  	
	 MODULEID_5051    =    0x11, //5051D 5051S
	/** ADAM-5052 Module ID: 0x0F  */  	 
	 MODULEID_5052    =    0x0F,
	/** ADAM-5053 Module ID: 0x53   */  	 
	 MODULEID_5053    =    0x53,
	//DIO
	/** ADAM-5055 Module ID: 0x15   */  	
	 MODULEID_5055    =    0x15,
	//DO
	/** ADAM-5056 Module ID: 0x12  */  
	 MODULEID_5056    =    0x12, //5056D 5056S 5056SO
	/** ADAM-5057 Module ID: 0x57   */  	 
	 MODULEID_5057    =    0x57,	
	//Relay
	/** ADAM-5060 Module ID: 0x14   */  	
	 MODULEID_5060    =    0x14,
	 /** ADAM-5068 Module ID: 0x13   */  		 
	 MODULEID_5068    =    0x13,
	 /** ADAM-5069 Module ID: 0x69   */  	
	 MODULEID_5069    =    0x69,
	//Cnt
	/** ADAM-5080 Module ID: 0x06   */  	
	 MODULEID_5080    =    0x06,
	/** ADAM-5081 Module ID: 0x81   */  		 
	 MODULEID_5081    =    0x81,

	/** Unknown Module */  		 
	 MODULEID_UNDEFINE=    0xff,
	 MODULEID_MASK    =    0x1f,
}ModuleId;

/** AI Filter Type */
typedef enum tagAiIntegrationTime{
	/**60Hz filter(Default)*/
   Hz_60 = 0,
	/**50Hz filter*/   
   Hz_50,
} AiIntegrationTime;

/** tagValueRange info */
typedef enum tagValueRange {
 /** Unknown when get, ignored when set */
   V_OMIT = -1,       
	/** +/- 15 V  */   
   V_Neg15To15 = 0,   
	/** +/- 10 V  */   
   V_Neg10To10,     
	/** +/- 5 V */   
   V_Neg5To5,    
 	/** +/- 2.5 V */   
   V_Neg2pt5To2pt5,    
   /** +/- 1.25 V */
   V_Neg1pt25To1pt25,     
    /** +/- 1 V */
   V_Neg1To1,             
 	/** 0~15 V  */
   V_0To15,        
    /** 0~10 V  */  
   V_0To10, 
   /** 0~5 V */ 
   V_0To5,      
   /** 0~2.5 V */
   V_0To2pt5,     
    /** 0~1.25 V */ 
   V_0To1pt25,           
   /** 0~1 V */
   V_0To1,                 
   /** +/- 625mV */
   mV_Neg625To625,    
   /** +/- 500 mV */
   mV_Neg500To500,         
   /** +/- 312.5 mV */
   mV_Neg312pt5To312pt5,   
   /** +/- 200 mV */
   mV_Neg200To200,   
   /** +/- 150 mV */
   mV_Neg150To150,         
   /** +/- 100 mV */
   mV_Neg100To100,         
   /** +/- 50 mV */
   mV_Neg50To50,         
   /** +/- 30 mV */
   mV_Neg30To30,    
   /** +/- 20 mV */
   mV_Neg20To20,        
   /** +/- 15 mV  */
   mV_Neg15To15,           
   /** +/- 10 mV */
   mV_Neg10To10,           
    /** +/- 5 mV */
   mV_Neg5To5,           
   /** 0 ~ 625 mV */
   mV_0To625,      
   /** 0 ~ 500 mV */
   mV_0To500,              
   /** 0 ~ 150 mV */
   mV_0To150,       
    /** 0 ~ 100 mV */
   mV_0To100,         
    /** 0 ~ 50 mV */
   mV_0To50,      
   /** 0 ~ 20 mV */
   mV_0To20,
   /** 0 ~ 15 mV */
   mV_0To15,   
   /** 0 ~ 10 mV */
   mV_0To10,   
   
   /** +/- 20mA */
   mA_Neg20To20,          
   /** 0 ~ 20 mA */
   mA_0To20,   
    /** 4 ~ 20 mA */
   mA_4To20,             
    /** 0 ~ 24 mA */
   mA_0To24,             

   /** +/- 2 V */
   V_Neg2To2,    
    /** +/- 4 V */
   V_Neg4To4,            
   /** +/- 20 V */
   V_Neg20To20,            

   /** T/C J type 0~760 'C */
   Jtype_0To760C, 
    /** T/C K type 0~1370 'C */
   Ktype_0To1370C,		  
   /** T/C T type -100~400 'C */
   Ttype_Neg100To400C,     
   /** T/C E type 0~1000 'C */
   Etype_0To1000C,		   
   /** T/C R type 500~1750 'C */
   Rtype_500To1750C,	    
    /** T/C S type 500~1750 'C */
   Stype_500To1750C,	 
   /** T/C B type 500~1800 'C */
   Btype_500To1800C,	      

 /** Pt392 -50~150 'C  */
   Pt392_Neg50To150,	     
   /** Pt385 -200~200 'C */
   Pt385_Neg200To200,	
   /** Pt385 0~400 'C */
   Pt385_0To400,		      
   /** Pt385 -50~150 'C */
   Pt385_Neg50To150,	      
   /** Pt385 -100~100 'C */
   Pt385_Neg100To100,      
   /** Pt385 0~100 'C  */
   Pt385_0To100,		      
   /** Pt385 0~200 'C */
   Pt385_0To200,		
   /** Pt385 0~600 'C */
   Pt385_0To600,		      
   /** Pt392 -100~100 'C  */
   Pt392_Neg100To100,      
   /** Pt392 0~100 'C */
   Pt392_0To100,          
   /** Pt392 0~200 'C */
   Pt392_0To200,     
    /** Pt392 0~600 'C */
   Pt392_0To600,     
    /** Pt392 0~400 'C */
   Pt392_0To400,          
    /** Pt392 -200~200 'C  */
   Pt392_Neg200To200,     
   /** Pt1000 -40~160 'C  */
   Pt1000_Neg40To160,      

	/** Balcon500 -30~120 'C  */
   Balcon500_Neg30To120,   
	/** Ni518 -80~100 'C */
   Ni518_Neg80To100,       
   /** Ni518 0~100 'C */
   Ni518_0To100,           
   /** Ni508 0~100 'C */
   Ni508_0To100,           
   /** Ni508 -50~200 'C */
   Ni508_Neg50To200,       

	/** Thermistor 3K 0~100 'C */
   Thermistor_3K_0To100,   
   /** Thermistor 10K 0~100 'C */
   Thermistor_10K_0To100,  
   
	/** T/C J type -210~1200 'C */
   Jtype_Neg210To1200C,  
    /** T/C K type -270~1372 'C */
   Ktype_Neg270To1372C,   
   /** T/C T type -270~400 'C */
   Ttype_Neg270To400C,     
   /** T/C E type -270~1000 'C */
   Etype_Neg270To1000C,		
    /** T/C R type -50~1768 'C */
   Rtype_Neg50To1768C,	  
   /** T/C S type -50~1768 'C */
   Stype_Neg50To1768C,	   
   /** T/C B type 40~1820 'C */
   Btype_40To1820C,	    
   
  /** T/C J type -210~870 'C */
   Jtype_Neg210To870C ,
    /** T/C R type 0~1768 'C */
   Rtype_0To1768C,	   
   /** T/C S type 0~1768 'C */
   Stype_0To1768C,	      
   /** T/C T type -20~135 'C */
   Ttype_Neg20To135C,      

   /** 0xC000 ~ 0xF000 : user customized value range type */
   UserCustomizedVrgStart = 0xC000,
   UserCustomizedVrgEnd = 0xF000,

   /** AO external reference type */
   V_ExternalRefBipolar = 0xF001, /** External reference voltage unipolar */
   V_ExternalRefUnipolar = 0xF002, /** External reference voltage bipolar */
} ValueRange;

/** Counter Mode */
typedef enum tagSignalCountingType {
   /** counting direction is determined by two signals, one is clock, the other is direction signal */
   ContrMod_Bi_DIR = 0,  
   /** counter value increases on each clock, or increases on each clock */   
   ContrMod_UP_DOWN,   
   /** counter value increases on each clock*/
   ContrMod_UP,        
   /** counting frequency*/
   ContrMod_Freq,
   /** AB phase, 1x rate up/down counting */ 
   AbPhaseX1,     
   /** AB phase, 2x rate up/down counting */ 
   AbPhaseX2,   
   /** AB phase, 4x rate up/down counting */
   AbPhaseX4,      
} SignalCountingType;

/** AI Data Type */
typedef enum tagAIDataUnit {
	/** Engineer Unit*/
    EngineerUnit = 0,
 	/** Percent*/   
    Percent = 1,
 	/** Raw data*/       
    TwosComplementHex = 2,
   	/** Resistance unit, for ADAM-5013*/         
    Ohms = 3,
    /** Unknown type*/          
    Unknown = 255,
}AIDataUnit;

/** AI Calibration Type */
typedef enum AI_CALI_MODE{
	/** Zero calibration*/
   Cali_Zero = 0,
 	/** Span calibration*/  
   Cali_Span,
  	/** Cjc calibration, for Thermocouple module*/    
   Cali_Cjc,
}AiCaliMode;

typedef enum AO_CALI_MODE{
   Cali_4mA = 0,
   Cali_20mA,
   Cali_Trim,  
}AoCaliMode;
/**
* Property Attribute and Id
*/
typedef enum tagPropertyAttribute {
   ReadOnly = 0,
   Writable = 1,
   Modal = 0,
   Nature = 2,
} PropertyAttribute;

#define BioFailed(c)  ((unsigned)(c) >= (unsigned)0xE0000000)   

/// <summary>
/// tagErrorCode info.
/// </summary>
typedef enum tagErrorCode {
   /// <summary>
   /// The operation is completed successfully. 
   /// </summary>
   Success = 0, 

   ///*************************************************************************
   /// warning                                                              
   ///*************************************************************************
   /// <summary>
   /// The interrupt resource is not available. 
   /// </summary>
   WarningIntrNotAvailable = 0xA0000000,

   /// <summary>
   /// The parameter is out of the range. 
   /// </summary>
   WarningParamOutOfRange = 0xA0000001,

   /// <summary>
   /// The property value is out of range. 
   /// </summary>
   WarningPropValueOutOfRange = 0xA0000002,

   /// <summary>
   /// The property value is not supported. 
   /// </summary>
   WarningPropValueNotSpted = 0xA0000003,

   /// <summary>
   /// The property value conflicts with the current state.
   /// </summary>
   WarningPropValueConflict = 0xA0000004,

   /// <summary>
   /// The value range of all channels in a group should be same.
   /// </summary>
   WarningVrgOfGroupNotSame = 0xA0000005,
   
   ///***********************************************************************
   /// error                                                                
   ///***********************************************************************
   /// <summary>
   /// The handle is NULL or its type doesn't match the required operation. 
   /// </summary>
   ErrorHandleNotValid = 0xE0000000,

   /// <summary>
   /// The parameter value is out of range.
   /// </summary>
   ErrorParamOutOfRange = 0xE0000001,

   /// <summary>
   /// The parameter value is not supported.
   /// </summary>
   ErrorParamNotSpted = 0xE0000002,

   /// <summary>
   /// The parameter value format is not the expected. 
   /// </summary>
   ErrorParamFmtUnexpted = 0xE0000003,

   /// <summary>
   /// Not enough memory is available to complete the operation. 
   /// </summary>
   ErrorMemoryNotEnough = 0xE0000004,

   /// <summary>
   /// The data buffer is null. 
   /// </summary>
   ErrorBufferIsNull = 0xE0000005,

   /// <summary>
   /// The data buffer is too small for the operation. 
   /// </summary>
   ErrorBufferTooSmall = 0xE0000006,

   /// <summary>
   /// The data length exceeded the limitation. 
   /// </summary>
   ErrorDataLenExceedLimit = 0xE0000007,

   /// <summary>
   /// The required function is not supported. 
   /// </summary>
   ErrorFuncNotSpted = 0xE0000008,

   /// <summary>
   /// The required event is not supported. 
   /// </summary>
   ErrorEventNotSpted = 0xE0000009,

   /// <summary>
   /// The required property is not supported. 
   /// </summary>
   ErrorPropNotSpted = 0xE000000A, 

   /// <summary>
   /// The required property is read-only. 
   /// </summary>
   ErrorPropReadOnly = 0xE000000B,

   /// <summary>
   /// The specified property value conflicts with the current state.
   /// </summary>
   ErrorPropValueConflict = 0xE000000C,

   /// <summary>
   /// The specified property value is out of range.
   /// </summary>
   ErrorPropValueOutOfRange = 0xE000000D,

   /// <summary>
   /// The specified property value is not supported. 
   /// </summary>
   ErrorPropValueNotSpted = 0xE000000E,

   /// <summary>
   /// The handle hasn't own the privilege of the operation the user wanted. 
   /// </summary>
   ErrorPrivilegeNotHeld = 0xE000000F,

   /// <summary>
   /// The required privilege is not available because someone else had own it. 
   /// </summary>
   ErrorPrivilegeNotAvailable = 0xE0000010,

   /// <summary>
   /// The driver of specified device was not found. 
   /// </summary>
   ErrorDriverNotFound = 0xE0000011,

   /// <summary>
   /// The driver version of the specified device mismatched. 
   /// </summary>
   ErrorDriverVerMismatch = 0xE0000012,

   /// <summary>
   /// The loaded driver count exceeded the limitation. 
   /// </summary>
   ErrorDriverCountExceedLimit = 0xE0000013,

   /// <summary>
   /// The device is not opened. 
   /// </summary>
   ErrorDeviceNotOpened = 0xE0000014,      

   /// <summary>
   /// The required device does not exist. 
   /// </summary>
   ErrorDeviceNotExist = 0xE0000015,

   /// <summary>
   /// The required device is unrecognized by driver. 
   /// </summary>
   ErrorDeviceUnrecognized = 0xE0000016,

   /// <summary>
   /// The configuration data of the specified device is lost or unavailable. 
   /// </summary>
   ErrorConfigDataLost = 0xE0000017,

   /// <summary>
   /// The function is not initialized and can't be started. 
   /// </summary>
   ErrorFuncNotInited = 0xE0000018,

   /// <summary>
   /// The function is busy. 
   /// </summary>
   ErrorFuncBusy = 0xE0000019,

   /// <summary>
   /// The interrupt resource is not available. 
   /// </summary>
   ErrorIntrNotAvailable = 0xE000001A,

   /// <summary>
   /// The DMA channel is not available. 
   /// </summary>
   ErrorDmaNotAvailable = 0xE000001B,

   /// <summary>
   /// Time out when reading/writing the device. 
   /// </summary>
   ErrorDeviceIoTimeOut = 0xE000001C,

   /// <summary>
   /// The given signature does not match with the device current one.
   /// </summary>
   ErrorSignatureNotMatch = 0xE000001D,

   /// <summary>
   /// Undefined error 
   /// </summary>
   ErrorUndefined = 0xE000FFFF,
} ErrorCode;

#endif // _BDAQ_TYPES_DEFINED


extern "C" {
	 // Global APIs
	/*!*************************************************************************
	* \brief
	*	Opens a device. It is used to establish a handle to access the device.
	*
	* \details
	*	This function must be called before any other board functions.
	*
	* \param[out] hHandle
	*	Type: int32 * 
	*  Handle of the device.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/
	uint32 ADAMDrvOpen(
		int32* hHandle);
 	/*!*************************************************************************
	* \brief
	*	Closes a device. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32 ADAMDrvClose(
		int32* hHandle);
 	/*!*************************************************************************
	* \brief
	*	Gets the DIP switch setting of the Node ID. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[out] o_usNodeID
	*  Node id of ADAM-5630. 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32 SYS_GetNodeID(
		int32	 hHandle,
		uint16*	 o_usNodeID);
 	/*!*************************************************************************
	* \brief
	*	Gets the module ID of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  moduleid
	* The module id of the indicated slot. 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32 SYS_GetModuleID(
		int32	 hHandle,
		uint16   i_usSlot,
		uint16*	 moduleid);
 	/*!*************************************************************************
	* \brief
	*	Gets the module name of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_szName
	* The module name of the indicated slot. 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32 SYS_GetModuleName(
		int32 hHandle, 
		uint16 i_usSlot, 
		char *o_szName);

	//AI
 	/*!*************************************************************************
	* \brief
	*	Gets the total AI channel counts of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_usCh
	* The total AI channel counts of the indicated slot. 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32	AI_GetChannelTotal(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16* o_usCh);
 	/*!*************************************************************************
	* \brief
	*	Gets the channel status(enable or disable) of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_bEnabled
	* The channel status of the indicated slot. Eg:{0,0,0,0,0,0,0,0} 1:Enable;0:Disable
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32  AI_GetChannelEnabled(
		int32 hHandle,
		uint16 i_usSlot, 
		bool* o_bEnabled);
 	/*!*************************************************************************
	* \brief
	*	Sets the channel status(enable or disable) of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_bEnabled
	* The channel status of the indicated slot. Eg:{0,0,0,0,0,0,0,0} 1:Enable;0:Disable
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32	AI_SetChannelEnabled(
		int32 hHandle, 
		uint16 i_usSlot, 
		bool* i_bEnabled);
	/*!*************************************************************************
	* \brief
	*	Gets the range code  of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_byRangeCode
	* The range code of the indicated channel and slot.See \ref tagValueRange
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32	AI_GetInputRange(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8* o_byRangeCode);
	
	/*!*************************************************************************
	* \brief
	*	Sets the range code  of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_byRangeCode
	* The range code of the indicated channel and slot.See \ref tagValueRange
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32	AI_SetInputRange(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8 i_byRangeCode);
 	/*!*************************************************************************
	* \brief
	*	Gets AI integration time of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_iIntegrationTime
	* AI integration time of the indicated slot. See \ref tagAiIntegrationTime
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32	AI_GetRangeIntegrationTime(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16* o_iIntegrationTime);
 	/*!*************************************************************************
	* \brief
	*	Sets AI integration time of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_iIntegrationTime
	* AI integration time of the indicated slot. See \ref tagAiIntegrationTime.
	*
	* \return
	*    result, Success if successful. See \ref tagErrorCode.
	*
	***************************************************************************/  		
	uint32	AI_SetRangeIntegrationTime(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_iIntegrationTime);
 	/*!*************************************************************************
	* \brief
	*	Gets AI data format of the indicated slot. For ADAM-5013 only.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_iDataFormat
	* AI data format of the indicated slot. See \ref tagAIDataUnit
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32 AI_GetDataFormat(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16* o_iDataFormat );
 	/*!*************************************************************************
	* \brief
	*	Sets AI data format of the indicated slot. For ADAM-5013 only.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_iDataFormat
	* AI data format of the indicated slot. See \ref tagAIDataUnit
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 AI_SetDataFormat(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_iDataFormat);
	/*!*************************************************************************
	* \brief
	*	Gets all analog input data  of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_sValue
	* Raw data  of the indicated slot.	
	*
	* \param[out]  o_fValue
	* Scaled data of the indicated slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32	AI_GetValues(
		int32 hHandle,
		uint16 i_usSlot,
	    void * o_sValue, 	
		double* o_fValue);
	/*!*************************************************************************
	* \brief
	*	Gets analog input data  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_sValue
	* Raw data  of the indicated channel and slot.	
	*
	* \param[out]  o_fValue
	* Scaled data of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32	AI_GetValue(
		int32 hHandle,
		uint16 i_usSlot,
		uint16 i_usCh,	
	    void * o_sValue, 	
		double* o_fValue);	
	/*!*************************************************************************
	* \brief
	*	AI Calibration of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_caliType
	* Calibration type. See \ref AiCaliMode.	
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32 AI_Calibration(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8 i_caliType);
	
	uint32 AI_Calibration_CJC(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_iChannel, 
		uint8 i_caliType, 
		long l_cjcValue);

	//AO
 	/*!*************************************************************************
	* \brief
	*	Gets the total AO channel counts of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_usCh
	* The total AO channel counts of the indicated slot. 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32	AO_GetChannelTotal(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16* o_usCh);
	/*!*************************************************************************
	* \brief
	*	Gets the range code  of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_byRangeCode
	* The range code of the indicated channel and slot.See \ref tagValueRange
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32	AO_GetOutputRange(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8* o_byRangeCode);
	/*!*************************************************************************
	* \brief
	*	Sets the range code  of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_byRangeCode
	* The range code of the indicated channel and slot.See \ref tagValueRange
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32	AO_SetOutputRange(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8 i_byRangeCode);
	/*!*************************************************************************
	* \brief
	*	Gets the startup value of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_fValue
	* The startup value of the indicated channel and slot.See \ref tagValueRange
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32  AO_GetStartup(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		double* o_fValue);
	/*!*************************************************************************
	* \brief
	*	Sets the startup value of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_fValue
	* The startup value of the indicated channel and slot.See \ref tagValueRange
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32  AO_SetStartup(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		double i_fValue);
	/*!*************************************************************************
	* \brief
	*	Gets all analog output data  of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_fValue
	* Scaled data of the indicated slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32	AO_GetValues(
		int32 hHandle,
		uint16 i_usSlot,
		double* o_fValue);
	/*!*************************************************************************
	* \brief
	*	Gets analog output data  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_fValue
	* Scaled data of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32	AO_GetValue(
		int32 hHandle,
		uint16 i_usSlot,
		uint16 i_usCh,		
		double* o_fValue);	
	/*!*************************************************************************
	* \brief
	*	Sets the analog output data  of all channels of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  i_fValue
	* Scaled data of the indicated slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32	AO_SetValues(
		int32 hHandle,
		uint16 i_usSlot,
		double i_fValue);
	/*!*************************************************************************
	* \brief
	*	Sets the analog output data  of  the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.		
	*
	* \param[out]  i_fValue
	* Scaled data of the indicated slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
	uint32	AO_SetValue(
		int32 hHandle,
		uint16 i_usSlot,
		uint16 i_usCh,		
		double i_fValue);	
	
	uint32 AO_Calibration_4mA(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_iChannel);
	
	uint32 AO_Calibration_20mA(
		int32 hHandle, 
		uint16 i_usSlot,
		uint16 i_iChannel);
	
	uint32 AO_Calibration_Trim(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_iChannel, 
		long l_gradient);

    //DIO
 	/*!*************************************************************************
	* \brief
	*	Gets the total DIO channel counts of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_usCh
	* The total DIO channel counts of the indicated slot. 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	    
	uint32	DIO_GetChannelTotal(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16* o_usCh);
  	/*!*************************************************************************
	* \brief
	*	Gets all digital input data  of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_dwValue
	* Digital input data of the indicated slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/    
	uint32 DI_GetValues(
		int32 hHandle,
		uint16 i_usSlot,
		uint32* o_dwValue);
   	/*!*************************************************************************
	* \brief
	*	Gets the digital input data  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.		
	*	
	* \param[out]  o_bValue
	* Digital input data of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/    
	uint32 DI_GetValue(
		int32 hHandle, 
		uint16 i_usSlot,
		uint16 i_usCh, 
		BOOL* o_bValue);
 	/*!*************************************************************************
	* \brief
	*	Gets direction of the indicated slot(For ADAM-5050). 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[out]  o_dwStatus
	* The channel direction of the indicated slot.(Digital input: 1; Digital output: 0) 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/    	
	uint32 DIO_GetUniversalStatus(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16* o_dwStatus);
	
   	/*!*************************************************************************
	* \brief
	*	Gets the invert status  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[out]  o_InvertMode
	* Invert status of the indicated slot.(Invert:1; Not invert:0)
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/    	
	uint32  DI_GetInvert(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint32* o_InvertMode);	

   	/*!*************************************************************************
	* \brief
	*	Sets the invert status  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.	
	*	
	* \param[in]  i_InvertMode
	* Invert status of the indicated channel and slot.(Invert:1; Not invert:0)
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/    		
	uint32  DI_SetInvert(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8 i_InvertMode);
		
   	/*!*************************************************************************
	* \brief
	*	Sets the digital output data  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*
	* \param[in]  i_usCh
	* The indicated channel index.		
	*	
	* \param[in]  i_bValue
	* Digital output data of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/    	
	uint32  DO_SetValue(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		BOOL i_bValue);
		
   	/*!*************************************************************************
	* \brief
	*	Sets all digital output data  of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  o_dwValue
	* Digital output data of the indicated slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/    		
	uint32  DO_SetValues(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint32 o_dwValue);

   	/*!*************************************************************************
	* \brief
	*	Gets all digital output data  of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[out]  o_dwValue
	* Digital output data of the indicated slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32  DO_GetValues(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint32 * o_dwValue);
	
    //Counter
   	/*!*************************************************************************
	* \brief
	*	Gets counter value  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_lValue
	* Counter value of the indicated slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	    
	uint32 CNT_GetValue(
		int32 hHandle,
		uint16 i_usSlot,
		uint16 i_usCh, 
		uint32 *o_lValue);
   	/*!*************************************************************************
	* \brief
	*	Clear the counter value  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32 CNT_ClearValue(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh);
   	/*!*************************************************************************
	* \brief
	*	Gets the counter mode  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_byRange
	* Counter mode of the indicated slot.	See \ref tagSignalCountingType.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_GetRange(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8 *o_byRange);
   	/*!*************************************************************************
	* \brief
	*	Sets the counter mode  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_byRange
	* Counter mode of the indicated slot.	See \ref tagSignalCountingType.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_SetRange(
		uint32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8 i_byRange);
		
   	/*!*************************************************************************
	* \brief
	*	Gets the counter filter width of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[out]  o_lFilter
	* Counter filter width of the indicated slot. For ADAM-5080: 8~65000 (us),for ADAM-5081: 1~65000 (us) %
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_GetFilter(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint32 *o_lFilter);
		
   	/*!*************************************************************************
	* \brief
	*	Sets the counter filter width of the indicated slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_lFilter
	* Counter filter width of the indicated slot. For ADAM-5080: 8~65000 (us),for ADAM-5081: 1~65000 (us) %
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_SetFilter(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint32 i_lFilter);
		
	/*!*************************************************************************
	* \brief
	*	Gets the startup value  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_lStartup
	* Counter startup value of the indicated slot.	
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_GetStartup(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint32 *o_lStartup);
	/*!*************************************************************************
	* \brief
	*	Sets the startup value  of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_lStartup
	* Counter startup value of the indicated slot.	
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_SetStartup(
		int32 hHandle,
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint32 i_lStartup);
	
	/*!*************************************************************************
	* \brief
	*	Gets the counter state of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_bCounting
	* Counter state of the indicated channel and slot.(1:counter start;0: counter stop)
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  			
	uint32 CNT_GetState(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		BOOL *o_bCounting);
	
	/*!*************************************************************************
	* \brief
	*	Sets the counter state of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_bCounting
	* Counter state of the indicated channel and slot.(1:counter start;0: counter stop)
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  				
	uint32 CNT_SetState(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		BOOL i_bCounting);
	
	/*!*************************************************************************
	* \brief
	*	Gets the counter overflow of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_bOverflow
	* Counter overflow flag of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_GetOverflow(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		BOOL *o_bOverflow);

	/*!*************************************************************************
	* \brief
	*	Clears the counter overflow flag of the indicated channel and slot. 
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_ClearOverflow(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh);
	/*!*************************************************************************
	* \brief
	*	Gets the counter alarm flag of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_bAlarmFlag
	* Counter alarm flag of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_GetAlarmFlag(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		BOOL *o_bAlarmFlag);
	
	/*!*************************************************************************
	* \brief
	*	Clears the counter alarm flag of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32 CNT_ClearAlarmFlag(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh);
	
	/*!*************************************************************************
	* \brief
	*	Gets the counter alarm type of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_byAlarmType
	* Counter alarm type of the indicated channel and slot.(1: High alarm 0: Low alarm) 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32 CNT_GetAlarmType(
		uint32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8 *o_byAlarmType);

	/*!*************************************************************************
	* \brief
	*	Sets the counter alarm type of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_byAlarmType
	* Counter alarm type of the indicated channel and slot.(1: High alarm 0: Low alarm) 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_SetAlarmType(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8 i_byAlarmType);
	
	/*!*************************************************************************
	* \brief
	*	Gets the counter alarm state of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_bAlarmEnable
	* Counter alarm state of the indicated channel and slot.(1: Alarm Enable 0:Alarm Disable) 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_GetAlarmEnable(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		BOOL *o_bAlarmEnable);
	
	/*!*************************************************************************
	* \brief
	*	Sets the counter alarm state of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_bAlarmEnable
	* Counter alarm state of the indicated channel and slot.(1: Alarm Enable 0:Alarm Disable) 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_SetAlarmEnable(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		BOOL i_bAlarmEnable);

	/*!*************************************************************************
	* \brief
	*	Gets the local mapping channel index of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_byAlarmMap
	* Local alarm mapping channel index of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  			
	uint32 CNT_GetAlarmMap(
		int32 hHandle, 
		uint16 i_usSlot,
		uint16 i_usCh,
		uint8 *o_byAlarmMap);
	
	/*!*************************************************************************
	* \brief
	*	Sets the local mapping channel index of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_byAlarmMap
	* Local alarm mapping channel index of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  				
	uint32 CNT_SetAlarmMap(
		uint32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh,
		uint8 i_byAlarmMap);

	/*!*************************************************************************
	* \brief
	*	Gets the alarm limit of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[out]  o_lAlarmLimit
	* Local alarm limit value of the indicated channel and slot.Range:[0, (2^32 -1)] .
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  			
	uint32 CNT_GetAlarmLimit(
		int32 hHandle, 
		uint16 i_usSlot,
		uint16 i_usCh, 
		uint32 *o_lAlarmLimit);
	
	/*!*************************************************************************
	* \brief
	*	Sets the alarm limit of the indicated channel and slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.	
	*
	* \param[in]  i_lAlarmLimit
	* Local alarm limit value of the indicated channel and slot.Range:[0, (2^32 -1)] .
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32 CNT_SetAlarmLimit(
		int32 hHandle, 
		uint16 i_usSlot,
		uint16 i_usCh, 
		uint32 i_lAlarmLimit);

   	/*!*************************************************************************
	* \brief
	*	Gets the DO value of the indicated channel and  slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*
	* \param[out]  o_bDO
	* Digital output data of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32 CNT_GetDoValue(
		int32 hHandle, 
		uint16 i_usSlot,
		uint16 i_usCh, 
		BOOL *o_bDO);
		
   	/*!*************************************************************************
	* \brief
	*	Sets the DO value of the indicated channel and  slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*	
	* \param[in]  i_bDO
	* Digital output data of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_SetDoValue(
		int32 hHandle,
		uint16 i_usSlot,
		uint16 i_usCh, 
		BOOL i_bDO);
		
   	/*!*************************************************************************
	* \brief
	*	Gets all  DO value of the indicated channel and  slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[out]  o_byDOs
	* Digital output data of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  			
	uint32 CNT_GetDoValues(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint8 *o_byDOs);
   	/*!*************************************************************************
	* \brief
	*	Sets all  DO value of the indicated channel and  slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_byDOs
	* Digital output data of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_SetDoValues(
		int32 hHandle, 
		uint16 i_usSlot,
		uint8 i_byDOs);
		
   	/*!*************************************************************************
	* \brief
	*	Gets the counter frequency acquisition time of the indicated slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[out]  o_lFreqActTime
	* Counter frequency acquisition time of the indicated slot. 1000~10000000 (us) 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  			
	uint32 CNT_GetFreqAcqTime(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint32 *o_lFreqActTime);
   	/*!*************************************************************************
	* \brief
	*	Sets the counter frequency acquisition time of the indicated slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_lFreqActTime
	* Counter frequency acquisition time of the indicated slot. 1000~10000000 (us) 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  		
	uint32 CNT_SetFreqAcqTime(
		int32 hHandle,
		uint16 i_usSlot, 
		uint32 i_lFreqActTime);
		
	/*!*************************************************************************
	* \brief
	*	Gets the remote counter alarm flag of the indicated channel and slot.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*	
	* \param[in]  i_bType
	* The remote alram type, high or low alarm.
	*
	* \param[out]  o_bAlarmFlag
	* Type: bool * 
	* Counter alarm flag of the indicated channel and slot.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  			
	uint32 CNT_GetRemoteAlarmFlag(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh,
		uint8 i_bType,
		bool *o_bAlarmFlag);
	
	/*!*************************************************************************
	* \brief
	*	Clears the remote counter alarm flag of the indicated channel and slot.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*	
	* \param[in]  i_bType
	* The remote alram type, high or low alarm.		
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  	
	uint32 CNT_ClearRemoteAlarmFlag(
		int32 hHandle, 
		uint16 i_usSlot,
		uint16 i_usCh,
		uint8 i_bType);
	
	/*!*************************************************************************
	* \brief
	*	Gets the remote counter alarm state of the indicated channel and slot.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*	
	* \param[in]  i_bType
	* The remote alram type, high or low alarm.
	*
	* \param[out]  o_bAlarmEnable
	* Counter alarm state of the indicated channel and slot.(1: Alarm Enable 0:Alarm Disable) 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  				
	uint32 CNT_GetRemoteAlarmEnable(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh,		
		uint8 i_bType,
		bool *o_bAlarmEnable);
	
	/*!*************************************************************************
	* \brief
	*	Sets the remote counter alarm state of the indicated channel and slot.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*	
	* \param[in]  i_bType
	* The remote alram type, high or low alarm.
	*
	* \param[in]  i_bAlarmEnable
	* Counter alarm state of the indicated channel and slot.(1: Alarm Enable 0:Alarm Disable) 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/ 	
	uint32 CNT_SetRemoteAlarmEnable(
		int32 hHandle,
		uint16 i_usSlot,
		uint16 i_usCh, 
		uint8 i_bType,
		bool i_bAlarmEnable);
	
	/*!*************************************************************************
	* \brief
	*	Gets the remote mapping slot and channel index of the indicated channel and slot.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*	
	* \param[in]  i_bType
	* The remote alram type, high or low alarm.
	*
	* \param[out]  o_usrtSlot
	* The remote mapping slot index.
	*	
	* \param[out]  o_usrtCh
	* The remote mapping channel index.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/ 			
	uint32 CNT_GetRemoteAlarmMap(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh,  
		uint8 i_bType,
		uint16 *o_usrtSlot, 
		uint16 *o_usrtCh);
	
	/*!*************************************************************************
	* \brief
	*	Sets the remote mapping slot and channel index of the indicated channel and slot.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*	
	* \param[in]  i_bType
	* The remote alram type, high or low alarm.
	*
	* \param[in]  i_usrtSlot
	* The remote mapping slot index.
	*	
	* \param[in]  i_usrtCh
	* The remote mapping channel index.
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/ 		
	uint32 CNT_SetRemoteAlarmMap(
		int32 hHandle,
		uint16 i_usSlot,
		uint16 i_usCh, 
		uint8 i_bType,
		uint16 i_usrtSlot, 
		uint16 i_usrtCh);
	
	/*!*************************************************************************
	* \brief
	*	Gets the remote counter alarm limit of the indicated channel and slot.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*	
	* \param[in]  i_bType
	* The remote alram type, high or low alarm.
	*
	* \param[out]  o_lAlarmLimit
	* Remote alarm limit of the indicated channel and slot. Range:[0, (2^32 -1)] 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/ 			
	uint32 CNT_GetRemoteAlarmLimit(
		int32 hHandle, 
		uint16 i_usSlot, 
		uint16 i_usCh, 
		uint8 i_bType,		
		uint32 *o_lAlarmLimit);
	
	/*!*************************************************************************
	* \brief
	*	Sets the remote counter alarm limit of the indicated channel and slot.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[in]  i_usCh
	* The indicated channel index.
	*	
	* \param[in]  i_bType
	* The remote alram type, high or low alarm.
	*
	* \param[in]  i_lAlarmLimit
	* Remote alarm limit of the indicated channel and slot. Range:[0, (2^32 -1)] 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/ 		
	uint32 CNT_SetRemoteAlarmLimit(
		int32 hHandle,
		uint16 i_usSlot,
		uint16 i_usCh, 		
		uint8 i_bType,
		uint32 i_lAlarmLimit);
	
   	/*!*************************************************************************
	* \brief
	*	Gets the firmware version of the indicated slot. For ADAM-5081.
	*
	*\details
	*	
	* \param[in] hHandle
	*  Handle of the device.
	*
	* \param[in]  i_usSlot
	* The indicated slot index.
	*	
	* \param[out]  o_lFwVer
	* Firmware version of the indicated slot. 
	*
	* \return
	*    result, Success if successful.
	*
	***************************************************************************/  
    uint32 CNT_GetFwVersion(
			int32 hHandle, 
			uint16 i_usSlot, 
			uint8 *o_lFwVer);

	}

/*!*************************************************************************
* \example AdamAIread.cpp
*	This is an example of AI read value.
*
* \example AIIntgrationTime.cpp
*	This is an example of change ai integratiom time.
*
* \example Adam5024.cpp
* 	This is an example of AO wirte value.
*
* \example Adamdi.cpp
*	This is an example of DI read value in normal mode.
*
* \example Adamdo.cpp
*	This is an example of DO write value in normal mode.
*
* \example Adam5080.cpp
*	This is an example of ADAM-5080 operation.
*
* \example Adam5081.cpp
*	This is an example of ADAM-5081 operation..
*
* \example GetSlotInfo.cpp
*	This is an example of module search.
***************************************************************************/
//OS 
//#if !defined(_BDAQ_NO_NAMESPACE) && defined(__cplusplus)
//    } // namespace : BDAQADAM
//} // namespace : Automation
//#endif

#endif	// _BDAQADAM_H_
