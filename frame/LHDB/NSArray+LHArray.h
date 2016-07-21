//
//  NSArray+LHArray.h
//  LHDB
//
//  Created by 李浩 on 16/7/21.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "LHArray.h"

@interface NSArray (LHArray)

+ (NSMutableArray*)lh_transfromNSArray:(LHArrayRef)array;

- (LHArrayRef)lh_transfromLHArray;

- (NSArray*)lh_transfromModel;

@end
