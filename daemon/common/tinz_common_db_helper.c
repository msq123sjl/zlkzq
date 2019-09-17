#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>

#include "sqlite3.h"

#include "tinz_common_db_helper.h"
#include "tinz_common_helper.h"
#include "tinz_base_def.h"

/*
当synchronous设置为FULL (2), SQLite数据库引擎在紧急时刻会暂停以确定数据已经写入磁盘。这使系统崩溃或电源出问题时能确保数据库在重起后不会损坏。FULL synchronous很安全但很慢。
当synchronous设置为NORMAL, SQLite数据库引擎在大部分紧急时刻会暂停，但不像FULL模式下那么频繁。 NORMAL模式下有很小的几率(但不是不存在)发生电源故障导致数据库损坏的情况。但实际上，在这种情况 下很可能你的硬盘已经不能使用，或者发生了其他的不可恢复的硬件错误。
设置为synchronous OFF (0)时，SQLite在传递数据给系统以后直接继续而不暂停。若运行SQLite的应用程序崩溃， 数据不会损伤，但在系统崩溃或写入数据时意外断电的情况下数据库可能会损坏。另一方面，在synchronous OFF时 一些操作可能会快50倍甚至更多。在SQLite 2中，缺省值为NORMAL.而在3中修改为FULL。
*/
int tinz_db_open(tinz_db_ctx_t *ctx)
{
    if (SQLITE_OK != sqlite3_open((const char*)ctx->name, (sqlite3**)&ctx->db)) {
		DEBUG_PRINT_ERR(5,"open database failed:[%s]\n",sqlite3_errmsg((sqlite3*)ctx->db));
        return TINZ_ERROR;
    }

    if (SQLITE_OK != sqlite3_exec(ctx->db, "PRAGMA synchronous = OFF", NULL, NULL, &ctx->errmsg)) {
		DEBUG_PRINT_ERR(5, "set synchronous = OFF failed:[%s]", ctx->errmsg);
		sqlite3_free(ctx->errmsg);
        return TINZ_ERROR;
    }

    return TINZ_OK;
}

int tinz_db_close(tinz_db_ctx_t* ctx)
{
    if (SQLITE_OK != sqlite3_close(ctx->db)) {
		DEBUG_PRINT_ERR(1,"close database failed:[%s]", sqlite3_errmsg((sqlite3*)ctx->db));
        return TINZ_ERROR;
    }
    return TINZ_OK;
}

void tinz_db_exec(tinz_db_ctx_t* ctx, char *sql)
{
	DEBUG_PRINT_INFO(2,"sql[%s]\n",sql);
	if(SQLITE_OK != sqlite3_exec(ctx->db, sql, NULL, NULL,&ctx->errmsg)){
		DEBUG_PRINT_ERR(1, "SQL error: %s\n",ctx->errmsg);
		sqlite3_free(ctx->errmsg);
	}
}
//查找某数据表是否存在
int TableIsExist(tinz_db_ctx_t* ctx, char *tableName)
{
	int	cnt = 0;
	char sql[SQL_LEN];
	snprintf(sql,sizeof(sql)-1,"select count(*) from sqlite_master where type=\'table\' and name=\'%s\';",tableName);
    DEBUG_PRINT_INFO(2, "sql: %s\n",sql);
	if(SQLITE_OK != sqlite3_prepare(ctx->db, sql, -1, &ctx->stat, 0)){
        return TINZ_ERROR;
    }
	if(SQLITE_ROW == sqlite3_step(ctx->stat)){
		cnt = sqlite3_column_int(ctx->stat,0);
	}
    sqlite3_finalize(ctx->stat);
	
    return cnt>0 ? TINZ_OK : TINZ_ERROR;
}

//创建实时数据表
void RtdTableCreate(tinz_db_ctx_t* ctx, char *tableName)
{
    char sql[SQL_LEN];	
	snprintf(sql,sizeof(sql)-1,"create table %s(\
								GetTime DATETIME(20),\
								Rtd NUMERIC(4,2),\
								Cou NUMERIC(8,2),\
								primary key (GetTime));",tableName);
	tinz_db_exec(ctx,sql);
}

//创建扬尘污染物数据表
void DustDataTableCreate(tinz_db_ctx_t* ctx, char *tableName)
{
    char sql[SQL_LEN];  
    snprintf(sql,sizeof(sql)-1,"create table %s(\
                                GetTime DATETIME(20),\
                                Avg NUMERIC(4,2),\
                                primary key (GetTime));",tableName);
    tinz_db_exec(ctx,sql);
}

