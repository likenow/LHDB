//
//  NSObject+LHSqlite.m
//  LHDB
//
//  Created by 李浩 on 16/8/1.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import "NSObject+LHSqlite.h"
#import "LHSqliteManager.h"
#import "NSObject+SQLStateMent.h"
#import "NSDictionary+LHDictionary.h"
#import "NSArray+LHArray.h"
#import "NSObject+LHModel.h"

#define CURRENT_SQLITE [LHSqliteManager shareInstance].currentSqlite

@implementation NSObject (LHSqlite)

+ (executeUpdate)lh_sqliteExecuteUpdate:(LHSqliteRef)sqlite sql:(char*)sqlStr dic:(LHDictionaryRef)dic
{
    executeUpdate opeartion = ^ BOOL (){
        LHSqliteError* error = NULL;
        
        LHSqliteExecuteUpdate(sqlite, sqlStr, dic, &error);
        if (error) {
            LHSqliteErrorFree(sqlite, error);
            LHSqliteClose(sqlite, YES);
            return NO;
        }
        return YES;
    };
    return opeartion;
}

+ (executeQuery)lh_sqliteExecuteQuery:(LHSqliteRef)sqlite sql:(char*)sqlStr
{
    executeQuery operation = ^ NSArray* (){
        LHSqliteError* error = NULL;
        
        LHArrayRef arrayRef = LHSqliteExecuteQuery(sqlite, sqlStr, &error);
        if (error) {
            LHSqliteErrorFree(sqlite, error);
            LHSqliteClose(sqlite, YES);
            return nil;
        }
        if (arrayRef == NULL) {
            return nil;
        }
        return [[NSArray lh_transfromNSArray:arrayRef] lh_transfromModelWithClass:self];
    };
    return operation;
}

+ (BOOL)lh_openDB
{
    return [self lh_openDB:CURRENT_SQLITE];
}

+ (BOOL)lh_openDB:(LHSqliteRef)sqlite
{
    return LHSqliteOpen(sqlite, YES);
}

+ (BOOL)lh_closeDB
{
    return [self lh_closeDB:CURRENT_SQLITE];
}

+ (BOOL)lh_closeDB:(LHSqliteRef)sqlite
{
    return LHSqliteClose(sqlite, YES);
}

+ (BOOL)lh_createTable
{
    return [self lh_createTable:CURRENT_SQLITE];
}

+ (BOOL)lh_createTable:(LHSqliteRef)sqlite
{
    return [self lh_sqliteExecuteQuery:sqlite sql:[self lh_statementForCreateTable]]();
}

+ (BOOL)lh_insertWith:(NSDictionary*)dic
{
    return [self lh_insertWith:dic sqlite:CURRENT_SQLITE];
}

+ (BOOL)lh_insertWith:(NSDictionary*)dic sqlite:(LHSqliteRef)sqlite
{
    char* sql_str = [self lh_statementForInsert:dic];
    LHDictionaryRef lh_dic = [dic lh_transfromLHDictionary];
    executeUpdate updateOpeartion = [self lh_sqliteExecuteUpdate:sqlite sql:sql_str dic:lh_dic];
    return updateOpeartion();
}

- (BOOL)lh_insert
{
    return [self lh_insert:CURRENT_SQLITE];
}

- (BOOL)lh_insert:(LHSqliteRef)sqlite
{
    
    return [[self class] lh_insertWith:[self lh_ModelToDictionary] sqlite:sqlite];
}

+ (BOOL)lh_updateWith:(NSDictionary*)dic predicate:(LHPredicate*)predicate
{
    return [self lh_updateWith:dic predicate:predicate sqlite:CURRENT_SQLITE];
}

+ (BOOL)lh_updateWith:(NSDictionary*)dic predicate:(LHPredicate*)predicate sqlite:(LHSqliteRef)sqlite
{
    char* sql_str = [self lh_statementForUpdate:dic predicate:predicate];
    LHDictionaryRef lh_dic = [dic lh_transfromLHDictionary];
    return [self lh_sqliteExecuteUpdate:sqlite sql:sql_str dic:lh_dic]();
}

+ (BOOL)lh_deleteWithPredicate:(LHPredicate*)predicate
{
    return [self lh_deleteWithPredicate:predicate sqlite:CURRENT_SQLITE];
}

+ (BOOL)lh_deleteWithPredicate:(LHPredicate *)predicate sqlite:(LHSqliteRef)sqlite
{
    char* sql_str = [self lh_statementForDelete:predicate];
    return [self lh_sqliteExecuteUpdate:sqlite sql:sql_str dic:NULL]();
}

+ (NSArray*)lh_selectWithPredicate:(LHPredicate*)predicate
{
    return [self lh_selectWithPredicate:predicate sqlite:CURRENT_SQLITE];
}

+ (NSArray*)lh_selectWithPredicate:(LHPredicate *)predicate sqlite:(LHSqliteRef)sqlite
{
    char* sql_str = [self lh_statementForSelect:predicate];
    return [self lh_sqliteExecuteQuery:sqlite sql:sql_str]();
}

+ (BOOL)lh_beginTransaction
{
    return [self lh_beginTransaction:CURRENT_SQLITE];
}

+ (BOOL)lh_beginTransaction:(LHSqliteRef)sqlite
{
    char* sql_str = "BEGIN TRANSACTION";
    return [self lh_sqliteExecuteUpdate:sqlite sql:sql_str dic:NULL]();
}

+ (BOOL)lh_commit
{
    return [self lh_commit:CURRENT_SQLITE];
}

+ (BOOL)lh_commit:(LHSqliteRef)sqlite
{
    char* sql_str = "COMMIT TRANSACTION";
    return [self lh_sqliteExecuteUpdate:sqlite sql:sql_str dic:NULL]();
}

+ (BOOL)lh_rollback
{
    return [self lh_rollback:CURRENT_SQLITE];
}

+ (BOOL)lh_rollback:(LHSqliteRef)sqlite
{
    char* sql_str = "ROLLBACK TRANSACTION";
    return [self lh_sqliteExecuteUpdate:sqlite sql:sql_str dic:NULL]();
}

@end
