//
//  LHArray.c
//  LHDB
//
//  Created by 李浩 on 16/8/2.
//  Copyright © 2016年 3wchina01. All rights reserved.
//

#include "LHArray.h"
#include <stdlib.h>

static float relloc_count = 1.5;

#define __STATIC__INLINE static inline

__STATIC__INLINE bool lh_array_default_string_equal(const void* value1,const void* value2)
{
    return strcmp(value1,value2) == 0;
}

__STATIC__INLINE bool lh_arrayReallocBucket(LHArrayRef array)
{
    if (array->count >= array->bucket_count) {
        const void** new_bucket = calloc(relloc_count*(array->bucket_count), sizeof(void*));
        if (new_bucket == NULL) {
            return false;
        }
        for (int i=0; i<array->count; i++) {
            new_bucket[i] = array->bucket[i];
        }
        free(array->bucket);
        array->bucket = NULL;
        array->bucket = new_bucket;
        array->bucket_count = relloc_count*(array->bucket_count);
    }
    return true;
}


LHArrayCallBacks lh_default_string_callback = {
    (lh_arrayValueRetain)strdup,
    (lh_arrayValueRelease)free,
    lh_array_default_string_equal
};

LHArrayRef LHArrayCreate()
{
    return LHArrayCreateWithOptions(10, NULL);
}

LHArrayRef LHArrayCreateWithCapacity(lh_uint capacity)
{
    return LHArrayCreateWithOptions(capacity, NULL);
}

LHArrayRef LHArrayCreateWithOptions(lh_uint capacity,LHArrayCallBacks* callback)
{
    LHArrayRef arrayRef = malloc(sizeof(struct lh_array));
    if (arrayRef == NULL) {
        return NULL;
    }
    arrayRef->bucket = calloc(capacity, sizeof(void*));
    if (arrayRef->bucket == NULL) {
        free(arrayRef);
        return NULL;
    }
    
    arrayRef->bucket_count = capacity;
    
    arrayRef->count = 0;
    if (callback) arrayRef->callback = *callback;
    else {
        arrayRef->callback.retain = NULL;
        arrayRef->callback.release = NULL;
    }
    return arrayRef;
}

lh_uint LHArrayGetCount(LHArrayRef arrayRef)
{
    if (arrayRef == NULL) return 0;
    return arrayRef->count;
}

void LHArrayApplyFunction(LHArrayRef arrayRef,lh_arrayApplierFunction applier,const void* context)
{
    if (arrayRef == NULL || applier == NULL) return;
    
    for (int i=0; i<LHArrayGetCount(arrayRef); i++) {
        const void* value = arrayRef->bucket[i];
        applier(i,value,context);
    }
}

__BOOL LHArrayContainValue(LHArrayRef arrayRef,const void* value)
{
    if (arrayRef == NULL || arrayRef->count == 0 ||arrayRef->callback.equal == NULL || value == NULL)
        return false;
    
    for (int i=0; i<arrayRef->count; i++) {
        const void* sub_value = arrayRef->bucket[i];
        if (arrayRef->callback.equal(sub_value,value)) {
            return true;
        }
    }
    return false;
}

void LHArrayAppentValue(LHArrayRef arrayRef,const void* value)
{
    LHArrayInsertValueAtIndex(arrayRef, arrayRef->count, value);
}

void LHArrayInsertValueAtIndex(LHArrayRef arrayRef,lh_uint index,const void* value)
{
    if (arrayRef == NULL||value == NULL||index > arrayRef->count) return;
    if (arrayRef->count >= arrayRef->bucket_count) {
        if (!lh_arrayReallocBucket(arrayRef)) {
            return;
        }
    }
    
    
    if (arrayRef->count != index) {
        for (lh_uint i=arrayRef->count-1; i>=index; i--) {
            const void* move_value = arrayRef->bucket[i];
            arrayRef->bucket[i+1] = move_value;
        }
    }
    
    !arrayRef->callback.retain ? NULL:(value = arrayRef->callback.retain(value));
    arrayRef->bucket[index] = value;
    arrayRef->count += 1;
}

void* LHArrayGetValueWithIndex(LHArrayRef arrayRef,lh_uint index)
{
    if (arrayRef == NULL || index >= arrayRef->count) return NULL;
    
    return (void*)arrayRef->bucket[index];
}

void* LHArrayGetFirstValue(LHArrayRef arrayRef)
{
    return LHArrayGetValueWithIndex(arrayRef, 0);
}

void* LHArrayGetLastValue(LHArrayRef arrayRef)
{
    return LHArrayGetValueWithIndex(arrayRef, arrayRef->count-1);
}

void LHArrayRemoveValueAtIndex(LHArrayRef arrayRef,lh_uint index)
{
    if (arrayRef == NULL || index >= arrayRef->count) return;
    
    const void* value = arrayRef->bucket[index];
    if (arrayRef->callback.release) arrayRef->callback.release(value);
    
    if (index != arrayRef->count-1) {
        for (lh_uint i=index+1; i<arrayRef->count; i++) {
            arrayRef->bucket[i-1] = arrayRef->bucket[i];
        }
    }
    arrayRef->bucket[arrayRef->count-1] = NULL;
    arrayRef->count -= 1;
}

void LHArrayRemoveLastValue(LHArrayRef arrayRef)
{
    LHArrayRemoveValueAtIndex(arrayRef, arrayRef->count-1);
}

void LHArrayRemoveFirstValue(LHArrayRef arrayRef)
{
    LHArrayRemoveValueAtIndex(arrayRef, 0);
}

void LHArrayRemoveAllValue(LHArrayRef arrayRef)
{
    if (arrayRef == NULL || arrayRef->count == 0) return;
    
    for (int i=0; i<arrayRef->count; i++) {
        const void* value = arrayRef->bucket[i];
        if (arrayRef->callback.release) arrayRef->callback.release(value);
        arrayRef->bucket[i] = NULL;
    }
    arrayRef->count = 0;
}

void LHArrayExchangeValuesAtIndexWithIndex(LHArrayRef arrayRef,lh_uint fromIndex,lh_uint toIndex)
{
    if (arrayRef == NULL || arrayRef->count <= 1 || fromIndex >= arrayRef->count ||toIndex >= arrayRef->count)
            return;
    
    const void* temp_value = arrayRef->bucket[fromIndex];
    arrayRef->bucket[fromIndex] = arrayRef->bucket[toIndex];
    arrayRef->bucket[toIndex] = temp_value;
}

void LHArrayReplaceValueAtIndex(LHArrayRef arrayRef,lh_uint index,const void* value)
{
    if (arrayRef == NULL || index >= arrayRef->count || value == NULL)
        return;
    const void* old_value = arrayRef->bucket[index];
    if (arrayRef->callback.release) arrayRef->callback.release(old_value);
    
    if (arrayRef->callback.retain) value = arrayRef->callback.retain(value);
    
    arrayRef->bucket[index] = value;
}

void LHArrayRelease(LHArrayRef arrayRef)
{
    if (arrayRef == NULL)  return;
    
    free(arrayRef->bucket);
    arrayRef->bucket = NULL;
    free(arrayRef);
    arrayRef = NULL;
}
