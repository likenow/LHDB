//
//  GroupController.m
//  LHDB
//
//  Created by 李浩 on 16/8/10.
//  Copyright © 2016年 李浩. All rights reserved.
//

#import "GroupController.h"
#import "EGroupCell.h"
#import "GroupModel.h"
#import "LHDB.h"

@interface GroupController ()<UITableViewDataSource,UITableViewDelegate>

@property (nonatomic,strong) UITableView* tableView;

@property (nonatomic,strong) NSArray* dataSource;

@end

@implementation GroupController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"群组";
    [self.view addSubview:self.tableView];
    [GroupModel lh_openDB];
    [GroupModel lh_createTable];
    self.dataSource = [self loadDataFromSQL];
    [GroupModel lh_closeDB];
    [self.tableView reloadData];
    [self loadDataFromInter];
    // Do any additional setup after loading the view.
}

- (UITableView*)tableView
{
    if (!_tableView) {
        _tableView = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height) style:UITableViewStylePlain];
        _tableView.delegate = self;
        _tableView.dataSource = self;
        _tableView.rowHeight = 60;
    }
    return _tableView;
}


#pragma mark- UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.dataSource.count;
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    EGroupCell* cell = [tableView dequeueReusableCellWithIdentifier:@"cell"];
    if (!cell) {
        cell = [[EGroupCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"cell"];
    }
    cell.model = self.dataSource[indexPath.row];
    return cell;
}

#pragma mark- loadData
- (NSArray*)loadDataFromSQL
{
    return [GroupModel lh_selectWithPredicate:nil];
}

- (void)loadDataFromInter
{
    NSURL* url = [NSURL URLWithString:@"http://interface2.ejiandu.com/api/easemob/getGroups"];
    NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL:url];
    request.HTTPMethod = @"POST";
    request.HTTPBody = [[NSString stringWithFormat:@"{\"token\":\"7f427da6-6e6a-48ca-b0cc-9e6965d236dd\"}"] dataUsingEncoding:NSUTF8StringEncoding];
    NSURLSession* session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]];
    NSURLSessionDataTask* task = [session dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        if (!error) {
            dispatch_async(dispatch_get_global_queue(0, 0), ^{
                NSDictionary* dic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
                if (dic.count>0) {
                    [GroupModel lh_openDB];
                    [self importData:dic[@"Data"]];
                    self.dataSource = [self loadDataFromSQL];
                    [GroupModel lh_closeDB];
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [self.tableView reloadData];
                    });
                }
            });
        }
    }];
    [task resume];
}

- (void)importData:(NSArray*)array
{
    for (NSDictionary* dic in array) {
        NSMutableDictionary* saveDic = [NSMutableDictionary dictionaryWithDictionary:dic];
        [saveDic removeObjectForKey:@"newFlag"];
        LHPredicate* predicate = [LHPredicate predicateWithFormat:@"groupNo = '%@'",saveDic[@"groupNo"]];
        if ([GroupModel lh_selectWithPredicate:predicate].count>0) {
            [GroupModel lh_updateWith:saveDic predicate:predicate];
        }else
            [GroupModel lh_insertWith:saveDic];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
