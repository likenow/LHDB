//
//  NSArray+LHArray.m
//  LHDB
//
//  Created by 李浩 on 16/7/21.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import "NSArray+LHArray.h"
#include "LHDictionary.h"
#import "NSDictionary+LHDictionary.h"
#import "NSObject+LHModel.h"

@implementation NSArray (LHArray)

static inline void lh_arrayAppleFunction(lh_uint index,const void* value,const void* context)
{
    NSMutableArray* array = (__bridge NSMutableArray *)(context);
    if (value) {
        LHDictionaryRef dic = (LHDictionaryRef)value;
        NSMutableDictionary* transfromDic = [NSDictionary lh_transfromNSDictionary:dic];
        if (transfromDic) {
            [array addObject:transfromDic];
        }
    }
}


+ (NSMutableArray*)lh_transfromNSArray:(LHArrayRef)array
{
    if (array == NULL || LHArrayGetCount(array) == 0) {
        return nil;
    }
    
    NSMutableArray* transfromArray = [NSMutableArray arrayWithCapacity:LHArrayGetCount(array)];
    
    LHArrayApplyFunction(array, lh_arrayAppleFunction,(__bridge void *)(transfromArray));
    LHArrayRelease(array);
    return transfromArray;
}

- (LHArrayRef)lh_transfromLHArray
{
    if (self == nil || self.count == 0) {
        return NULL;
    }
    LHArrayRef array = LHArrayCreate();
    if (array == NULL) {
        return NULL;
    }
    [self enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([obj isKindOfClass:[NSDictionary class]]) {
            NSDictionary* dic = (NSDictionary*)obj;
            LHDictionaryRef transfromDic = [dic lh_transfromLHDictionary];
            if (transfromDic) {
                LHArrayAppentValue(array, transfromDic);
            }
        }
    }];
    if (LHArrayGetCount(array) == 0) {
        LHArrayRelease(array);
    }
    
    return array;
}

//数组中的元素需要是NSDictionary
- (NSArray*)lh_transfromModelWithClass:(Class)cls;
{
    if (self == nil || self.count == 0) {
        return nil;
    }
    NSMutableArray* transfromArray = [NSMutableArray arrayWithCapacity:self.count];
    [self enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([obj isKindOfClass:[NSDictionary class]]) {
            NSObject* object = [cls lh_ModelWithDictionary:obj];
            if (object) {
                [transfromArray addObject:object];
            }
        }
    }];
    
    return transfromArray;
}

@end
