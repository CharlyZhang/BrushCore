//
//  BrushSizePannelView.m
//  HYDrawing
//
//  Created by CharlyZhang on 15/12/7.
//  Copyright © 2015年 Founder. All rights reserved.
//

#import "BrushSizePannelView.h"

#define BackgroundColor [UIColor colorWithRed:255/255.0 green:250/255.0f blue:238/255.0f alpha:1.0]
#define BorderColor [UIColor colorWithRed:145/255.0 green:105/255.0 blue:33/255.0 alpha:1.0f]
#define ThemeColor [UIColor colorWithRed:255/255.f green:232/255.f blue:179/255.f alpha:1.f]
#define BorderWidth     2.f
#define BorderRadius    12.f

#define BUTTON_WIDTH_FACTOR     0.15f
#define BUTTON_HEIGHT_FACTOR    0.33f
#define SLIDER_HEIGHT_FACTOR    0.20f
#define MARK_OFFSET_FACTOR      0.12f
#define TRACK_RADIUS_FACTOR     0.50f

#define SLIDER_DEFAULT_MAX      100.0f
#define SLIDER_DEFAULT_MIN      1.0f
#define SLIDER_DEFAULT_VAL      0.8f

@interface BrushSizePannelView()
{
    CGRect thisRect;
}

@property (nonatomic,strong) UISlider *slider;
@property (nonatomic,strong) UIButton *increaseButton;
@property (nonatomic,strong) UIButton *decreaseButton;

@end

@implementation BrushSizePannelView


- (instancetype) initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame]) {
        thisRect = frame;
        [self addSubview:self.slider];
        [self addSubview:self.increaseButton];
        [self addSubview:self.decreaseButton];
        
        self.backgroundColor = BackgroundColor;
        self.layer.cornerRadius = BorderRadius;
        self.layer.borderWidth  = BorderWidth;
        self.layer.borderColor = BorderColor.CGColor;
        
        self.transform = CGAffineTransformMakeRotation( M_PI * - 0.5f );
    }
    
    return self;
}

#pragma mark Properties

