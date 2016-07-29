//
//  BrushSizePannelView.h
//  HYDrawing
//
//  Created by CharlyZhang on 15/12/7.
//  Copyright © 2015年 Founder. All rights reserved.
//

#import <UIKit/UIKit.h>

@class BrushSizePannelView;

@protocol BrushSizePannelViewDelegate <NSObject>

@required
- (void)brushSizePannelView:(BrushSizePannelView*)brushSizePannelView valueChanged:(float)value;

@end

@interface BrushSizePannelView : UIView

@property (nonatomic, weak) id<BrushSizePannelViewDelegate> delegate;

- (void)setBrushSize:(float)value;

@end
