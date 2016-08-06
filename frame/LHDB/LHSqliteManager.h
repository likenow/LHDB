//
//  LHSqliteManager.h
//  LHDB
//
//  Created by 李浩 on 16/8/1.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "LHSqlite.h"

#define DEFAULT_PATH [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject]stringByAppendingPathComponent:@"data.sqlite"]

@interface LHSqliteManager : NSObject


+ (instancetype)shareInstance;

/**  当前默认操作的数据库路径,修改后currentSqlite会跟随变化. */
@property (nonatomic,copy) NSString* sqlitePath;

/** 默认数据库操作对象 */
@property (nonatomic,readonly) LHSqliteRef currentSqlite;

/**
 @param sqlitePath 数据库路径
 @return 返回一个sqlite对象,会缓存到Manager中,如果存在就从缓存中取,如果不存在则创建.
 */
- (LHSqliteRef)sqliteWithPath:(NSString*)sqlitePath;

@end
