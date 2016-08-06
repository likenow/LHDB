//
//  NSDictionary+LHDictionary.m
//  LHDB
//
//  Created by 李浩 on 16/7/18.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import "NSDictionary+LHDictionary.h"
#include "LHSqlite.h"
#import <UIKit/UIKit.h>

@implementation NSDictionary (LHDictionary)

static inline void lh_sqliteDictionaryRelease(const void* value)
{
    LHSqliteValue* sqliteValue = (LHSqliteValue*)value;
    LHSqliteValueRelease(sqliteValue);
}

void stringFromDate(NSDate* date,NSDateFormatter* matter,NSString** dateString)
{
    if (date == nil || dateString == nil) {
        return;
    }
    if (matter == nil) {
        matter = [[NSDateFormatter alloc] init];
        matter.dateFormat = @"yyyy-MM-dd HH:mm:ss";
    }
    *dateString = [matter stringFromDate:date];
    return ;
    
}

static inline void lh_dictionaryApplierFunction(const void* key,const void* value,void* context)
{
    NSMutableDictionary* dic = (__bridge NSMutableDictionary*)context;
    NSString* dic_key = [NSString stringWithUTF8String:key];
    LHSqliteValue* sqlite_type = (LHSqliteValue*)value;
    NSObject* dic_value = nil;
    switch (sqlite_type->vale_type) {
        case LHSqliteValueBLOB:
        {
            LHSqliteBlobValue* blob_value = (LHSqliteBlobValue*)(sqlite_type->value);
            NSData* data = [NSData dataWithBytes:blob_value->blob length:blob_value->blob_lenght];
            dic_value = data;
            
        }
            break;
            
        case LHSqliteValueINTEGER:
        {
            LHSqliteIntValue* int_value = (LHSqliteIntValue*)sqlite_type->value;
            int i = int_value->int_value;
            dic_value = [NSNumber numberWithInt:i];
        }
            break;
            
        case LHSqliteValueFLOAT:
        {
            LHSqliteFloatValue* float_value = (LHSqliteFloatValue*)sqlite_type->value;
            float f = float_value->float_value;
            dic_value = [NSNumber numberWithFloat:f];
        }
            break;
            
        case LHSqliteValueTEXT:
        {
            dic_value = [NSString stringWithUTF8String:(char*)(sqlite_type->value)];
        }
            break;
        default:
            dic_value = nil;
            break;
    }
    if (dic_value) {
        [dic setValue:dic_value forKey:dic_key];
    }
    LHSqliteValueRelease(sqlite_type);
}

static inline LHSqliteValue* lh_transfromSqliteValue(NSObject* object)
{
    LHSqliteValue* sqliteValue = malloc(sizeof(LHSqliteValue));
    if (sqliteValue == NULL) {
        return NULL;
    }
    if ([object isKindOfClass:[NSString class]]) {
        char* c = (char*)((NSString*)object).UTF8String;
        sqliteValue->vale_type = LHSqliteValueTEXT;
        sqliteValue->value = strdup(c);
    }else if ([object isKindOfClass:[NSArray class]] || [object isKindOfClass:[NSDictionary class]]){
        if ([NSJSONSerialization isValidJSONObject:object]) {
            NSData* data = [NSJSONSerialization dataWithJSONObject:object options:NSJSONWritingPrettyPrinted error:nil];
            NSString* dataStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
            if (dataStr) {
                sqliteValue->vale_type = LHSqliteValueTEXT;
                sqliteValue->value = strdup(dataStr.UTF8String);
//                LHSqliteBlobValue* blobValue = malloc(sizeof(LHSqliteBlobValue));
//                if (blobValue) {
//                    blobValue->blob_lenght = (int)data.length;
//                    blobValue->blob = data.bytes;
//                    sqliteValue->vale_type = LHSqliteValueBLOB;
//                    sqliteValue->value = blobValue;
//                }
            }
        }
    }else if ([object isKindOfClass:[NSData class]]) {
        NSData* data = (NSData*)object;
        LHSqliteBlobValue* blobValue = malloc(sizeof(LHSqliteBlobValue));
        if (blobValue) {
            blobValue->blob = data.bytes;
            blobValue->blob_lenght = (int)data.length;
            sqliteValue->value = blobValue;
            sqliteValue->vale_type = LHSqliteValueBLOB;
        }
    }else if ([object isKindOfClass:[UIImage class]]) {
        NSData* data = UIImageJPEGRepresentation((UIImage*)object, 1.0);
        LHSqliteBlobValue* blobValue = malloc(sizeof(LHSqliteValueBLOB));
        if (blobValue) {
            blobValue->blob_lenght = (int)data.length;
            blobValue->blob = data.bytes;
            sqliteValue->vale_type = LHSqliteValueBLOB;
            sqliteValue->value = blobValue;
        }
    }else if ([object isKindOfClass:[NSNumber class]]) {
        NSNumber* number = (NSNumber*)object;
        if (strcmp([number objCType], @encode(float)) == 0 || strcmp([number objCType], @encode(double)) == 0) {
            sqliteValue->vale_type = LHSqliteValueFLOAT;
            LHSqliteFloatValue* float_value = malloc(sizeof(LHSqliteFloatValue));
            float f = [number floatValue];
            float_value->float_value = f;
            sqliteValue->value = float_value;
        }else {
            sqliteValue->vale_type = LHSqliteValueINTEGER;
            LHSqliteIntValue* int_value = malloc(sizeof(LHSqliteIntValue));
            int i = [number intValue];
            int_value->int_value = i;
            sqliteValue->value = int_value;
        }
    }else if ([object isKindOfClass:[NSDate class]]) {
        NSDate* date = (NSDate*)object;
        NSString* dateString = nil;
        stringFromDate(date,nil,&dateString);
        sqliteValue->vale_type = LHSqliteValueTEXT;
        sqliteValue->value = strdup(dateString.UTF8String);
    }
    return sqliteValue;
}

+ (NSMutableDictionary*)lh_transfromNSDictionary:(LHDictionaryRef)dic
{
    if (dic == NULL || lh_dictionaryGetCount(dic)==0) {
        return nil;
    }
    NSMutableDictionary* transfromDic = [NSMutableDictionary dictionaryWithCapacity:lh_dictionaryGetCount(dic)];
    lh_dictionaryApplyFunction(dic, lh_dictionaryApplierFunction, (__bridge void *)(transfromDic));
    lh_dictionaryRelease(dic);
    return transfromDic;
}

- (LHDictionaryRef)lh_transfromLHDictionary
{
    NSInteger count = (NSInteger)lh_realCapacityWithCount((UInt64)self.count);
    map_value_callback value_callback = {
        NULL,
        lh_sqliteDictionaryRelease,
        NULL
    };
    LHDictionaryRef dicRef = lh_dictionary_create_with_options((uint)count, &lh_default_key_callback, &value_callback);
    if (dicRef == NULL) {
        return NULL;
    }
    
    [self enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
        if (![key isKindOfClass:[NSString class]]) {
            return ;
        }
        char* dic_key = (char*)((NSString*)key).UTF8String;
        LHSqliteValue* dic_value = lh_transfromSqliteValue(obj);
        if (dic_value) {

            if (dic_value->value == NULL) {
                LHSqliteValueRelease(dic_value);
                return;
            }

            lh_dictionarySetValueForKey(dicRef, dic_key, dic_value);
        }
    }];
    
    
    //字典长度为0时,释放内存
    if (lh_dictionaryGetCount(dicRef) == 0) {
        lh_dictionaryRelease(dicRef);
    }
    return dicRef;
}

@end
