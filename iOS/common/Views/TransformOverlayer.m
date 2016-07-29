//
//  TransformOverlayer.m
//  HYDrawing
//
//  Created by CharlyZhang on 15/11/19.
//  Copyright © 2015年 Founder. All rights reserved.
//

#import "TransformOverlayer.h"
#import "HYBrushCore.h"
#import "ViewDefine.h"

#define NAVBAR_HEIGHT       64
#define NAVBAR_BTN_HEIGHT   30
#define NAVBAR_BTN_WIDTH    60

@interface TransformOverlayer()

@property (nonatomic, strong) UINavigationBar* navigationBar;

@end

@implementation TransformOverlayer
{
    CGAffineTransform transform_;
    
    float initialScale;
    float scaleFactor;      ///< equals screenScale / canvasScale
    NSString *title;
}

@synthesize alignedTransform = transform_;

- (instancetype) initWithFrame:(CGRect)frame title:(NSString*)t scaleFactor:(float)f
{
    if (self = [super initWithFrame:frame]) {
        
        self.opaque = NO;
        self.backgroundColor = [UIColor clearColor];
        self.userInteractionEnabled = YES;
        
        scaleFactor = f;
        title = t;
        
        // Add Gestures
        UIPanGestureRecognizer *pan = [[UIPanGestureRecognizer alloc]initWithTarget:self action:@selector(handlePanGesture:)];
        pan.delegate = self;
        [self addGestureRecognizer:pan];
        
        UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc]initWithTarget:self action:@selector(handlePinchGesture:)];
        pinch.delegate = self;
        [self addGestureRecognizer:pinch];
        
        UIRotationGestureRecognizer *rotation = [[UIRotationGestureRecognizer alloc]initWithTarget:self action:@selector(handleRotationGesture:)];
        rotation.delegate = self;
        [self addGestureRecognizer:rotation];
        
        UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(acceptTransform)];
        [self addGestureRecognizer:tapGesture];
        tapGesture.delegate = self;
        tapGesture.numberOfTapsRequired = 2;
        
        
        transform_ = CGAffineTransformIdentity;
    
        [self addSubview:self.navigationBar];
        
    }
    
    return self;
}

- (CGAffineTransform) configureInitialPhotoTransform:(UIImage*) photo
{
    CGSize      paintingSize = [[HYBrushCore sharedInstance]getPaintingSize];
    CGSize      photoSize = photo.size;
    CGPoint     paintingCenter = CGPointMake(paintingSize.width / 2, paintingSize.height / 2);
    CGPoint     photoCenter = CGPointMake(photoSize.width / 2, photoSize.height / 2);
    
    CGPoint offset = CGPointMake(paintingCenter.x - photoCenter.x, paintingCenter.y - photoCenter.y);
    transform_ = CGAffineTransformIdentity;
    transform_ = CGAffineTransformTranslate(transform_, offset.x, offset.y);
    
    return transform_;
}

#pragma mark Properties

