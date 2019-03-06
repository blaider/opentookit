// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2014, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:    DataLoggerSDK.cpp
// Author:  Liu Kun
// Created: 11/27/2014   16:44
//
// Description:  DataLoggerSDK class implementation file.
// -----------------------------------------------------------------------------



#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
//#include <errno.h>
#include <sqlite3.h>
#include <list>
#include <dirent.h>
#include <libxml/parser.h>
#include <iostream>
#include <time.h>
#include <sys/mman.h>

using namespace std;

#ifdef __cplusplus
#define EXTERN_C						extern "C"
#else
#define EXTERN_C
#endif /* __cplusplus */


// Declare statistics enum type
enum STATISTICS_TYPE
{
	MINUTE 	= 0, 	// minute statistics
	HOUR 	= 1,  	// hour statistics
	DAY 	= 2  	// day statistics
};

//
#ifndef DB_HEADER_NAME
#define DB_HEADER_NAME		"DB_Data"
#endif

#ifndef DB_EXT_NAME
#define DB_EXT_NAME			"adb"
#endif

#ifndef DB_SD_DIR
#define DB_SD_DIR			"/media/mmcblk1p1/Data/"
#endif

#ifndef DB_SD_PATH
#define DB_SD_PATH			"/media/mmcblk1p1"
#endif

#define DB_MEDIA			"/media"

#ifndef max
#define max(a,b)			(((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)			(((a) < (b)) ? (a) : (b))
#endif

//
#define DBFILE_EXT			".adb"
#define DBFILE_EXT_LENGTH	strlen (DBFILE_EXT)
#define DBFILE_MIN_START 	"DB_Data_19700101.adb"
#define DBFILE_MAX_END	 	"DB_Data_29991231.adb"
#define DBFILE_LENGTH		strlen ("DB_Data_XXXXXXXX.adb")

#define TRY_COUNT		 	20 		// Try operation count
#define SLEEP_TIME		 	10000 	// Used for usleep() to sleep 10 milliseconds

// The max count of tables in database
#define MAX_TBL_COUNT		3000

//
#define BUSY_TIMEOUT		3000 	// Used for sqlite3_busy_timeout();

//
#ifndef SECONDS_PER_DAY
#define SECONDS_PER_DAY 	86400 	// 1 * 24 * 60 *60
#endif

//
//static sqlite3 	* pDBTag = NULL; 	// SQLite handle for current opened DB_Data_xxxxxxxx.db3.

static char 	cur_db_name[ APAL_NAME_MAX ] = { 0 };
//char 			g_indexTable[64] = "T_Index_Table";

//
static int isSDKInitialized = 0;

//
//pthread_mutex_t 	g_tagMutex;
//APAL_HANDLE 		g_semaphoreTag;
//APAL_HANDLE			g_tags_crit_sect;

//
#ifndef PROJECT_PATH
#define PROJECT_PATH			"/home/root/project"
#endif

#ifndef CONFIG_FILE_NAME
#define CONFIG_FILE_NAME		"DataLogger.acr"
#endif

//
#define XELEM_TAGLOGGER			"TagLogger"
#define XELEM_LOGTAGLIST		"LogTagList"
#define XELEM_LOGTAG			"LogTag"

#define XATTR_TAGNAME			"TagName"
#define XATTR_TYPE				"Type"
#define XATTR_LOGTYPE			"logType"

#define XELEM_DATABASE			"Database"
#define XATTR_STORAGEPATH		"StoragePath"
#define XATTR_PERIOD			"Period"
#define XATTR_GROUP_PERIOD		"period"

//
string 	g_StoragePath;
char 	dbDir[APAL_PATH_MAX] = {0};

#if 0
//
typedef struct _INSERT_DATA
{
	APAL_HANDLE htbl;
	APAL_HANDLE key_list;
}INSERT_DATA, *PINSERT_DATA;
INSERT_DATA g_insert_data;

#define STATISTIC_TYPE_COUNT 		3
INSERT_DATA g_st_insert_data[STATISTIC_TYPE_COUNT];
#endif

//
#define MAX_TBL_NAME_LEN 			128
#define MAX_DB_PATH_LEN 			512
#define MAX_SQL_CMD_LEN 			512
#define MAX_DB_INS_COUNT			65536
#define MAX_LIST_SIZE				3000
#define MIN_VAL_INSERT_COUNT		100
#define MAX_STAT_VAL_LIST_SIZE		30
#define MIN_TRANS_COUNT				200
#define MAX_TRANS_COUNT				400
#define TAG_UPDATE_TIMEOUT			10 		// Timeout seconds for update value
#define SEC_PER_HOUR 				3600	// Seconds per hour: 60 * 60
#define SEC_PER_DAY 				86400 	// Seconds per day: 24 * 60 * 60
//#define MAX_QUERY_TIME_LEN 			(1 * SEC_PER_HOUR -1) // 1 Hour
#define MAX_QUERY_TIME_LEN 			(1 * SECONDS_PER_DAY -1) // 1 Day

#ifndef APAL_NAME_MAX
#define APAL_NAME_MAX				108
#endif

// For thread loop_cycle and timeout
#define UPDATE_THREAD_LOOP_CYCLE	1000
#define INSERT_THREAD_LOOP_CYCLE	0
#define THREAD_WAIT_TIMEOUT			5000
#define INSERT_TIMEOUT_VAL			5

//
enum ins_event_flag
{
	EVENT_INS_TAG			= 1,
	EVENT_INS_STATM_TAG		= 2,
	EVENT_INS_STATH_TAG		= 4,
	EVENT_INS_STATD_TAG		= 8,
	EVENT_INS_FINISHED		= 16,
};
typedef enum ins_event_flag		INS_EVENT_FLAG;

#if 0
//
typedef struct _STATISTICS_DATA
{
	// Propressing data to save
	//STATISTICS_VALUE value;

	// Started to statistics
	//int16_t started;

	// Tag value count, used to calculate average value
    //uint32_t value_count;

	// Tag time stamp, in UTC format
    uint64_t time;

	// Data integrity, 0: complete, 1: partial
    int16_t partial;

	//int statisticsType;

	// Tag quality, negative value mean exception
	int16_t quality;

	// The last tag value
	double last_value;

	// The minimum tag value
	double min_value;

	// The maximum tag value
	double max_value;

	// The average tag value
	double avg_value;


}STATISTICS_DATA, *PSTATISTICS_DATA;
#endif

//
struct ins_tag_ref
{
	char 		tag_name[ APAL_NAME_MAX ];			// Tag name
	uint32_t 	log_type;						// Log type. 0: Period; 1: Run-time
	uint32_t 	period;						// Update period (in seconds)
	uint32_t 	min_ins_cnt;					// Minimum count to log to database
	uint64_t 	final_update_time;				// Minimum count to log to database
	APAL_HANDLE value_list;					// TAG_VALUE value list

	uint32_t 	hv_index;			// header value index
	int32_t 	log_id;			// value block id
	uint32_t 	start_offset;
	uint32_t 	end_offset;

	APAL_HANDLE statm_value_list;			// MINUTE STAT_VALUE value list
	APAL_HANDLE stath_value_list;			// HOUR STAT_VALUE value list
	APAL_HANDLE statd_value_list;			// DAY STAT_VALUE value list

	uint32_t 	wr_count;				//
	uint32_t 	stam_wr_count;			//
	uint32_t 	stah_wr_count;			//
	uint32_t 	stad_wr_count;			//
};
typedef struct ins_tag_ref INS_TAG_REF;

//
struct ins_data
{
	char db_name[ DBFILE_LENGTH + 1 ]; 	// Database name (DB_Data_xxxxxxxx.db3)
	//sqlite3 * pDb; 					// SQLite handle for current opened DB.

	uint32_t tag_count;				// Tag counts
	uint32_t ins_counts;			// Insert counts
	int32_t  event_flag;			// Event flags
	uint32_t min_trans_count;		// Minimum value counts to end transaction
	APAL_HANDLE thread_insert;
	APAL_HANDLE thread_update;
	APAL_HANDLE rwlock;
	APAL_HANDLE event_sem;
	APAL_HANDLE tag_ref_list;		// list of tag INS_TAG_REF*
	//APAL_HANDLE rt_tag_ref_list;	// list of run-time tag INS_TAG_REF*
	APAL_HANDLE timeout_update_list; // list of run-time timeout tag INS_TAG_REF*

	APAL_HANDLE tags_update_list;	// list of updated INS_TAG_REF*
	APAL_HANDLE statm_update_list;	// list of updated MINUTE statistics INS_TAG_REF*
	APAL_HANDLE stath_update_list;	// list of updated HOUR statistics INS_TAG_REF*
	APAL_HANDLE statd_update_list;	// list of updated DAY statistics INS_TAG_REF*

	APAL_HANDLE tag_ref_htbl; 		// hash table for each tag INS_TAG_REF
	int 		is_init;
	//bool		db_begin_trans;

	int 		fd;
	//APAL_HANDLE sm_header;			// list of updated INS_TAG_REF*
	void 		*dbf_header;//FILE_HEADER *dbf_header;
	bool 		is_new_file ;
	APAL_HANDLE tag_ref_index_htbl; 	// hash table for each tag INS_TAG_REF
	APAL_HANDLE tag_ref_index_list; 	// hash table for each tag INS_TAG_REF
};
typedef struct ins_data INS_DATA;

APAL_HANDLE 	g_ins_db_htbl = APAL_INVALID_HANDLE; 	// hash table for each INS_DATA
APAL_HANDLE 	g_ins_db_key_list = APAL_INVALID_HANDLE; 	// key list for INS_DATA

APAL_HANDLE 	g_tag_list = APAL_INVALID_HANDLE;		// list of tag
int 			g_time_zone = 0;
static int 		is_init_for_insert = 0;
FILE_HEADER* 	cur_ins_header = NULL;

//
struct query_data
{
	char 		db_name[ DBFILE_LENGTH + 1 ]; 	// Database name (DB_Data_xxxxxxxx.db3)
	int 		fd;
	uint32_t 	tag_count;				// Tag counts
	uint32_t 	ins_counts;				// Insert counts
	int32_t  	event_flag;				// Event flags
	uint32_t 	min_trans_count;		// Minimum value counts to end transaction
	APAL_HANDLE thread_insert;
	APAL_HANDLE thread_update;
	APAL_HANDLE rwlock;
	APAL_HANDLE event_sem;
	APAL_HANDLE tag_ref_list;			// list of tag INS_TAG_REF*
	APAL_HANDLE tag_ref_htbl; 			// hash table for each tag INS_TAG_REF
	int 		is_init;
	void 		*dbf_mem;				// file mapping of the database file;
	APAL_HANDLE tag_ref_index_htbl; 	// hash table for each tag INS_TAG_REF
	APAL_HANDLE tag_ref_index_list; 	// hash table for each tag INS_TAG_REF
};
typedef struct query_data QUERY_DATA;

// Log Type
enum LOG_TYPE
{
	PERIOD_SAVING		= 0,
	RUNTIME_SAVING_C	= 1,
	RUNTIME_SAVING_T	= 2,
};


//#define SQL_TRANS_BEGIN		"BEGIN TRANSACTION"
//#define SQL_TRANS_END			"COMMIT TRANSACTION"
#define DB_OPEN_READONLY         0x00000001
#define DB_OPEN_READWRITE        0x00000002
#define DB_OPEN_CREATE           0x00000004

#define DB_QUERY_OPEN_FLAG  	DB_OPEN_READONLY | O_LARGEFILE | O_BINARY
#define DB_INSERT_OPEN_FLAG  	DB_OPEN_READWRITE | DB_OPEN_CREATE | O_LARGEFILE | O_BINARY

//
typedef struct _Log_Tag
{
	char 	 name[APAL_NAME_MAX];
	uint32_t period;	// Update(Read/Write) period (in seconds)
	uint32_t log_type;	// Log type. 0: Period logging; 1/2: Run-time logging
}LOG_TAG, *PLOG_TAG;
int16_t 		last_id = -1;


// Structure for query tag value
typedef struct _QUERY_DB
{
	//sqlite3_stmt 	* stmt; 	// SQL Statement Object
	//sqlite3 		* pDB;		// SQLite handle;
	//int 			columns ;

	char 		db_name[ DBFILE_LENGTH + 1 ]; 	// Database name (DB_Data_xxxxxxxx.db3)
	int 		fd;

	int32_t 	id;
    int8_t 		type;
    //uint8_t 	size;

    uint32_t 	start_offset;
    uint32_t 	end_offset;

	uint64_t 	start_time;
	uint64_t 	end_time;

	void		* value;
	uint32_t 	rd_offset;
	uint32_t 	map_size;
}QUERY_DB, *PQUERY_DB;

// QUERY_DB list
typedef list<QUERY_DB *> QUERY_DB_LIST;

//
typedef struct _QUERY_RESULT
{
	char tag_name[ APAL_NAME_MAX ];			// Tag name
	uint64_t 				cur_end_time;	// Start time stamp.
	uint64_t 				end_time;		// End time stamp.
	QUERY_DB_LIST 			queryList;
	QUERY_DB_LIST::iterator curIt;
}QUERY_RESULT, *PQUERY_RESULT;

struct dlsdk_data_s
{
	//APAL_HANDLE rwlock;
	//uint32_t max_tag_count;
	//uint32_t max_dc_tag_count;
	//DC_TAG_CALLBACK callback;
	//void * user_context;
	APAL_HANDLE 	thread;
	//APAL_HANDLE ipc;
	//APAL_HANDLE shmem;
	//APAL_HANDLE shmem_rwlock;
	//TAG_DATA * shmem_tags;
	//APAL_HANDLE tag_ref_list;
	//TAG_REF ** tags_added;
	//TAG_REF ** tags_opened;
	//APAL_HANDLE tags_connected;		// list of TAG_REF*
	//APAL_HANDLE tags_unconnected;	// list of TAG_REF*
	//APAL_HANDLE tags_in_retry;		// list of TAG_REF*
	//APAL_HANDLE event_sem;
	//int32_t event_flag;
	//APAL_HANDLE update_list;
	//int32_t revision;	// updated each time connection status changed.
	//DC_SUBSCRIPTION_DATA * subscr;
	//bool delay_operation;
	//uint32_t last_read_tick;		// last tick when DataCenter read tags
	//uint32_t last_add_tick;			// last tick when user add tags

	APAL_HANDLE 	ins_db_htbl; 	// hash table for each INS_DATA
	APAL_HANDLE 	ins_db_key_list; 	// key list for INS_DATA

	//APAL_HANDLE 	tag_list;		// list of tag
	APAL_HANDLE 	query_db_htbl; 	// hash table for each INS_DATA
	APAL_HANDLE 	query_db_key_list; 	// key list for INS_DATA
};
typedef struct dlsdk_data_s	DLSDK_DATA;
static DLSDK_DATA * dlsdk_data = NULL;


static bool AddLogTag( char const * tagName, int32_t period, uint32_t log_type );
static int GetTimezone( void );


static int DBFileOpen( const char *path, int flags );
static int DBFileClose( int fd, bool sync );
static int DBFileSync( int fd );
static int32_t DBFileWriteValue(
		int fd, const void *buf, uint32_t size, uint32_t *start_offset );

// -------------------------------------------------------------------------
// Design Notes:
//     Open and possibly create a file.
//
// Parameters:
//		path: [in] Path for a file
//		flags: [in]
//
// Returns:
//		int: File descriptor, successful; negative: failed/error code.
//
// Remarks:
// -------------------------------------------------------------------------
static int DBFileOpen(const char *path, int flags)
{
    int fd = -1;

    fd = open(path, flags);
    if (fd == -1)
    {
    	printf("%s: open(%s, 0x%X) errno: %d\n", __func__, path, flags, errno);
    	return -errno;
    }

	return fd;
}

