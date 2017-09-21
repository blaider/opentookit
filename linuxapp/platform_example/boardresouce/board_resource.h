#ifndef BOARD_RESOURCE_H
#define BOARD_RESOURCE_H

/*!
* \file board_resource.h
*
* For using Board Resource SDK, please include this file in your source code.
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/** board resource(Watchdog/LED) handle */
	typedef int BR_HANDLE;

	/** The error code of BoardResource SDK */
	typedef enum board_resource_result
	{
		/** success */
		BR_SUCCESS						= 0,
		/** device not exist */
		BR_DEVICE_NOT_EXIST				= -1,
		/** device already opened */
		BR_DEVICE_ALREADY_OPENED		= -2,
		/** the handle not opened */
		BR_ERR_DEVICE_NOT_OPENED		= -3,
		/** i2c error */
		BR_ERR_I2C						= -4,
		/** the i2c bus not exist */
		BR_ERR_I2C_BUS_NOT_EXIST		= -5,
		/** the i2c device does not exist */
		BR_ERR_I2C_DEVICE_NOT_EXIST		= -6,
		/** read the i2c command status error */
		BR_ERR_I2C_CONFIRM				= -7,
		/** out of the parameter range */
		BR_ERR_OUT_RANGE				= -8,
		/** timeout */
		BR_ERR_TIMEOUT					= -9,
		/** local socket not exist */
		BR_ERR_SERVER_NOTEXIST			= -10,
		/** local socket not exist */
		BR_ERR_NOTSUPPORT				= -11,
	}BR_RESULT;

	/** The watchdog works mode */
	typedef enum watch_mode
	{
		/** the watchdog will stop on machine start */
		WATCH_MODE_MANUAL		= 0xaa,
		/** the watchdog will work on machine start default */
		WATCH_MODE_AUTO			= 0x55,
	}WatchMode;

	/** The watchdog status */
	typedef enum
	{
		/** the watchdog was work now */
		WDTStart = 0x55,
		/** the watchdog was stop now */
		WDTStop = 0xaa,
		/** this device don't has watchdog */
		WDTNOTInclude = 0xa5
	} WDTStatus;

	/** The LED type */
	typedef enum led_type
	{
		/** RUN LED */
		LED_TYPE_RUN				= 0,
		/** ERROR LED */
		LED_TYPE_ERROR				= 1,
		/** PROG LED */
		LED_TYPE_PROGRAM			= 2,
		/** LED 1*/
		LED_TYPE_P1				= 0,
		/** LED 2*/
		LED_TYPE_P2			= 1,
		/** LED 3*/
		LED_TYPE_P3			= 2,

	}LEDType;

	/*!*************************************************************************
	* \brief
	*	Initialize the Board device.
	*
	* \details
	*	This function must be called before any other board functions.
	*
	* \param[out] handle
	*	Handle of the Board device.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT Board_Init( BR_HANDLE * handle );

	/*!*************************************************************************
	* \brief
	*	De-initialize the Board device.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the Board device.
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT Board_DeInit( BR_HANDLE handle );

	/*!*************************************************************************
	* \brief
	*	get node id of the Board device.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the node id.
	*
	* \param[out] nodeID
	*	node id.
	*
	* \return
	*    node id.
	*
	***************************************************************************/
	BR_RESULT Board_GetNodeID( BR_HANDLE handle, unsigned int * nodeID );

	/*!*************************************************************************
	* \brief
	*	get battery status of the Board device.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the node id.
	*
	* \param[out] status
	*	baterry status. 0 means is too low, 1 means normal
	*
	* \return
	*    status.
	*
	***************************************************************************/
	BR_RESULT Board_GetBattery( BR_HANDLE handle, unsigned int * status );

	/*!*************************************************************************
	* \brief
	*	Initialize the Watchdog.
	*
	* \details
	*	This function must be called before any other watchdog functions.
	*
	* \param[out] handle
	*	Handle of the Watchdog.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT WDT_Init( BR_HANDLE * handle );

	/*!*************************************************************************
	* \brief
	*	De-initialize the Watchdog.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the Watchdog.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT WDT_DeInit( BR_HANDLE handle );

	/*!*************************************************************************
	* \brief
	*	Enable the Watchdog.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the Watchdog.
	*
	* \param[in] spanSeconds
	*	time span of the Watchdog.range from 1 to 3600 seconds
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT WDT_Enable( BR_HANDLE handle , unsigned int spanSeconds);

	/*!*************************************************************************
	* \brief
	*	Disable the Watchdog.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the Watchdog.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT WDT_Disable( BR_HANDLE handle );

	/*!*************************************************************************
	* \brief
	*	Set the watch mode of the Watchdog.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the Watchdog.
	*
	* \param[in] watchMode
	*	mode of the Watchdog.
	*
	* \param[in] spanSeconds
	*	time span of the Watchdog.range from 120 to 3600 seconds
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT WDT_SetMode(
		BR_HANDLE handle,
		WatchMode watchMode,
		unsigned int spanSeconds);

	/*!*************************************************************************
	* \brief
	*	Set the time span of the Watchdog.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the Watchdog.
	*
	* \param[in] spanSeconds
	*	time span of the Watchdog. range from 1 to 3600 seconds
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT WDT_SetTimeSpan(
		BR_HANDLE handle,
		unsigned int spanSeconds );

	/*!*************************************************************************
	* \brief
	*	Strobe the Watchdog.
	*
	* \details
	*	 after enabling the Watchdog using WDT_Enable, the application must
	*	continuously call WDT_Strobe to trigger the Watchdog.
	*
	* \param[in] handle
	*	Handle of the Watchdog.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT WDT_Strobe( BR_HANDLE handle );

	/*!*************************************************************************
	* \brief
	*	Initialize the LED device.
	*
	* \details
	*	This function must be called before any other LED functions.
	*
	* \param[in] type
	*	type of the LED device.
	*
	* \param[out] handle
	*	Handle of the LED device.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT LED_Init(
		LEDType type,
		BR_HANDLE* handle );

	/*!*************************************************************************
	* \brief
	*	De-initialize the LED device.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the LED device.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT LED_DeInit( BR_HANDLE handle );

	/*!*************************************************************************
	* \brief
	*	Light the LED.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the LED device.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT LED_On(
		BR_HANDLE handle );

	/*!*************************************************************************
	* \brief
	*	Turn off the LED.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the LED device.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT LED_Off(
		BR_HANDLE handle );

	/*!*************************************************************************
	* \brief
	*	Blink the LED.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the LED device.
	*
	* \param[in] OnMseconds
	*	LED powerOn milli-seconds.
	*
	* \param[in] OffMseconds
	*	LED powerOff milli-seconds.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT LED_Blink(
				BR_HANDLE handle, unsigned int OnMseconds,unsigned int OffMseconds );

	/*!*************************************************************************
	* \brief
	*	Initialize the DIO device.
	*   Note: This API can only be used in devices with programmable GPIO, please refer to hardware manual.
	*
	* \details
	*	This function must be called before any other DIO functions.
	*
	* \param[out] handle
	*	Handle of the LED device.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT DIO_Init(BR_HANDLE* handle );

	/*!*************************************************************************
	* \brief
	*	De-initialize the DIO device.
	*   Note: This API can only be used in devices with programmable GPIO, please refer to hardware manual.
	*
	* \details
	*
	* \param[in] handle
	*	Handle of the DIO device.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT DIO_DeInit( BR_HANDLE handle );

	/*!*************************************************************************
	* \brief
	*	read di value.
	*   Note: This API can only be used in devices with programmable GPIO, please refer to hardware manual.
	*
	* \details
	*	This function can read di value.
	*
	* \param[in] handle
	*	Handle of the DIO device.
	*
	* \param[in] startChannel
	*	di channel start number.
	*
	* \param[in] channelCount
	*	di channel count.
	*
	* \param[out] data
	*	di value, bitmap.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT Di_Read(BR_HANDLE handle ,
			int startChannel,
			int channelCount,
			unsigned int *data);

	/*!*************************************************************************
	* \brief
	*	read do value.
	*   Note: This API can only be used in devices with programmable GPIO, please refer to hardware manual.
	*
	* \details
	*	This function can read do value.
	*
	* \param[in] handle
	*	Handle of the DIO device.
	*
	* \param[in] startChannel
	*	do channel start number.
	*
	* \param[in] channelCount
	*	do channel count.
	*
	* \param[out] data
	*	do value, bitmap.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT Do_Read(BR_HANDLE handle ,
			int startChannel,
			int channelCount,
			unsigned int *data);

	/*!*************************************************************************
	* \brief
	*	write do value.
	*   Note: This API can only be used in devices with programmable GPIO, please refer to hardware manual.
	*
	* \details
	*	This function can write do value.
	*
	* \param[in] handle
	*	Handle of the DIO device.
	*
	* \param[in] startChannel
	*	do channel start number.
	*
	* \param[in] channelCount
	*	do channel count.
	*
	* \param[out] data
	*	do value, bitmap.
	*
	* \return
	*    result, BR_SUCCESS if successful.
	*
	***************************************************************************/
	BR_RESULT Do_Write(BR_HANDLE handle ,
			int startChannel,
			int channelCount,
			unsigned int data);



	/*!*************************************************************************
	* \brief
	*	get di count.
	*   Note: This API can only be used in devices with programmable GPIO, please refer to hardware manual.
	*
	* \details
	*	This function return di count.
	*
	* \return
	*    di count.
	*
	***************************************************************************/
	int 	Get_DICount();

	/*!*************************************************************************
	* \brief
	*	get do count.
	*   Note: This API can only be used in devices with programmable GPIO, please refer to hardware manual.
	*
	* \details
	*	This function return do count.
	*
	* \return
	*    do count.
	*
	***************************************************************************/
	int 	Get_DOCount();

	/*!*************************************************************************
	* \example ledtest.c
	*	This is an example of how to control leds.
	*
	* \example nodeidtest.c
	*	This is an example of how to read node ID.
	*
	* \example wdttest.c
	*	This is an example of how to control watchdog.
	*
	* \example diotest.c
	*	This is an example of how to control DIO.
	*   Note: This API can only be used in devices with programmable GPIO, please refer to hardware manual.
	***************************************************************************/


	typedef BR_RESULT (*BRF_Board_Init)( BR_HANDLE * handle );
	typedef BR_RESULT (*BRF_Board_DeInit)( BR_HANDLE handle );
	typedef BR_RESULT (*BRF_Board_GetNodeID)( BR_HANDLE handle, unsigned int * nodeID );
	typedef BR_RESULT (*BRF_Board_GetBattery)( BR_HANDLE handle, unsigned int * status );
	typedef BR_RESULT (*BRF_WDT_Init)( BR_HANDLE * handle );
	typedef BR_RESULT (*BRF_WDT_DeInit)( BR_HANDLE handle );
	typedef BR_RESULT (*BRF_WDT_Enable)( BR_HANDLE handle , unsigned int spanSeconds);
	typedef BR_RESULT (*BRF_WDT_Disable)( BR_HANDLE handle );
	typedef BR_RESULT (*BRF_WDT_SetMode)(
		BR_HANDLE handle,
		WatchMode watchMode,
		unsigned int spanSeconds);
	typedef BR_RESULT (*BRF_WDT_SetTimeSpan)(
		BR_HANDLE handle,
		unsigned int spanSeconds );
	typedef BR_RESULT (*BRF_WDT_Strobe)( BR_HANDLE handle );
	typedef BR_RESULT (*BRF_LED_Init)(
		LEDType type,
		BR_HANDLE* handle );
	typedef BR_RESULT (*BRF_LED_DeInit)( BR_HANDLE handle );
	typedef BR_RESULT (*BRF_LED_On)(
		BR_HANDLE handle );
	typedef BR_RESULT (*BRF_LED_Off)(
		BR_HANDLE handle );
	typedef BR_RESULT (*BRF_LED_Blink)(
				BR_HANDLE handle, unsigned int OnMseconds,unsigned int OffMseconds );
	typedef BR_RESULT (*BRF_DIO_Init)(BR_HANDLE* handle );
	typedef BR_RESULT (*BRF_DIO_DeInit)( BR_HANDLE handle );
	typedef BR_RESULT (*BRF_Di_Read)(BR_HANDLE handle ,
			int startChannel,
			int channelCount,
			unsigned int *data);
	typedef BR_RESULT (*BRF_Do_Read)(BR_HANDLE handle ,
			int startChannel,
			int channelCount,
			unsigned int *data);
	typedef BR_RESULT (*BRF_Do_Write)(BR_HANDLE handle ,
			int startChannel,
			int channelCount,
			unsigned int data);
	typedef int 	(*BRF_Get_DICount)();
	typedef int 	(*BRF_Get_DOCount)();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // BOARD_RESOURCE
