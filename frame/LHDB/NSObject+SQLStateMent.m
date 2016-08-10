//
//  NSObject+SQLStateMent.m
//  LHDB
//
//  Created by 李浩 on 16/7/26.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import "NSObject+SQLStateMent.h"
#import "NSObject+LHModel.h"
#import "LHPredicate.h"

@implementation NSObject (SQLStateMent)

static inline NSString* columnType(NSString* propertyType)
{
    if (propertyType == nil || propertyType.length == 0) {
        return nil;
    }
    
    if ([propertyType isEqualToString:@"i"]||[propertyType isEqualToString:@"Q"]) {
        return @"INTEGER";
    }else if ([propertyType isEqualToString:@"f"]) {
        return @"FLOAT";
    }else if ([propertyType isEqualToString:@"B"]) {
        return @"BOOL";
    }else if ([propertyType isEqualToString:@"d"]) {
        return @"DOUBLE";
    }else if ([propertyType isEqualToString:@"q"]) {
        return @"LONG";
    }else if ([propertyType isEqualToString:@"NSData"]||[propertyType isEqualToString:@"UIImage"]) {
        return @"BLOB";
    }else
        return @"TEXT";
}

+ (char*)lh_statementForCreateTable
{
    NSString* table_name = NSStringFromClass(self);
    NSMutableString* sql_str = [NSMutableString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@",table_name];
    
    NSDictionary* propertyInfo = [self lh_propertyInfo];
    
    NSMutableString* column_str = [NSMutableString string];
    [propertyInfo enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
        NSString* column_name = [NSString stringWithFormat:@"%@",key];
        NSString* column_type = columnType([NSString stringWithFormat:@"%@",obj]);
        [column_str appendFormat:@"%@ %@,",column_name,column_type];
    }];
    if (column_str.length>0) {
        [column_str deleteCharactersInRange:NSMakeRange(column_str.length-1, 1)];
    }
    
    [sql_str appendFormat:@"(%@)",column_str];
    return (char*)sql_str.UTF8String;
}


+ (char*)lh_statementForCreateTableWithConstraint:(NSDictionary<NSString*,NSArray<NSNumber*>*>*)constraints;
{
    NSArray* constrains_keys = constraints.allKeys;
    NSString* table_name = NSStringFromClass(self);
    NSMutableString* sql_str = [NSMutableString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@",table_name];
    NSDictionary* propertyInfo = [self lh_propertyInfo];
    NSMutableString* column_str = [NSMutableString string];
    [propertyInfo enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
        NSString* column_name = [NSString stringWithFormat:@"%@",key];
        NSString* column_type = columnType([NSString stringWithFormat:@"%@",obj]);
        NSMutableString* constraint_str = [NSMutableString string];
        if ([constrains_keys containsObject:column_name]) {
            NSArray* constrains_value = constraints[column_name];
            [constrains_value enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
                LHSqliteConstraint constraint = [(NSNumber*)obj integerValue];
                switch (constraint) {
                    case LHSqliteConstraintNOTNULL:
                        [constraint_str appendString:@" NOT NULL"];
                        break;
                    case LHSqliteConstraintPRIMARYKEY:
                        [constraint_str appendString:@" PRIMARY KEY"];
                        break;
                    case LHSqliteConstraintUNIQUE:
                        [constraint_str appendString:@" UNIQUE"];
                    default:
                        break;
                }
            }];
        }
        
        [column_str appendFormat:@"%@ %@%@,",column_name,column_type,constraint_str];
    }];
    if (column_str.length>0) {
        [column_str deleteCharactersInRange:NSMakeRange(column_str.length-1, 1)];
    }
    [sql_str appendFormat:@"(%@)",column_str];
    return (void*)sql_str.UTF8String;
}

+ (char*)lh_statementForAddColumn:(NSString*)columnName
{
    NSString* sql_str = [NSString stringWithFormat:@"alter table %@ add %@ %@",NSStringFromClass(self),columnName,[self lh_propertyType:columnName]];
    return (char*)sql_str.UTF8String;
}

+ (char*)lh_statementForInsert:(NSDictionary*)value
{
    NSMutableString* sql_str = [NSMutableString stringWithFormat:@"insert into %@",NSStringFromClass(self)];
    NSMutableString* key_str = [NSMutableString string];
    NSMutableString* value_str = [NSMutableString string];
    [value enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
        [key_str appendFormat:@"%@,",key];
        [value_str appendFormat:@"%@,",[@":" stringByAppendingString:key]];
    }];
    if (key_str.length>0) {
        [key_str deleteCharactersInRange:NSMakeRange(key_str.length-1, 1)];
    }
    if (value_str.length>0) {
        [value_str deleteCharactersInRange:NSMakeRange(value_str.length-1, 1)];
    }
    [sql_str appendFormat:@"(%@) values (%@)",key_str,value_str];
    return (char*)sql_str.UTF8String;
}

+ (char*)lh_statementForUpdate:(NSDictionary*)value predicate:(LHPredicate*)predicate
{
    NSMutableString* sql_str = [NSMutableString stringWithFormat:@"update %@ set ",NSStringFromClass(self)];
    [value enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
        NSString* key_str = [NSString stringWithFormat:@"%@ = :%@,",key,key];
        [sql_str appendString:key_str];
    }];
    [sql_str deleteCharactersInRange:NSMakeRange(sql_str.length-1, 1)];
    [sql_str appendFormat:@" where %@",predicate.predicateFormat];
    return (char*)sql_str.UTF8String;
}

+ (char*)lh_statementForDelete:(LHPredicate*)predicate
{
    NSMutableString* sql_str = [NSMutableString stringWithFormat:@"delete form %@ ",NSStringFromClass(self)];
    if (predicate.predicateFormat) {
        [sql_str appendFormat:@"where %@",predicate.predicateFormat];
    }
    return (char*)sql_str.UTF8String;
}

+ (char*)lh_statementForSelect:(LHPredicate*)predicate
{
    NSMutableString* sql_str = [NSMutableString stringWithFormat:@"select * from %@",NSStringFromClass(self)];
    if (predicate.predicateFormat) {
        [sql_str appendFormat:@" where %@",predicate.predicateFormat];
    }
    if (predicate.sortDescriptor) {
        [sql_str appendFormat:@" order by %@",predicate.sortDescriptor];
    }
    return (char*)sql_str.UTF8String;
}

+ (char*)lh_statementForDropTable
{
    NSMutableString* sql_str = [NSMutableString stringWithFormat:@"drop table %@",NSStringFromClass(self)];
    return (char*)sql_str.UTF8String;
}

@end
