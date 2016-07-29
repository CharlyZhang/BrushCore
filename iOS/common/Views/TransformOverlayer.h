//
//  TransformOverlayer.h
//  HYDrawing
//
//  Created by CharlyZhang on 15/11/19.
//  Copyright © 2015年 Founder. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TransformOverlayer : UIControl <UIGestureRecognizerDelegate>

@property (nonatomic, readonly) CGAffineTransform alignedTransform;
@property (nonatomic, copy) void (^cancelBlock)(void);
@property (nonatomic, copy) void (^acceptBlock)(void);

- (instancetype) initWithFrame:(CGRect)frame title:(NSString*)t scaleFactor:(float)f;

- (CGAffineTransform) configureInitialPhotoTransform:(UIImage*) photo;

@end
