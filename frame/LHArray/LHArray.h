//
//  LHArray.h
//  LHDB
//
//  Created by 李浩 on 16/8/2.
//  Copyright © 2016年 3wchina01. All rights reserved.
//

#ifndef LHArray_h
#define LHArray_h

#include <stdio.h>
#include <stdbool.h>
#include <MacTypes.h>

#define LH_EXTERN extern

#define __BOOL bool

typedef struct lh_array* LHArrayRef;

typedef unsigned long lh_uint;

typedef void (*lh_arrayApplierFunction)(lh_uint index,const void *value,const void *context);

typedef void* (*lh_arrayValueRetain)(const void* value);

typedef void (*lh_arrayValueRelease)(const void* value);

typedef bool (*lh_arrayValueEqual)(const void* value1,const void* value2);

typedef struct {
    lh_arrayValueRetain retain;
    lh_arrayValueRelease release;
    lh_arrayValueEqual equal;
}LHArrayCallBacks;


struct lh_array {
    uint count;
    UInt64 bucket_count;
    LHArrayCallBacks callback;
    const void** bucket;
};

LH_EXTERN LHArrayCallBacks lh_default_string_callback;

LHArrayRef LHArrayCreate();

LHArrayRef LHArrayCreateWithCapacity(lh_uint capacity);

LHArrayRef LHArrayCreateWithOptions(lh_uint capacity,LHArrayCallBacks* callback);

lh_uint LHArrayGetCount(LHArrayRef arrayRef);

void LHArrayApplyFunction(LHArrayRef arrayRef,lh_arrayApplierFunction applier,const void* context);

__BOOL LHArrayContainValue(LHArrayRef arrayRef,const void* value);

void LHArrayAppentValue(LHArrayRef arrayRef,const void* value);

void LHArrayInsertValueAtIndex(LHArrayRef arrayRef,lh_uint index,const void* value);

void* LHArrayGetValueWithIndex(LHArrayRef arrayRef,lh_uint index);

void* LHArrayGetFirstValue(LHArrayRef arrayRef);

void* LHArrayGetLastValue(LHArrayRef arrayRef);

void LHArrayRemoveValueAtIndex(LHArrayRef arrayRef,lh_uint index);

void LHArrayRemoveLastValue(LHArrayRef arrayRef);

void LHArrayRemoveFirstValue(LHArrayRef arrayRef);

void LHArrayRemoveAllValue(LHArrayRef arrayRef);

void LHArrayExchangeValuesAtIndexWithIndex(LHArrayRef arrayRef,lh_uint fromIndex,lh_uint toIndex);

void LHArrayReplaceValueAtIndex(LHArrayRef arrayRef,lh_uint index,const void* value);

void LHArrayRelease(LHArrayRef arrayRef);

#endif /* LHArray_h */
