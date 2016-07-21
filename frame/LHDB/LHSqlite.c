//
//  LHSqlite.c
//  LHDemo
//
//  Created by 3wchina01 on 16/7/5.
//  Copyright © 2016年 3wchina01. All rights reserved.
//

#include "LHSqlite.h"
#include <libkern/OSAtomic.h>
#include <stdlib.h>
#include <string.h>
#include "LHDictionary.h"
#import "unistd.h"

#define __STATIC__INLINE static inline

#define __LOCK !onLock ? NULL:OSSpinLockLock(&sqliteRef->_lock)

#define __UNLOCK !onLock ? NULL:OSSpinLockUnlock(&sqliteRef->_lock)

void LHSqliteBlobValueRelease(LHSqliteBlobValue* blob_value);

struct lh_sqlite {
    sqlite3* _db;
    const char* fileName;
    OSSpinLock _lock;
    LHDictionaryRef _cacheStmt;
    LHSqliteCallBacks callback;
};

__STATIC__INLINE void lh_sqliteStmtCacheApplierFunction(const void *key, const void *value, void *context)
{
    sqlite3_stmt* stmt = (sqlite3_stmt*)value;
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }
}

__STATIC__INLINE __LHBOOL lh_sqlite_string_equal(const void* value1,const void* value2)
{
    return strcmp(value1, value2) == 0;
}

LHSqliteCallBacks kLHSqliteDefaultStringCallBacks = {
    (LHSqliteRetainCallBack)strdup,
    (LHSqliteReleaseCallBack)free,
    lh_sqlite_string_equal
};

__STATIC__INLINE sqlite3_stmt* __LHSqlitePrepareSql(LHSqliteRef sqliteRef,const char* zSql,LHSqliteError** error)
{
    if (zSql == NULL || strlen(zSql) == 0 || sqliteRef == NULL) {
        return NULL;
    }
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(sqliteRef->_db, zSql, -1, &stmt, NULL) == SQLITE_OK) {
        return stmt;
    }else {
        if (error) {
            *error = malloc(sizeof(LHSqliteError));
            if (*error == NULL) {
                return NULL;
            }
            (*error)->error_code = sqlite3_errcode(sqliteRef->_db);
            (*error)->error_msg = sqliteRef->callback.retain(sqlite3_errmsg(sqliteRef->_db));
        }
    }
    return NULL;
}

__STATIC__INLINE void __LHSqliteBindWithValue(sqlite3_stmt* stmt,int idx,char* name,const void* value,int blob_length,LHSqliteValueType bindType)
{
    if (name) {
        switch (bindType) {
            case LHSqliteValueINTEGER:
                sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, name), *(int*)value);
                break;
                
            case LHSqliteValueFLOAT:
                sqlite3_bind_double(stmt, sqlite3_bind_parameter_index(stmt, name), *(float*)value);
                break;
                
            case LHSqliteValueTEXT:
                sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, name), value, -1, NULL);
                break;
                
            case LHSqliteValueBLOB:
                sqlite3_bind_blob(stmt, sqlite3_bind_parameter_index(stmt, name), value, blob_length, NULL);
                break;
                
            default:
                break;
        }
    }else {
        switch (bindType) {
            case LHSqliteValueINTEGER:
                sqlite3_bind_int(stmt, idx, *(int*)value);
                break;
                
            case LHSqliteValueFLOAT:
                sqlite3_bind_double(stmt, idx, *(float*)value);
                break;
                
            case LHSqliteValueTEXT:
            {
                char* c = (char*)value;
                sqlite3_bind_text(stmt, idx, (char*)value, -1, NULL);
            }
                
                break;
                
            case LHSqliteValueBLOB:
                sqlite3_bind_blob(stmt, idx, value, blob_length, NULL);
                break;
                
            default:
                break;
        }
    }
    
}
__STATIC__INLINE void __LHSqliteStepUpdate(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error)
{
    if (sqliteRef == NULL) {
        return;
    }
    int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE && result != SQLITE_OK) {
        if (error) {
            *error = malloc(sizeof(LHSqliteError));
            if (*error == NULL) {
                return;
            }
            (*error)->error_code = sqlite3_errcode(sqliteRef->_db);
            (*error)->error_msg = sqliteRef->callback.retain(sqlite3_errmsg(sqliteRef->_db));
        }
    }
}

