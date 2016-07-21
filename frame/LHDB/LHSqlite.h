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
    const char* error_msg;
    int error_code;
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
    LHSqliteRetainCallBack retain;
    LHSqliteReleaseCallBack release;
    LHSqliteEqualCallBack equal;
}LHSqliteCallBacks;

extern LHSqliteCallBacks kLHSqliteDefaultStringCallBacks;


typedef struct lh_sqlite* LHSqliteRef;

LHSqliteRef LHSqliteCreateWithFileName(const void* fileName);

LHSqliteRef LHSqliteCreateWithOptions(const void* fileName,LHSqliteCallBacks* callback);

__LHBOOL LHSqliteOpen(LHSqliteRef sqliteRef,__LHBOOL onLock);

__LHBOOL LHSqliteOpen_e(LHSqliteRef sqliteRef,LHSqliteError** error,__LHBOOL onLock);

sqlite3_stmt* LHSqlitePrepareSQL(LHSqliteRef sqliteRef,const char* zSql,LHSqliteError** error);

void LHSqliteBindWithName(sqlite3_stmt* stmt,char* name,const void* value,int blob_length,LHSqliteValueType bindType);

void LHSqliteBindWithIndex(sqlite3_stmt* stmt,int idx,const void* value,int blob_length,LHSqliteValueType bindType);

void LHSqliteStepUpdate(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error);

LHArrayRef LHSqliteStepQuery(LHSqliteRef sqliteRef,sqlite3_stmt* stmt,LHSqliteError** error);

__LHBOOL LHSqliteClose(LHSqliteRef sqliteRef,__LHBOOL onLock);

void LHSqliteErrorFree(LHSqliteRef sqliteRef,LHSqliteError* sql_error);

void LHSqliteValueRelease(LHSqliteValue* value);

void LHSqliteRelease(LHSqliteRef sqliteRef);


#endif /* LHSqlite_h */
