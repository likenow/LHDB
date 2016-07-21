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
            int i = *(int*)(sqlite_type->value);
            dic_value = [NSNumber numberWithInt:i];
        }
            break;
            
        case LHSqliteValueFLOAT:
        {
            float f = *(float*)sqlite_type->value;
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
        sqliteValue->value = c;
    }else if ([object isKindOfClass:[NSArray class]] || [object isKindOfClass:[NSDictionary class]]){
        if ([NSJSONSerialization isValidJSONObject:object]) {
            NSData* data = [NSJSONSerialization dataWithJSONObject:object options:NSJSONWritingPrettyPrinted error:nil];
            if (data) {
                LHSqliteBlobValue* blobValue = malloc(sizeof(LHSqliteBlobValue));
                if (blobValue) {
                    blobValue->blob_lenght = (int)data.length;
                    blobValue->blob = data.bytes;
                    sqliteValue->vale_type = LHSqliteValueBLOB;
                    sqliteValue->value = blobValue;
                }
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
            float f = [number floatValue];
            sqliteValue->value = &f;
        }else {
            sqliteValue->vale_type = LHSqliteValueINTEGER;
            int i = [number intValue];
            sqliteValue->value = &i;
        }
    }else if ([object isKindOfClass:[NSDate class]]) {
        NSDate* date = (NSDate*)object;
        NSString* dateString = nil;
        stringFromDate(date,nil,&dateString);
        sqliteValue->vale_type = LHSqliteValueTEXT;
        sqliteValue->value = dateString.UTF8String;
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
    NSInteger count = lh_realCapacityWithCount(self.count);
    LHDictionaryRef dicRef = lh_dictionary_create_with_options((uint)count, &lh_default_key_callback, NULL);
    if (dicRef == NULL) {
        return NULL;
    }
    
    [self enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
        if (![key isKindOfClass:[NSString class]]) {
            return ;
        }
        LHSqliteValue* dic_value = lh_transfromSqliteValue(obj);
        if (dic_value) {
            if (dic_value->value == NULL) {
                LHSqliteValueRelease(dic_value);
                return;
            }
            char* dic_key = (char*)((NSString*)key).UTF8String;
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
