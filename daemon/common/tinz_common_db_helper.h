#ifndef __TINZ_COMMON_DB_HELPER__
#define __TINZ_COMMON_DB_HELPER__
#include "sqlite3.h"
#define ZLKZQ_DATA		"/mnt/sdcard/zlkzq.db"
#define ZLKZQ_DATA_bak	"/mnt/nandflash/zlkzq.db.bak"

#define TABLE_NAME_LEN	32
#define SQL_LEN			2048
typedef struct {
    sqlite3    *db;
	sqlite3_stmt *stat;
	char 		*errmsg;
    char   		name[64];
} tinz_db_ctx_t;

int TableIsExist(tinz_db_ctx_t* ctx, char *tableName);
void RtdTableCreate(tinz_db_ctx_t* ctx, char *tableName);
void DustDataTableCreate(tinz_db_ctx_t* ctx, char *tableName); //创建扬尘污染物数据表
void CountDataTableCreate(tinz_db_ctx_t* ctx, char *tableName);
void MessageSendTableCreate(tinz_db_ctx_t* ctx, char *tableName);
void EventTableCreate(tinz_db_ctx_t* ctx, char *tableName);
void tinz_db_exec(tinz_db_ctx_t* ctx, char *sql);
int tinz_db_close(tinz_db_ctx_t* ctx);
int tinz_db_open(tinz_db_ctx_t *ctx);
void PEMSDataTableCreate(tinz_db_ctx_t* ctx, char *tableName);
void PEMSRtdTableCreate(tinz_db_ctx_t* ctx, char *tableName);
#endif

