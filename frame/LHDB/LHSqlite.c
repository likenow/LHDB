//
//  LHSqlite.c
//  LHDemo
//
//  Created by 3wchina01 on 16/7/5.
//  Copyright © 2016年 3wchina01. All rights reserved.
//

#include "LHSqlite.h"
#include <sqlite3.h>
#include <libkern/OSAtomic.h>
#include <stdlib.h>
#include <string.h>

#define __STATIC__INLINE static inline

struct lh_sqlite {
    sqlite3* _db;
    const char* fileName;
    OSSpinLock _lock;
    LHSqliteCallBacks callback;
};

__STATIC__INLINE __LHBOOL lh_sqlite_string_equal(const void* value1,const void* value2)
{
    return strcmp(value1, value2) == 0;
}

LHSqliteCallBacks kLHSqliteDefaultStringCallBacks = {
    (LHSqliteRetainCallBack)strdup,
    (LHSqliteReleaseCallBack)free,
    lh_sqlite_string_equal
};

__STATIC__INLINE sqlite3_stmt* __LHSqlitePrepareSql(LHSqliteRef sqliteRef,const char* zSql)
{
    if (zSql == NULL || strlen(zSql) == 0 || sqliteRef == NULL) {
        return NULL;
    }
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(sqliteRef->_db, zSql, -1, &stmt, NULL) == SQLITE_OK) {
        return stmt;
    }
    return NULL;
}

__STATIC__INLINE sqlite3_stmt* __LHSqliteBindValue(LHSqliteRef sqliteRef,const void* value,LHSqliteValueType type)
{
    
}

LHSqliteRef __LHSqliteCreateWithFileName(const void* fileName)
{
    LHSqliteRef sqlite = calloc(1, sizeof(LHSqliteRef));
    sqlite->fileName = fileName;
    return sqlite;
}

LHSqliteRef __LHSqliteCreate(const void* fileName,LHSqliteCallBacks* callback)
{
    if (!fileName) {
        return NULL;
    }
    LHSqliteRef sqlite = calloc(1, sizeof(LHSqliteRef));
    callback ? (sqlite->callback = *callback) : (sqlite->callback = kLHSqliteDefaultStringCallBacks);
    sqlite->fileName = sqlite->callback.retain(fileName);
    sqlite->_lock = OS_SPINLOCK_INIT;
    return sqlite;
}

__LHBOOL __LHSqliteOpen(LHSqliteRef sqliteRef)
{
    if (sqlite3_open(sqliteRef->fileName, &sqliteRef->_db) == SQLITE_OK) {
        return true;
    }
    sqlite3_close(sqliteRef->_db);
    return false;
}

__LHBOOL __LHSqliteOpen_e(LHSqliteRef sqliteRef,LHSqliteError* sql_error)
{
    if (sqliteRef == NULL) {
        return false;
    }
    if (sqlite3_open(sqliteRef->fileName, &sqliteRef->_db) == SQLITE_OK) {
        return true;
    }
    if (sql_error == NULL) {
        sql_error = calloc(1, sizeof(LHSqliteError));
    }
    if (sql_error) {
        sql_error->error_code = sqlite3_errcode(sqliteRef->_db);
        sql_error->error_msg = sqliteRef->callback.retain(sqlite3_errmsg(sqliteRef->_db));
    }
    sqlite3_close(sqliteRef->_db);
    return false;
}

void LHSqliteErrorFree(LHSqliteRef sqliteRef,LHSqliteError* sql_error)
{
    if (sqliteRef == NULL) {
        return;
    }
    if (sql_error) {
        if (sql_error->error_msg) {
            !(sqliteRef->callback.release) ? free((void*)sql_error->error_msg):(sqliteRef->callback.release)(sql_error->error_msg);
        }
        !(sqliteRef->callback.release) ? free(sql_error):(sqliteRef->callback.release)(sql_error);
    }
    sql_error = NULL;
}



