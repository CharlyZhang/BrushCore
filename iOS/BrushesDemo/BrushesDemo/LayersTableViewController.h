//
//  LayersTableViewController.h
//  BrushesDemo
//
//  Created by CharlyZhang on 15-7-8.
//  Copyright (c) 2015年 CharlyZhang. All rights reserved.
//

#import <UIKit/UIKit.h>

class CZPainting;

@interface LayersTableViewController : UITableViewController <UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, assign) CZPainting* painting;

@end
