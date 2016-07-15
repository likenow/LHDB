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

#define __LHBOOL bool

typedef const void* (*LHSqliteRetainCallBack)(const void* value);

typedef void (*LHSqliteReleaseCallBack)(const void* value);

typedef __LHBOOL (*LHSqliteEqualCallBack)(const void* value1,const void* value2);

typedef enum LHSqliteValueType {
    LHSqliteValueINTEGER
}LHSqliteValueType;

typedef struct {
    const char* error_msg;
    int error_code;
}LHSqliteError;

typedef struct {
    LHSqliteRetainCallBack retain;
    LHSqliteReleaseCallBack release;
    LHSqliteEqualCallBack equal;
}LHSqliteCallBacks;

extern LHSqliteCallBacks kLHSqliteDefaultStringCallBacks;


typedef struct lh_sqlite* LHSqliteRef;

#endif /* LHSqlite_h */
