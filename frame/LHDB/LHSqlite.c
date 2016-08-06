//
//  LHSqlite.c
//  LHDemo
//
//  Created by 3wchina01 on 16/7/5.
//  Copyright © 2016年 3wchina01. All rights reserved.
//

#include "LHSqlite.h"
#include <stdlib.h>
#include <string.h>
#include "LHDictionary.h"
#import "unistd.h"

#define __STATIC__INLINE static inline

#define __LOCK !onLock ? NULL:OSSpinLockLock(&sqliteRef->_lock)

#define __UNLOCK !onLock ? NULL:OSSpinLockUnlock(&sqliteRef->_lock)

void LHSqliteBlobValueRelease(LHSqliteBlobValue* blob_value);



__STATIC__INLINE void lh_sqliteStmtCacheApplierFunction(const void *key, const void *value, void *context)
{
    sqlite3_stmt* stmt = (sqlite3_stmt*)value;
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }
}

__STATIC__INLINE void lh_sqliteUpdateApplierFunction(const void* key,const void* value,void* context)
{
    LHSqliteValue* sqliteValue = (LHSqliteValue*)value;
    sqlite3_stmt* stmt = (sqlite3_stmt*)context;
    int length = (int)strlen(key);
    char* c = calloc(length+2, sizeof(char));
    strcat(c, ":");
    strcat(c, key);
    LHSqliteBindWithName(stmt, c, sqliteValue);
    
}

__STATIC__INLINE sqlite3_stmt* __LHSqlitePrepareSql(LHSqliteRef sqliteRef,const char* zSql,LHSqliteError** error)
{
    if (zSql == NULL || strlen(zSql) == 0 || sqliteRef == NULL || sqliteRef->_isConnection == false) {
        return NULL;
    }
    sqlite3_stmt* stmt = lh_dictionaryGetValueForKey(sqliteRef->_cacheStmt, zSql);
    if (stmt == NULL) {
        if (sqlite3_prepare_v2(sqliteRef->_db, zSql, -1, &stmt, NULL) == SQLITE_OK) {
            lh_dictionarySetValueForKey(sqliteRef->_cacheStmt, zSql, stmt);
            return stmt;
        }else {
            if (error) {
                *error = malloc(sizeof(LHSqliteError));
                if (*error == NULL) {
                    return NULL;
                }
                (*error)->error_code = sqlite3_errcode(sqliteRef->_db);
                (*error)->error_msg = strdup(sqlite3_errmsg(sqliteRef->_db));
            }
        }
    }else
        sqlite3_reset(stmt);
    return stmt;
}

__STATIC__INLINE void __LHSqliteBindWithValue(sqlite3_stmt* stmt,int idx,char* name,LHSqliteValue* value)
{
    if (name) {
        switch (value->vale_type) {
            case LHSqliteValueINTEGER:
            {
                LHSqliteIntValue* int_value = (LHSqliteIntValue*)value->value;
                sqlite3_bind_int(stmt, (int)sqlite3_bind_parameter_index(stmt, name), int_value->int_value);
            }
                break;
                
            case LHSqliteValueFLOAT:
            {
                LHSqliteFloatValue* float_value = (LHSqliteFloatValue*)value->value;
                sqlite3_bind_double(stmt, (int)sqlite3_bind_parameter_index(stmt, name), float_value->float_value);
            }
                break;
                
            case LHSqliteValueTEXT:
            {
                sqlite3_bind_text(stmt, (int)sqlite3_bind_parameter_index(stmt, name), value->value, -1, NULL);
            }
                break;
                
            case LHSqliteValueBLOB:
            {
                LHSqliteBlobValue* blobValue = (LHSqliteBlobValue*)value->value;
                sqlite3_bind_blob(stmt, (int)sqlite3_bind_parameter_index(stmt, name), blobValue->blob, blobValue->blob_lenght, NULL);
            }
                break;
                
            default:
                break;
        }
        free(name);
    }else {
        switch (value->vale_type) {
            case LHSqliteValueINTEGER:
                sqlite3_bind_int(stmt, idx, *(int*)value->value);
                break;
                
            case LHSqliteValueFLOAT:
                sqlite3_bind_double(stmt, idx, *(float*)value->value);
                break;
                
            case LHSqliteValueTEXT:
            {
                sqlite3_bind_text(stmt, idx,value->value, -1, NULL);
            }
                
                break;
                
            case LHSqliteValueBLOB:
            {
                LHSqliteBlobValue* blobValue = (LHSqliteBlobValue*)value->value;
                sqlite3_bind_blob(stmt, sqlite3_bind_parameter_index(stmt, name), blobValue->blob, blobValue->blob_lenght, NULL);
            }
                break;
                
            default:
                break;
        }
    }
    
}
__STATIC__INLINE void __LHSqliteStepUpdate(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error)
{
    if (sqliteRef == NULL || sqliteRef->_isConnection == false) {
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
            (*error)->error_msg = strdup(sqlite3_errmsg(sqliteRef->_db));
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
            LHSqliteIntValue* int_value = malloc(sizeof(LHSqliteIntValue));
            int_value->int_value = i;
            value->value = int_value;
        }
            break;
            
        case SQLITE_FLOAT:
        {
            value->vale_type = LHSqliteValueFLOAT;
            float f = sqlite3_column_double(stmt, idx);
            LHSqliteFloatValue* float_value = malloc(sizeof(LHSqliteFloatValue));
            float_value->float_value = f;
            value->value = float_value;
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
    if (sqliteRef == NULL || stmt == NULL || sqliteRef->_isConnection == false) {
        return NULL;
    }
    LHArrayRef arrR = LHArrayCreate();
    int query_count = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        LHDictionaryRef dicR = lh_dictionary_create();
        for (int i=0; i<query_count; i++) {
            lh_dictionarySetValueForKey(dicR, sqlite3_column_name(stmt, i), __LHSqliteValueWith(stmt, sqlite3_column_type(stmt, i), i));
            
        }
        LHArrayAppentValue(arrR, dicR);
    }
    if (LHArrayGetCount(arrR) == 0) {
        LHArrayRelease(arrR);
        arrR = NULL;
    }
    
    return arrR;
    
}

