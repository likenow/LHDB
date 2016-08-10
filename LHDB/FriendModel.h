//
//  FriendModel.h
//  LHDB
//
//  Created by 李浩 on 16/8/10.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FriendModel : NSObject

@property (nullable, nonatomic, strong) NSString *headerChar;

@property (nullable,nonatomic,strong) NSNumber* sortNumber;
@property (nullable, nonatomic, strong) NSString *userName;
@property (nullable, nonatomic, strong) NSString *unitName;
@property (nullable, nonatomic, strong) NSString *photoPath;
@property (nullable, nonatomic, strong) NSString *nickName;
@property (nullable, nonatomic, strong) NSString *roleName;
@property (nullable, nonatomic, strong) NSString *initial;
@property (nullable, nonatomic, strong) NSString *acronym;
@property (nullable, nonatomic, strong) NSString *spelling;
@property (nullable, nonatomic, strong) NSString *realName;
@property (nullable, nonatomic, strong) NSString *sex;
@property (nullable, nonatomic, strong) NSString *mobile;
@property (nullable, nonatomic, strong) NSString *email;
@property (nullable, nonatomic, strong) NSString *weChat;
@property (nullable, nonatomic, strong) NSString *deptName;
@property (nullable, nonatomic, strong) NSString *roleType;
@property (nullable,nonatomic,strong) NSString* unitSpelling;

@property (nullable,nonatomic,strong) NSString* unitInitial;

@property (nullable,nonatomic,strong) NSString* unitAcronym;

@end
