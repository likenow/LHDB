//
//  NSObject+SQLStateMent.h
//  LHDB
//
//  Created by 李浩 on 16/7/26.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import <Foundation/Foundation.h>

@class LHPredicate;

typedef NS_ENUM(NSUInteger,LHSqliteConstraint) {
    LHSqliteConstraintNOTNULL = 1<<3,
    LHSqliteConstraintUNIQUE = 1<<4,
    LHSqliteConstraintPRIMARYKEY = 1<<5
};
@interface NSObject (SQLStateMent)

+ (char*)lh_statementForCreateTable;

+ (char*)lh_statementForCreateTableWithConstraint:(NSDictionary<NSString*,NSArray<NSNumber*>*>*)constraints;

+ (char*)lh_statementForAddColumn:(NSString*)columnName;

+ (char*)lh_statementForInsert:(NSDictionary*)value;

+ (char*)lh_statementForUpdate:(NSDictionary*)value predicate:(LHPredicate*)predicate;

+ (char*)lh_statementForDelete:(LHPredicate*)predicate;

+ (char*)lh_statementForSelect:(LHPredicate*)predicate;

+ (char*)lh_statementForDropTable;

+ (char*)lh_statementForClearTable;

@end