__STATIC__INLINE LHSqliteRef __LHSqliteCreateWithOptions(const void* fileName)
{
    if (!fileName) {
        return NULL;
    }
    LHSqliteRef sqlite = calloc(1, sizeof(struct lh_sqlite));
    if (sqlite == NULL) {
        return NULL;
    }
    sqlite->_isConnection = false;
    sqlite->fileName = strdup(fileName);
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
    int run_result = sqlite3_open(sqliteRef->fileName, &sqliteRef->_db);
    if (run_result == SQLITE_OK) {
        sqliteRef->_isConnection = true;
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
        sqliteRef->_isConnection = true;
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
        (*error)->error_msg = strdup(sqlite3_errmsg(sqliteRef->_db));
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
    LHSqliteClearStmtCache(sqliteRef);
    bool retry = false;
    int result = 0;
    int sqliteCloseTimeout = 3;
    bool triedFinalizingOpenStatements = false;
    do {
        retry = false;
        result = sqlite3_close(sqliteRef->_db);
        if (result == SQLITE_BUSY || result == SQLITE_LOCKED) {
            retry = true;
            usleep(20);
            sqliteCloseTimeout --;
            if (!triedFinalizingOpenStatements) {
                triedFinalizingOpenStatements = true;
                sqlite3_stmt *pStmt;
                while ((pStmt = sqlite3_next_stmt(sqliteRef->_db, NULL)) !=0) {
                    sqlite3_finalize(pStmt);
                }
            }
        }else if (result != SQLITE_OK){
            __UNLOCK;
            return false;
        }
    } while (retry&&sqliteCloseTimeout);
    sqliteRef->_isConnection = false;
    __UNLOCK;
    return true;
}

#pragma mark- API

LHSqliteRef LHSqliteCreateWithFileName(const void* fileName)
{
    return __LHSqliteCreateWithOptions(fileName);
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

void LHSqliteBindWithName(sqlite3_stmt* stmt,char* name,LHSqliteValue* value)
{
    __LHSqliteBindWithValue(stmt,0, name, value);
}

void LHSqliteBindWithIndex(sqlite3_stmt* stmt,int idx,LHSqliteValue* value)
{
    __LHSqliteBindWithValue(stmt, idx, NULL,value);
}

void LHSqliteStepUpdate(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error)
{
    __LHSqliteStepUpdate(sqliteRef, stmt, error);
}

LHArrayRef LHSqliteStepQuery(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error)
{
    return __LHSqliteStepQuery(sqliteRef, stmt, error);
}

void LHSqliteExecuteUpdate(LHSqliteRef sqliteRef,const char* zSql,LHDictionaryRef dictionary,LHSqliteError** error)
{
    
    sqlite3_stmt* stmt = LHSqlitePrepareSQL(sqliteRef, zSql, error);
    if (stmt == NULL) {
        return;
    }
    
    if (*error != NULL) {
        return;
    }
    
    lh_dictionaryApplyFunction(dictionary, lh_sqliteUpdateApplierFunction, stmt);
    LHSqliteStepUpdate(sqliteRef, stmt, error);
    lh_dictionaryRelease(dictionary);
}

LHArrayRef LHSqliteExecuteQuery(LHSqliteRef sqliteRef,const char* zSql,LHSqliteError** error)
{
    sqlite3_stmt* stmt = LHSqlitePrepareSQL(sqliteRef, zSql, error);
    if (stmt == NULL || *error != NULL) {
        return NULL;
    }
    
    return LHSqliteStepQuery(sqliteRef, stmt, error);
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
            free((void*)sql_error->error_msg);
        }
        free(sql_error);
    }
    sql_error = NULL;
}

void LHSqliteValueRelease(LHSqliteValue* value)
{
    if (value) {
        switch (value->vale_type) {
            case LHSqliteValueBLOB:
                LHSqliteBlobValueRelease((LHSqliteBlobValue*)(value->value));
                break;
                
            case LHSqliteValueTEXT:
                free((void*)value->value);
                value->value = NULL;
                break;
            case LHSqliteValueFLOAT:
                free((void*)value->value);
                value->value = NULL;
                break;
            case LHSqliteValueINTEGER:
                free((void*)value->value);
                value->value = NULL;
                break;
            default:
                break;
        }
        free(value);
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
    free((void*)sqliteRef->fileName);
    sqliteRef->fileName = NULL;
    free(sqliteRef);
    sqliteRef = NULL;
}