// -------------------------------------------------------------------------
// Design Notes:
//     Close the database file. https://linux.die.net/man/2/open
//
// Parameters:
//		fd: [in] Path for a file
//		sync: [in]
//
// Returns:
//		int. nonnegative integer: File descriptor, successful; negative: failed/error code.
//
// Remarks:
// 		The argument flags must include one of the following access modes:
// 		O_RDONLY, O_WRONLY, or O_RDWR. These request opening the file
// 		read-only, write-only, or read/write, respectively.
// -------------------------------------------------------------------------
static int DBFileClose( int fd )
{
    //
    if (fd == -1)
    {
        return -1;
    }

    close(fd);
    fd = -1;

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  synchronize a file.
//
// Parameters:
//		fd: 	[in] An open file descriptor.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
static int DBFileSync( int fd)
{
	int rc = 0;

	// sync file
	rc = fsync(fd);
    if( rc == -1 )
    {
    	printf("SDK::%s: fsync failed, errno: %d\n", __func__, errno);
    	return -errno;
    }
    return rc;
}

// -------------------------------------------------------------------------
// Design Notes:
//      writes up to size bytes from the buffer to the file.
//
// Parameters:
//		fd: 		  [in] file descriptor.
//		buf: 		  [in] buffer.
//		size: 		  [in] count bytes.
//		start_offset: [out] The start offset to write.
//
// Returns:
//		int32_t.
//		On success, the number of bytes written is returned.
//
// Remarks:
// -------------------------------------------------------------------------
static int32_t DBFileWriteValue(int fd, const void *buf, uint32_t size, uint32_t *start_offset)
{
    //printf("SDK::%s: fd: %d, size: %u\n", __func__, fd, size);

    //
    if ((fd == -1) || (buf == NULL))
    {
        return -1;
    }

	ssize_t count = 0;
    off_t offset = lseek( fd, 0, SEEK_END);
    *start_offset = (uint32_t)offset;
    //printf("SDK::%s: SEEK_END offset: %ld, start_offset: %u\n", __func__, offset, *start_offset);

    count = write( fd, buf, size);
    if( count <= 0 )
    {
    	printf("SDK::%s: write failed, count: %d, errno: %d", __func__, count, errno);
    }
    printf("SDK::%s: fd: %d, size: %u, return write size: %d, start_offset: %u\n", __func__, fd, size, count, *start_offset);

	return count;
}

// -------------------------------------------------------------------------
// Design Notes:  Load DataLogger configuration information.
//
// Parameters:
//		No.
// Returns:
//		int. 0: successful; nonzero: failed/error code.
// -------------------------------------------------------------------------
static int LoadConfiguration( void )
{
	char * config_file = NULL;
	char file_path[ APAL_PATH_MAX + 1 ];

	APAL_LOG_TRACE( "SDK::%s\n", __func__ );

	// load configuration file
	// file name: "/home/root/project/DataLogger.acr"
	apal_mem_clear( file_path, sizeof( file_path ) );

	// parse xml file
	if ( NULL == config_file )
	{
		char const * projdir = apal_sys_get_project_directory();
		projdir = projdir != NULL ? projdir : PROJECT_PATH;
		apal_str_printf(
			file_path,
			sizeof( file_path ),
			"%s/%s",
			projdir,
			CONFIG_FILE_NAME );
		config_file = file_path;
	}
	if ( apal_file_access( config_file, APAL_FILE_ACCESS_MODE_READ ) != 0 )
	{
		APAL_LOG_ERROR(
			"SDK::%s: failed to open file: %s! error = %d\n",
			__func__, config_file, errno );
		return -6;
	}

	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;

	xmlInitParser();

	//
	// parse an XML file and build a tree from a the file;
	// File("/home/root/project/DataLogger.acr").
	//
	doc = xmlParseFile( config_file );
	if (doc == NULL)
	{
		// Document not parsed successfully.
		APAL_LOG_ERROR( "SDK::%s: Failed to load file (%s)!\n",
				__func__, config_file );
		return -1;
	}

	//
	// Check the document is of the right kind
	//

	// Get the root element of the document
	root = xmlDocGetRootElement( doc );
	if (root == NULL)
	{
		// Document is empty
		APAL_LOG_ERROR(
				"SDK::%s: Root Element is NULL, empty document!\n", __func__ );
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return false;
	}

	if ( apal_str_cmp( (char const *)root->name, (char const *)XELEM_TAGLOGGER ) != 0 )
	{
		APAL_LOG_ERROR(
				"SDK::%s: Wrong root type (\"%s\"), \"%s\" not found!\n",
				__func__, root->name, XELEM_TAGLOGGER );
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return -1;
	}

	xmlNodePtr Config = NULL;
	Config = root->xmlChildrenNode;
	if (Config == NULL)
	{
		// Top level element is null.
		APAL_LOG_ERROR(
				"SDK::%s: Config is NULL! Top level element is null!\n", __func__ );
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return -1;
	}

	//
	// Now, walk the tree.
	//

	uint32_t defPeriod = 1;

	//
	while (Config != NULL)
	{
		char * stopstring;
		if ( apal_str_cmp( (char const *)Config->name, (char const *)XELEM_DATABASE ) == 0 )
		{
			//
			// Get attribute: "StoragePath"
			//
			xmlChar * path;
			path = xmlGetProp( Config, (xmlChar *) XATTR_STORAGEPATH );
			if( path != NULL )
			{
				g_StoragePath.clear();
				g_StoragePath.append((char *)path );
			}
			//
			// Get attribute: "Period"
			//
			xmlChar * defPeriodStr;
			defPeriodStr = xmlGetProp(Config, (xmlChar *) XATTR_PERIOD);
			if(defPeriodStr != NULL)
			{
				// Convert a string to a long value;
				defPeriod = strtoul( (char const *)defPeriodStr, &stopstring, 10 );
			}
		}
		// First level: "LogTagList"
		else if ( apal_str_cmp( (char const *)Config->name, (char const *)XELEM_LOGTAGLIST ) == 0 )
		{
			uint32_t log_type;
			int32_t period;

			//
			// Get attribute: "logType"
			//
			xmlChar * strLogType;
			strLogType = xmlGetProp( Config, (xmlChar *) XATTR_LOGTYPE );
			if( strLogType != NULL )
			{
				log_type = strtoul( (char const *)strLogType, &stopstring, 10 );
			}
			else
			{
				log_type = 0;
			}

			//
			// Get attribute: "period"
			//
			xmlChar * strPeriod;
			strPeriod = xmlGetProp(Config, (xmlChar *) XATTR_GROUP_PERIOD);
			if(strPeriod != NULL)
			{
				period = strtol( (char const *)strPeriod, &stopstring, 10 );
			}
			else
			{
				period = defPeriod;
			}

			xmlNodePtr child;
			child = Config->xmlChildrenNode;

			while (child != NULL)
			{
				// Second level: "LogTag"
				if ( apal_str_cmp( (char const *)child->name, (char const *)XELEM_LOGTAG ) == 0 )
				{
					//
					// Get attribute: "TagName"
					//
					xmlChar * tagName;
					tagName = xmlGetProp(child, (xmlChar *) XATTR_TAGNAME);
					if(tagName != NULL)
					{
						AddLogTag( (char const *)tagName, period, log_type );
					}

				} // End of Second level: "LogTag"

				child = child->next;

			} // End of while (child != NULL)

		}// End of First level: "LogTagList"

		Config = Config->next;

	} // End of while (Config != NULL)

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 0;
}
// -------------------------------------------------------------------------
// Design Notes:  Load DataLogger configuration information.
//
// Parameters:
//		No.
// Returns:
//		int. 0: successful; nonzero: failed/error code.
// -------------------------------------------------------------------------
static int CheckStorePath( void )
{
	APAL_LOG_TRACE( "SDK::%s\n", __func__ );

	//
	int rc = 0;
	int ret = 0;
	char command[APAL_PATH_MAX] = {0};

	// Check the StoragePath is valid or not
	if ( g_StoragePath.size() == 0 )
	{
		APAL_LOG_ERROR( "%s: StoragePath is empty.\n", __func__ );
		return DATALOGGER_ERROR_GET_CONFIG;
	}

	char firstChar = g_StoragePath[0];
	if ( strcmp((char *)g_StoragePath.c_str(), "1") == 0 ) // StoragePath is "1"
	{
		// Checks whether the calling process can access the DB_SD_PATH.
		rc = access( DB_SD_PATH, F_OK);
		if( rc != 0 )
		{
			APAL_LOG_ERROR( "%s: SD Card is not exist.\n", __func__ );
			return DATALOGGER_ERROR_GET_CONFIG;
		}
		apal_str_printf(dbDir, APAL_PATH_MAX, "%s", DB_SD_DIR);
		apal_str_printf(command, APAL_PATH_MAX, "mkdir -p %s", DB_SD_DIR);
	}
	else if (firstChar == '/' ) // StoragePath is begin with '/'
	{
		uint32_t len = strlen(DB_MEDIA);

		// Check whether the StoragePath is begin with "/media"
		if ( strncmp(g_StoragePath.c_str(), DB_MEDIA, len) != 0)
		{
			// StoragePath is not begin with "/media", create the StoragePath
			apal_str_printf(dbDir, APAL_PATH_MAX, "%s/", g_StoragePath.c_str());
			apal_str_printf(command, APAL_PATH_MAX, "mkdir -p %s", g_StoragePath.c_str());
		}
		else // StoragePath is begin with "/media"
		{
			if( g_StoragePath.size() < len+2 )
			{
				// The StoragePath is "/media" or "/media/"
				APAL_LOG_ERROR( "%s: The specified StoragePath is invalid.\n", __func__ );
				return DATALOGGER_ERROR_GET_CONFIG;
			}
			else
			{
				int pos = g_StoragePath.find_first_of('/', len+1 );
				if( pos == -1 )
				{
					rc = access( g_StoragePath.c_str(), F_OK);
					if( rc != 0 )
					{
						APAL_LOG_ERROR( "%s: The specified database path is not exist.\n", __func__ );
						return DATALOGGER_ERROR_GET_CONFIG;
					}
					else
					{
						apal_str_printf(
								dbDir, APAL_PATH_MAX, "%s/", g_StoragePath.c_str());
						apal_str_printf(
								command, APAL_PATH_MAX, "mkdir -p %s", g_StoragePath.c_str());
					}
				}
				else
				{
					// "/media/xxx"
					string sub_str = g_StoragePath.substr( 0, pos );
					rc = access( sub_str.c_str(), F_OK);
					if( rc != 0 )
					{
						APAL_LOG_ERROR( "%s: The specified database path is not exist.\n", __func__ );
						return DATALOGGER_ERROR_GET_CONFIG;
					}
					else
					{
						apal_str_printf(dbDir, APAL_PATH_MAX, "%s/", g_StoragePath.c_str());
						apal_str_printf(command, APAL_PATH_MAX, "mkdir -p %s", g_StoragePath.c_str());
					}
				}
			}
		}
	}
	else // StoragePath is not "1" or begin with '/'
	{
		APAL_LOG_ERROR( "%s: StoragePath is invalid.\n", __func__ );
		return DATALOGGER_ERROR_GET_CONFIG;
	}

	// Create Database Storage directory
	rc = system(command);
	if ( rc != 0 )
	{
		APAL_LOG_ERROR( "%s: Fail to create Database path. errno = %d\n", __func__, errno );
		if ( EPERM == errno )
		{
			return DATALOGGER_ERROR_CREATE_DBPATH_EPERM;
		}
		else if ( EACCES == errno )
		{
			return DATALOGGER_ERROR_CREATE_DBPATH_EACCES;
		}
		else if ( ENOTDIR == errno )
		{
			return DATALOGGER_ERROR_CREATE_DBPATH_ENOTDIR;
		}
		else if ( ENOSPC == errno )
		{
			return DATALOGGER_ERROR_CREATE_DBPATH_ENOSPC;
		}
		else if ( EROFS == errno )
		{
			return DATALOGGER_ERROR_CREATE_DBPATH_EROFS;
		}
		else
		{
			return DATALOGGER_ERROR_CREATE_DBPATH;
		}
	}

	return 0;
}
// -------------------------------------------------------------------------
// Design Notes:  Search DB_Data_xxxxxxxx.db3 name list, sorting by ASC.
//
// Parameters:
//		name_list: 	[out] Out put Database file name list.
//		startTime: 	[in] Start time stamp.
//		endTime: 	[in] End time stamp.
//
// Returns:
//		int. 0: successful; nonzero: failed/error code.
// -------------------------------------------------------------------------
static int FindDatabaseFile(
		APAL_HANDLE *name_list, uint64_t startTime, uint64_t endTime )
{
	printf(
			"SDK::%s: startTime: %llu, endTime: %llu\n",
			__func__, startTime, endTime );

	//
	time_t start_t = startTime;
	tm * tm_start;
	char startName[32] = {0};

	time_t end_t = endTime;
	tm * tm_end;
	char endName[32] = {0};

	//
	if( startTime > 0 )
	{
		tm_start = localtime(&start_t);
		apal_str_printf(
				startName, 32, "%s_%d%02d%02d.%s",	DB_HEADER_NAME,
				1900 + tm_start->tm_year, 1 + tm_start->tm_mon,
				tm_start->tm_mday, DB_EXT_NAME );
	}
	else
	{
		apal_str_printf( startName, 32, "%s", DBFILE_MIN_START);
	}
	if( endTime > 0 )
	{
		tm_end = localtime(&end_t);
		apal_str_printf(
				endName, 32, "%s_%d%02d%02d.%s", DB_HEADER_NAME,
				1900 + tm_end->tm_year, 1 + tm_end->tm_mon,
				tm_end->tm_mday, DB_EXT_NAME );
	}
	else
	{
		apal_str_printf( endName, 32, "%s", DBFILE_MAX_END);
	}
    printf( "SDK::%s: startName:%s, endName: %s\n", __func__, startName, endName);

    struct dirent **namelist;
    int file_count = -1;
    //file_count = scandir("/media/mmcblk1p1/Data/", &namelist, NULL, alphasort);
    file_count = scandir(dbDir, &namelist, NULL, alphasort);

    //printf( "SDK::%s: scandir(\"/media/mmcblk1p1/Data/\", &namelist, NULL, alphasort);\n",	__func__);
    printf( "SDK::%s: scandir dbDir: %s, file_count: %d\n", __func__, dbDir, file_count );

    if (file_count < 0)
    {
        return file_count;
    }

    *name_list = apal_list_create(file_count);
    if ( APAL_INVALID_HANDLE == *name_list )
    {
    	return -1;
    }

	int offset = 0;
	for ( int i = 0; i < file_count; i++ )
    {
		offset = strlen(namelist[i]->d_name)-DBFILE_EXT_LENGTH;
        int low = apal_str_casecmp( namelist[i]->d_name, startName );
        int high = apal_str_casecmp( namelist[i]->d_name, endName );
        if ((namelist[i]->d_type == DT_REG)
        		&& (low>=0)
        		&& (high<=0)
        		&& ( strlen (namelist[i]->d_name) == DBFILE_LENGTH )
        		&& ( apal_str_casecmp( namelist[i]->d_name + offset, DBFILE_EXT ) == 0 )
        		)
        {
        	apal_list_push_back( *name_list, namelist[i] );
        	namelist[i] = NULL;
        }

        if ( namelist[i] != NULL )
        {
            free(namelist[i]);
        }
    }

    free(namelist);

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  Initialize the DataLogger SDK.
//
// Parameters:
//		openAll: Open all existing database file. This param will be ignored.
//
// Details:
//		Before any other API can be used correctly, you must call this
//		function first to perform a runtime initialization.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int DataLoggerSDKInit( bool openAll = false )
{
	APAL_LOG_TRACE( "SDK::%s\n", __func__ );

	//
	int rc = 0;
	int ret = 0;
	char command[APAL_PATH_MAX] = {0};

	apal_init();

	//
	if ( isSDKInitialized == 1 )
	{
		return 0;
	}

	do
	{
		// Create tag list;
		g_tag_list = apal_list_create( 3000 );
		if ( APAL_INVALID_HANDLE == g_tag_list )
		{
			APAL_LOG_ERROR( "%s: Fail to create g_tag_list\n", __func__ );
			ret = DATALOGGER_ERROR_INIT;
			break; //return DATALOGGER_ERROR_INIT;
		}

		// Read the configuration
		char datalogger_user_name[ APAL_NAME_MAX ] = { 0 };
		apal_str_printf(
				datalogger_user_name, sizeof( datalogger_user_name ),
				"DataLoggerSDK_%s_%d", apal_process_get_short_name(), getpid() );

		rc = LoadConfiguration();
		if( rc != 0 )
		{
			APAL_LOG_ERROR( "%s: Fail to read the configuration file.\n", __func__ );
			ret = DATALOGGER_ERROR_GET_CONFIG;
			break; //return DATALOGGER_ERROR_GET_CONFIG;
		}
		//printf( "SDK::%s: g_StoragePath: %s\n",	__func__, (char *)g_StoragePath.c_str() );

		// Check the StoragePath is valid or not
		ret = CheckStorePath();
		if( ret != 0 )
		{
			APAL_LOG_ERROR( "%s: StoragePath is empty.\n", __func__ );
			break; //return DATALOGGER_ERROR_GET_CONFIG;
		}
		//printf( "SDK::%s: dbDir: %s\n",	__func__, dbDir );

		g_ins_db_htbl = apal_hash_create( MAX_DB_INS_COUNT );
		if ( APAL_INVALID_HANDLE == g_ins_db_htbl )
		{
			APAL_LOG_ERROR( "%s: Fail to create g_ins_db_htbl hash table!\n", __func__ );
			ret = DATALOGGER_ERROR_INS_INIT;
			break;
		}
		g_ins_db_key_list = apal_list_create( MAX_DB_INS_COUNT );
		if ( APAL_INVALID_HANDLE == g_ins_db_key_list )
		{
			APAL_LOG_ERROR( "%s: Fail to create g_ins_db_key_list hash table!\n", __func__ );
			ret = DATALOGGER_ERROR_INS_INIT;
			break;
		}//g_ins_db_key_list

		//
		isSDKInitialized = 1;

		g_time_zone = GetTimezone();

		return 0;

	} while ( 0 );

	DataLoggerSDKUninit ();

	return ret;
}

// -------------------------------------------------------------------------
// Design Notes:  Uninitialize the DataLogger SDK.
//
// Parameters:
//		No.
//
// Details:
//		Call this function before application exit to release all resources
//		allocated by the DataLogger SDK.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API void DataLoggerSDKUninit ( void )
{
	APAL_LOG_TRACE( "SDK::%s\n", __func__ );

	if(APAL_INVALID_HANDLE != g_ins_db_key_list)
	{
		while ( !apal_list_empty( g_ins_db_key_list ) )
		{
			uint64_t * timeStamp = (uint64_t *)apal_list_pop_front( g_ins_db_key_list );
			UninitInsertForLogger ( *timeStamp );
			APAL_MEM_SAFE_FREE( timeStamp );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( g_ins_db_key_list );

	//
	if(APAL_INVALID_HANDLE != g_tag_list)
	{
		while ( !apal_list_empty( g_tag_list ) )
		{
			LOG_TAG * logTag = (LOG_TAG *)apal_list_pop_front( g_tag_list );
			APAL_MEM_SAFE_FREE( logTag );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( g_tag_list );

	//
	APAL_SAFE_CLOSE_HANDLE( g_ins_db_htbl );

	//
	isSDKInitialized = 0;
	apal_uninit();
}

// -------------------------------------------------------------------------
// Design Notes:  Delete database file.
//
// Parameters:
//		startTime: 	[in] Start time stamp.
//		endTime: 	[in] End time stamp.
// Returns:
//		int. 0: successful; nonzero: failed.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int DeleteDatabaseFile(uint64_t startTime, uint64_t endTime)
{
	APAL_LOG_TRACE(
			"SDK::%s: startTime: %llu, endTime: %llu\n", __func__, startTime, endTime );

	//
	if( isSDKInitialized == 0 )
	{
		APAL_LOG_FATAL( "%s: The DataLoggerSDK has not been initialized!\n", __func__ );

		return DATALOGGER_ERROR_NOT_INITIAL;
	}

	//
	time_t curTime_t;
	tm * tmStamp;

	//
	time( &curTime_t );
	tmStamp = localtime(&curTime_t);

	//
	char curName[APAL_NAME_MAX] = { 0 };

	// The Tags Database naming rule: DB_Data_YYYYMMDD.db3
    apal_str_printf(
    		curName, APAL_NAME_MAX, "%s_%d%02d%02d.%s",
			DB_HEADER_NAME, 1900 + tmStamp->tm_year, 1 + tmStamp->tm_mon,
			tmStamp->tm_mday, DB_EXT_NAME );

    //
    uint64_t timeStamp = curTime_t;
	uint64_t day_elapsed = (timeStamp + g_time_zone * SEC_PER_HOUR ) / SEC_PER_DAY;

	char key[32] = {0};
    apal_str_printf( key, sizeof(key), "%llu", day_elapsed );

	//
	char delCmd[MAX_DB_PATH_LEN] = {0};

    APAL_HANDLE name_list = APAL_INVALID_HANDLE;
	FindDatabaseFile( &name_list, startTime, endTime);
	if( name_list == APAL_INVALID_HANDLE )
	{
		return 0;
	}

	//
    INS_DATA * ins_data = NULL;
    if( g_ins_db_htbl != APAL_INVALID_HANDLE )
    {
        apal_hash_find( g_ins_db_htbl, key, (void**) &ins_data );
    }

    while ( !apal_list_empty( name_list ) )
    {
    	//void * file_name = apal_list_pop_front( name_list );
    	struct dirent * dir = (struct dirent *) apal_list_pop_front( name_list );

    	// If the current database file is in use, don't delete this file.
#if 0
    	if( ( apal_str_cmp( dir->d_name, (char const *)curName ) == 0 )	&& (NULL != pDBTag ) )
#endif
    	if( apal_str_cmp( dir->d_name, (char const *)curName ) == 0 )
    	{
     		free( dir );
    		continue;
    	}
    	if( ins_data != NULL )
    	{
#if 0
    		if(( ins_data->pDb != NULL ) &&
#endif
    		if(( ins_data->fd != -1 ) &&
    				( apal_str_cmp( dir->d_name, (char const *)ins_data->db_name ) == 0 ) )
    		{
         		free( dir );
        		continue;
    		}
    	}

		// Delete the Database file form system
    	apal_str_printf (delCmd, MAX_DB_PATH_LEN, "rm -f %s%s", dbDir, dir->d_name);
		system( delCmd );
		apal_str_printf (delCmd, MAX_DB_PATH_LEN, "rm -f %s%s-journal", dbDir, dir->d_name);
		system( delCmd );
    	free( dir );
    }
    APAL_SAFE_CLOSE_HANDLE( name_list );

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  Delete the earliest database file to free disk space.
//
// Parameters:
//		No.
// Returns:
//		int. 0: successful; nonzero: failed.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int DeleteEarliestDatabaseFile( void )
{
	APAL_LOG_TRACE("SDK::%s\n", __func__ );

	//
	if( isSDKInitialized == 0 )
	{
		APAL_LOG_FATAL( "%s: The DataLoggerSDK has not been initialized!\n", __func__ );

		return DATALOGGER_ERROR_NOT_INITIAL;
	}

	//
	time_t curTime_t;
	tm * tmStamp;

	//
	time( &curTime_t );
	tmStamp = localtime(&curTime_t);

	//
	char curName[APAL_NAME_MAX] = { 0 };

	// The Tags Database naming rule: DB_Data_YYYYMMDD.db3
    apal_str_printf(
    		curName, APAL_NAME_MAX, "%s_%d%02d%02d.%s",
			DB_HEADER_NAME, 1900 + tmStamp->tm_year,
			1 + tmStamp->tm_mon, tmStamp->tm_mday, DB_EXT_NAME );

    //
	int ret = 0;
    APAL_HANDLE name_list = APAL_INVALID_HANDLE;
	FindDatabaseFile( &name_list, 0, 0);
	if( name_list == APAL_INVALID_HANDLE )
	{
		return 0;
	}

    if ( !apal_list_empty( name_list ) )
    {
    	// The first file is the earlist database file
    	struct dirent * dir = (struct dirent *) apal_list_pop_front( name_list );

    	if(apal_str_cmp( dir->d_name, (char const *)curName ) == 0)
    	{
        	//free( dir );
        	//return -1;
        	ret = -1;
    	}
    	else
    	{
    		// Delete the earliest Database file form system
        	char delCmd[MAX_DB_PATH_LEN] = {0};
        	apal_str_printf (delCmd, MAX_DB_PATH_LEN, "rm -f %s%s", dbDir, dir->d_name);
    		system( delCmd );
    		apal_str_printf (delCmd, MAX_DB_PATH_LEN, "rm -f %s%s-journal", dbDir, dir->d_name);
    		system( delCmd );
    	}

    	free( dir );
    }

    while ( !apal_list_empty( name_list ) )
    {
    	struct dirent * dir = (struct dirent *) apal_list_pop_front( name_list );
    	free( dir );
    }
    APAL_SAFE_CLOSE_HANDLE( name_list );

	return ret;
}

// -------------------------------------------------------------------------
// Design Notes:  Insert record into the database.
//
// Parameters:
//		pTagName: 	[in] Tag name.
//		value: 		[in] Tag value.
//		quality: 	[in] Tag quality.
//		timeStamp: 	[in] Tag time stamp, in UTC format.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int InsertTagValue(
		char const * pTagName,
		double value,
		int16_t quality,
		uint64_t timeStamp )
{
	DC_TAG tag_value = {0};
	tag_value.time = timeStamp;
	tag_value.quality = quality;
	tag_value.value = value;

	return InsertTagValueV2 ( pTagName, &tag_value );
}

// -------------------------------------------------------------------------
// Design Notes:  Find the earliest saved database file and
// 				  compute the save time (seconds).
//
// Parameters:
//		earliest_time: 	[out] The earliest database save time.
// Returns:
//		int. 0: successful; nonzero: failed.
// -------------------------------------------------------------------------
static int GetEarliestSaveTime( uint64_t * earliest_time )
{
	APAL_LOG_TRACE("SDK::%s\n", __func__ );

	//
	time_t curTime_t;
	tm * tmStamp;

	//
	time( &curTime_t );
	tmStamp = localtime(&curTime_t);

	//
	char earliest_name[APAL_NAME_MAX] = { 0 };

	// The Tags Database naming rule: DB_Data_YYYYMMDD.db3
    apal_str_printf(
    		earliest_name, APAL_NAME_MAX, "%s_%d%02d%02d.%s",
			DB_HEADER_NAME, 1900 + tmStamp->tm_year,
			1 + tmStamp->tm_mon, tmStamp->tm_mday, DB_EXT_NAME );

    //
    APAL_HANDLE name_list = APAL_INVALID_HANDLE;
	FindDatabaseFile( &name_list, 0, 0);
	if( name_list == APAL_INVALID_HANDLE )
	{
		return 0;
	}

    while ( !apal_list_empty( name_list ) )
    {
    	struct dirent * dir = (struct dirent *) apal_list_pop_front( name_list );
    	if ( dir != NULL )
    	{
         	if(apal_str_cmp( dir->d_name, earliest_name ) < 0)
        	{
        		apal_mem_clear( earliest_name, sizeof( earliest_name ) );
        		apal_str_cpy( earliest_name, dir->d_name, sizeof( earliest_name ) );
        	}
    	}
    	free( dir );
    }
    APAL_SAFE_CLOSE_HANDLE( name_list );

	//
	char str_year[DBFILE_LENGTH] = {0};
	char str_mon[DBFILE_LENGTH] = {0};
	char str_day[DBFILE_LENGTH] = {0};

	//
    apal_str_printf( str_year, strlen("1900")+1, "%s", (char *)earliest_name+8 );
    apal_str_printf( str_mon, strlen("01")+1, "%s", (char *)earliest_name+12 );
    apal_str_printf( str_day, strlen("01")+1, "%s", (char *)earliest_name+14 );

    struct tm tmDate = {0};
    tmDate.tm_year = atoi(str_year) - 1900;
    tmDate.tm_mon = atoi(str_mon) - 1;      // Month: start from 0
    tmDate.tm_mday = atoi(str_day);         // Day: start from 1
    tmDate.tm_hour = 0;
    tmDate.tm_min = 0;
    tmDate.tm_sec = 0;
    time_t earliest_tm = mktime(&tmDate);

    //
    *earliest_time = (uint64_t)earliest_tm;
    printf( "SDK:%s: earliest_name:%s, earliest_time:%llu\n", __func__, earliest_name, *earliest_time );

	return 0;
}


// -------------------------------------------------------------------------
// Design Notes:  Find the last saved database file and
// 				  compute the save time (seconds).
//
// Parameters:
//		last_time: 	[out] The last saved database save time.
// Returns:
//		int. 0: successful; nonzero: failed.
// -------------------------------------------------------------------------
static int GetLastSaveTime( uint64_t * last_time )
{
	APAL_LOG_TRACE("SDK::%s\n", __func__ );

	//
	char last_name[APAL_NAME_MAX] = "DB_Data_19700101.db3";

    //
    APAL_HANDLE name_list = APAL_INVALID_HANDLE;
	FindDatabaseFile( &name_list, 0, 0);
	if( name_list == APAL_INVALID_HANDLE )
	{
		return 0;
	}

    while ( !apal_list_empty( name_list ) )
    {
    	struct dirent * dir = (struct dirent *) apal_list_pop_front( name_list );
    	if ( dir != NULL )
    	{
         	if(apal_str_cmp( dir->d_name, last_name ) >= 0)
        	{
        		apal_mem_clear( last_name, sizeof( last_name ) );
        		apal_str_cpy( last_name, dir->d_name, sizeof( last_name ) );
        	}
    	}
    	free( dir );
    }
    APAL_SAFE_CLOSE_HANDLE( name_list );

	//
	char str_year[DBFILE_LENGTH] = {0};
	char str_mon[DBFILE_LENGTH] = {0};
	char str_day[DBFILE_LENGTH] = {0};

	//
    apal_str_printf( str_year, strlen("1900")+1, "%s", (char *)last_name+8 );
    apal_str_printf( str_mon, strlen("01")+1, "%s", (char *)last_name+12 );
    apal_str_printf( str_day, strlen("01")+1, "%s", (char *)last_name+14 );

    struct tm tmDate = {0};
    tmDate.tm_year = atoi(str_year) - 1900;
    tmDate.tm_mon = atoi(str_mon) - 1;      // Month: start from 0
    tmDate.tm_mday = atoi(str_day);         // Day: start from 1
    tmDate.tm_hour = 0;
    tmDate.tm_min = 0;
    tmDate.tm_sec = 0;
    time_t last_tm = mktime(&tmDate);

    //
    *last_time = (uint64_t)last_tm + SECONDS_PER_DAY - 1;
    printf( "SDK:%s: last_name:%s, last_time:%llu\n", __func__, last_name, *last_time );

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:
// 		Prepare for query (Open a database connection and compile an
// 		SQL statement).
//
// Parameters:
//		dbInfoName: 	[in] Database filename.
//		pTagName: 		[in] Tag name.
//		startTime: 		[in] Start time stamp.
//		endTime: 		[in] End time stamp.
//		queryResult: 	[in/out] Output query result.
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
static int PrepareForQuery(
		char const * dbInfoName,
		char const * pTagName,
		uint64_t startTime,
		uint64_t endTime,
		QUERY_RESULT * queryResult)
{
	//
	int rc = 0;

	printf( "SDK::%s: %s, Tag: %s, Start: %llu, End: %llu\n",
			__func__, dbInfoName, pTagName, startTime, endTime );

    //
	QUERY_DB * queryDB = (QUERY_DB * )apal_mem_alloc( sizeof( QUERY_DB ) );

	//
	// 1. Opening A Database Connection
	//

	char dbName[ MAX_DB_PATH_LEN ] = { 0 };
	apal_str_printf(dbName, MAX_DB_PATH_LEN, "%s%s", dbDir, dbInfoName);

	printf( "SDK::%s: O_RDONLY: %d, O_RDWR: %d, O_CREAT: %d\n", __func__, O_RDONLY, O_RDWR, O_CREAT );
	printf( "SDK::%s: call DBFileOpen(%s, %d)\n", __func__, dbName, O_RDONLY );
	queryDB->fd = -1;
	queryDB->fd = DBFileOpen(dbName, O_RDONLY);
		if ( queryDB->fd < 0 )
	{
		//
		APAL_LOG_ERROR("%s: DBFileOpen(%s) return: %d\n\n", __func__, dbName, rc);
		APAL_MEM_SAFE_FREE(queryDB);
		return DATALOGGER_ERROR_OPEN_DATABASE;
	}

    // To obtain file size
	struct stat sb;
	if ( fstat(queryDB->fd, &sb) == -1 )
	{
		APAL_LOG_ERROR("%s: failed to get file size. errno: %d\n\n", __func__, errno);

		APAL_MEM_SAFE_FREE(queryDB);
		DBFileClose(queryDB->fd);
		return DATALOGGER_ERROR_NO_RECORD;
	}
	printf("SDK::%s: file size: %d\n", __func__, sb.st_size);

	//
	if( sb.st_size <= HEADER_LENGTH)
	{
		printf("SDK::%s: no data\n", __func__);

		APAL_MEM_SAFE_FREE(queryDB);
		DBFileClose(queryDB->fd);
		return DATALOGGER_ERROR_NO_RECORD;
	}

	// mapping file header
	void *addr;
	lseek( queryDB->fd, 0, SEEK_SET );
	addr = mmap( NULL, HEADER_LENGTH, PROT_READ, MAP_PRIVATE, queryDB->fd, 0 );
	if(addr == MAP_FAILED)
	{
		printf("SDK::%s: Could not map view of file header, errno: %d, line: %d\n\n", __func__, errno, __LINE__);

		APAL_MEM_SAFE_FREE(queryDB);
		DBFileClose(queryDB->fd);
		return DATALOGGER_ERROR_NO_RECORD;
    }
	//EACCES

    FILE_HEADER *header = (FILE_HEADER *)addr;
    printf( "SDK::%s: header: write_flag:%d, len: %d, last_id: %d, addr: %p\n",
    		__func__, header->write_flag, header->len, header->last_id, addr);

	//
    FileHeaderPrintInfo( header, pTagName);

    //
	// 3. Get log_id
	//
	uint32_t i = 0;
	bool found = false;
	for( i = 0; i < header->len; i++ )
	{
		if( apal_str_cmp( pTagName, header->tag_ids[i].tag_name ) == 0 )
		{
			found = true;
			queryDB->id = header->tag_ids[i].id;
			queryDB->type = PERIOD_DATA;
			queryDB->start_offset = header->tag_ids[i].start_offset;
			queryDB->end_offset = header->tag_ids[i].end_offset;
			//queryDB->map_size = queryDB->end_offset - queryDB->start_offset + TAG_VALUE_LENGTH;
			queryDB->map_size = sb.st_size;
			break;
		}
	}
    printf( "SDK::%s: %s id: %u, type: %d, start_offset: 0x%x, end_offset: 0x%x, map_size: 0x%x\n",
    		__func__, pTagName, queryDB->id, queryDB->type, queryDB->start_offset, queryDB->end_offset, queryDB->map_size);

	// Unmap file header
	munmap(addr, HEADER_LENGTH);
	//FileHeaderUnmap(dbf_header);

	//
	if( found == false )
	{
		printf("%s: no record in (%s)\n\n", __func__, dbInfoName);
		APAL_MEM_SAFE_FREE(queryDB);
		DBFileClose(queryDB->fd);
		return DATALOGGER_ERROR_NO_RECORD;
	}

	// mapping file buffer for query value
	//off_t offset = queryDB->start_offset;
	//off_t offset = lseek( queryDB->fd, queryDB->start_offset, SEEK_SET );
	//printf( "SDK::%s: offset :%u, queryDB->start_offset: %u\n\n", __func__, offset, queryDB->start_offset);
	//off_t offset = queryDB->start_offset;
	lseek( queryDB->fd, 0, SEEK_SET );
	queryDB->value = mmap( NULL, queryDB->map_size, PROT_READ, MAP_PRIVATE, queryDB->fd, 0 );
	if(queryDB->value == MAP_FAILED)
	{
		printf("SDK::%s: Could not map view of file, errno: %d, line: %d\n\n", __func__, errno, __LINE__);

		APAL_MEM_SAFE_FREE(queryDB);
		DBFileClose(queryDB->fd);
		return DATALOGGER_ERROR_NO_RECORD;
    }

    printf( "SDK::%s: queryDB->value: %p, queryDB: %p\n\n", __func__, queryDB->value, queryDB);
	queryDB->start_time = startTime;
	queryDB->end_time = endTime;
	queryDB->rd_offset = 0;

    //
	queryResult->queryList.push_back(queryDB);

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  Step by step querying record from the database.
//
// Parameters:
//		pTagName: 	[in] Tag name.
//		startTime: 	[in] Start time stamp.
//		endTime: 	[in] End time stamp.
//		queryResult:[out] Output query result.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
//
// Remarks:
// 		This routine malloc space to hold the prepared statement object and
// 		sqlite3 object.
// 		The query should be passed to GetNextValue() to get tag value or
// 		ReleaseQuery() when the calling procedure is finished using it.
// -------------------------------------------------------------------------
static int OnQueryTag(
		char const * pTagName,
		uint64_t startTime,
		uint64_t endTime,
		QUERY_RESULT * queryResult
		)
{
	//
	printf( "SDK::%s: Tag: %s, Start: %llu, End: %llu\n",
			__func__, pTagName, startTime, endTime );

	// Find database file name according startTime and endTime
    APAL_HANDLE name_list = APAL_INVALID_HANDLE;
	FindDatabaseFile( &name_list, startTime, endTime);
	if( name_list == APAL_INVALID_HANDLE )
	{
		return DATALOGGER_ERROR_QUERY;
	}

	printf( "SDK::%s: apal_list_size( name_list ): %d, Start: %llu, End: %llu\n",
				__func__, apal_list_size( name_list ), startTime, endTime );

    while ( !apal_list_empty( name_list ) )
    {
    	struct dirent * dir = (struct dirent *) apal_list_pop_front( name_list );
    	if ( dir != NULL )
    	{
    		printf( "SDK::%s: call PrepareForQuery(%s, %s, %llu, %llu)\n",
    					__func__, dir->d_name, pTagName, startTime, endTime );
         	PrepareForQuery( dir->d_name, pTagName, startTime, endTime, queryResult );
         	free( dir );
    	}
    }
    APAL_SAFE_CLOSE_HANDLE( name_list );

	if ( !queryResult->queryList.empty() )
	{
		queryResult->curIt = queryResult->queryList.begin();
		return 0;
	}

	return DATALOGGER_ERROR_NO_RECORD;
}

// -------------------------------------------------------------------------
// Design Notes:  API. Query record from the database.
//
// Parameters:
//		pTagName: 	[in] Tag name.
//		startTime: 	[in] Start time stamp.
//		endTime: 	[in] End time stamp.
//		query: 		[out] Output query result.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
//
// Remarks:
// 		This routine malloc space to hold the prepared statement object and
// 		sqlite3 object.
// 		The query should be passed to GetNextValue() to get tag value or
// 		ReleaseQuery() when the calling procedure is finished using it.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int QueryTagValue(
		char const * pTagName,
		uint64_t startTime,
		uint64_t endTime,
		void **query
		)
{
	//
	printf( "SDK::%s: Tag: %s, Start: %llu, End: %llu\n",
			__func__, pTagName, startTime, endTime );

	*query = NULL;

	//
	if( isSDKInitialized == 0 )
	{
		APAL_LOG_FATAL( "%s: The DataLoggerSDK has not been initialized!\n", __func__ );

		return DATALOGGER_ERROR_NOT_INITIAL;
	}

	//
	if ( NULL == pTagName )
	{
		APAL_LOG_ERROR( "%s: Tag name is NULL!\n", __func__ );

		return DATALOGGER_ERROR_PARAM_TAG;
	}

	int ret = 0;
	QUERY_RESULT * queryResult = new QUERY_RESULT;
	apal_str_cpy( queryResult->tag_name, pTagName, sizeof( queryResult->tag_name ) );

	// Find the earliest database file and compute the save time
	// This time will be the start time
	uint64_t start_time = startTime;
	GetEarliestSaveTime(&start_time);
	start_time = max( start_time, startTime );

	if( endTime == 0 )
	{
		time_t curTimeValue;
		time(& curTimeValue);

		// Find the last saved database file and compute the save time
		// This time will be the end time
		uint64_t end_time = 0;
		GetLastSaveTime( &end_time);

		queryResult->end_time = min((uint64_t)curTimeValue, end_time );
	}
	else
	{
		queryResult->end_time = max(endTime, start_time);
	}

	//queryResult->cur_end_time = min( MAX_QUERY_TIME_LEN + start_time/SEC_PER_HOUR * SEC_PER_HOUR, queryResult->end_time);
	// cur_end_time is 23:59:59 of the day of start_time
	queryResult->cur_end_time = min( MAX_QUERY_TIME_LEN + start_time/SEC_PER_DAY * SEC_PER_DAY, queryResult->end_time);

	//
	do
	{
		ret = OnQueryTag( pTagName, start_time, queryResult->cur_end_time , queryResult );
		if (ret == 0)
		{
			break;
		}

		// the next day
		start_time = queryResult->cur_end_time + 1;
		queryResult->cur_end_time = min( MAX_QUERY_TIME_LEN + start_time, queryResult->end_time);
	} while( start_time < queryResult->end_time );

	if ( !queryResult->queryList.empty() )
	{
		*query = queryResult;
		return 0;
	}

	//
	delete queryResult;
	queryResult = NULL;

	return DATALOGGER_ERROR_NO_RECORD;
}

// -------------------------------------------------------------------------
// Design Notes:  Deallocate or free the space the QueryTagValue() allocated.
//
// Parameters:
//		queryResult: [in] Previously allocated memory block to be freed.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
static void OnReleaseQuery( QUERY_RESULT * queryResult )
{
	APAL_LOG_TRACE( "SDK::%s\n", __func__ );

	//
	while ( !queryResult->queryList.empty() )
	{
		//
		// The first element in the queryList
		//
		QUERY_DB * queryDB = queryResult->queryList.front();
		queryResult->queryList.pop_front();
		if( queryDB->value )
		{
			DBFileUnmap(queryDB->value, queryDB->map_size);
		}
		queryDB->value = NULL;

		if( queryDB->fd != -1 )
		{
			DBFileClose(queryDB->fd);
		}

		APAL_MEM_SAFE_FREE(queryDB);
	}
}

// -------------------------------------------------------------------------
// Design Notes:  API.
// 				  Deallocate or free the space the QueryTagValue() allocated.
//
// Parameters:
//		query: [in] Previously allocated memory block to be freed.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API void ReleaseQuery( void *query )
{
	APAL_LOG_TRACE( "SDK::%s: query: %p\n", __func__, query );

	QUERY_RESULT * queryResult = static_cast <QUERY_RESULT *> (query);

	if ( queryResult == NULL )
	{
		return;
	}

	//
	if( isSDKInitialized == 0 )
	{
		return;
	}

	//
	while ( !queryResult->queryList.empty() )
	{
		//
		// The first element in the queryList
		//
		QUERY_DB * queryDB = queryResult->queryList.front();
		queryResult->queryList.pop_front();
		if( queryDB->value )
		{
			DBFileUnmap(queryDB->value, queryDB->map_size);
		}
		queryDB->value = NULL;

		if( queryDB->fd != -1 )
		{
			DBFileClose(queryDB->fd);
		}

		APAL_MEM_SAFE_FREE(queryDB);
	}

	delete queryResult;
	queryResult = NULL;
}

// -------------------------------------------------------------------------
// Design Notes:  Evaluate sqlite3_step SQL Statement to
// 				  get the next tag value from database.
//
// Parameters:
//		queryResult:[in] Previously allocated memory block in QueryTagValue().
//		tagValue: 	[out] Output tag value.
// Returns:
//		int. 0: successful; negative: failed/error code.
//
//		DATALOGGER_ERROR_BUSY (-11), means that the database file is locked.
//		DATALOGGER_ERROR_DONE (-12), means that no more value, the statement
//		has finished executing successfully.
// 		DATALOGGER_ERROR_IOERR (-13), means that that the operation could not
//		finish because the operating system reported an I/O error.
// 		DATALOGGER_ERROR_CORRUPT (-14), means that the database file has
// 		been corrupted.
// -------------------------------------------------------------------------
static int ReadTagValueFromFile( QUERY_DB * queryDB, DC_TAG * value, uint32_t * next_start )
{
    TAG_VALUE  *pvalue;
	uint32_t start = queryDB->rd_offset + queryDB->start_offset;
	printf( "SDK::%s, queryDB: %p, queryDB->id: %u, queryDB->type: %d, rd_offset: 0x%x, queryDB->value: %p\n\n",
			__func__, queryDB, queryDB->id, queryDB->type, queryDB->rd_offset, queryDB->value );

#if 0
	// just for testing
    FILE_HEADER *header = (FILE_HEADER *)((char *) queryDB->value);
    printf( "SDK::%s: header: write_flag:%d, len: %d, last_id: %dp\n",
    		__func__, header->write_flag, header->len, header->last_id);

	//
    FileHeaderPrintInfo( header, "ReadTagValueFromFile");
#endif

	int i = 0;
	do
	{
		TAG_VALUE tag_value = {0};
		lseek(queryDB->fd, start, SEEK_SET);
		ssize_t rd_count = read(queryDB->fd, &tag_value, TAG_VALUE_LENGTH);
		if( rd_count > 0 )
		{
			printf( "\tstart: 0x%x, rd_count: %u, id: %d, type: %d, size: %u time: %llu\n",
					start, rd_count, tag_value.id, tag_value.type, tag_value.size, tag_value.time );
		}
		start = start + tag_value.size;

#if 0
      	pvalue = (TAG_VALUE *) ((char *) queryDB->value + start);
      	// just for testing
		if(i <100)
		{
			printf( "\t%d start: 0x%x, pvalue: %p, id: %d, type: %d, size: %u time: %llu\n", i, start, pvalue, pvalue->id, pvalue->type, pvalue->size, pvalue->time );
		}
		i++;

		if(( pvalue->id == queryDB->id) && ( pvalue->type == queryDB->type ))
		{
			if ((pvalue->time >= queryDB->start_time) && (pvalue->time <= queryDB->end_time))
			{
				value->value = pvalue->value;
				value->quality = pvalue->quality;
				value->time = pvalue->time;
				value->usec = pvalue->usec;
				*next_start = start + pvalue->size;
				printf( "SDK::%s: value: %.4lf, time: %lld, quality: %d, value: %u, next_start: %lld\n",
						__func__, value->value, value->time, value->quality, value->usec, *next_start );
				return 0;

			}
		}
		start = start + pvalue->size;
#endif
	} while( start <= queryDB->end_offset);	//} while( (start + pvalue->size) <= queryDB->map_size );


	return DATALOGGER_ERROR_GET_VALUE;
}

// -------------------------------------------------------------------------
// Design Notes:  Evaluate sqlite3_step SQL Statement to
// 				  get the next tag value from database.
//
// Parameters:
//		queryResult:[in] Previously allocated memory block in QueryTagValue().
//		tagValue: 	[out] Output tag value.
// Returns:
//		int. 0: successful; negative: failed/error code.
//
//		DATALOGGER_ERROR_BUSY (-11), means that the database file is locked.
//		DATALOGGER_ERROR_DONE (-12), means that no more value, the statement
//		has finished executing successfully.
// 		DATALOGGER_ERROR_IOERR (-13), means that that the operation could not
//		finish because the operating system reported an I/O error.
// 		DATALOGGER_ERROR_CORRUPT (-14), means that the database file has
// 		been corrupted.
// -------------------------------------------------------------------------
static int OnGetNextValue( QUERY_RESULT * queryResult, DC_TAG * tagValue )
{
	//
	int rc = 0;
	int lastResult = 0;

	APAL_LOG_TRACE( "SDK::%s\n", __func__ );

	off_t next_offset = 0;
	ssize_t count = 0;
	void * addr;
	uint32_t next_start = 0;

	//
	QUERY_DB * queryDB = *queryResult->curIt;
	printf( "SDK::%s, queryDB: %p\n", __func__, queryDB );

	printf( "SDK::%s, fd: %d, id: %u, start_offset: 0x%x, end_offset: 0x%x, map_size: 0x%x, rd_offset: 0x%x, start_time: %llu, end_time: %llu\n",
			__func__, queryDB->fd, queryDB->id, queryDB->start_offset,
			queryDB->end_offset, queryDB->map_size, queryDB->rd_offset,
			queryDB->start_time, queryDB->end_time );
	rc = ReadTagValueFromFile( queryDB, tagValue, &next_start );
	if(rc == 0)
	{
		queryDB->rd_offset = next_start;
		return 0;
	}

	do
	{
		++queryResult->curIt;
		if (queryResult->curIt == queryResult->queryList.end())
		{
			printf( "SDK::%s, (queryResult->curIt == queryResult->queryList.end()\n" );
			break;
		}

		queryDB = *queryResult->curIt;
		next_start = 0;

		printf( "SDK::%s, fd: %d, id: %u, start_offset: %u, end_offset: %u, map_size: %u, rd_offset: %u, start_time: %llu, end_time: %llu, line: %d\n",
				__func__, queryDB->fd, queryDB->id, queryDB->start_offset,
				queryDB->end_offset, queryDB->map_size, queryDB->rd_offset,
				queryDB->start_time, queryDB->end_time, __LINE__ );
		rc = ReadTagValueFromFile( queryDB, tagValue, &next_start );
		if(rc == 0)
		{
			queryDB->rd_offset = next_start;
			return 0;
		}

	} while(queryResult->curIt != queryResult->queryList.end()) ;

	return DATALOGGER_ERROR_DONE;
}

// -------------------------------------------------------------------------
// Design Notes:  The next cycle query.
// 				  It will release the last cycle allocated memory;
// 				  prepare the next cycle query statement;
// 				  and get one value.
//
// Parameters:
//		queryResult:[in] Previously allocated memory block in QueryTagValue().
//		tagValue: 	[out] Output tag value.
// Returns:
//		int. 0: successful; negative: failed/error code.
//
//		DATALOGGER_ERROR_BUSY (-11), means that the database file is locked
//		DATALOGGER_ERROR_DONE (-12), means that no more value, the statement
//									 has finished executing successfully
// -------------------------------------------------------------------------
static int DoNextCycQuery( QUERY_RESULT * queryResult, DC_TAG * tagValue )
{
	//
	int rc = 0;
	printf( "SDK::%s\n", __func__ );

	// release current SQL statement
	OnReleaseQuery( queryResult );

	if( queryResult->cur_end_time >= queryResult->end_time )
	{
		return DATALOGGER_ERROR_DONE;
	}

	// Prepare for next cycle query
	uint64_t start_time = queryResult->cur_end_time + 1;
	uint64_t end_time = min( MAX_QUERY_TIME_LEN + start_time, queryResult->end_time);
	queryResult->cur_end_time = end_time;
	do
	{
		printf( "SDK::%s: call OnQueryTag( tag_name: %s, start_time: %llu, end_time: %llu, queryResult )\n",
				__func__, queryResult->tag_name, start_time, end_time, queryResult );
		rc = OnQueryTag( queryResult->tag_name, start_time, end_time, queryResult );
		if (rc == 0)
		{
			break;
		}

		start_time = end_time;
		end_time = min( MAX_QUERY_TIME_LEN + start_time, queryResult->end_time);
		queryResult->cur_end_time = end_time;
	} while( start_time < queryResult->end_time );

	//
	if ( queryResult->queryList.empty() )
	{
		return DATALOGGER_ERROR_DONE;
	}

	rc = OnGetNextValue( queryResult, tagValue );
	return rc;
}

// -------------------------------------------------------------------------
// Design Notes:  API. Get the next tag value.
//
// Parameters:
//		query: 		[in] Previously allocated memory block in QueryTagValue().
//		tagValue: 	[out] Output tag value.
// Returns:
//		int. 0: successful; negative: failed/error code.
//
//		DATALOGGER_ERROR_BUSY (-11), means that the database file is locked
//		DATALOGGER_ERROR_DONE (-12), means that no more value, the statement
//									 has finished executing successfully
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int GetNextValue( void * query, DC_TAG * tagValue )
{
	//
	int rc = 0;
	printf( "SDK::%s\n", __func__ );

	//
	if( isSDKInitialized == 0 )
	{
		APAL_LOG_FATAL( "%s: The DataLoggerSDK has not been initialized!\n", __func__ );

		return DATALOGGER_ERROR_NOT_INITIAL;
	}

	QUERY_RESULT * queryResult = static_cast <QUERY_RESULT *> (query);

	//
	if ( queryResult == NULL )
	{
		return DATALOGGER_ERROR_PARAM;
	}

	if ( tagValue == NULL )
	{
		return DATALOGGER_ERROR_PARAM_VALUE;
	}

	printf( "SDK::%s: call rc = OnGetNextValue( queryResult, tagValue );\n", __func__ );
	rc = OnGetNextValue( queryResult, tagValue );
	if (rc == 0)
	{
		return 0;
	}

	//
	int ret = DATALOGGER_ERROR_DONE;
	do
	{
		printf( "SDK::%s: call rc = DoNextCycQuery( queryResult, tagValue );\n", __func__ );
		ret = DoNextCycQuery( queryResult, tagValue );
		if (ret == 0)
		{
			return 0;
		}
	} while(queryResult->cur_end_time < queryResult->end_time);

	return ret;
}

// -------------------------------------------------------------------------
// Design Notes:  Insert statistics data into the database.
//
// Parameters:
//		type: 		[in] The Statistics type.  0: minute data, 1: hour date, 2: day data.
//		pTagName: 	[in] Tag name.
//		timeStamp: 	[in] Time stamp, in UTC format.
//		quality: 	[in] Quality.
//		partial: 	[in] Partial. 0: Complete; 1: Partial.
//		lastValue: 	[in] The Last value.
//		minValue: 	[in] The minimum value.
//		maxValue: 	[in] The maximum value.
//		avgValue: 	[in] The average value.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int InsertStatisticsValue(
		int type,
		char const * pTagName,
		uint64_t timeStamp,
		int16_t quality,
		int16_t partial,
		double lastValue,
		double minValue,
		double maxValue,
		double avgValue
		)
{
	return InsertStatValueV2(
			type, pTagName, timeStamp, quality, partial,
			lastValue, minValue, maxValue, avgValue );
}

// -------------------------------------------------------------------------
// Design Notes:  Prepare for statistics record query
// 				  (Open a database connection and compile an SQL statement).
//
// Parameters:
//		dbInfoName: 	[in] Database filename.
//		pTagName: 		[in] Tag name.
//		startTime: 		[in] Start time stamp.
//		endTime: 		[in] End time stamp.
//		type: 			[in] Statistics type,
//		queryResult: 	[in/out] Output query result.
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
static int PrepareForStatisticsQuery(
		char const * dbInfoName,
		char const * pTagName,
		uint64_t startTime,
		uint64_t endTime,
		STATISTICS_TYPE type,
		QUERY_RESULT * queryResult)
{
	//
	int rc = SQLITE_OK;
	char* errmsg = NULL;
#if 0
	APAL_LOG_TRACE( "SDK::%s: %s, Tag: %s, Start: %llu, End: %llu\n",
			__func__, dbInfoName, pTagName, startTime, endTime );

    int flags = SQLITE_OPEN_READWRITE;
	QUERY_DB * queryDB = (QUERY_DB * )apal_mem_alloc( sizeof( QUERY_DB ) );

	//
	// 1. Opening A New Database Connection
	//

	char dbName[ MAX_DB_PATH_LEN ] = { 0 };
	apal_str_printf(dbName, MAX_DB_PATH_LEN, "%s%s", dbDir, dbInfoName);

	rc = SQLExecOpenDB( dbName, &queryDB->pDB, flags );
    if ( SQLITE_OK != rc )
	{
		//
		APAL_LOG_ERROR("%s: SQLExecOpenDB (%s) return: %d\n\n", __func__, dbName, rc);
		return DATALOGGER_ERROR_OPEN_DATABASE;
	}

	//
	// 2. Compiling An SQL Statement
	//

	//
	char dbTable[MAX_TBL_NAME_LEN] = { 0 };
	char strCmd[MAX_SQL_CMD_LEN] = { 0 };
	char strIdentify[32] = { 0 };

	switch ( type )
	{
	case MINUTE:
		apal_str_printf( strIdentify, 32, "%s", "MINUTE" );
		break;

	case HOUR:
		apal_str_printf( strIdentify, 32, "%s", "HOUR" );
		break;

	case DAY:
		apal_str_printf( strIdentify, 32, "%s", "DAY" );
		break;
	}

	rc = GetTableName( queryDB->pDB, pTagName, MAX_TBL_NAME_LEN, dbTable, true );
	if ( rc != 0 )
	{
		sqlite3_close(queryDB->pDB);
		APAL_MEM_SAFE_FREE(queryDB);

		return rc;
	}

	apal_mem_clear( strCmd, MAX_SQL_CMD_LEN );

	// 1	F_TimeStamp Primary key	DATETIME		Data time stamp, 2008-06-17 16:00:00
	// 2	F_Quality	Not NULL 	SMALLINT		0: Normal; Other values: abnormal
	// 3	F_Partial	Not NULL 	SMALLINT		0: complete; Other values: partial
	// 4	F_LastValue	Not NULL 	NUMERIC(18,4)	Last tag value
	// 5	F_MinValue	Not NULL 	NUMERIC(18,4)	Minimum tag value
	// 6	F_MaxValue	Not NULL 	NUMERIC(18,4)	Maximum tag value
	// 7	F_Average	Not NULL 	NUMERIC(18,4)	Average value

	//
	if( ( 0 == startTime ) && ( 0 != endTime ) )
	{
		apal_str_printf(strCmd, MAX_SQL_CMD_LEN,
				"SELECT F_TimeStamp, F_Quality, F_Partial, F_LastValue, "
				"F_MinValue, F_MaxValue, F_Average FROM %s_%s "
				"WHERE F_TimeStamp<=%llu ORDER BY F_TimeStamp ASC",
				dbTable, strIdentify, endTime );
	}
	else if( ( 0 != startTime ) && ( 0 == endTime ) )
	{
		apal_str_printf(strCmd, MAX_SQL_CMD_LEN,
				"SELECT F_TimeStamp, F_Quality, F_Partial, F_LastValue, "
				"F_MinValue, F_MaxValue, F_Average FROM %s_%s "
				"WHERE F_TimeStamp>=%llu ORDER BY F_TimeStamp ASC",
				dbTable, strIdentify, startTime);
	}
	else if( ( 0 != startTime ) && ( 0 != endTime ) )
	{
		apal_str_printf( strCmd, MAX_SQL_CMD_LEN,
				"SELECT F_TimeStamp, F_Quality, F_Partial, F_LastValue, "
				"F_MinValue, F_MaxValue, F_Average FROM %s_%s "
				"WHERE F_TimeStamp<=%llu AND F_TimeStamp>=%llu "
				"ORDER BY F_TimeStamp ASC",
				dbTable, strIdentify, endTime, startTime );
	}
	else
	{
		apal_str_printf( strCmd, MAX_SQL_CMD_LEN,
				"SELECT F_TimeStamp, F_Quality, F_Partial, F_LastValue, "
				"F_MinValue, F_MaxValue, F_Average FROM %s_%s "
				"ORDER BY F_TimeStamp ASC",
				dbTable, strIdentify );
	}

	//Compiling an SQL statement that can be executed using sqlite3_step().
	rc = sqlite3_prepare_v2( queryDB->pDB, strCmd, -1, &queryDB->stmt, NULL );
    if ( SQLITE_OK != rc )
	{
		//
		APAL_LOG_ERROR(
				"%s: sqlite3_prepare_v2 (%s::%s) failed! rc = %d. Message: %s\n",
				__func__, dbInfoName, pTagName, rc, sqlite3_errmsg( queryDB->pDB ) );

		if (queryDB->stmt)
		{
			sqlite3_finalize(queryDB->stmt);
		}

		sqlite3_close(queryDB->pDB);
		APAL_MEM_SAFE_FREE(queryDB);

		//
		sqlite3_free( errmsg );
		errmsg = 0;

		return rc;
	}

    //
	queryResult->queryList.push_back(queryDB);
#endif
	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  Query statistics record from the database.
//
// Parameters:
//		type: 		[in] Statistics type, 0: minute; 1: hour; 2: day
//		pTagName: 	[in] Tag name.
//		startTime: 	[in] Start time stamp.
//		endTime: 	[in] End time stamp.
//		query: 		[out] Output query result.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
//
// Remarks:
// 		This routine malloc space to hold the prepared statement object and
// 		sqlite3 object.
// 		The query should be passed to GetNextStatisticsValue() to get tag value
// 		or ReleaseQuery() when the calling procedure is finished using it.
// -------------------------------------------------------------------------
DATALOGGERSDK_API int QueryStatisticsValue(
		int type,
		char const * pTagName,
		uint64_t startTime,
		uint64_t endTime,
		void **query
		)
{
	//
	APAL_LOG_TRACE( "SDK::%s: Tag: %s, Start: %llu, End: %llu, Type: %d\n",
			__func__, pTagName, startTime, endTime, type );

	*query = NULL;
#if 0
	//
	if( isSDKInitialized == 0 )
	{
		APAL_LOG_FATAL( "%s: The DataLoggerSDK has not been initialized!\n", __func__ );

		return DATALOGGER_ERROR_NOT_INITIAL;
	}

	//
	if ( NULL == pTagName )
	{
		APAL_LOG_ERROR( "%s: Tag name is NULL!\n", __func__ );

		return DATALOGGER_ERROR_PARAM_TAG;
	}

	//
	time_t timeValue = startTime;

	//
	tm * tmStamp = gmtime( &timeValue );
	if( NULL == tmStamp )
	{
		//
		APAL_LOG_ERROR( "%s: Invalid start time value.\n", __func__ );
		return DATALOGGER_ERROR_PARAM_TIME;
	}

	//
	timeValue = endTime;

	//
	tmStamp = gmtime( &timeValue );
	if( NULL == tmStamp )
	{
		//
		APAL_LOG_ERROR( "%s: Invalid end time value.\n", __func__ );
		return DATALOGGER_ERROR_PARAM_TIME;
	}

	//
	if(( type < (int)MINUTE ) || ( type > (int)DAY ))
	{
		//
		APAL_LOG_ERROR(
				"%s: Invalid statistics type. Valid value should be %d to %d\n",
				__func__, (int)MINUTE, (int)DAY );
		return DATALOGGER_ERROR_PARAM_TYPE;
	}

	QUERY_RESULT * queryResult = new QUERY_RESULT;

    APAL_HANDLE name_list = APAL_INVALID_HANDLE;
	FindDatabaseFile( &name_list, startTime, endTime);
	if( name_list == APAL_INVALID_HANDLE )
	{
		//
		APAL_LOG_ERROR(
				"%s: Failed to find database file!\n", __func__ );
		return DATALOGGER_ERROR_QUERY;
	}

    while ( !apal_list_empty( name_list ) )
    {
    	struct dirent * dir = (struct dirent *) apal_list_pop_front( name_list );

    	PrepareForStatisticsQuery(dir->d_name, pTagName, startTime, endTime,
    			(STATISTICS_TYPE)type, queryResult );
    	free( dir );
    }
    APAL_SAFE_CLOSE_HANDLE( name_list );

	if ( !queryResult->queryList.empty() )
	{
		queryResult->curIt = queryResult->queryList.begin();
		*query = queryResult;
		return 0;
	}

	//
	delete queryResult;
	queryResult = NULL;
#endif
	return DATALOGGER_ERROR_NO_RECORD;
}

// -------------------------------------------------------------------------
// Design Notes:  Get the next statistics value.
//
// Parameters:
//		query: 		[in] Previously allocated memory block in QueryStatisticsValue().
//		timeStamp: 	[out] Output tag time value.
//		quality: 	[out] Output quality value.
//		partial: 	[out] Output partial value.
//		lastValue: 	[out] Output the last tag value.
//		minValue: 	[out] Output the minimum tag value.
//		maxValue: 	[out] Output the maximum tag value.
//		avgValue: 	[out] Output the average tag value.
// Returns:
//		int. 0: successful; negative: failed/error code.
//
//		DATALOGGER_ERROR_BUSY (-11), means that the database file is locked
//		DATALOGGER_ERROR_DONE (-12), means that no more value,
//							the statement has finished executing successfully
// -------------------------------------------------------------------------
DATALOGGERSDK_API int GetNextStatisticsValue(
		void * query,
		uint64_t * timeStamp,
		int16_t * quality,
		int16_t * partial,
		double * lastValue,
		double * minValue,
		double * maxValue,
		double * avgValue
 )
{
	//
	int rc = SQLITE_OK;
	int lastResult = 0;
#if 0
	APAL_LOG_TRACE( "SDK::%s\n", __func__ );

	//
	if( isSDKInitialized == 0 )
	{
		APAL_LOG_FATAL( "%s: The DataLoggerSDK has not been initialized!\n", __func__ );

		return DATALOGGER_ERROR_NOT_INITIAL;
	}

	//
	if (( timeStamp == NULL )
			|| ( quality == NULL )
			|| ( partial == NULL )
			|| ( lastValue == NULL )
			|| ( minValue == NULL )
			|| ( maxValue == NULL )
			|| ( avgValue == NULL ))
	{
		APAL_LOG_FATAL( "%s: Invalid input parameter!", __func__ );
		return DATALOGGER_ERROR_PARAM;
	}

	QUERY_RESULT * queryResult = static_cast <QUERY_RESULT *> (query);

	// Deallocates or frees a memory block.
	if ( queryResult == NULL )
	{
		return DATALOGGER_ERROR_PARAM;
	}

	//
	QUERY_DB * queryDB;

	queryDB = *queryResult->curIt;

	// Historical data statistics table structure as follows
	// Item Field name 	Primary/Foreign key Data type 		Description
	// 1	F_TimeStamp Primary key			DATETIME		Data time stamp, 2008-06-17 16:00:00
	// 2	F_Quality	Not NULL 			SMALLINT		0: Normal; Other values: abnormal
	// 3	F_Partial	Not NULL 			SMALLINT		0: complete; Other values: partial
	// 4	F_LastValue	Not NULL 			NUMERIC(18,4)	Last tag value
	// 5	F_MinValue	Not NULL 			NUMERIC(18,4)	Minimum tag value
	// 6	F_MaxValue	Not NULL 			NUMERIC(18,4)	Maximum tag value
	// 7	F_Average	Not NULL 			NUMERIC(18,4)	Average value
	//

	rc = sqlite3_step( queryDB->stmt );
	switch ( rc )
	{
	case SQLITE_ROW: // 100 sqlite3_step() has another row ready
		* timeStamp = sqlite3_column_int64( queryDB->stmt, 0 );
		* quality = sqlite3_column_int( queryDB->stmt, 1 );
		* partial = sqlite3_column_int( queryDB->stmt, 2 );
		* lastValue = sqlite3_column_double( queryDB->stmt, 3 );
		* minValue = sqlite3_column_double( queryDB->stmt, 4 );
		* maxValue = sqlite3_column_double( queryDB->stmt, 5 );
		* avgValue = sqlite3_column_double( queryDB->stmt, 6 );

		break;

	case SQLITE_DONE: // 101  sqlite3_step() has finished executing
		//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_DONE (%d)\n", __func__, SQLITE_DONE );
		break;

	case SQLITE_BUSY: // 5  The database file is locked
		//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_BUSY (%d), %s\n", __func__, SQLITE_BUSY, sqlite3_errmsg(queryDB->pDB) );
		break;

	case SQLITE_ERROR: // 1  SQL error or missing database
		//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_ERROR (%d), %s\n", __func__, SQLITE_ERROR, sqlite3_errmsg(queryDB->pDB) );
		break;

	case SQLITE_MISUSE: // 21  Library used incorrectly
		//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_MISUSE (%d), %s\n", __func__, SQLITE_MISUSE, sqlite3_errmsg(queryDB->pDB) );
		break;

	case SQLITE_IOERR: // 10  Some kind of disk I/O error occurred
		//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_IOERR (%d), %s\n", __func__, SQLITE_IOERR, sqlite3_errmsg(queryDB->pDB) );
		break;

	case SQLITE_CORRUPT: // 11   The database disk image is malformed
		//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_CORRUPT (%d), %s\n", __func__, SQLITE_CORRUPT, sqlite3_errmsg(queryDB->pDB) );
		break;

	default:
		break;
	}

	lastResult = rc;

	if ( lastResult != SQLITE_ROW ) // ( lastResult == SQLITE_DONE )
	{
		do
		{
			++queryResult->curIt;
			if (queryResult->curIt == queryResult->queryList.end())
			{
				break;
			}

			queryDB = *queryResult->curIt;

			rc = sqlite3_step( queryDB->stmt );
			lastResult = rc;
			switch ( rc )
			{
			case SQLITE_ROW: // 100 sqlite3_step() has another row ready
				* timeStamp = sqlite3_column_int64( queryDB->stmt, 0 );
				* quality = sqlite3_column_int( queryDB->stmt, 1 );
				* partial = sqlite3_column_int( queryDB->stmt, 2 );
				* lastValue = sqlite3_column_double( queryDB->stmt, 3 );
				* minValue = sqlite3_column_double( queryDB->stmt, 4 );
				* maxValue = sqlite3_column_double( queryDB->stmt, 5 );
				* avgValue = sqlite3_column_double( queryDB->stmt, 6 );

				break;

			case SQLITE_DONE: // 101  sqlite3_step() has finished executing
				break;

			case SQLITE_BUSY: // 5  The database file is locked
				//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_BUSY (%d), %s\n", __func__, SQLITE_BUSY, sqlite3_errmsg(queryDB->pDB) );
				break;

			case SQLITE_ERROR: // 1  SQL error or missing database
				//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_ERROR (%d), %s\n", __func__, SQLITE_ERROR, sqlite3_errmsg(queryDB->pDB) );
				break;

			case SQLITE_MISUSE: // 21  Library used incorrectly
				//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_MISUSE (%d), %s\n", __func__, SQLITE_MISUSE, sqlite3_errmsg(queryDB->pDB) );
				break;

			case SQLITE_IOERR: // 10  Some kind of disk I/O error occurred
				//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_IOERR (%d), %s\n", __func__, SQLITE_IOERR, sqlite3_errmsg(queryDB->pDB) );
				break;

			case SQLITE_CORRUPT: // 11   The database disk image is malformed
				//APAL_LOG_INFO( "%s: sqlite3_step return SQLITE_CORRUPT (%d), %s\n", __func__, SQLITE_CORRUPT, sqlite3_errmsg(queryDB->pDB) );
				break;

			default:
				return DATALOGGER_ERROR_GET_VALUE;
				break;
			}

		} while( (queryResult->curIt != queryResult->queryList.end()) && ( lastResult != SQLITE_ROW ) );
	}

	if ( lastResult == SQLITE_ROW )
	{
		return 0;
	}
	if ( lastResult == SQLITE_BUSY )
	{
		return DATALOGGER_ERROR_BUSY;
	}
	if ( lastResult == SQLITE_DONE )
	{
		return DATALOGGER_ERROR_DONE;
	}
	if ( lastResult == SQLITE_IOERR )
	{
		return DATALOGGER_ERROR_IOERR;
	}
	if ( lastResult == SQLITE_CORRUPT )
	{
		return DATALOGGER_ERROR_CORRUPT;
	}
#endif
	return DATALOGGER_ERROR_GET_VALUE;
}

// -------------------------------------------------------------------------
// Design Notes:   Get the time zone.
//
// Parameters:
//		NO.
// Returns:
//		Time zone value.
// -------------------------------------------------------------------------
static int GetTimezone( void )
{
    time_t time_utc;
    struct tm tm_local;

    // Get the UTC time
    time(&time_utc);

    // Get the local time
    // Use localtime_r for threads safe
    localtime_r(&time_utc, &tm_local);

    //time_t time_local;
    struct tm tm_gmt;

    // Change it to GMT tm
    gmtime_r(&time_utc, &tm_gmt);

    int time_zone = tm_local.tm_hour - tm_gmt.tm_hour;
    if (time_zone < -12)
    {
        time_zone += 24;
    }
    else if (time_zone > 12)
    {
        time_zone -= 24;
    }

    return time_zone;
}

// -------------------------------------------------------------------------
// Design Notes:  Add LogTag into tag list.
//
// Parameters:
//		tagName: 	[in] Tag name.
//		period: 	[in] Log Period.
//		log_type: 	[in] Tag log type (0: Period/1 or 2: Run-time).
// Returns:
//		bool. true: successful; false: failed.
// -------------------------------------------------------------------------
static bool AddLogTag(
		char const * tagName,
		int32_t period,
		uint32_t log_type )
{
	APAL_LOG_TRACE("SDK::%s: tagName: %s\n", __func__, tagName);

	LOG_TAG * logTag;
	logTag = (LOG_TAG * )apal_mem_alloc( sizeof( LOG_TAG ) );
	if( logTag == NULL)
	{
		return false;
	}

	apal_str_cpy( logTag->name, tagName, sizeof( logTag->name ) );

	//
	logTag->period = period;

	//
	logTag->log_type = log_type;

	//
	apal_list_push_back( g_tag_list, (void*) logTag );

	return true;
}

// -------------------------------------------------------------------------
// Design Notes:  Sent an event.
//
// Parameters:
//		conn: 	[in] INS_DATA instance.
//		event: 	[in] The event to sent.
// -------------------------------------------------------------------------
static void SetEvent ( INS_DATA * conn, INS_EVENT_FLAG event )
{
	apal_atomic_or( &conn->event_flag, event );
	apal_semaphore_release( conn->event_sem, 1, NULL );
}

// -------------------------------------------------------------------------
// Design Notes:  Check event.
//
// Parameters:
//		conn: 	[in] INS_DATA instance.
//		event: 	[in] The event to sent.
// -------------------------------------------------------------------------
static bool CheckEvent ( INS_DATA * conn, INS_EVENT_FLAG event )
{
	int32_t event_flag = apal_atomic_fetch_and( &conn->event_flag, ~event );

	return ( ( event_flag & event ) != 0 );
}

// -------------------------------------------------------------------------
// Design Notes:  Clear event.
//
// Parameters:
//		conn: 	[in] INS_DATA instance.
// -------------------------------------------------------------------------
static void ClearEvent ( INS_DATA * conn )
{
	while ( apal_semaphore_wait( conn->event_sem, 0 ) )
	{
		;
	}

	apal_atomic_write( &conn->event_flag, 0 );
}

// -------------------------------------------------------------------------
// Design Notes:  Execute insert tag values to database.
//
// Parameters:
//		conn: 	[in] INS_DATA instance.
//		ref: 	[in] INS_TAG_REF.
//		min_ins_cnt: 	[in] INS_TAG_REF.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
static int InsertTagProc ( INS_DATA * ins_data, INS_TAG_REF * ref, uint32_t min_ins_cnt )
{
	APAL_LOG_TRACE( "SDK::%s: ins_data: %p, ref: %p, min_ins_cnt: %u\n", __func__, ins_data, ref, min_ins_cnt );

	//
	TAG_VALUE * tag;
	TAG_VALUE * values[MAX_LIST_SIZE] = {0};
	uint32_t value_count = 0;
	uint32_t i = 0;

	while ( 1 )
	{
		//
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			return -1;
		}

		if ( apal_list_empty( ref->value_list ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return 0;
		}

		//
		value_count = apal_list_size( ref->value_list );
		if(value_count < min_ins_cnt)
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return 0;
		}

		//
		value_count = min( MAX_LIST_SIZE, value_count );

		//
		for ( i = 0; i < value_count; i++ )
		{
			tag = (TAG_VALUE *)apal_list_pop_front( ref->value_list );
			values[ i ] = tag;
		}

		apal_rwlock_unlock( ins_data->rwlock );

		//
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			for ( i = 0; i < value_count; i++ )
			{
				APAL_MEM_SAFE_FREE( values[ i ] );
			}
			return -1;
		}

		int rc = 0;

		//
		FILE_HEADER * header = (FILE_HEADER *)ins_data->dbf_header;
		if( header->write_flag == 1 )
		{
			// sync file and update header
			rc = DBFileSync( ins_data->fd );
			if( rc == 0 )
			{
				header->write_flag = 0;
				msync(ins_data->dbf_header, HEADER_LENGTH, MS_SYNC);
			}
			else
			{
				printf( "%s: File Sync failed rc: %d; line: %d\n", __func__, rc, __LINE__ );
			}
		}

		// write data to file
		uint32_t start_offset = 0;
		ssize_t wr_cnt = 0;

		wr_cnt = DBFileWriteValue(ins_data->fd, (const void *)values, value_count*TAG_VALUE_LENGTH, &start_offset);
		ref->wr_count = ref->wr_count + value_count;
		//printf( "%s: wr_cnt: %d, start_offset: %u; value_count: %u, ref->wr_count: %u\n\n", __func__, wr_cnt, start_offset, value_count, ref->wr_count );

		// sync file and update header
		header->write_flag = 1;
		msync(ins_data->dbf_header, HEADER_LENGTH, MS_SYNC);
		rc = DBFileSync( ins_data->fd );
		if( rc == 0 )
		{
			if( ref->start_offset == 0)
			{
				ref->start_offset = start_offset;
				header->tag_ids[ref->hv_index].start_offset = ref->start_offset;
			}
			ref->end_offset = start_offset + (value_count - 1 ) * TAG_VALUE_LENGTH;
			header->tag_ids[ref->hv_index].end_offset = ref->end_offset;

			//
			header->write_flag = 0;
			msync(ins_data->dbf_header, HEADER_LENGTH, MS_SYNC);
			ins_data->ins_counts = ins_data->ins_counts - value_count;
		}
		else
		{
			printf( "%s: Sync File failed rc: %d; line: %d\n", __func__, rc, __LINE__ );
		}
		printf( "%s: wr_cnt: %d, start_offset: %u; value_count: %u, header->tag_ids[].end_offset: %u, ref->end_offset: %u, ref->wr_count: %u\n\n",
				__func__, wr_cnt, start_offset, value_count, header->tag_ids[ref->hv_index].end_offset, ref->end_offset, ref->wr_count );

		apal_rwlock_unlock( ins_data->rwlock );

		///
		TAG_VALUE test_value = {0};
		lseek(ins_data->fd, start_offset, SEEK_SET);
		ssize_t rd_count = read(ins_data->fd, &test_value, TAG_VALUE_LENGTH);
		if( rd_count > 0 )
		{
			printf( "\tstart_offset: 0x%x, rd_count: %u, id: %d, type: %d, size: %u time: %llu\n",
					start_offset, rd_count, test_value.id, test_value.type, test_value.size, test_value.time );
		}

		printf( "\tvalues[0], id: %d, type: %d, size: %u time: %llu\n",
				values[0]->id, values[0]->type, values[0]->size, values[0]->time );
		///

		//
		for ( i = 0; i < value_count; i++ )
		{
			APAL_MEM_SAFE_FREE( values[ i ] );
		}
	}

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  Execute insert statistics values to database.
//
// Parameters:
//		conn: 		[in] INS_DATA instance.
//		ref: 		[in] INS_TAG_REF.
//		statType: 	[in] statistics type.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
static int InsertStatTagProc (
		INS_DATA * ins_data,
		INS_TAG_REF * ref,
		STATISTICS_TYPE statType )
{
	APAL_LOG_TRACE( "SDK::%s: ins_data: %p, ref: %p, statType: %d\n",
			__func__, ins_data, ref, statType );

	//
	STAT_VALUE * stat_value;
	STAT_VALUE * values[MAX_STAT_VAL_LIST_SIZE] = {0};
	uint32_t value_count = 0;
	uint32_t i = 0;

	switch ( statType )
	{
	case MINUTE:
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			return -1;
		}

		if ( apal_list_empty( ref->statm_value_list ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return 0;
		}

		//
		value_count = apal_list_size( ref->statm_value_list );
		value_count = min( MAX_STAT_VAL_LIST_SIZE, value_count );

		//
		for ( i = 0; i < value_count; i++ )
		{
			stat_value = (STAT_VALUE *)apal_list_pop_front( ref->statm_value_list );
			values[ i ] = stat_value;
		}

		apal_rwlock_unlock( ins_data->rwlock );

		break;

	case HOUR:
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			return -1;
		}

		if ( apal_list_empty( ref->stath_value_list ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return 0;
		}

		//
		value_count = apal_list_size( ref->stath_value_list );
		value_count = min( MAX_STAT_VAL_LIST_SIZE, value_count );

		//
		for ( i = 0; i < value_count; i++ )
		{
			stat_value = (STAT_VALUE *)apal_list_pop_front( ref->stath_value_list );
			values[ i ] = stat_value;
		}

		apal_rwlock_unlock( ins_data->rwlock );

		break;

	case DAY:
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			return -1;
		}

		if ( apal_list_empty( ref->statd_value_list ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return 0;
		}

		//
		value_count = apal_list_size( ref->statd_value_list );
		value_count = min( MAX_STAT_VAL_LIST_SIZE, value_count );

		//
		for ( i = 0; i < value_count; i++ )
		{
			stat_value = (STAT_VALUE *)apal_list_pop_front( ref->statd_value_list );
			values[ i ] = stat_value;
		}

		apal_rwlock_unlock( ins_data->rwlock );

		break;

	default :
		return -1;
	}

	//
	if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
	{
		for ( i = 0; i < value_count; i++ )
		{
			APAL_MEM_SAFE_FREE( values[ i ] );
		}
		return -1;
	}

	int rc = 0;

	//
	FILE_HEADER * header = (FILE_HEADER *)ins_data->dbf_header;
	if( header->write_flag == 1 )
	{
		// sync file and update header
		rc = DBFileSync( ins_data->fd );
		if( rc == 0 )
		{
			header->write_flag = 0;
			msync(ins_data->dbf_header, HEADER_LENGTH, MS_SYNC);
		}
		else
		{
			printf( "%s: Sync File failed rc: %d; line: %d\n", __func__, rc, __LINE__ );
		}
	}

	// write data to file
	uint32_t start_offset = 0;
	ssize_t wr_cnt = 0;
	wr_cnt = DBFileWriteValue(ins_data->fd, (const void *)values, value_count*STAT_VALUE_LENGTH, &start_offset);
	//printf( "%s: wr_cnt: %d, start_offset:%u; value_count: %u\n", __func__, wr_cnt, start_offset, value_count );

	// sync file and update header
	header->write_flag = 1;
	msync(ins_data->dbf_header, HEADER_LENGTH, MS_SYNC);
	rc = DBFileSync( ins_data->fd );
	if( rc == 0 )
	{
		if( ref->start_offset == 0)
		{
			ref->start_offset = start_offset;
			header->tag_ids[ref->hv_index].start_offset = ref->start_offset;
		}
		ref->end_offset = start_offset + (value_count - 1 ) * STAT_VALUE_LENGTH;
		header->tag_ids[ref->hv_index].end_offset = ref->end_offset;

		header->write_flag = 0;
		msync(ins_data->dbf_header, HEADER_LENGTH, MS_SYNC);
		ins_data->ins_counts = ins_data->ins_counts - value_count;
	}
	else
	{
		printf( "%s: Sync File failed rc: %d; line: %d\n", __func__, rc, __LINE__ );
	}
	printf( "%s: wr_cnt: %d, start_offset: %u; value_count: %u, header->tag_ids[].end_offset: %u, ref->end_offset: %u, ref->wr_count: %u\n\n",
			__func__, wr_cnt, start_offset, value_count, header->tag_ids[ref->hv_index].end_offset, ref->end_offset, ref->wr_count );

	apal_rwlock_unlock( ins_data->rwlock );

	///
	STAT_VALUE test_value = {0};
	lseek(ins_data->fd, start_offset, SEEK_SET);
	ssize_t rd_count = read(ins_data->fd, &test_value, STAT_VALUE_LENGTH);
	if( rd_count > 0 )
	{
		printf( "\tstart_offset: 0x%x, rd_count: %u, id: %d, type: %d, size: %u time: %llu\n",
				start_offset, rd_count, test_value.id, test_value.type, test_value.size, test_value.time );
	}

	printf( "\tvalues[0], id: %d, type: %d, size: %u time: %llu\n",
			values[0]->id, values[0]->type, values[0]->size, values[0]->time );
	//
	for ( i = 0; i < value_count; i++ )
	{
		APAL_MEM_SAFE_FREE( values[ i ] );
	}

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  Handle insert tag event.
//
// Parameters:
//		conn: 	[in] INS_DATA instance.
// -------------------------------------------------------------------------
static void OnEventInsTag ( INS_DATA * ins_data )
{
	APAL_LOG_TRACE( "SDK::%s: ins_data: %p\n", __func__, ins_data );

	//
	INS_TAG_REF * ref;
	INS_TAG_REF * tags_update[ MAX_LIST_SIZE ];
	uint32_t tag_count = 0;
	uint32_t i = 0;

	//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
	if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
	{
		return;
	}

	if ( apal_list_empty( ins_data->tags_update_list ) )
	{
		apal_rwlock_unlock( ins_data->rwlock );
		return;
	}

	//
	tag_count = apal_list_size( ins_data->tags_update_list );
	tag_count = min( MAX_LIST_SIZE, tag_count );

	//
	for ( i = 0; i < tag_count; i++ )
	{
		ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->tags_update_list );
		tags_update[ i ] = ref;
	}

	apal_rwlock_unlock( ins_data->rwlock );

	//
	for ( i = 0; i < tag_count; i++ )
	{
		InsertTagProc ( ins_data, tags_update[i], tags_update[i]->min_ins_cnt );
	}

}

// -------------------------------------------------------------------------
// Design Notes:  Handle insert statistics date event.
//
// Parameters:
//		conn: 		[in] INS_DATA instance.
//		statType: 	[in] statistics type.
// -------------------------------------------------------------------------
static void OnEventInsStatTag ( INS_DATA * ins_data, STATISTICS_TYPE statType )
{
	APAL_LOG_TRACE(
			"SDK::%s: ins_data: %p, statType: %d\n", __func__, ins_data, statType );

	//
	INS_TAG_REF * ref;
	INS_TAG_REF * tags_update[ MAX_LIST_SIZE ];
	uint32_t tag_count = 0;
	uint32_t i = 0;

	switch ( statType )
	{
	case MINUTE:

		//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			return;
		}

		if ( apal_list_empty( ins_data->statm_update_list ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return;
		}

		//
		tag_count = apal_list_size( ins_data->statm_update_list );
		tag_count = min( MAX_LIST_SIZE, tag_count );

		//
		for ( i = 0; i < tag_count; i++ )
		{
			ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->statm_update_list );
			tags_update[ i ] = ref;
		}

		apal_rwlock_unlock( ins_data->rwlock );

		//
		for ( i = 0; i < tag_count; i++ )
		{
			InsertStatTagProc ( ins_data, tags_update[i], MINUTE );
		}

		break;

	case HOUR:
		//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			return;
		}

		if ( apal_list_empty( ins_data->stath_update_list ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return;
		}

		//
		tag_count = apal_list_size( ins_data->stath_update_list );
		tag_count = min( MAX_LIST_SIZE, tag_count );

		//
		for ( i = 0; i < tag_count; i++ )
		{
			ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->stath_update_list );
			tags_update[ i ] = ref;
		}

		apal_rwlock_unlock( ins_data->rwlock );

		//
		for ( i = 0; i < tag_count; i++ )
		{
			InsertStatTagProc ( ins_data, tags_update[i], HOUR );
		}

		break;

	case DAY:
		//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			return;
		}

		if ( apal_list_empty( ins_data->statd_update_list ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return;
		}

		//
		tag_count = apal_list_size( ins_data->statd_update_list );
		tag_count = min( MAX_LIST_SIZE, tag_count );

		//
		for ( i = 0; i < tag_count; i++ )
		{
			ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->statd_update_list );
			tags_update[ i ] = ref;
		}

		apal_rwlock_unlock( ins_data->rwlock );

		//
		for ( i = 0; i < tag_count; i++ )
		{
			InsertStatTagProc ( ins_data, tags_update[i], DAY );
		}

		break;

	default :
		return ;
	}

}

// -------------------------------------------------------------------------
// Design Notes:  Handle finished event.
// 				  All date in list will be insert into database
//
// Parameters:
//		conn: 		[in] INS_DATA instance.
// -------------------------------------------------------------------------
static void OnEventInsFinished ( INS_DATA * ins_data )
{
	APAL_LOG_TRACE( "SDK::%s: ins_data: %p\n", __func__, ins_data );

	//
	INS_TAG_REF * ref;

	ClearEvent( ins_data );

	//
	while ( !apal_list_empty( ins_data->tags_update_list ) )
	{
		//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			ins_data->is_init = 0;
			return;
		}

		ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->tags_update_list );
		apal_rwlock_unlock( ins_data->rwlock );
		InsertTagProc ( ins_data, ref, 1 );
	}

	//
	while ( !apal_list_empty( ins_data->statm_update_list ) )
	{
		//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			ins_data->is_init = 0;
			return;
		}
		ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->statm_update_list );
		apal_rwlock_unlock( ins_data->rwlock );
		InsertStatTagProc ( ins_data, ref, MINUTE );
	}

	//
	while ( !apal_list_empty( ins_data->stath_update_list ) )
	{
		//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			ins_data->is_init = 0;
			return;
		}
		ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->stath_update_list );
		apal_rwlock_unlock( ins_data->rwlock );
		InsertStatTagProc ( ins_data, ref, HOUR );
	}

	//
	while ( !apal_list_empty( ins_data->statd_update_list ) )
	{
		//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			ins_data->is_init = 0;
			return;
		}
		ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->statd_update_list );
		apal_rwlock_unlock( ins_data->rwlock );
		InsertStatTagProc ( ins_data, ref, DAY );
	}

	//
	while ( !apal_list_empty( ins_data->tag_ref_list ) )
	{
		//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
		if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
		{
			ins_data->is_init = 0;
			return;
		}
		ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->tag_ref_list );
		apal_rwlock_unlock( ins_data->rwlock );
		InsertTagProc ( ins_data, ref, 1 );
		InsertStatTagProc ( ins_data, ref, MINUTE );
		InsertStatTagProc ( ins_data, ref, HOUR );
		InsertStatTagProc ( ins_data, ref, DAY );
	}

	ins_data->is_init = 0;

}

