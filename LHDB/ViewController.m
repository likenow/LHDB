//
//  ViewController.m
//  LHDB
//
//  Created by 李浩 on 16/7/13.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import "ViewController.h"
#import "LHDB.h"
#import "FriendModel.h"
#import "EFriendsCell.h"
#import "GroupController.h"

@interface ViewController ()<UITableViewDataSource,UITableViewDelegate>

@property (nonatomic,strong) UITableView* tableView;

@property (nonatomic,strong) NSArray* dataSource;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"好友";
    
    UIButton* button = [UIButton buttonWithType:UIButtonTypeContactAdd];
    button.frame = CGRectMake(0, 0, 40, 40);
    [button addTarget:self action:@selector(btnAction) forControlEvents:UIControlEventTouchUpInside];
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:button];
    [FriendModel lh_openDB];
    
    [self createTable];
    [self.view addSubview:self.tableView];
    self.dataSource = [self loadDataFromSQL];
    [FriendModel lh_closeDB];
    [self.tableView reloadData];
    [self loadDataFromInter];
}

- (UITableView*)tableView
{
    if (!_tableView) {
        _tableView = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height) style:UITableViewStyleGrouped];
        _tableView.delegate = self;
        _tableView.dataSource = self;
        _tableView.rowHeight = 60;
    }
    return _tableView;
}

- (void)btnAction
{
    GroupController* vc = [[GroupController alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

#pragma mark- UITableView
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return self.dataSource.count;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self.dataSource[section] count];
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    EFriendsCell* cell = [tableView dequeueReusableCellWithIdentifier:@"cell"];
    if (!cell) {
        cell = [[EFriendsCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"cell"];
    }
    FriendModel* model = self.dataSource[indexPath.section][indexPath.row];
    cell.model = model;
    return cell;
}

- (UIView*)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    FriendModel* model = [self.dataSource[section] firstObject];
    if ([model.headerChar isEqualToString:@"*"]) {
        return nil;
    }
    UIView* view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, 25)];
    view.backgroundColor = [UIColor lightGrayColor];
    UILabel* label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 50, 25)];
    label.text = [NSString stringWithFormat:@"  %@",model.headerChar];
    label.textColor = [UIColor blackColor];
    label.font = [UIFont systemFontOfSize:14];
    [view addSubview:label];
    return view;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    FriendModel* model = [self.dataSource[section] firstObject];
    
    if ([model.headerChar isEqualToString:@"*"]) {
        return 0.1;
    }
    return 25;
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    return 0.1;
}

-(NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView{
    NSMutableArray *arr = [[NSMutableArray alloc]init];
    for (NSArray* array in self.dataSource) {
        for (FriendModel* model in array) {
            [arr addObject:model.headerChar];
            break;
        }
    }
    return arr;
}

- (void)createTable
{
    /** 打开数据库 */
    /**
     model中userName是唯一且可以确定一条数据的所以可以作为主键，并且不能为空 约束NOT NULL
     */
    NSNumber* primaryKey = [NSNumber numberWithInteger:LHSqliteConstraintPRIMARYKEY];
    NSNumber* notNULL = [NSNumber numberWithInteger:LHSqliteConstraintNOTNULL];
    [FriendModel lh_createTableWithContraints:@{@"userName":@[primaryKey,notNULL]}];
}

- (void)importData:(NSDictionary*)dic
{
    /** 手动管理事务 */
    [FriendModel lh_beginTransaction];
    for (NSString* key in dic.allKeys) {
        NSArray* valueArray = dic[key];
        for (NSDictionary* valueDic in valueArray) {
            NSMutableDictionary* importDic = [NSMutableDictionary dictionaryWithDictionary:valueDic];
            [importDic setValue:key forKey:@"headerChar"];
            LHPredicate* predicate = [LHPredicate predicateWithFormat:@"userName = '%@'",importDic[@"userName"]];
            if ([FriendModel lh_selectWithPredicate:predicate].count>0) {
                [FriendModel lh_updateWith:importDic predicate:predicate];
            }else
                [FriendModel lh_insertWith:importDic];
        }
    }
    /** 提交事务 */
    [FriendModel lh_commit];
}

#pragma mark- 从数据库中去数据
- (NSArray*)loadDataFromSQL
{
    NSMutableArray* dataArray = [NSMutableArray array];
    LHPredicate* predicate = [LHPredicate predicateWithFormat:@"headerChar = '*'"];
    NSArray* result = [FriendModel lh_selectWithPredicate:predicate];
    if (result.count>0) {
        [dataArray addObject:result];
    }
    for (int i='A'; i<='Z'; i++) {
        NSString* str = [NSString stringWithFormat:@"%C",(unichar)i];
        predicate.predicateFormat = [NSString stringWithFormat:@"headerChar = '%@'",str];
        NSArray* result = [FriendModel lh_selectWithPredicate:predicate];
        if (result.count>0) {
            [dataArray addObject:result];
        }
    }
    predicate.predicateFormat = [NSString stringWithFormat:@"headerChar = '#'"];
    NSArray* array2 = [FriendModel lh_selectWithPredicate:predicate];
    if (array2.count>0) {
        [dataArray addObject:array2];
    }
    return dataArray;
}

#pragma mark- 从网络取数据
- (void)loadDataFromInter
{
    NSURL* url = [NSURL URLWithString:@"http://interface2.ejiandu.com/api/easemob/getFriends"];
    NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL:url];
    request.HTTPMethod = @"POST";
    request.HTTPBody = [[NSString stringWithFormat:@"{\"token\":\"403590f5-6b4c-42a3-9f3c-33322a930401\"}"] dataUsingEncoding:NSUTF8StringEncoding];
    NSURLSession* session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]];
    NSURLSessionDataTask* task = [session dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        if (!error) {
            dispatch_async(dispatch_get_global_queue(0, 0), ^{
                NSDictionary* dic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
                if (dic.count>0) {
                    [FriendModel lh_openDB];
                    [self importData:dic[@"Data"]];
                    self.dataSource = [self loadDataFromSQL];
                    [FriendModel lh_closeDB];
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [self.tableView reloadData];
                    });
                }
            });
        }
    }];
    [task resume];
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
