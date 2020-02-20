#ifndef __TINZ_PUB_MESSAGE__
#define __TINZ_PUB_MESSAGE__

#define MSG_PATH_MSG		            "/mnt/nandflash/msg/"
#define MSG_NAME_UPPROC_TO_SQLITE		"/mnt/nandflash/msg/upproc_to_sqlite"
#define MSG_ID_UPPROC1_TO_SQLITE_ID		0x01
#define MSG_ID_UPPROC2_TO_SQLITE_ID		0x02
#define MSG_ID_UPPROC3_TO_SQLITE_ID		0x03
#define MSG_ID_UPPROC4_TO_SQLITE_ID		0x04
#define MSG_NAME_UPPROC_TO_CONTROL		"/mnt/nandflash/msg/upproc_to_control"
#define MSG_ID_UPPROC1_TO_CONTROL_ID	0x01
#define MSG_ID_UPPROC2_TO_CONTROL_ID	0x02
#define MSG_ID_UPPROC3_TO_CONTROL_ID	0x03
#define MSG_ID_UPPROC4_TO_CONTROL_ID	0x04
#define MSG_NAME_INTERFACE_TO_SQLITE    "/mnt/nandflash/msg/interface_to_sqlite"
#define MSG_ID_INTERFACE_TO_SQLITE_ID   0x01
#define MSG_NAME_INTERFACE_TO_CONTROL   "/mnt/nandflash/msg/interface_to_control"
#define MSG_ID_INTERFACE_TO_CONTROL_ID  0x01
#define MSG_NAME_DATAPROC_TO_UPPROC		"/mnt/nandflash/msg/DataProc_to_upproc"
#define MSG_ID_DATAPROC_TO_UPPROC		0x01

#define MAX_MSG_DATA_LEN 2048
#define MSG_RTYCNT	30

#define MSG_SQLITE_TYTE 1
#define MSG_SQLITE_EVENT_USER_TYTE 80
#define MSG_SQLITE_SEND_TYTE 99
#define MSG_POLLUTANT_RTD_TYTE      10
#define MSG_POLLUTANT_PERMIN_TYTE   11
#define MSG_POLLUTANT_MINS_TYTE     12
#define MSG_POLLUTANT_HOUR_TYTE     13
#define MSG_POLLUTANT_DAY_TYTE      14
#define MSG_POLLUTANT_MONTH_TYTE    15
#define MSG_CONTROL_VALVE_TYTE      20



struct _msgbuf
{
    long int mtype;
	char data[MAX_MSG_DATA_LEN];
};

struct _msg
{
    int msgid;
	struct _msgbuf msgbuf;
};

int prepareMsg(char* ftokpath,char* ftokname,int ftokid,struct _msg* msg);
int MsgSend(struct _msg* msg,long int mtype,char *data,int len);
int MsgRcv(struct _msg* msg, long int mtype);
void rmMsg(struct _msg* msg);
struct _msg* InterfaceMessageInit(struct _msg *msg);
struct _msg* InterfaceToControlMessageInit(struct _msg *msg);
#endif

