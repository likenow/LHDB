//
//  LHSqliteManager.m
//  LHDB
//
//  Created by 李浩 on 16/8/1.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import "LHSqliteManager.h"
#include "LHDictionary.h"

@interface LHSqliteManager()

@property (nonatomic,readwrite) LHSqliteRef currentSqlite;

@end

@implementation LHSqliteManager{
    LHDictionaryRef _sqliteCache;
    NSString* _sqlitePath;
}

+ (instancetype)shareInstance
{
    static LHSqliteManager* manager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        manager = [[self alloc] init];
    });
    return manager;
}

- (instancetype)init
{
    if (self = [super init]) {
        _sqliteCache = lh_dictionary_create();
        _sqlitePath = DEFAULT_PATH;
        
    }
    return self;
}

- (void)setSqlitePath:(NSString *)sqlitePath
{
    void (^handle)() = ^(){
        _sqlitePath = [sqlitePath copy];
        self.currentSqlite = [self sqliteWithPath:sqlitePath];
    };
    [sqlitePath isEqualToString:_sqlitePath]?:handle();
    
}

- (LHSqliteRef)currentSqlite
{
    if (!_currentSqlite) {
        _currentSqlite = [self sqliteWithPath:_sqlitePath];
    }
    return _currentSqlite;
}

- (LHSqliteRef)sqliteWithPath:(NSString*)sqlitePath
{
    LHSqliteRef sqlite = lh_dictionaryGetValueForKey(_sqliteCache, sqlitePath.UTF8String);
    if (sqlite == NULL) {
        sqlite =  LHSqliteCreateWithFileName(sqlitePath.UTF8String);
        lh_dictionarySetValueForKey(_sqliteCache, sqlitePath.UTF8String, sqlite);
    }
    return sqlite;
}

@end
