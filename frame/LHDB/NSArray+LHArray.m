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

static inline void lh_arrayAppleFunction(const void* value,void* context)
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
    if (array == NULL || lh_arrayGetCount(array) == 0) {
        return nil;
    }
    
    NSMutableArray* transfromArray = [NSMutableArray arrayWithCapacity:lh_arrayGetCount(array)];
    
    lh_arrayApplyFunction(array, lh_arrayAppleFunction,(__bridge void *)(transfromArray));
    return transfromArray;
}

- (LHArrayRef)lh_transfromLHArray
{
    if (self == nil || self.count == 0) {
        return NULL;
    }
    LHArrayRef array = lh_arrayCreate();
    if (array == NULL) {
        return NULL;
    }
    [self enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([obj isKindOfClass:[NSDictionary class]]) {
            NSDictionary* dic = (NSDictionary*)obj;
            LHDictionaryRef transfromDic = [dic lh_transfromLHDictionary];
            if (transfromDic) {
                lh_arrayAppentValue(array, transfromDic);
            }
        }
    }];
    if (lh_arrayGetCount(array) == 0) {
        lh_arrayRelease(array);
    }
    return array;
}

//数组中的元素需要是NSDictionary
- (NSArray*)lh_transfromModel
{
    if (self == nil || self.count == 0) {
        return nil;
    }
    NSMutableArray* transfromArray = [NSMutableArray arrayWithCapacity:self.count];
    [self enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([obj isKindOfClass:[NSDictionary class]]) {
            NSObject* object = [obj lh_ModelToDictionary];
            if (object) {
                [transfromArray addObject:object];
            }
        }
    }];
    
    return transfromArray;
}

@end