// -------------------------------------------------------------------------
// Design Notes:  Handle timeout tags.
//
// Parameters:
//		conn: 	[in] INS_DATA instance.
// -------------------------------------------------------------------------
static void OnTimeoutTagsProc ( INS_DATA * ins_data )
{
	APAL_LOG_TRACE( "SDK::%s: ins_data: %p\n", __func__, ins_data );

	//
	INS_TAG_REF * ref;
	INS_TAG_REF * tags_update[ MAX_LIST_SIZE ];
	uint32_t tag_count = 0;
	uint32_t i = 0;

	//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
	if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
	{
		return;
	}

	if ( apal_list_empty( ins_data->timeout_update_list ) )
	{
		apal_rwlock_unlock( ins_data->rwlock );
		return;
	}

	//
	tag_count = apal_list_size( ins_data->timeout_update_list );
	tag_count = min( MAX_LIST_SIZE, tag_count );

	//
	for ( i = 0; i < tag_count; i++ )
	{
		ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->timeout_update_list );
		tags_update[ i ] = ref;
	}

	apal_rwlock_unlock( ins_data->rwlock );

	//
	for ( i = 0; i < tag_count; i++ )
	{
#if 0
		UpdateTagProc ( ins_data, tags_update[i] );
#endif
		InsertTagProc ( ins_data, tags_update[i], 1 );
	}

}

