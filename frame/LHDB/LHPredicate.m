//
//  LHPredicate.m
//  LHDB
//
//  Created by 李浩 on 16/7/26.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import "LHPredicate.h"

@implementation LHPredicate


- (LHPredicate*)initWithString:(NSString*)str
{
    return [[LHPredicate alloc] initWithString:str sort:nil];
}

- (LHPredicate*)initWithFormat:(NSString*)format, ...
{
    if (self = [super init]) {
        va_list args;
        va_start(args, format);
        _predicateFormat = [[NSString alloc] initWithFormat:format arguments:args];
        va_end(args);
    }
    return self;
}

+ (LHPredicate*)predicateWithString:(NSString*)str
{
    LHPredicate* predicate = [[self alloc] initWithString:str];
    return predicate;
}

+ (LHPredicate*)predicateWithFormat:(NSString *)format, ...
{
    LHPredicate* predicate = [[self alloc] init];
    va_list args;
    va_start(args, format);
    predicate->_predicateFormat = [[NSString alloc] initWithFormat:format arguments:args];
    va_end(args);
    return predicate;
}

- (LHPredicate*)initWithString:(NSString *)str sort:(NSString*)sortDescriptor
{
    if (self = [super init]) {
        _predicateFormat = [str copy];
        _sortDescriptor = [sortDescriptor copy];
    }
    return self;
}

- (LHPredicate*)initWithSort:(NSString*)sortDescriptor predicateFormat:(NSString*)format, ...
{
    if (self = [super init]) {
        _sortDescriptor = [sortDescriptor copy];
        va_list args;
        va_start(args, format);
        _predicateFormat = [[NSString alloc] initWithFormat:format arguments:args];
        va_end(args);
    }
    return self;
}

+ (LHPredicate*)predicateWithString:(NSString*)str sort:(NSString*)sortDescriptor
{
    return [LHPredicate predicateWithString:str sort:sortDescriptor];
}

+ (LHPredicate*)predicateWithSort:(NSString*)sortDescriptor predicateFormat:(NSString*)format, ...
{
    LHPredicate* predicate = [[LHPredicate alloc] initWithString:nil sort:sortDescriptor];
    va_list args;
    va_start(args, format);
    predicate.predicateFormat = [[NSString alloc] initWithFormat:format arguments:args];
    va_end(args);
    return predicate;
}

@end
