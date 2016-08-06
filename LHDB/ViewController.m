//
//  ViewController.m
//  LHDB
//
//  Created by 李浩 on 16/7/13.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import "ViewController.h"
#include "LHSqlite.h"
#include "LHArray.h"
#include "LHDictionary.h"
#import "NSDictionary+LHDictionary.h"
#import "NSObject+LHSqlite.h"
#import "Teacher.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self notTransaction];
    [self transaction];
}


#pragma mark 不开启手动事务
- (void)notTransaction
{
    NSDate* date = [NSDate date];
    [Teacher lh_openDB];
    [Teacher lh_createTable];
    for (int i=0; i<1000; i++) {
        [Teacher lh_insertWith:@{@"date":[NSDate date],@"name":@"lihao",@"age":@18,@"data":[@"lihao" dataUsingEncoding:NSUTF8StringEncoding],@"array":@[@"123",@"213",@"321"]}];
    }
    [Teacher lh_closeDB];
    NSTimeInterval t = [[NSDate date] timeIntervalSinceDate:date];
    NSLog(@"notTransaction ===== %f",t);
}


#pragma mark 开始手动事务
- (void)transaction
{
    NSDate* date = [NSDate date];
    [Teacher lh_openDB];
    [Teacher lh_createTable];
    [Teacher lh_beginTransaction];
    for (int i=0; i<1000; i++) {
        [Teacher lh_insertWith:@{@"date":[NSDate date],@"name":@"lihao",@"age":@18,@"data":[@"lihao" dataUsingEncoding:NSUTF8StringEncoding],@"array":@[@"123",@"213",@"321"]}];
    }
    [Teacher lh_commit];
    [Teacher lh_closeDB];
    NSTimeInterval t = [[NSDate date] timeIntervalSinceDate:date];
    NSLog(@"Transaction ==== %f",t);

}

- (NSString*)deocument
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory=[paths objectAtIndex:0];
    return [documentsDirectory stringByAppendingPathComponent:@"mysqlite.sqlite"];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
