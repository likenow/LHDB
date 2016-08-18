//
//  LHSqlite.h
//  LHDemo
//
//  Created by 3wchina01 on 16/7/5.
//  Copyright © 2016年 3wchina01. All rights reserved.
//

#ifndef LHSqlite_h
#define LHSqlite_h

#include <stdio.h>
#include <stdbool.h>
#include <sqlite3.h>
#include "LHArray.h"
#include "LHDictionary.h"
#include <libkern/OSAtomic.h>

#define __LHBOOL bool

typedef const void* (*LHSqliteRetainCallBack)(const void* value);

typedef void (*LHSqliteReleaseCallBack)(const void* value);

typedef __LHBOOL (*LHSqliteEqualCallBack)(const void* value1,const void* value2);

typedef enum LHSqliteValueType {
    LHSqliteValueINTEGER = 1<<2,
    LHSqliteValueFLOAT = 1<<3,
    LHSqliteValueTEXT = 1<<4,
    LHSqliteValueBLOB = 1<<5,
    LHSqliteValueNULL = 1<<6
}LHSqliteValueType;

typedef struct {
    int error_code;
    const char* error_sql;
    const char* error_msg;
}LHSqliteError;

typedef struct {
    LHSqliteValueType vale_type;
    const void* value;
}LHSqliteValue;

typedef struct {
    int blob_lenght;
    const void* blob;
}LHSqliteBlobValue;

typedef struct {
    int int_value;
}LHSqliteIntValue;

typedef struct {
    float float_value;
}LHSqliteFloatValue;

struct lh_sqlite {
    bool _isConnection;
    sqlite3* _db;
    const char* fileName;
    OSSpinLock _lock;
    LHDictionaryRef _cacheStmt;
};

typedef struct lh_sqlite* LHSqliteRef;

LHSqliteRef LHSqliteCreateWithFileName(const void* fileName);

__LHBOOL LHSqliteOpen(LHSqliteRef sqliteRef,__LHBOOL onLock);

__LHBOOL LHSqliteOpen_e(LHSqliteRef sqliteRef,LHSqliteError** error,__LHBOOL onLock);

void LHSqliteClearStmtCache(LHSqliteRef sqliteRef);

sqlite3_stmt* LHSqlitePrepareSQL(LHSqliteRef sqliteRef,const char* zSql,LHSqliteError** error);

void LHSqliteBindWithName(sqlite3_stmt* stmt,char* name,LHSqliteValue* value);

void LHSqliteBindWithIndex(sqlite3_stmt* stmt,int idx,LHSqliteValue* value);

void LHSqliteStepUpdate(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error);

LHArrayRef LHSqliteStepQuery(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error);

void LHSqliteExecuteUpdate(LHSqliteRef sqliteRef,const char* zSql,LHDictionaryRef dictionary,LHSqliteError** error);

LHArrayRef LHSqliteExecuteQuery(LHSqliteRef sqliteRef,const char* zSql,LHSqliteError** error);

__LHBOOL LHSqliteClose(LHSqliteRef sqliteRef,__LHBOOL onLock);

void LHSqliteErrorFree(LHSqliteError* sql_error);

void LHSqliteValueRelease(LHSqliteValue* value);

void LHSqliteRelease(LHSqliteRef sqliteRef);


#endif /* LHSqlite_h */
