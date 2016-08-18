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
    LHSqliteConstraintPRIMARYKEY = 1<<5,
    LHSqliteConstraintAUTOINCREMENT = 1<<6, /** 自增  只对整形有用 */
    LHSqliteConstraintFOREIGNKEY = 1<<7 /** 外键约束,iOS的sqlite3默认不开启外键,需手动开启,LHDB默认在每次打开数据库时都会开启外键 */
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