__STATIC__INLINE LHSqliteValue* __LHSqliteValueWith(sqlite3_stmt* stmt,int value_type,int idx)
{
    LHSqliteValue* value = malloc(sizeof(LHSqliteValue));
    if (value == NULL) {
        return NULL;
    }
    switch (value_type) {
        case SQLITE_INTEGER:
        {
            value->vale_type = LHSqliteValueINTEGER;
            int i = sqlite3_column_int(stmt, idx);
            value->value = &i;
        }
            break;
            
        case SQLITE_FLOAT:
        {
            value->vale_type = LHSqliteValueFLOAT;
            float f = sqlite3_column_double(stmt, idx);
            value->value = &f;
        }
            break;
            
        case SQLITE_TEXT:
        {
            value->vale_type = LHSqliteValueTEXT;
            value->value = strdup((const char*)sqlite3_column_text(stmt, idx));
        }
            break;
            
        case SQLITE_BLOB:
        {
            value->vale_type = LHSqliteValueBLOB;
            LHSqliteBlobValue* blob_value = malloc(sizeof(LHSqliteBlobValue));
            blob_value->blob_lenght = sqlite3_column_bytes(stmt, idx);
//            void* blob = malloc(blob_value->blob_lenght*sizeof(int));
//            memmove(blob, sqlite3_column_blob(stmt, idx), blob_value->blob_lenght);
//            blob_value->blob = blob;
            blob_value->blob = sqlite3_column_blob(stmt, idx);
            value->value = blob_value;
        }
            break;
            
        default:
        {
            value->vale_type = LHSqliteValueNULL;
            value->value = NULL;
        }
            break;
    }
    return value;
}

__STATIC__INLINE LHArrayRef __LHSqliteStepQuery(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error)
{
    if (sqliteRef == NULL || stmt == NULL) {
        return NULL;
    }
    LHArrayRef arrR = lh_arrayCreate();
    int query_count = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        LHDictionaryRef dicR = lh_dictionary_create();
        for (int i=0; i<query_count; i++) {
            lh_dictionarySetValueForKey(dicR, sqlite3_column_name(stmt, i), __LHSqliteValueWith(stmt, sqlite3_column_type(stmt, i), i));
            
        }
        lh_arrayAppentValue(arrR, dicR);
    }
    if (lh_arrayGetCount(arrR) == 0) {
        lh_arrayRelease(arrR);
        arrR = NULL;
    }
    
    return arrR;
    
}

__STATIC__INLINE LHSqliteRef __LHSqliteCreateWithOptions(const void* fileName,LHSqliteCallBacks* callback)
{
    if (!fileName) {
        return NULL;
    }
    LHSqliteRef sqlite = calloc(1, sizeof(LHSqliteRef));
    if (sqlite == NULL) {
        return NULL;
    }
    callback ? (sqlite->callback = *callback) : (sqlite->callback = kLHSqliteDefaultStringCallBacks);
    sqlite->fileName = sqlite->callback.retain(fileName);
    sqlite->_lock = OS_SPINLOCK_INIT;
    sqlite->_cacheStmt = lh_dictionary_create_with_options(100, &lh_default_key_callback, NULL);
    return sqlite;
}

__STATIC__INLINE __LHBOOL __LHSqliteOpen(LHSqliteRef sqliteRef,__LHBOOL onLock)
{
    if (sqliteRef == NULL) {
        return false;
    }
    __LOCK;
    if (sqlite3_open(sqliteRef->fileName, &sqliteRef->_db) == SQLITE_OK) {
        return true;
    }
    sqlite3_close(sqliteRef->_db);
    __UNLOCK;
    return false;
}

__STATIC__INLINE __LHBOOL __LHSqliteOpen_e(LHSqliteRef sqliteRef,LHSqliteError** error,__LHBOOL onLock)
{
    if (sqliteRef == NULL) {
        return false;
    }
    __LOCK;
    if (sqlite3_open(sqliteRef->fileName, &sqliteRef->_db) == SQLITE_OK) {
        return true;
    }
    if (error) {
        *error = malloc(sizeof(LHSqliteError));
        if (*error == NULL) {
            sqlite3_close(sqliteRef->_db);
            __UNLOCK;
            return false;
        }
        (*error)->error_code = sqlite3_errcode(sqliteRef->_db);
        (*error)->error_msg = sqliteRef->callback.retain(sqlite3_errmsg(sqliteRef->_db));
    }
    sqlite3_close(sqliteRef->_db);
    __UNLOCK;
    return false;
}

