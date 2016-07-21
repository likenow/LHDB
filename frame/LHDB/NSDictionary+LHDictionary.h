//
//  NSDictionary+LHDictionary.h
//  LHDB
//
//  Created by 李浩 on 16/7/18.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "LHDictionary.h"

@interface NSDictionary (LHDictionary)

+ (NSMutableDictionary*)lh_transfromNSDictionary:(LHDictionaryRef)dic;

- (LHDictionaryRef)lh_transfromLHDictionary;

@end
