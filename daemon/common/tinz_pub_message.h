#ifndef __TINZ_PUB_MESSAGE__
#define __TINZ_PUB_MESSAGE__

#define MSG_PATH_UPPROC_TO_SQLITE		"/mnt/nandflash/msg/"
#define MSG_NAME_UPPROC_TO_SQLITE		"/mnt/nandflash/msg/upproc_to_sqlite"
#define MSG_ID_UPPROC1_TO_SQLITE_ID		0x01
#define MSG_ID_UPPROC2_TO_SQLITE_ID		0x02
#define MSG_ID_UPPROC3_TO_SQLITE_ID		0x03
#define MSG_ID_UPPROC4_TO_SQLITE_ID		0x03


#define MSG_PATH_DATAPROC_TO_SQLITE		"/mnt/nandflash/msg/"
#define MSG_NAME_DATAPROC_TO_SQLITE		"/mnt/nandflash/msg/DataProc_to_sqlite"
#define MSG_ID_DATAPROC_TO_SQLITE		0x01

#define MAX_MSG_DATA_LEN 256
#define MSG_RTYCNT	3

#define MSG_SQLITE_TYTE 1
#define MSG_SQLITE_RTD_TYTE 1
#define MSG_SQLITE_DAY_TYTE 1
#define MSG_SQLITE_MON_TYTE 1
#define MSG_SQLITE_QUT_TYTE 1
#define MSG_SQLITE_YEA_TYTE 1

struct _msgbuf
{
    long int mtype;
	char data[MAX_MSG_DATA_LEN];
};

struct _msg
{
    int msgid;;
	struct _msgbuf msgbuf;
};

int prepareMsg(char* ftokpath,char* ftokname,int ftokid,struct _msg* msg);
int MsgSend(struct _msg* msg);
void MsgRcv(struct _msg* msg, long int mtype);
void rmMsg(struct _msg* msg);

#endif