__STATIC__INLINE __LHBOOL __LHSqliteClose(LHSqliteRef sqliteRef,__LHBOOL onLock)
{
    if (sqliteRef == NULL) {
        return false;
    }
    if (sqlite3_close(sqliteRef->_db) == SQLITE_BUSY || sqlite3_close(sqliteRef->_db) == SQLITE_LOCKED) {
        usleep(20);
        if (sqlite3_close(sqliteRef->_db) != SQLITE_OK) {
            __UNLOCK;
            return false;
        }
        __UNLOCK;
        return true;
    }
    
    if (sqlite3_close(sqliteRef->_db) != SQLITE_OK) {
        __UNLOCK;
        return false;
    }
    __UNLOCK;
    return true;
}

#pragma mark- API

LHSqliteRef LHSqliteCreateWithFileName(const void* fileName)
{
    return __LHSqliteCreateWithOptions(fileName, NULL);
}

LHSqliteRef LHSqliteCreateWithOptions(const void* fileName,LHSqliteCallBacks* callback)
{
    return __LHSqliteCreateWithOptions(fileName, callback);
}

__LHBOOL LHSqliteOpen(LHSqliteRef sqliteRef,__LHBOOL onLock)
{
    return __LHSqliteOpen(sqliteRef, onLock);
}

__LHBOOL LHSqliteOpen_e(LHSqliteRef sqliteRef,LHSqliteError** error,__LHBOOL onLock)
{
    return __LHSqliteOpen_e(sqliteRef, error, onLock);
}

sqlite3_stmt* LHSqlitePrepareSQL(LHSqliteRef sqliteRef,const char* zSql,LHSqliteError** error)
{
    return __LHSqlitePrepareSql(sqliteRef, zSql, error);
}

void LHSqliteBindWithNmae(sqlite3_stmt* stmt,char* name,const void* value,int blob_length,LHSqliteValueType bindType)
{
    __LHSqliteBindWithValue(stmt,0, name, value, blob_length, bindType);
}

void LHSqliteBindWithIndex(sqlite3_stmt* stmt,int idx,const void* value,int blob_length,LHSqliteValueType bindType)
{
    __LHSqliteBindWithValue(stmt, idx, NULL, value, blob_length, bindType);
}

void LHSqliteStepUpdate(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error)
{
    __LHSqliteStepUpdate(sqliteRef, stmt, error);
}

LHArrayRef LHSqliteStepQuery(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error)
{
    return __LHSqliteStepQuery(sqliteRef, stmt, error);
}

__LHBOOL LHSqliteClose(LHSqliteRef sqliteRef,__LHBOOL onLock)
{
    return __LHSqliteClose(sqliteRef, onLock);
}

void LHSqliteClearStmtCache(LHSqliteRef sqliteRef)
{
    if (sqliteRef == NULL) {
        return;
    }
    
    if (!sqliteRef->_cacheStmt || lh_dictionaryGetCount(sqliteRef->_cacheStmt) == 0) {
        return;
    }
    
    lh_dictionaryApplyFunction(sqliteRef->_cacheStmt, lh_sqliteStmtCacheApplierFunction, NULL);
    lh_dictionaryRemoveAllValues(sqliteRef->_cacheStmt);
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

void LHSqliteValueRelease(LHSqliteValue* value)
{
    if (value) {
        switch (value->vale_type) {
            case LHSqliteValueBLOB:
                LHSqliteBlobValueRelease((LHSqliteBlobValue*)(value->value));
                free(value);
                break;
                
            case LHSqliteValueTEXT:
                free((void*)value->value);
                free(value);
                break;
            default:
                free(value);
                break;
        }
    }
    value = NULL;
}

void LHSqliteBlobValueRelease(LHSqliteBlobValue* blob_value)
{
    if (blob_value) {
        free(blob_value);
        blob_value = NULL;
    }
}

void LHSqliteRelease(LHSqliteRef sqliteRef)
{
    if (sqliteRef == NULL) {
        return;
    }
    LHSqliteClearStmtCache(sqliteRef);
    lh_dictionaryRelease(sqliteRef->_cacheStmt);
    if (sqliteRef->callback.release) sqliteRef->callback.release(sqliteRef->fileName);
    sqliteRef->fileName = NULL;
    
    free(sqliteRef);
    sqliteRef = NULL;
}

