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
#include <list>
#include <dirent.h>
#include <iostream>
#include <time.h>
#include <sys/mman.h>
#include <errno.h>

using namespace std;

#ifdef __cplusplus
#define EXTERN_C						extern "C"
#else
#define EXTERN_C
#endif /* __cplusplus */

//#define DBFILE_LENGTH		strlen ("DB_Data_XXXXXXXX.adb")
#define DBFILE_LENGTH		20
typedef void* APAL_HANDLE;
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



struct ins_tag_ref
{
	char 		tag_name[ 4096 ];			// Tag name
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

typedef struct _TAG_VALUE_S
{
	int32_t 	id;
    int8_t 		type;
    //int8_t 		size;
    uint8_t 	size;
    //DC_TAG 	value;
    uint64_t 	time;
	int16_t 	quality;
	double 		value;
    uint32_t 	usec;
    //int64_t 		next_offset;
}TAG_VALUE, *PTAG_VALUE;
#define TAG_VALUE_LENGTH 	(sizeof(TAG_VALUE))

#define DB_OPEN_READONLY         0x00000001
#define DB_OPEN_READWRITE        0x00000002
#define DB_OPEN_CREATE           0x00000004

static int InsertTagProc ( INS_DATA * ins_data, INS_TAG_REF * ref, uint32_t min_ins_cnt )
{
	uint32_t value_count = 0;
	uint32_t i = 0;
	TAG_VALUE *values[30] = {0};

//	while ( 1 )
	do
	{
		//
		value_count = 2;
		values[0] = (TAG_VALUE *)malloc(sizeof(TAG_VALUE));
		values[1] = (TAG_VALUE *)malloc(sizeof(TAG_VALUE));

		int rc = 0;

		// open database file
		ins_data->fd = -1;
		ins_data->fd = DBFileOpen("data.dat", O_RDWR | O_CREAT );
		if ( ins_data->fd < 0 )
		{
			break;
		}
		//
//		FILE_HEADER * header = (FILE_HEADER *)ins_data->dbf_header;

		// write data to file
		uint32_t start_offset = 0;
		ssize_t wr_cnt = 0;

		wr_cnt = DBFileWriteValue(ins_data->fd, (const void *)values, value_count*TAG_VALUE_LENGTH, &start_offset);
		//printf( "%s: wr_cnt: %d, start_offset: %u; value_count: %u, ref->wr_count: %u\n\n", __func__, wr_cnt, start_offset, value_count, ref->wr_count );

		// sync file and update header
		rc = DBFileSync( ins_data->fd );

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
		break;

	}while(0);

	printf("end\n");
	return 0;
}


int main(int argc,char *argv[])
{
	INS_DATA insd;
	INS_TAG_REF tagref;
	InsertTagProc(&insd,&tagref,10);
	printf("quit\n");
}