// -------------------------------------------------------------------------
// Design Notes:  Thread for insert.
//
// Parameters:
//		param1: [in] INS_DATA instance.
//		param2:
// -------------------------------------------------------------------------
static void InsertThreadProc ( void * param1, void * param2 )
{
	INS_DATA * ins_data = (INS_DATA*) param1;
	if ( apal_semaphore_wait( ins_data->event_sem, 10 ) )
	{
		if ( CheckEvent( ins_data, EVENT_INS_TAG ) )
		{
			OnEventInsTag( ins_data );
		}
		else if ( CheckEvent( ins_data, EVENT_INS_STATM_TAG ) )
		{
			OnEventInsStatTag( ins_data, MINUTE );
		}
		else if ( CheckEvent( ins_data, EVENT_INS_STATH_TAG ) )
		{
			OnEventInsStatTag( ins_data, HOUR );
		}
		else if ( CheckEvent( ins_data, EVENT_INS_STATD_TAG ) )
		{
			OnEventInsStatTag( ins_data, DAY );
		}
		else if ( CheckEvent( ins_data, EVENT_INS_FINISHED ) )
		{
			OnEventInsFinished( ins_data );
		}
	}
}

// -------------------------------------------------------------------------
// Design Notes:  Thread for insert.
//
// Parameters:
//		param1: [in] INS_DATA instance.
//		param2:
// -------------------------------------------------------------------------
static void UpdateThreadProc ( void * param1, void * param2 )
{
	INS_DATA * ins_data = (INS_DATA*) param1;

	//
	INS_TAG_REF * ref;
	uint32_t tag_count = 0;
	uint32_t value_count = 0;
	uint64_t elapsed_time = 0;
	uint32_t i = 0;

	//
	APAL_TIME current_time;
	apal_time_get( &current_time );

	//
	if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
	{
		return;
	}

	if ( apal_list_empty( ins_data->tag_ref_list ) )
	{
		apal_rwlock_unlock( ins_data->rwlock );
		return;
	}

	//
	tag_count = apal_list_size( ins_data->tag_ref_list );

	for ( i = 0; i < tag_count; i++ )
	{
		ref = (INS_TAG_REF *)apal_list_get_at ( ins_data->tag_ref_list, i );
		value_count = apal_list_size( ref->value_list );
		elapsed_time = current_time.time - ref->final_update_time;

		if( ( elapsed_time >= TAG_UPDATE_TIMEOUT ) && ( value_count > 0 ) )
		{
			apal_list_push_back( ins_data->timeout_update_list, (void *)ref );
		}
	}

	apal_rwlock_unlock( ins_data->rwlock );

	OnTimeoutTagsProc ( ins_data );
}

