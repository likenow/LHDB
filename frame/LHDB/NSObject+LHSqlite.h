//
//  NSObject+LHSqlite.h
//  LHDB
//
//  Created by 李浩 on 16/8/1.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "LHSqlite.h"

typedef BOOL(^executeUpdate)(void);

typedef NSArray*(^executeQuery)(void);

@class LHPredicate;
@interface NSObject (LHSqlite)

/**
 @dis 打开数据库,会启动一个锁,当执行closeDB时释放锁。
 每个线程在执行数据库任务时都应该调用openDB;
 */
+ (BOOL)lh_openDB;

/**
 @param sqlite  指定执行的数据库,可以从LHSqliteManager中获取.
 */
+ (BOOL)lh_openDB:(LHSqliteRef)sqlite;

/**
 @dis 关闭数据库连接同时释放锁
 */
+ (BOOL)lh_closeDB;

/**
 @param sqlite 指定操作的数据库.
 */
+ (BOOL)lh_closeDB:(LHSqliteRef)sqlite;

/**
 @dis 建表
 */
+ (BOOL)lh_createTable;

+ (BOOL)lh_createTable:(LHSqliteRef)sqlite;


/**
 @dis 建表,添加约束
 @prarm contraints 一个约束的字典,key对应表的cloumn value对应一个存放约束的数组 数组中对应LHSqliteConstraint枚举
 */
+ (BOOL)lh_createTableWithContraints:(NSDictionary<NSString*,NSArray<NSNumber*>*>*)contraints;

+ (BOOL)lh_createTable:(LHSqliteRef)sqlite contraints:(NSDictionary<NSString*,NSArray<NSNumber*>*>*)contraints;

/**
 @dis 插入数据
 */
- (BOOL)lh_insert;

- (BOOL)lh_insert:(LHSqliteRef)sqlite;

/**
 @param dic  要插入的数据,其中key必须和表中字段匹配,不允许存在表中没有的字段
 */
+ (BOOL)lh_insertWith:(NSDictionary*)dic;

+ (BOOL)lh_insertWith:(NSDictionary*)dic sqlite:(LHSqliteRef)sqlite;

/**
 @param dic  更新数据
 @prarm predicate 范围,相当于NSPredicate
 */
+ (BOOL)lh_updateWith:(NSDictionary*)dic predicate:(LHPredicate*)predicate;

/**
 @prarm predicate  谓词
 */
+ (BOOL)lh_deleteWithPredicate:(LHPredicate*)predicate;

+ (BOOL)lh_deleteWithPredicate:(LHPredicate *)predicate sqlite:(LHSqliteRef)sqlite;

/**
 @dis 查询
 @return 包含了所有查询数据的数组,其中元素是model
 */
+ (NSArray*)lh_selectWithPredicate:(LHPredicate*)predicate;

+ (NSArray*)lh_selectWithPredicate:(LHPredicate *)predicate sqlite:(LHSqliteRef)sqlite;

/**
 @dis 开启事务 当执行大量插入,更新,删除操作时建议使用(速度快了大概10倍)
 */
+ (BOOL)lh_beginTransaction;

+ (BOOL)lh_beginTransaction:(LHSqliteRef)sqlite;

/**
 @dis 提交事务
 */
+ (BOOL)lh_commit;

+ (BOOL)lh_commit:(LHSqliteRef)sqlite;

/**
 @dis 回滚事务
 */
+ (BOOL)lh_rollback;

+ (BOOL)lh_rollback:(LHSqliteRef)sqlite;

/**
 @dis 数据库的更新执行,无需打开数据库,只需将更新操作放入handle中
 */
+ (void)lh_executeUpdateHandle:(void(^)())handle;

@end
