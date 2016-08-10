//
//  EGroupCell.h
//  eproject
//
//  Created by 3wchina01 on 16/5/12.
//  Copyright © 2016年 com.ejiandu. All rights reserved.
//

#import <UIKit/UIKit.h>

@class GroupModel;
@interface EGroupCell : UITableViewCell

@property (nonatomic,strong) GroupModel* model;

@property (nonatomic,copy) void(^callback)(GroupModel* model);

@end