// -------------------------------------------------------------------------
// Design Notes:  Destruct INS_TAG_REF instance.
//
// Parameters:
//		ref: [in] The INS_TAG_REF instance will be destructed.
// -------------------------------------------------------------------------
static int DestructInsTagRef ( INS_TAG_REF * ref )
{
	if( ref == NULL )
	{
		return 0;
	}

	//
	if(ref->value_list != APAL_INVALID_HANDLE)
	{
		while ( !apal_list_empty( ref->value_list ) )
		{
			TAG_VALUE * tag;
			tag = (TAG_VALUE *)apal_list_pop_front( ref->value_list );
			APAL_MEM_SAFE_FREE( tag );
		}
		APAL_SAFE_CLOSE_HANDLE( ref->value_list );
	}

	//
	if(ref->statm_value_list != APAL_INVALID_HANDLE)
	{
		while ( !apal_list_empty( ref->statm_value_list ) )
		{
			STAT_VALUE * stat_tag;
			stat_tag = (STAT_VALUE *)apal_list_pop_front( ref->statm_value_list );
			APAL_MEM_SAFE_FREE( stat_tag );
		}
		APAL_SAFE_CLOSE_HANDLE( ref->statm_value_list );

	}

	//
	if(ref->stath_value_list != APAL_INVALID_HANDLE)
	{
		while ( !apal_list_empty( ref->stath_value_list ) )
		{
			STAT_VALUE * stat_tag;
			stat_tag = (STAT_VALUE *)apal_list_pop_front( ref->stath_value_list );
			APAL_MEM_SAFE_FREE( stat_tag );
		}
		APAL_SAFE_CLOSE_HANDLE( ref->stath_value_list );

	}

	//
	if(ref->statd_value_list != APAL_INVALID_HANDLE)
	{
		while ( !apal_list_empty( ref->statd_value_list ) )
		{
			STAT_VALUE * stat_tag;
			stat_tag = (STAT_VALUE *)apal_list_pop_front( ref->statd_value_list );
			APAL_MEM_SAFE_FREE( stat_tag );
		}
		APAL_SAFE_CLOSE_HANDLE( ref->statd_value_list );

	}

	//
	APAL_MEM_SAFE_FREE( ref );

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  Destruct INS_DATA instance.
//
// Parameters:
//		ins_data: [in] The INS_DATA instance will be destructed.
// -------------------------------------------------------------------------
static int DestructInsData ( INS_DATA * ins_data )
{
	if( ins_data == NULL )
	{
		return 0;
	}

	FileHeaderUnmap(ins_data->dbf_header);
	DBFileClose(ins_data->fd);

	APAL_SAFE_CLOSE_HANDLE( ins_data->thread_insert );
	APAL_SAFE_CLOSE_HANDLE( ins_data->thread_update );

	/*
	if( ins_data->rt_tag_ref_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->rt_tag_ref_list ) )
		{
			(INS_TAG_REF *)apal_list_pop_front( ins_data->rt_tag_ref_list );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->rt_tag_ref_list );*/

	//
	if( ins_data->tag_ref_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->tag_ref_list ) )
		{
			INS_TAG_REF * ref;
			ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->tag_ref_list );
			DestructInsTagRef(ref);
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->tag_ref_list );

	APAL_SAFE_CLOSE_HANDLE( ins_data->tags_update_list );
	APAL_SAFE_CLOSE_HANDLE( ins_data->statm_update_list );
	APAL_SAFE_CLOSE_HANDLE( ins_data->stath_update_list );
	APAL_SAFE_CLOSE_HANDLE( ins_data->statd_update_list );
	APAL_SAFE_CLOSE_HANDLE( ins_data->timeout_update_list );

	APAL_SAFE_CLOSE_HANDLE( ins_data->rwlock );
	APAL_SAFE_CLOSE_HANDLE( ins_data->event_sem );

	APAL_MEM_SAFE_FREE( ins_data );

	return 0;
}

