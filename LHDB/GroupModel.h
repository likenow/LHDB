//
//  GroupModel.h
//  LHDB
//
//  Created by 李浩 on 16/8/10.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface GroupModel : NSObject

@property (nullable, nonatomic, strong) NSString *groupName;
@property (nullable, nonatomic, strong) NSString *groupNo;
@property (nullable, nonatomic, strong) NSString *photoPath;
@property (nullable, nonatomic, strong) NSString *acronym;
@property (nullable, nonatomic, strong) NSString *spelling;
@property (nullable, nonatomic, strong) NSString *topFlag;
@property (nullable, nonatomic, strong) NSString *timestamp;
@property (nullable,nonatomic,strong) NSString* createTime;

@end
