
#if defined(AMCMQ200N)
	#define COMPANY_NAME					"SAMSUNG"
	#define LEGAL_COPYRIGHT					"Copyright (C) 2017, SAMSUNG Corp"
	#define TAGLINK_STUDIO_PRODUCT_NAME		"SAMSUNG S-NET Studio"
	#define TAGLINK_STUDIO_PRODUCT_VERSION	"2.1.0"

	#define DEV_ADAM3600_CFGDLL_NAME		"DevACMQ200N#.dll"
	#define DEV_ADAM3600_DRIVER_NAME		"ACMQ200N"
	#define DEV_ADAM3600_PROTOCOL_NAME		"ACM-Q200N"

	#define DRIVERADPATER_FILE_NAME			"SNetStudio.DriverAdapter"

	#define PORT_PRJMGR						6100
	#define PORT_DEVMGR						6110
	#define PORT_ADSP						6613
	#define PORT_ICDMGR						7100

#elif ( defined(SYS800) || defined(SYS800021) )
	#define COMPANY_NAME					"SYSnet"
	#define LEGAL_COPYRIGHT					"Copyright (C) 2017, SYSnet Corp"
	#define TAGLINK_STUDIO_PRODUCT_NAME		"SYSnet SYS-800 Tool"
	#define TAGLINK_STUDIO_PRODUCT_VERSION	"2.1.0"

	#define DEV_ADAM3600_CFGDLL_NAME		"DevSYS800#.dll"
	#define DEV_ADAM3600_DRIVER_NAME		"SYS800"
	#define DEV_ADAM3600_PROTOCOL_NAME		"SYS-800"

	#define DRIVERADPATER_FILE_NAME			"SYSnet.DriverAdapter"


	#define PORT_PRJMGR						6200
	#define PORT_DEVMGR						6210
	#define PORT_ADSP						6713
	#define PORT_ICDMGR						7200

#elif defined(TMS10)
	#define COMPANY_NAME					"TelChina"
	#define LEGAL_COPYRIGHT					"Copyright (C) 2017, TelChina"
	#define TAGLINK_STUDIO_PRODUCT_NAME		"TelChina TMS10 Tools"
	#define TAGLINK_STUDIO_PRODUCT_VERSION	"2.1.0"

	#define DEV_ADAM3600_CFGDLL_NAME		"DevTMS10#.dll"
	#define DEV_ADAM3600_DRIVER_NAME		"TMS10"
	#define DEV_ADAM3600_PROTOCOL_NAME		"TMS10"

	#define DRIVERADPATER_FILE_NAME			"TelChina.DriverAdapter"

	#define PORT_PRJMGR						6300
	#define PORT_DEVMGR						6310
	#define PORT_ADSP						6813
	#define PORT_ICDMGR						7300

#elif ( defined(SSNC) || defined(ICG1110) || defined(ICG1120) )
	#define COMPANY_NAME					"SSNC"
	#define LEGAL_COPYRIGHT					"Copyright (C) 2017, SSNC"
	#define TAGLINK_STUDIO_PRODUCT_NAME		"SSNC ICG"
	#define TAGLINK_STUDIO_PRODUCT_VERSION	"2.1.0"

	#define DEV_ADAM3600_CFGDLL_NAME		"DevICG#.dll"
	#define DEV_ADAM3600_DRIVER_NAME		"ICG"
	#define DEV_ADAM3600_PROTOCOL_NAME		"ICG"

	#define DRIVERADPATER_FILE_NAME			"ICG.DriverAdapter"

	#define PORT_PRJMGR						60011
	#define PORT_DEVMGR						60013
	#define PORT_ADSP						60015
	#define PORT_ICDMGR						60017

#else
	#define COMPANY_NAME					"Advantech"
	#define LEGAL_COPYRIGHT					"Copyright (C) 2017, Advantech Corp"
	#define TAGLINK_STUDIO_PRODUCT_NAME		"Advantech TagLink Studio"
	#define TAGLINK_STUDIO_PRODUCT_VERSION	"2.2.0"

	#define DEV_ADAM3600_CFGDLL_NAME		"DevADAM3600#.dll"
	#define DEV_ADAM3600_DRIVER_NAME		"ADAM3600"
	#define DEV_ADAM3600_PROTOCOL_NAME		"ADAM-3600"

	#define DRIVERADPATER_FILE_NAME			"AdvRTUStudio.DriverAdapter"

	#define PORT_PRJMGR						6000
	#define PORT_DEVMGR						6010
	#define PORT_ADSP						6513
	#define PORT_ICDMGR						7000

	#define DEVICE_HAS_BOARDIO				1

#endif

#if ( defined(AMCMQ200N) \
	|| defined(SSNC) \
	|| defined(SYS800) \
	|| defined(TMS10) \
	|| defined(ADAM3600) \
	|| defined(ADAM3600DS) )

	#define SRAM_CHECK_NODE					"/dev/sram"

#endif 


#if ( defined(ECU1051) \
	|| defined(ECU1050) )
	#define SD_CARD_PATH					"/media/mmcblk0p1"
	#define SDCARD_POS 						"/sys/class/block/mmcblk0/size"
#else
	#define SD_CARD_PATH					"/media/mmcblk1p1"
	#define SDCARD_POS 						"/sys/class/block/mmcblk1/size"
#endif


#define AUTHORIZED_DEVICE_LIST \
	"ADAM3600", \
	"ADAM3600DS", \
	"ECU1152", \
	"ECU4552", \
	"ECU4553", \
	"SYS800", \
	"SYS800021", \
	"AMCMQ200N", \
	"ECU1251", \
	"TMS10", \
	"ICG1110", \
	"ICG1120", \
	"ECU1051", \
	"ECU1050"

