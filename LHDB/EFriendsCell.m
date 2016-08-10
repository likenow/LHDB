//
//  EFriendsCell.m
//  eproject
//
//  Created by 3wchina01 on 16/5/11.
//  Copyright © 2016年 com.ejiandu. All rights reserved.
//

#import "EFriendsCell.h"
#import "FriendModel.h"
#import "Masonry.h"
#import "UIImageView+WebCache.h"

@implementation EFriendsCell{
    UIImageView* headImage;
    UILabel* nameLabel;
    UILabel* detailLabel;
    UILabel* roleLabel;
}

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        [self subviewsInit];
        self.selectionStyle = UITableViewCellSelectionStyleNone;
    }
    return self;
}

- (void)subviewsInit
{
    headImage = [[UIImageView alloc] init];
    [self.contentView addSubview:headImage];
    
    nameLabel = [[UILabel alloc] init];
    nameLabel.font = [UIFont systemFontOfSize:15];
    [self.contentView addSubview:nameLabel];
    
    detailLabel = [[UILabel alloc] init];
    detailLabel.textColor = [UIColor colorWithWhite:0.6 alpha:1];
    detailLabel.textAlignment = NSTextAlignmentRight;
    detailLabel.font = [UIFont systemFontOfSize:14];
    [self.contentView addSubview:detailLabel];
    
    roleLabel = [[UILabel alloc] init];
    roleLabel.textColor = [UIColor colorWithWhite:0.6 alpha:1];
    roleLabel.font = [UIFont systemFontOfSize:14];
    roleLabel.textAlignment = NSTextAlignmentRight;
    [self.contentView addSubview:roleLabel];
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    UIView* superView = self.contentView;
    [headImage mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.equalTo(superView.mas_top).offset(5);
        make.left.equalTo(superView.mas_left).offset(5);
        make.bottom.equalTo(superView.mas_bottom).offset(-5);
        make.right.equalTo(nameLabel.mas_left).offset(-5);
        make.width.equalTo(headImage.mas_height);
    }];
    
    [nameLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.equalTo(headImage.mas_top).offset(5);
        make.left.equalTo(headImage.mas_right).offset(5);
        make.bottom.equalTo(headImage.mas_bottom).offset(-5);
        make.right.equalTo(headImage.mas_right).offset(140);
    }];
    
    [detailLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.equalTo(roleLabel.mas_bottom).offset(2);
        make.left.equalTo(headImage.mas_right).offset(20);
        make.bottom.equalTo(superView.mas_bottom).offset(-2);
        make.right.equalTo(superView.mas_right).offset(-10);
    }];
    
    [roleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.equalTo(superView.mas_top).offset(2);
        make.right.equalTo(superView.mas_right).offset(-10);
        make.bottom.equalTo(detailLabel.mas_top).offset(-2);
        make.left.equalTo(headImage.mas_right).offset(30);
        make.height.equalTo(@[detailLabel]);
    }];
}

- (void)setModel:(FriendModel *)model
{
    _model = model;
    [headImage sd_setImageWithURL:[NSURL URLWithString:model.photoPath] placeholderImage:[UIImage imageNamed:@"zc_tx"]];
    nameLabel.text = model.nickName;
    detailLabel.text = model.unitName;
//    detailLabel.text =
    NSString* text;
    if (!model.deptName) {
        model.deptName = @"";
    }
    roleLabel.textAlignment = NSTextAlignmentRight;
    if (model.roleName) {
        text = [NSString stringWithFormat:@"%@   %@",model.deptName,model.roleName];
    }else{
        text = [NSString stringWithFormat:@"   %@ ",model.deptName];
    }
    if (!model.deptName&&!model.roleName) {
        text = nil;
    }
    roleLabel.text = text;
}

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