- (UISlider*)slider
{
    if (!_slider) {
        
        CGSize size = thisRect.size;
        CGSize sliderSize = CGSizeMake(size.width * (1.0f - BUTTON_WIDTH_FACTOR*2), size.height*SLIDER_HEIGHT_FACTOR);
        CGSize markSize = CGSizeMake(size.height*BUTTON_HEIGHT_FACTOR, size.height*BUTTON_HEIGHT_FACTOR);
        CGFloat sliderOffsetX = size.width * BUTTON_WIDTH_FACTOR;
        CGFloat sliderOffsetY = size.height * (1.0f - SLIDER_HEIGHT_FACTOR) / 2.f;
        
        // track images
        UIImage *stetchLeftTrack= [self drawTrack: sliderSize];
        UIImage *stetchRightTrack = [self drawTrack: sliderSize];
        // mark image
        UIImage *thumbImage = [self drawMark:markSize];
        
        _slider=[[UISlider alloc]initWithFrame:CGRectMake(sliderOffsetX, sliderOffsetY, sliderSize.width , sliderSize.height)];
        _slider.backgroundColor = [UIColor clearColor];
        _slider.value = SLIDER_DEFAULT_VAL;
        _slider.minimumValue = SLIDER_DEFAULT_MIN;
        _slider.maximumValue = SLIDER_DEFAULT_MAX;
        
        [_slider setMinimumTrackImage:stetchLeftTrack forState:UIControlStateNormal];
        [_slider setMaximumTrackImage:stetchRightTrack forState:UIControlStateNormal];
        [_slider setThumbImage:thumbImage forState:UIControlStateHighlighted];
        [_slider setThumbImage:thumbImage forState:UIControlStateNormal];
        
        // actions
        [_slider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
    }
    
    return _slider;
}

- (UIButton*)increaseButton
{
    if (!_increaseButton) {
        CGSize size = thisRect.size;
        CGSize buttonSize = CGSizeMake(size.height*BUTTON_HEIGHT_FACTOR, size.height*BUTTON_HEIGHT_FACTOR);
        CGFloat buttonOffsetX = size.width * (1.f - BUTTON_WIDTH_FACTOR ) +  (size.width * BUTTON_WIDTH_FACTOR - buttonSize.width) / 2.f;
        CGFloat buttonOffsetY = size.height * (1.0f - BUTTON_HEIGHT_FACTOR) / 2.f;
        
        _increaseButton = [[UIButton alloc] initWithFrame:CGRectMake(buttonOffsetX, buttonOffsetY, buttonSize.width, buttonSize.height)];
        [_increaseButton setBackgroundImage:[self drawPlus:buttonSize] forState:UIControlStateNormal];
        [_increaseButton addTarget:self action:@selector(increaseAction) forControlEvents:UIControlEventTouchUpInside];
    }
    
    return _increaseButton;
}

- (UIButton*)decreaseButton
{
    if (!_decreaseButton) {
        CGSize size = thisRect.size;
        CGSize buttonSize = CGSizeMake(size.height*BUTTON_HEIGHT_FACTOR, size.height*BUTTON_HEIGHT_FACTOR);
        CGFloat buttonOffsetX = (size.width * BUTTON_WIDTH_FACTOR - buttonSize.width) / 2.f;
        CGFloat buttonOffsetY = size.height * (1.0f - BUTTON_HEIGHT_FACTOR) / 2.f;
        
        _decreaseButton = [[UIButton alloc] initWithFrame:CGRectMake(buttonOffsetX, buttonOffsetY, buttonSize.width, buttonSize.height)];
        [_decreaseButton setBackgroundImage:[self drawMinus:buttonSize] forState:UIControlStateNormal];
        [_decreaseButton addTarget:self action:@selector(decreaseAction) forControlEvents:UIControlEventTouchUpInside];
        
    }
    
    return _decreaseButton;
}

#pragma mark Private Methods

- (UIImage *)drawMark:(CGSize)size
{
    UIGraphicsBeginImageContextWithOptions(size, NO, 0);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    [[UIColor clearColor] setFill];
    CGRect rect = CGRectMake(0.0f, 0.0f, size.width, size.height);
    CGContextFillRect(ctx, rect);
    
    CGContextAddEllipseInRect(ctx, rect);
    CGContextSetFillColorWithColor(ctx, [UIColor grayColor].CGColor);
    CGContextFillPath(ctx);
    
    CGFloat offset = 1.f;
    rect.size = CGSizeMake(size.width - offset*2, size.height - offset*2);
    rect.origin = CGPointMake(offset, offset);
    CGContextAddEllipseInRect(ctx, rect);
    CGContextSetFillColorWithColor(ctx, [UIColor whiteColor].CGColor);
    CGContextFillPath(ctx);
    
    offset = size.width * MARK_OFFSET_FACTOR;
    rect.size = CGSizeMake(size.width - offset*2, size.height - offset*2);
    rect.origin = CGPointMake(offset, offset);
    CGContextAddEllipseInRect(ctx, rect);
    CGContextSetFillColorWithColor(ctx, [UIColor grayColor].CGColor);
    CGContextFillPath(ctx);
    
    offset = size.width * MARK_OFFSET_FACTOR * 2;
    rect.size = CGSizeMake(size.width - offset*2, size.height - offset*2);
    rect.origin = CGPointMake(offset, offset);
    CGContextAddEllipseInRect(ctx, rect);
    CGContextSetFillColorWithColor(ctx, [UIColor whiteColor].CGColor);
    CGContextFillPath(ctx);
    
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

- (UIImage*)drawTrack:(CGSize) size
{
    UIGraphicsBeginImageContextWithOptions(size, NO, 0);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    [[UIColor clearColor] setFill];
    CGRect rect = CGRectMake(0.0f, 0.0f, size.width, size.height);
    CGContextFillRect(ctx, rect);
    
    CGFloat radius = size.width < size.height? size.width * TRACK_RADIUS_FACTOR : size.height * TRACK_RADIUS_FACTOR;
    
    CGContextMoveToPoint(ctx, size.width, size.height-radius);
    CGContextAddArcToPoint(ctx, size.width, size.height, size.width-radius, size.height, radius);
    CGContextAddArcToPoint(ctx, 0, size.height, 0, size.height-radius, radius);
    CGContextAddArcToPoint(ctx, 0, 0, size.width-radius, 0, radius);
    CGContextAddArcToPoint(ctx, size.width, 0, size.width, size.height-radius, radius);
    
    CGContextSetFillColorWithColor(ctx, ThemeColor.CGColor);
    CGContextDrawPath(ctx, kCGPathFillStroke);
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    image = [image resizableImageWithCapInsets:UIEdgeInsetsMake(radius, radius, radius, radius)];
    return image;
}

- (UIImage*)drawMinus:(CGSize)size
{
    UIGraphicsBeginImageContextWithOptions(size, NO, 0);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    [[UIColor clearColor] setFill];
    CGContextFillRect(ctx, CGRectMake(0.0f, 0.0f, size.width, size.height));
    
    CGContextSetLineWidth(ctx, 2.0);
    CGContextSetStrokeColorWithColor(ctx, [UIColor grayColor].CGColor);
    CGContextMoveToPoint(ctx, size.width /2.f, 1.f);
    CGContextAddLineToPoint(ctx, size.width /2.f, size.height - 1.f);
    CGContextStrokePath(ctx);
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

- (UIImage*)drawPlus:(CGSize)size
{
    UIGraphicsBeginImageContextWithOptions(size, NO, 0);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    [[UIColor clearColor] setFill];
    CGContextFillRect(ctx, CGRectMake(0.0f, 0.0f, size.width, size.height));
    
    CGContextSetLineWidth(ctx, 2.0);
    CGContextSetStrokeColorWithColor(ctx, [UIColor grayColor].CGColor);
    CGContextMoveToPoint(ctx, 1.f, size.height / 2.f);
    CGContextAddLineToPoint(ctx, size.width - 1.f, size.height / 2.f);
    CGContextMoveToPoint(ctx, size.width /2.f, 1.f);
    CGContextAddLineToPoint(ctx, size.width /2.f, size.height - 1.f);
    CGContextStrokePath(ctx);
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

#pragma mark Actions

- (void)increaseAction
{
    if (self.slider.value < self.slider.maximumValue) {
        float delta = (self.slider.maximumValue - self.slider.minimumValue) / 100.f;
        [self.slider setValue:self.slider.value + delta animated:YES];
    }
}

- (void)decreaseAction
{
    if (self.slider.value > self.slider.minimumValue) {
        float delta = (self.slider.maximumValue - self.slider.minimumValue) / 100.f;
        [self.slider setValue:self.slider.value - delta animated:YES];
    }
}

- (void)sliderValueChanged:(UISlider*)slider
{
    [self.delegate brushSizePannelView:self valueChanged:slider.value];
}

#pragma mark Pulic methods

- (void)setBrushSize:(float)value
{
    if (value <= SLIDER_DEFAULT_MAX && value >= SLIDER_DEFAULT_MIN) {
        [self.slider setValue:value animated:YES];
    }
}

@end