static int InitLogId(INS_DATA * ins_data, INS_TAG_REF * ref)
{
	APAL_LOG_TRACE("SDK::%s: tag_name:%s\n", __func__, ref->tag_name );

    int32_t i = 0;
    FILE_HEADER *header = (FILE_HEADER *)ins_data->dbf_header;

    int32_t *index = NULL;
    if ( apal_hash_find( ins_data->tag_ref_index_htbl,
    		ref->tag_name, (void**) &index ) && (NULL != index) )
    {
    	//printf("SDK::%s: find %s: index: %d\n", __func__, ref->tag_name, *index );

    	ref->hv_index = *index;
    	ref->log_id = header->tag_ids[ref->hv_index].id;
    	ref->start_offset = header->tag_ids[ref->hv_index].start_offset;
    	ref->end_offset = header->tag_ids[ref->hv_index].end_offset;

    	printf("SDK::%s: tag_name:%s, ref: hv_index: %d, log_id: %d, start_offset: %u, end_offset: %u\n\n",
    			__func__, ref->tag_name, ref->hv_index, ref->log_id, ref->start_offset, ref->end_offset );

    	return 0;
    }

	//printf("SDK::%s: %s not find: ref->hv_index = header->len:%d\n", __func__, ref->tag_name, header->len );
    ref->hv_index = header->len;
	ref->log_id = header->last_id + 1;
	ref->start_offset = 0;
	ref->end_offset = 0;
	printf("SDK::%s: tag_name:%s, ref: hv_index: %d, log_id: %d, start_offset: %u, end_offset: %u\n\n",
			__func__, ref->tag_name, ref->hv_index, ref->log_id, ref->start_offset, ref->end_offset );

    header->len++;
    header->last_id = ref->log_id;
    header->tag_ids[ref->hv_index].id = ref->log_id;
    header->tag_ids[ref->hv_index].start_offset = ref->start_offset;
    header->tag_ids[ref->hv_index].end_offset = ref->end_offset;
    apal_str_printf( header->tag_ids[ref->hv_index].tag_name, MAX_NAME_LEN, "%s", ref->tag_name );

    int32_t *new_index = new (int32_t);
    *new_index = ref->hv_index;
    apal_hash_add( ins_data->tag_ref_index_htbl, ref->tag_name, (void *) new_index );
    apal_list_push_back( ins_data->tag_ref_index_list, (void*) new_index );

	return 0;
}


