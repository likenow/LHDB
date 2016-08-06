//
//  LHPredicate.h
//  LHDB
//
//  Created by 李浩 on 16/7/26.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface LHPredicate : NSObject

@property (nonatomic,copy) NSString* predicateFormat;

@property (nonatomic,copy) NSString* sortDescriptor;

- (LHPredicate*)initWithString:(NSString*)str;

- (LHPredicate*)initWithFormat:(NSString*)format,...NS_FORMAT_FUNCTION(1,2);

+ (LHPredicate*)predicateWithString:(NSString*)str;

+ (LHPredicate*)predicateWithFormat:(NSString*)format,...NS_FORMAT_FUNCTION(1, 2);

- (LHPredicate*)initWithString:(NSString *)str sort:(NSString*)sortDescriptor;

- (LHPredicate*)initWithSort:(NSString*)sortDescriptor predicateFormat:(NSString*)format, ... NS_FORMAT_FUNCTION(2, 3);

+ (LHPredicate*)predicateWithString:(NSString*)str sort:(NSString*)sortDescriptor;

+ (LHPredicate*)predicateWithSort:(NSString*)sortDescriptor predicateFormat:(NSString*)format, ... NS_FORMAT_FUNCTION(2, 3);

@end
