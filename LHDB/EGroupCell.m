//
//  EGroupCell.m
//  eproject
//
//  Created by 3wchina01 on 16/5/12.
//  Copyright © 2016年 com.ejiandu. All rights reserved.
//

#import "EGroupCell.h"
#import "GroupModel.h"
#import "Masonry.h"
#import "UIImageView+WebCache.h"


@implementation EGroupCell{
    UIImageView* headerView;
    UILabel* nameLabel;
}

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        [self subViewInit];
    }
    return self;
}

- (void)subViewInit
{
    headerView = [[UIImageView alloc] init];
    headerView.userInteractionEnabled = YES;
    
    [self.contentView addSubview:headerView];
    
    nameLabel = [[UILabel alloc] init];
    nameLabel.textColor = [UIColor colorWithWhite:0.1 alpha:1];
    nameLabel.font = [UIFont systemFontOfSize:15];
    [self.contentView addSubview:nameLabel];
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    UIView* superView = self.contentView;
    
    [headerView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.equalTo(superView.mas_top).offset(5);
        make.left.equalTo(superView.mas_left).offset(5);
        make.bottom.equalTo(superView.mas_bottom).offset(-5);
        make.width.equalTo(headerView.mas_height);
    }];
    
    [nameLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.equalTo(headerView.mas_top).offset(2);
        make.left.equalTo(headerView.mas_right).offset(5);
        make.bottom.equalTo(headerView.mas_bottom).offset(-5);
        make.right.equalTo(superView.mas_right).offset(-50);
    }];
}

- (void)setModel:(GroupModel *)model
{
    _model = model;
    [headerView sd_setImageWithURL:[NSURL URLWithString:model.photoPath] placeholderImage:[UIImage imageNamed:@"EaseUIResource.bundle/group"]];
    nameLabel.text = model.groupName;
}

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