- (UINavigationBar *)navigationBar {
    if (!_navigationBar) {
        CGRect frame = self.bounds;
        frame.size.height = NAVBAR_HEIGHT;
        _navigationBar = [[UINavigationBar alloc] initWithFrame:frame];
        _navigationBar.barStyle = UIBarStyleDefault;
        _navigationBar.translucent = YES;
        _navigationBar.autoresizingMask = UIViewAutoresizingFlexibleWidth;
        _navigationBar.barTintColor = kBackgroundColor;
        
        UINavigationItem *navItem = [[UINavigationItem alloc] initWithTitle:title];
        
        UIButton *cancelButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [cancelButton addTarget:self action:@selector(cancelTransform) forControlEvents:UIControlEventTouchUpInside];
        [cancelButton setFrame:CGRectMake(0, 0, NAVBAR_BTN_WIDTH, NAVBAR_BTN_HEIGHT)];
        [cancelButton setBackgroundColor:[UIColor whiteColor]];
        cancelButton.titleLabel.font = kTopBarButtonFont;
        [cancelButton setTitle:NSLocalizedString(@"取消", @"Cancel") forState:UIControlStateNormal];
        [cancelButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        cancelButton.layer.borderColor = kBorderColor.CGColor;
        cancelButton.layer.cornerRadius = 4;
        cancelButton.layer.borderWidth = 2;
        
        UIBarButtonItem *cancel = [[UIBarButtonItem alloc]initWithCustomView:cancelButton];
        
        UIButton *acceptButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [acceptButton addTarget:self action:@selector(acceptTransform) forControlEvents:UIControlEventTouchUpInside];
        [acceptButton setFrame:CGRectMake(0, 0, NAVBAR_BTN_WIDTH, NAVBAR_BTN_HEIGHT)];
        [acceptButton setBackgroundColor:[UIColor whiteColor]];
        acceptButton.titleLabel.font = kTopBarButtonFont;
        [acceptButton setTitle:NSLocalizedString(@"确定", @"Accept") forState:UIControlStateNormal];
        [acceptButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        acceptButton.layer.borderColor = kBorderColor.CGColor;
        acceptButton.layer.cornerRadius = 4;
        acceptButton.layer.borderWidth = 2;
        
        UIBarButtonItem *accept = [[UIBarButtonItem alloc]initWithCustomView:acceptButton];
        
        navItem.leftBarButtonItem = cancel;
        navItem.rightBarButtonItem = accept;
        
        [_navigationBar pushNavigationItem:navItem animated:NO];
    }
    
    return _navigationBar;
}

#pragma mark Actions

-(void)acceptTransform {
    if(self.acceptBlock)    self.acceptBlock();
}

-(void)cancelTransform {
    if(self.cancelBlock)    self.cancelBlock();
}

#pragma mark Private

- (void) updateTransform:(id)obj {
    [self sendActionsForControlEvents:UIControlEventValueChanged];
}

- (void) delayedUpdateTransform {
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [self performSelector:@selector(updateTransform:) withObject:nil afterDelay:0.0f];
}

#pragma mark - Gestures

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer {
    return YES;
}

-(void)handlePanGesture:(UIPanGestureRecognizer*)pan {
    if (pan.state == UIGestureRecognizerStateChanged) {
        CGPoint loc = [pan translationInView:self];
        [pan setTranslation:CGPointZero inView:self];
        loc = CGPointMake(loc.x * scaleFactor, loc.y * scaleFactor);
        
        CGAffineTransform tX = CGAffineTransformIdentity;
        tX = CGAffineTransformTranslate(tX, loc.x, loc.y);
        transform_ = CGAffineTransformConcat(transform_, tX);
        
        [self delayedUpdateTransform];
    }
}

-(void)handlePinchGesture:(UIPinchGestureRecognizer*)pinch {
    if (pinch.state == UIGestureRecognizerStateBegan) {
        initialScale = [(UIPinchGestureRecognizer *)pinch scale];
        return;
    }
    
    float scale = pinch.scale / initialScale;
    
    if (pinch.state == UIGestureRecognizerStateChanged) {
        CGPoint pivot = [pinch locationInView:self];
        pivot.y = self.bounds.size.height - pivot.y;
        pivot = [[HYBrushCore sharedInstance] convertToPainting:pivot];
        
        CGAffineTransform tX = CGAffineTransformIdentity;
        
        tX = CGAffineTransformIdentity;
        tX = CGAffineTransformTranslate(tX, pivot.x, pivot.y);
        tX = CGAffineTransformScale(tX, scale, scale);
        tX = CGAffineTransformTranslate(tX, -pivot.x, -pivot.y);
        
        transform_ = CGAffineTransformConcat(transform_, tX);
        
        initialScale = pinch.scale;
        
        [self delayedUpdateTransform];
    }
}

-(void)handleRotationGesture:(UIRotationGestureRecognizer*)rotation {
    if (rotation.state == UIGestureRecognizerStateChanged) {
        
        CGFloat angle = rotation.rotation;

        CGPoint pivot = [rotation locationInView:self];
        pivot.y = self.bounds.size.height - pivot.y;
        pivot = [[HYBrushCore sharedInstance] convertToPainting:pivot];
        
        CGAffineTransform tX = CGAffineTransformIdentity;
        tX = CGAffineTransformTranslate(tX, pivot.x, pivot.y);
        tX = CGAffineTransformRotate(tX, angle);
        tX = CGAffineTransformTranslate(tX, -pivot.x, -pivot.y);
        transform_ = CGAffineTransformConcat(transform_, tX);
        
        rotation.rotation = 0;
    
        [self delayedUpdateTransform];
    }
}


@end
