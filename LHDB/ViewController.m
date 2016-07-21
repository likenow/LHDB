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

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
//    [self dic];
    [self sql];
//    LHSqliteRelease(ref);
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)dic
{
    LHDictionaryRef dic = lh_dictionary_create();
    char* key = "11";
    char* value = "lihao";
    lh_dictionarySetValueForKey(dic, key, value);
    const void* v = lh_dictionaryGetValueForKey(dic, key);
    
}

- (void)sql
{
    LHSqliteRef ref = LHSqliteCreateWithFileName([self deocument].UTF8String);
    LHSqliteError* error0 = NULL;
    LHSqliteOpen_e(ref, &error0, false);
    char* sql = "CREATE TABLE IF NOT EXISTS teacher (NAME TEXT,age TEXT)";
    LHSqliteError* error1 = NULL;
    sqlite3_stmt* stmt = LHSqlitePrepareSQL(ref, sql, &error1);
    LHSqliteError* error2 = NULL;
    LHSqliteStepUpdate(ref, stmt, &error2);
    
    char* insert = "INSERT INTO teacher (NAME,age) VALUES (?,?)";
    sqlite3_stmt* stmt1 = LHSqlitePrepareSQL(ref, insert, &error1);
    LHSqliteBindWithIndex(stmt1, 1, "lihao", 0, LHSqliteValueTEXT);
    LHSqliteBindWithIndex(stmt1, 2, "11", 0, LHSqliteValueTEXT);
    LHSqliteStepUpdate(ref, stmt1, &error2);
    
    char* select = "select * from teacher";
    LHSqliteError* error3 = NULL;
    sqlite3_stmt* stmt2 = LHSqlitePrepareSQL(ref, select, &error3);
    
    LHArrayRef arrayRef = LHSqliteStepQuery(ref, stmt2, &error3);
    for (int i=0; i<arrayRef->count; i++) {
        LHDictionaryRef dic = lh_arrayGetValueWithIndex(arrayRef, i);
        NSDictionary* obdic = [NSDictionary lh_transfromDictionary:dic];
        NSLog(@"dic = %@",obdic);
    }
    LHSqliteClose(ref, false);
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