static int InsInitFileHeader(INS_DATA * ins_data)
{
	APAL_LOG_TRACE("SDK::%s: fd: %d\n", __func__, ins_data->fd);

    //
    if (ins_data->fd == -1)
    {
        return -1;
    }

    // To obtain file size
	struct stat sb;
	if ( fstat(ins_data->fd, &sb) == -1 )
	{
		printf("SDK::%s: fstat failed, errno: %d\n", __func__, errno);
		return -errno;
	}
	printf("SDK::%s: file size: %d\n", __func__, sb.st_size);

	//
	bool new_created = false;
	if( sb.st_size < HEADER_LENGTH)
	{
		FILE_HEADER new_header = {0};
		new_header.last_id = -1;
		new_created = true;

		lseek( ins_data->fd, 0, SEEK_SET );
		ssize_t wr_count = write( ins_data->fd, &new_header, HEADER_LENGTH );
		//printf("SDK::%s: write HEADER_LENGTH: %d, return wr_count: %d\n", __func__, HEADER_LENGTH, wr_count);
	    if( wr_count <= 0 )
	    {
	    	printf("SDK::%s: write header failed, return wr_count: %d, errno: %d\n", __func__, wr_count, errno);
	    	return -errno;
	    }

	    int rc = fsync(ins_data->fd);
	    if( rc < 0 )
	    {
	       	printf("SDK::%s: fsync failed, rc: %d, errno: %d\n", __func__, rc, errno);
	       	return -errno;
	    }
	}

	// mapping file header
	uint32_t offset = lseek( ins_data->fd, 0, SEEK_SET );
	ins_data->dbf_header = mmap( NULL, HEADER_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, ins_data->fd, 0 );
	if(ins_data->dbf_header == MAP_FAILED)
	{
		printf("SDK::%s: Could not map view of file (%d) header, errno: %d\n\n", __func__, ins_data->fd, errno);
		return -errno;
    }
	//printf("SDK::%s: ins_data->dbf_header: %p\n", __func__, ins_data->dbf_header);

    FILE_HEADER *header = (FILE_HEADER *)ins_data->dbf_header;
    printf( "SDK::%s: header->write_flag:%d, header->len:%d, header->last_id:%d, ins_data->dbf_header: %p\n\n",
    		__func__, header->write_flag, header->len, header->last_id, ins_data->dbf_header);

	//
	if(new_created == false)
	{
	    int32_t i = 0;
	    for ( i = 0; i < header->len; i++ )
	    {
	    	int32_t *index = new (int32_t);
	    	*index = i;
	    	if(( ins_data->tag_ref_index_htbl == APAL_INVALID_HANDLE) || ( ins_data->tag_ref_index_list == APAL_INVALID_HANDLE))
	    	{
	    		printf( "SDK::%s: tag_ref_index_list/tag_ref_index_htbl == APAL_INVALID_HANDLE\n", __func__ );
	    		APAL_MEM_SAFE_FREE( index );
	    		return -1;
	    	}

	    	apal_hash_add( ins_data->tag_ref_index_htbl, header->tag_ids[i].tag_name, (void *) index );
	    	apal_list_push_back( ins_data->tag_ref_index_list, (void*) index );
	    }
	}
	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:
//     API for DataLogger. Initialize for insert data into the database.
//
// Parameters:
//		timeStamp: [in] timeStamp
//
// Returns:
//		int. 0: successful; negative: failed/error code.
//
// Remarks:
// 		Before InsertTagValueV2 and InsertStatValueV2 API can be used correctly,
// 		you must call this function to perform a initialization firstly.
// -------------------------------------------------------------------------
static int InsertOpen( INS_DATA * ins_data, char const * pTagName )
{
	printf("SDK::%s: pTagName: %s\n", __func__, pTagName );

	do
	{
		uint32_t min_trans_count = 0;
		INS_TAG_REF * ref;

		// allocate INS_TAG_REF object
		ref = (INS_TAG_REF *)apal_mem_alloc( sizeof( INS_TAG_REF ) );
		if ( NULL == ref )
		{
			break;
		}

		// copy tag name
		apal_str_printf(
				ref->tag_name, sizeof(ref->tag_name), "%s", pTagName );
		ref->log_type = PERIOD_DATA;
		ref->period = 1;
		ref->wr_count = 0;				//
		ref->stam_wr_count= 0;			//
		ref->stah_wr_count= 0;			//
		ref->stad_wr_count= 0;			//

		// Create value list for tag
		ref->value_list = apal_list_create( MAX_LIST_SIZE );
		if( ref->value_list == APAL_INVALID_HANDLE )
		{
			DestructInsTagRef ( ref );
			break;
		}

		// find log id in header
		// if not found, add new one
		InitLogId( ins_data, ref);
	    printf("SDK::%s: ref: tag_name: %s: hv_index: %d, log_id: %d, start_offset: %u, end_offset: %u\n",
	    		__func__, ref->tag_name, ref->hv_index, ref->log_id,
	    		ref->start_offset, ref->end_offset );

		//
		ref->min_ins_cnt = 10;
		min_trans_count = ins_data->min_trans_count + ref->min_ins_cnt;
		ins_data->min_trans_count = min( MIN_TRANS_COUNT, min_trans_count);

		// Create "MINUTE" value list
		ref->statm_value_list = apal_list_create( MAX_STAT_VAL_LIST_SIZE );
		if( ref->statm_value_list == APAL_INVALID_HANDLE )
		{
			DestructInsTagRef ( ref );
			break;
		}

		// Create "HOUR" value list
		ref->stath_value_list = apal_list_create( MAX_STAT_VAL_LIST_SIZE );
		if( ref->stath_value_list == APAL_INVALID_HANDLE )
		{
			DestructInsTagRef ( ref );
			break;
		}

		// Create "DAY" value list
		ref->statd_value_list = apal_list_create( MAX_STAT_VAL_LIST_SIZE );
		if( ref->statd_value_list == APAL_INVALID_HANDLE )
		{
			DestructInsTagRef ( ref );
			break;
		}

	    // Add the key to hash table
		apal_hash_add( ins_data->tag_ref_htbl, ref->tag_name, (void *) ref );

		//
		apal_list_push_back( ins_data->tag_ref_list, (void*) ref );

		return 0;

	} while ( 0 );

	//
	//UninitInsertForLogger(timeStamp);

	return DATALOGGER_ERROR_INS_INIT;
}

// -------------------------------------------------------------------------
// Design Notes:
//     API for DataLogger. Initialize for insert data into the database.
//
// Parameters:
//		timeStamp: [in] timeStamp
//
// Returns:
//		int. 0: successful; negative: failed/error code.
//
// Remarks:
// 		Before InsertTagValueV2 and InsertStatValueV2 API can be used correctly,
// 		you must call this function to perform a initialization firstly.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int InitInsertForLogger( uint64_t timeStamp )
{
	//
	uint32_t i = 0;
	int rc = 0;

	APAL_LOG_TRACE("SDK::%s: timeStamp: %llu\n", __func__, timeStamp );

	do
	{
		//
		// 1. To check whether the SQLite database file is already created
		//
		INS_DATA * ins_data = NULL;

		uint64_t day_elapsed = (timeStamp + g_time_zone * SEC_PER_HOUR ) / SEC_PER_DAY;

		char key[32] = {0};
	    apal_str_printf( key, sizeof(key), "%llu", day_elapsed );
	    printf("SDK::%s: key: %s, timeStamp: %llu\n", __func__, key, timeStamp );

		// If not found, add one to the hash table
		if ( !apal_hash_find( g_ins_db_htbl, key, (void**) &ins_data )
				|| (NULL == ins_data)
				|| ((NULL != ins_data) && (-1 == ins_data->fd)))
		{
			//
			time_t curTime_t = timeStamp;
			tm * tmStamp = localtime(&curTime_t);
			bool is_new_file = true;

			// alloc memory
			ins_data = (INS_DATA *)apal_mem_alloc( sizeof( INS_DATA ) );
			BON( ins_data );

			//
			ins_data->rwlock = apal_rwlock_create( NULL );
			if( ins_data->rwlock == APAL_INVALID_HANDLE )
			{
				APAL_MEM_SAFE_FREE( ins_data );
				break;
			}

			//
			ins_data->event_sem = apal_semaphore_create( NULL, 0 );
			if( ins_data->event_sem == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			// Open/Create the new SQLite DataLogger database file
			// The Tags Database naming rule: DB_Data_YYYYMMDD.db3
		    char dbName[ MAX_DB_PATH_LEN ] = { 0 };
		    apal_str_printf(
		    		ins_data->db_name, sizeof(ins_data->db_name), "%s_%d%02d%02d.%s",
					DB_HEADER_NAME, 1900 + tmStamp->tm_year,
					1 + tmStamp->tm_mon, tmStamp->tm_mday, DB_EXT_NAME );
			apal_str_printf(dbName, sizeof(dbName), "%s%s", dbDir, ins_data->db_name);
			APAL_LOG_TRACE("SDK::%s: dbName: %s\n", __func__, dbName);


			// open database file
			ins_data->fd = -1;
			ins_data->fd = DBFileOpen(dbName, DB_OPEN_READWRITE | DB_OPEN_CREATE | O_CREAT );
			if ( ins_data->fd < 0 )
			{
				DestructInsData ( ins_data );
				break;
			}

			//
			ins_data->tag_count = 0 ;
			ins_data->ins_counts = 0;
			ins_data->min_trans_count = 0;

			// Create tag update list
			ins_data->tags_update_list = apal_list_create(
					max ( MAX_ID_COUNT, ins_data->tag_count ));
			if( ins_data->tags_update_list == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			// Create MINUTE statistics update list
			ins_data->statm_update_list = apal_list_create(
					max ( MAX_ID_COUNT, ins_data->tag_count ) );
			if( ins_data->statm_update_list == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			// Create HOUR statistics update list
			ins_data->stath_update_list = apal_list_create(
					max ( MAX_ID_COUNT, ins_data->tag_count ) );
			if( ins_data->stath_update_list == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			// Create DAY statistics update list
			ins_data->statd_update_list = apal_list_create(
					max ( MAX_ID_COUNT, ins_data->tag_count ) );
			if( ins_data->statd_update_list == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			// Create INS_TAG_REF list for update tags
			ins_data->tag_ref_list = apal_list_create( MAX_ID_COUNT );
			if( ins_data->tag_ref_list == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			/* Create INS_TAG_REF list for run-time tags
			ins_data->rt_tag_ref_list = apal_list_create( MAX_ID_COUNT );
			if( ins_data->rt_tag_ref_list == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}*/

			//
			ins_data->timeout_update_list = apal_list_create(
					max ( MAX_ID_COUNT, ins_data->tag_count ));
			if( ins_data->timeout_update_list == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			// create hash table for INS_TAG_REF for update tags
			ins_data->tag_ref_htbl = apal_hash_create( MAX_ID_COUNT );
			if ( ins_data->tag_ref_htbl == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			// Create INS_TAG_REF list for run-time tags
			ins_data->tag_ref_index_list = apal_list_create( MAX_ID_COUNT );
			if( ins_data->tag_ref_index_list == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			ins_data->tag_ref_index_htbl = apal_hash_create( MAX_ID_COUNT );
			if ( ins_data->tag_ref_index_htbl == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			//
			rc = InsInitFileHeader(ins_data);
			if( rc != 0 )
			{
				DestructInsData ( ins_data );
				break;
			}
			printf("SDK::%s: dbf_header: %p\n", __func__, ins_data->dbf_header );

			//
			ins_data->thread_insert = apal_thread_start(
				InsertThreadProc,
				ins_data,
				NULL,
				INSERT_THREAD_LOOP_CYCLE,
				"InsertThreadProc" );
			if( ins_data->thread_insert == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			if ( !(apal_thread_wait_for_start( ins_data->thread_insert, THREAD_WAIT_TIMEOUT )) )
			{
				DestructInsData ( ins_data );
				break;
			}

			//
			ins_data->thread_update = apal_thread_start(
				UpdateThreadProc,
				ins_data,
				NULL,
				UPDATE_THREAD_LOOP_CYCLE,
				"UpdateThreadProc" );
			if( ins_data->thread_update == APAL_INVALID_HANDLE )
			{
				DestructInsData ( ins_data );
				break;
			}

			if ( !(apal_thread_wait_for_start( ins_data->thread_update, THREAD_WAIT_TIMEOUT )) )
			{
				DestructInsData ( ins_data );
				break;
			}

			//
			ClearEvent( ins_data );

		    // Add the key to hash table
			apal_hash_add( g_ins_db_htbl, key, (void *) ins_data );
			ins_data->is_init = 1;

			uint64_t *ins_day = new (uint64_t);
			*ins_day = day_elapsed * SEC_PER_DAY;
			apal_list_push_back( g_ins_db_key_list, (void*) ins_day );

		} // end of "if ( !apal_hash_find( g_ins_db_htbl, curName, (void**) &ins_data ) || (NULL == ins_data) )"

		return 0;

	} while ( 0 );

	//
	UninitInsertForLogger(timeStamp);

	return DATALOGGER_ERROR_INS_INIT;
}

// -------------------------------------------------------------------------
// Design Notes:
//     API for DataLogger. Uninitialize for insert data into the database.
//
// Parameters:
//		timeStamp: [in] Specify the time stamp for creating database file
//
// Returns:
//		int. 0: successful; negative: failed/error code.
//
// Remarks:
// 		Call this function to release all resources allocated by the
// 		InitInsertForLogger API.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int UninitInsertForLogger ( uint64_t timeStamp )
{
	//
	INS_DATA * ins_data = NULL;

	APAL_LOG_TRACE("SDK::%s: timeStamp: %llu\n", __func__, timeStamp );

	//
    if( g_ins_db_htbl == APAL_INVALID_HANDLE )
    {
    	return 0;
    }

	uint64_t day_elapsed = (timeStamp + g_time_zone * SEC_PER_HOUR ) / SEC_PER_DAY;

	char key[32] = {0};
    apal_str_printf( key, sizeof(key), "%llu", day_elapsed );

    APAL_LOG_INFO("SDK::%s: timeStamp: %llu, key: %s\n", __func__, timeStamp, key );

	// If not found, return
 	if ( !apal_hash_find( g_ins_db_htbl, key, (void**) &ins_data ) || (NULL == ins_data) )
	{
		return 0;
	}

 	if  (-1 != ins_data->fd)
	{
 	 	//
 		SetEvent(ins_data, EVENT_INS_FINISHED);
	}

	uint32_t try_count = 0;
	while((ins_data->is_init == 1) && (try_count < 100))
	{
		apal_systick_sleep ( 100 );
		try_count++;
	}

	//
	APAL_SAFE_CLOSE_HANDLE( ins_data->thread_insert );
	APAL_SAFE_CLOSE_HANDLE( ins_data->thread_update );

	//
	if( ins_data->dbf_header != NULL)
	{
		FILE_HEADER * header = (FILE_HEADER *)ins_data->dbf_header;
		if( header->write_flag == 1 )
		{
			// sync file and update header
			if( DBFileSync( ins_data->fd ) == 0 )
			{
				header->write_flag = 0;
				msync(ins_data->dbf_header, HEADER_LENGTH, MS_SYNC);
			}
		}
		FileHeaderUnmap(ins_data->dbf_header);
	}
	DBFileClose(ins_data->fd);

	/*
	if( ins_data->rt_tag_ref_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->rt_tag_ref_list ) )
		{
			apal_list_pop_front( ins_data->rt_tag_ref_list );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->rt_tag_ref_list );*/

	//
	if( ins_data->timeout_update_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->timeout_update_list ) )
		{
			apal_list_pop_front( ins_data->timeout_update_list );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->timeout_update_list );

	//
	if( ins_data->statm_update_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->statm_update_list ) )
		{
			apal_list_pop_front( ins_data->statm_update_list );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->statm_update_list );

	//
	if( ins_data->stath_update_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->stath_update_list ) )
		{
			apal_list_pop_front( ins_data->stath_update_list );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->stath_update_list );

	//
	if( ins_data->statd_update_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->statd_update_list ) )
		{
			apal_list_pop_front( ins_data->statd_update_list );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->statd_update_list );

	//
	if( ins_data->tags_update_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->tags_update_list ) )
		{
			apal_list_pop_front( ins_data->tags_update_list );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->tags_update_list );

	//
	if( ins_data->tag_ref_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->tag_ref_list ) )
		{
			INS_TAG_REF * ref;
			ref = (INS_TAG_REF *)apal_list_pop_front( ins_data->tag_ref_list );
			DestructInsTagRef ( ref );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->tag_ref_list );

	//
	if( ins_data->tag_ref_index_list != APAL_INVALID_HANDLE )
	{
		while ( !apal_list_empty( ins_data->tag_ref_index_list ) )
		{
			int32_t *index;
			index = (int32_t *)apal_list_pop_front( ins_data->tag_ref_index_list );
			APAL_MEM_SAFE_FREE( index );
		}
	}
	APAL_SAFE_CLOSE_HANDLE( ins_data->tag_ref_index_list );
	APAL_SAFE_CLOSE_HANDLE( ins_data->tag_ref_index_htbl );
	APAL_SAFE_CLOSE_HANDLE( ins_data->tag_ref_htbl );
	APAL_SAFE_CLOSE_HANDLE( ins_data->rwlock );
	APAL_SAFE_CLOSE_HANDLE( ins_data->event_sem );

	APAL_MEM_SAFE_FREE( ins_data );

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  API for DataLogger. Insert record into the database.
//
// Parameters:
//		pTagName: 	[in] Tag name.
//		value: 		[in] DC_TAG value.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int InsertTagValueV2 ( char const * pTagName, DC_TAG const * value )
{
	INS_DATA * ins_data;
	INS_TAG_REF * ref;
	int ret = 0;

	APAL_LOG_TRACE(
			"SDK::%s: tag: %s, value: %.4lf, time: %llu, quality: %04xH, usec: %u\n",
			__func__, pTagName, value->value, value->time,
			value->quality, value->usec );

	if( pTagName == NULL )
	{
		APAL_LOG_ERROR( "%s: pTagName == NULL\n", __func__ );
		return DATALOGGER_ERROR_TAGNAME_NULL;
	}
	if( value == NULL )
	{
		APAL_LOG_ERROR( "%s: value == NULL\n", __func__ );
		return DATALOGGER_ERROR_PARAM_VALUE;
	}

	//
	APAL_TIME current_time;
	apal_time_get( &current_time );

	uint64_t day_elapsed = (value->time + g_time_zone * SEC_PER_HOUR ) / SEC_PER_DAY;

	char key[32] = {0};
	apal_str_printf( key, sizeof(key), "%llu", day_elapsed );

	// If INS_DATA object is not found, create one
	if ( !apal_hash_find( g_ins_db_htbl, key,
			(void**) &ins_data ) || (NULL == ins_data) )
	{
		//printf("SDK::%s: call InitInsertForLogger(time: %llu)\n", __func__, value->time);
		ret = InitInsertForLogger( value->time );
		if(ret != 0 )
		{
			return DATALOGGER_ERROR_INS_INIT;
		}

		if ( !apal_hash_find( g_ins_db_htbl, key,
				(void**) &ins_data ) || (NULL == ins_data) )
		{
			return DATALOGGER_ERROR_INS_NOT_INIT;
		}
	}

 	if  (-1 == ins_data->fd)
	{
		return DATALOGGER_ERROR_INS_NOT_INIT;
	}

	//
	if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
	{
		return DATALOGGER_ERROR_INS_ADD_VALUE;
	}

	// find the INS_TAG_REF tag object
	if ( !apal_hash_find( ins_data->tag_ref_htbl,
			pTagName, (void**) &ref ) || (NULL == ref) )
	{
		ret = InsertOpen( ins_data, pTagName );
		if(ret != 0 )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return ret;
		}

		// find the INS_TAG_REF tag object again
		if ( !apal_hash_find( ins_data->tag_ref_htbl,
				pTagName, (void**) &ref ) || (NULL == ref) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			APAL_LOG_ERROR( "%s: invalid name %s\n", __func__, pTagName );
			return DATALOGGER_ERROR_TAG_NONEXIST;
		}
	}

	//
	TAG_VALUE * tag_value = (TAG_VALUE *)apal_mem_alloc( sizeof( TAG_VALUE ) );
	if ( NULL == tag_value )
	{
		apal_rwlock_unlock( ins_data->rwlock );
		return DATALOGGER_ERROR_NO_MEM;
	}
	apal_mem_set(tag_value, 0, sizeof( TAG_VALUE ));

	//
	tag_value->id = ref->log_id;
	tag_value->type = ref->log_type;
	tag_value->size = TAG_VALUE_LENGTH;
	tag_value->value = value->value;
	tag_value->quality = value->quality;
	tag_value->time = value->time;
	tag_value->usec = value->usec;
	printf(
			"SDK::%s: tag: %s, tag_value: id: %u, type: %d, size: %u, value: %.4lf, time: %llu\n",
			__func__, pTagName, tag_value->id, tag_value->type, tag_value->size, tag_value->value, tag_value->time);

	// add tag value to value_list
	if ( !apal_list_push_back( ref->value_list, (void *)tag_value ) )
	{
		apal_rwlock_unlock( ins_data->rwlock );
		APAL_MEM_SAFE_FREE( tag_value );
		return DATALOGGER_ERROR_INS_ADD_VALUE;
	}

	//
	ref->final_update_time = current_time.time;

	//
	uint32_t counts = apal_list_size( ref->value_list );
	if( counts >= ref->min_ins_cnt )
	{
		// add ref object to update list
		if ( !apal_list_push_back( ins_data->tags_update_list, (void *)ref ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return DATALOGGER_ERROR_INS_ADD_VALUE;
		}

		SetEvent( ins_data, EVENT_INS_TAG );
	}

	//
	apal_rwlock_unlock( ins_data->rwlock );

	return 0;
}

// -------------------------------------------------------------------------
// Design Notes:  API for DataLogger. Insert statistics data into the database.
//
// Parameters:
//		type: 		[in] The Statistics type.
//						 0: minute data, 1: hour date, 2: day data.
//		pTagName: 	[in] Tag name.
//		timeStamp: 	[in] Time stamp, in UTC format.
//		quality: 	[in] Quality.
//		partial: 	[in] Partial. 0: Complete; 1: Partial.
//		lastValue: 	[in] The Last value.
//		minValue: 	[in] The minimum value.
//		maxValue: 	[in] The maximum value.
//		avgValue: 	[in] The average value.
//
// Returns:
//		int. 0: successful; negative: failed/error code.
// -------------------------------------------------------------------------
EXTERN_C
DATALOGGERSDK_API int InsertStatValueV2(
		int type,
		char const * pTagName,
		uint64_t timeStamp,
		int16_t quality,
		int16_t partial,
		double lastValue,
		double minValue,
		double maxValue,
		double avgValue
		)
{

	INS_DATA * ins_data;
	INS_TAG_REF * ref;

	APAL_LOG_TRACE( "SDK::%s: tag name: %s, type: %d\n", __func__, pTagName, type );

	if( pTagName == NULL )
	{
		APAL_LOG_ERROR( "%s: pTagName == NULL\n", __func__ );
		return DATALOGGER_ERROR_TAGNAME_NULL;
	}

	uint64_t day_elapsed = (timeStamp + g_time_zone * SEC_PER_HOUR ) / SEC_PER_DAY;

	char key[32] = {0};
    apal_str_printf( key, sizeof(key), "%llu", day_elapsed );

	// If INS_DATA object is not found, create one
    int ret = 0;
	if ( !apal_hash_find( g_ins_db_htbl, key,
			(void**) &ins_data ) || (NULL == ins_data) )
	{
		ret = InitInsertForLogger( timeStamp );
		if(ret != 0 )
		{
			return DATALOGGER_ERROR_INS_INIT;
		}

		if ( !apal_hash_find( g_ins_db_htbl, key,
				(void**) &ins_data ) || (NULL == ins_data) )
		{
			return DATALOGGER_ERROR_INS_NOT_INIT;
		}
	}

 	if  (-1 == ins_data->fd)
	{
		return DATALOGGER_ERROR_INS_NOT_INIT;
	}

	//
	//apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE );
	if ( !apal_rwlock_wrlock( ins_data->rwlock, APAL_INFINITE ) )
	{
		return DATALOGGER_ERROR_INS_ADD_VALUE;
	}

	// find the INS_TAG_REF tag object
	if ( !apal_hash_find( ins_data->tag_ref_htbl,
			pTagName, (void**) &ref ) || (NULL == ref) )
	{
		//
		ret = InsertOpen( ins_data, pTagName );
		if(ret != 0 )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return ret;
		}

		// find the INS_TAG_REF tag object again
		if ( !apal_hash_find( ins_data->tag_ref_htbl,
				pTagName, (void**) &ref ) || (NULL == ref) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			APAL_LOG_ERROR( "%s: invalid name %s\n", __func__, pTagName );
			return DATALOGGER_ERROR_TAG_NONEXIST;
		}
	}

	//
	STAT_VALUE * stat_value = (STAT_VALUE *)apal_mem_alloc( sizeof( STAT_VALUE ) );
	if ( NULL == stat_value )
	{
		apal_rwlock_unlock( ins_data->rwlock );
		APAL_LOG_ERROR(
				"%s: fail to alloc stat_value memory for %s\n", __func__, pTagName );
		return DATALOGGER_ERROR_NO_MEM;
	}
	apal_mem_set(stat_value, 0, sizeof( sizeof( STAT_VALUE ) ));

	//
    stat_value->id = ref->log_id;
	stat_value->size = STAT_VALUE_LENGTH;
	stat_value->time = timeStamp;
	stat_value->quality = quality;
	stat_value->partial = partial;
	stat_value->last_value = lastValue;
	stat_value->min_value = minValue;
	stat_value->max_value = maxValue;
	stat_value->avg_value = avgValue;
	APAL_LOG_TRACE(
			"SDK::%s: tag: %s, stat_value->id: %u, ref: hv_index: %u, log_id: %u, start_offset: %u, end_offset: %u\n",
			__func__, pTagName, stat_value->id, ref->hv_index, ref->log_id, ref->start_offset, ref->end_offset);

	//
	switch ( type )
	{
	case MINUTE:
		stat_value->type = STATM_DATA;

		// add statistics value to statm_value_list
		if ( !apal_list_push_back( ref->statm_value_list, (void *)stat_value ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			APAL_MEM_SAFE_FREE( stat_value );
			return DATALOGGER_ERROR_INS_ADD_VALUE;
		}

		// add ref object to statm_update_list
		if ( !apal_list_push_back( ins_data->statm_update_list, (void *)ref ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return DATALOGGER_ERROR_INS_ADD_VALUE;
		}

		SetEvent( ins_data, EVENT_INS_STATM_TAG );
		break;

	case HOUR:
		stat_value->type = STATH_DATA;
		// add statistics value to stath_value_list
		if ( !apal_list_push_back( ref->stath_value_list, (void *)stat_value ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			APAL_MEM_SAFE_FREE( stat_value );
			return DATALOGGER_ERROR_INS_ADD_VALUE;
		}

		// add ref object to stath_update_list
		if ( !apal_list_push_back( ins_data->stath_update_list, (void *)ref ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return DATALOGGER_ERROR_INS_ADD_VALUE;
		}

		SetEvent( ins_data, EVENT_INS_STATH_TAG );

		break;

	case DAY:
		stat_value->type = STATD_DATA;
		// add statistics value to statd_value_list
		if ( !apal_list_push_back( ref->statd_value_list, (void *)stat_value ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			APAL_MEM_SAFE_FREE( stat_value );
			return DATALOGGER_ERROR_INS_ADD_VALUE;
		}

		// add ref object to statd_update_list
		if ( !apal_list_push_back( ins_data->statd_update_list, (void *)ref ) )
		{
			apal_rwlock_unlock( ins_data->rwlock );
			return DATALOGGER_ERROR_INS_ADD_VALUE;
		}

		SetEvent( ins_data, EVENT_INS_STATD_TAG );

		break;

	default :
		apal_rwlock_unlock( ins_data->rwlock );
		APAL_MEM_SAFE_FREE( stat_value );
		APAL_LOG_ERROR(	"%s: invalid stat type: %d\n", __func__, type );
		return DATALOGGER_ERROR_PARAM_STATTYPE;
	}
	printf(
			"SDK::%s: tag: %s, stat_value: id: %u, type: %d, size: %u, last_value: %.4lf, time: %llu\n",
			__func__, pTagName, stat_value->id, stat_value->type, stat_value->size, stat_value->last_value, stat_value->time);

	apal_rwlock_unlock( ins_data->rwlock );

	return 0;
}