//创建工况用电监测实时数据表
void PEMSRtdTableCreate(tinz_db_ctx_t* ctx, char *tableName)
{
    char sql[SQL_LEN];  
    snprintf(sql,sizeof(sql)-1,"create table %s(\
    GetTime DATETIME(20),\
    success integer not null,CNT integer not null,rate integer not null,\
    E integer not null,Ep integer not null,Eq integer not null,Ept integer not null,Epnt integer not null,Eqpt integer not null,Eqnt integer not null,\
    Pt integer not null,Pa integer not null,Pb integer not null,Pc integer not null,\
    Qt integer not null,Qa integer not null,Qb integer not null,Qc integer not null,\
    Pf integer not null,Pfa integer not null,Pfb integer not null,Pfc integer not null,\
    Ua  integer not null,Ub  integer not null,Uc  integer not null,\
    Ia  integer not null,Ib  integer not null,Ic  integer not null,\
    Tpa integer not null,Tpb integer not null,Tpc integer not null,\
    dem integer not null,demtime integer not null,\
    THD_Ua integer not null,THD_Ub integer not null,THD_Uc integer not null,\
    THD_Ia integer not null,THD_Ib integer not null,THD_Ic integer not null,\
    THV_Ua integer not null,THV_Ub integer not null,THV_Uc integer not null,\
    THC_Ia integer not null,THC_Ib integer not null,THC_Ic integer not null,\
    primary key (GetTime));",tableName);
    tinz_db_exec(ctx,sql);
}

//创建工况用电监测天/月数据表
void PEMSDataTableCreate(tinz_db_ctx_t* ctx, char *tableName)
{
    char sql[SQL_LEN];  
    snprintf(sql,sizeof(sql)-1,"create table %s(\
    GetTime DATETIME(20),\
    E_cou  integer not null,Ep_cou integer not null,Eq_cou integer not null,Ept_cou integer not null,Epnt_cou  integer not null,Eqpt_cou  integer not null,Eqnt_cou  integer not null,\
    Pt_min integer not null,Pt_max integer not null,Pt_avg integer not null,\
    Pa_min integer not null,Pa_max integer not null,Pa_avg integer not null,\
    Pb_min integer not null,Pb_max integer not null,Pb_avg integer not null,\
    Pc_min integer not null,Pc_max integer not null,Pc_avg integer not null,\
    Qt_min integer not null,Qt_max integer not null,Qt_avg integer not null,\
    Qa_min integer not null,Qa_max integer not null,Qa_avg integer not null,\
    Qb_min integer not null,Qb_max integer not null,Qb_avg integer not null,\
    Qc_min integer not null,Qc_max integer not null,Qc_avg integer not null,\
    Pf_min integer not null,Pf_max integer not null,Pf_avg integer not null,\
    Pfa_min integer not null,Pfa_max integer not null,Pfa_avg integer not null,\
    Pfb_min integer not null,Pfb_max integer not null,Pfb_avg integer not null,\
    Pfc_min integer not null,Pfc_max integer not null,Pfc_avg integer not null,\
    Ua_min  integer not null,Ua_max  integer not null,Ua_avg  integer not null,\
    Ub_min  integer not null,Ub_max  integer not null,Ub_avg  integer not null,\
    Uc_min  integer not null,Uc_max  integer not null,Uc_avg  integer not null,\
    Ia_min  integer not null,Ia_max  integer not null,Ia_avg  integer not null,\
    Ib_min  integer not null,Ib_max  integer not null,Ib_avg  integer not null,\
    Ic_min  integer not null,Ic_max  integer not null,Ic_avg  integer not null,\
    Tpa_min integer not null,Tpa_max integer not null,Tpa_avg integer not null,\
    Tpb_min integer not null,Tpb_max integer not null,Tpb_avg integer not null,\
    Tpc_min integer not null,Tpc_max integer not null,Tpc_avg integer not null,\
    primary key (GetTime));",tableName);
    tinz_db_exec(ctx,sql);
}

//创建统计数据表
void CountDataTableCreate(tinz_db_ctx_t* ctx, char *tableName)
{
	char sql[SQL_LEN];
	snprintf(sql,sizeof(sql)-1,"create table %s(\
								GetTime DATETIME(20),\
								Data NUMERIC(8,2),\
								Cou NUMERIC(8,2),\
								primary key (GetTime));",tableName);
	tinz_db_exec(ctx,sql);
}

//创建发送报文数据表
void MessageSendTableCreate(tinz_db_ctx_t* ctx, char *tableName)
{
	char sql[SQL_LEN];
	snprintf(sql,sizeof(sql)-1,"create table %s(\
								QN NVARCHAR(20),\
								Content NVARCHAR(1024),\
								Target1_SendTimes NVARCHAR(3),\
								Target2_SendTimes NVARCHAR(3),\
								Target3_SendTimes NVARCHAR(3),\
								Target4_SendTimes NVARCHAR(3),\
								Target1_IsRespond NVARCHAR(3),\
                                Target2_IsRespond NVARCHAR(3),\
                                Target3_IsRespond NVARCHAR(3),\
                                Target4_IsRespond NVARCHAR(3),\
								primary key (QN));",tableName);
	tinz_db_exec(ctx,sql);
}
//创建事件数据表
void EventTableCreate(tinz_db_ctx_t* ctx, char *tableName)
{
	char sql[SQL_LEN];
	snprintf(sql,sizeof(sql)-1,"create table %s(\
								GetTime DATETIME(20),\
								INFO NVARCHAR(256));",tableName);
	tinz_db_exec(ctx,sql);
}


