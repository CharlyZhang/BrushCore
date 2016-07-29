//
//  CanvasView.h
//  HYDrawing
//
//  oc class  which use funcitons of cpp
//
//  Created by CharlyZhang on 15/8/15.
//  Copyright (c) 2015年 Founder. All rights reserved.
//

#import <UIKit/UIKit.h>

/// 显示的遮罩消息类型
typedef enum ShowingMessageType {
    kInvisible = 0,
    kLocked,
    kInvisibleAndLocked
} ShowingMessageType;

@protocol CanvasViewDelegate <NSObject>

@required
- (void) showMessageView:(ShowingMessageType)msgType;
- (void) dismissMessageView;
- (void) displayToolView:(BOOL) flag;

@end

@interface CanvasView : UIView<UIGestureRecognizerDelegate>

@property (nonatomic, weak) id<CanvasViewDelegate> delegate;
@property (nonatomic, assign) void* canvas;

- (void)drawView;
- (void)setPainting:(void*)painting;

@end

