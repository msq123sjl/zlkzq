#ifndef __TINZ_COMMON_DB_HELPER__
#define __TINZ_COMMON_DB_HELPER__
#include "sqlite3.h"
#define SCY_DATA		"/mnt/nandflash/bin/zlkzq.db"
#define SCY_DATA_bak	"/mnt/sdcard/zlkzq.db.bak"

#define TABLE_NAME_LEN	32
#define SQL_LEN			512
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

#endif

