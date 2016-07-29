//
//  CanvasView.m
//  HYDrawing
//
//  Created by CharlyZhang on 15/8/15.
//  Copyright (c) 2015年 Founder. All rights reserved.
//

#import "CanvasView.h"
#include "BrushesCore.h"

#define kMaxZoom 16

NSString *CZActivePaintColorDidChange = @"CZActivePaintColorDidChange";
NSString *CZCanvasDirtyNotification = @"CZCanvasDirtyNotification";

////////////////////CanvasView////////////////////
@interface CanvasView()
{
    CZMat4 projMat;
    EAGLContext *context;               ///
    BOOL isBarVisible;
    CZCanvas *ptrCanvas;
    
    /// canvas transform related
    NSInteger               lastTouchCount;
    float                   previousScale;
}

@property (nonatomic, assign) CZFbo* fbo;
@property (nonatomic, assign) CZPainting* ptrPainting;
@property (nonatomic, assign) float  trueCanvasScale;       ///< used for continuous scaling

@end

@implementation CanvasView

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (!self) {
        return nil;
    }
    
    self.multipleTouchEnabled = YES;
    self.contentMode = UIViewContentModeCenter;
    self.contentScaleFactor = [UIScreen mainScreen].scale;
    self.autoresizingMask = UIViewAutoresizingNone;
    self.exclusiveTouch = YES;
    
    // configure the layer
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    
    eaglLayer.opaque = NO;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8,
                                    kEAGLDrawablePropertyColorFormat, nil];
    
    CGSize size = frame.size;
    projMat.SetOrtho(0,size.width, 0, size.height, -1.0f, 1.0f);
    
    glEnable(GL_TEXTURE_2D);        /// will emit gl error, but cause showing nothing if be deleted
    CZCheckGLError();
    glActiveTexture(GL_TEXTURE0);
    
    [self configureGestrues];
    
    isBarVisible = YES;
    
    _trueCanvasScale = 1.f;         /// can't set by self.trueCanvasScale when ptrCanvas == null

    ptrCanvas = nullptr;
    
    return self;
}

- (void)dealloc
{
    [EAGLContext setCurrentContext:context];
    delete _fbo;
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void) layoutSubviews
{
    CGSize size = self.bounds.size;
    projMat.SetOrtho(0,size.width, 0, size.height, -1.0f, 1.0f);
    [self drawView];
}

#pragma mark - Properties

- (CZFbo*)fbo
{
    if (!_fbo && context) {
        [EAGLContext setCurrentContext:context];
        _fbo = new CZFbo;
        _fbo->setRenderBufferWithContext((__bridge void*)context, (__bridge void*)self.layer);
    }
    
    return _fbo;
}

- (void)setPtrPainting:(CZPainting *)ptrPainting
{
    if (ptrPainting) {
        context = (__bridge EAGLContext*) ptrPainting->getGLContext()->getRealContext();
    }
    else {
        context = nil;
    }
    
    _ptrPainting = ptrPainting;
    
}

- (void)setCanvas:(void *)canvas
{
    if (canvas == nullptr) {
        LOG_INFO("param 'canvas' is NULL\n");
        return;
    }
    
    ptrCanvas = (CZCanvas*)canvas;
}

- (void) setTrueCanvasScale:(float)scale
{
    _trueCanvasScale = scale;
    
    if(ptrCanvas == nullptr)
    {
        LOG_ERROR("ptrCanvas is null \n");
        return;
    }
    
    if (_trueCanvasScale > 0.95f && _trueCanvasScale < 1.05) {
        ptrCanvas->setScale(1.0f);
    }
    else {
        ptrCanvas->setScale(_trueCanvasScale);
    }
}

#pragma mark - Geusture
- (void)configureGestrues
{
    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc]initWithTarget:self action:@selector(handlePanGesture:)];
    panGesture.maximumNumberOfTouches = 1;
    panGesture.delegate = self;
    [self addGestureRecognizer:panGesture];
    
    UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(handleTapGesture:)];
    tapGesture.numberOfTapsRequired = 1;
    tapGesture.numberOfTouchesRequired = 1;
    [self addGestureRecognizer:tapGesture];
    
    UITapGestureRecognizer *doubleTapGesture = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(handleDoubleTapGesture:)];
    doubleTapGesture.numberOfTapsRequired = 2;
    doubleTapGesture.numberOfTouchesRequired = 1;
    [self addGestureRecognizer:doubleTapGesture];
    
    [tapGesture requireGestureRecognizerToFail:doubleTapGesture];
    
    UITapGestureRecognizer *double2TapGesture = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(handleDouble2TapGesture:)];
    double2TapGesture.numberOfTapsRequired = 2;
    double2TapGesture.numberOfTouchesRequired = 2;
    [self addGestureRecognizer:double2TapGesture];
    
    // Create a pinch recognizer to scale the canvas
    UIPinchGestureRecognizer *pinchGesture = [[UIPinchGestureRecognizer alloc]
                                              initWithTarget:self action:@selector(handlePinchGesture:)];
    pinchGesture.delegate = self;
    [self addGestureRecognizer:pinchGesture];

}

- (void) handlePinchGesture:(UIPinchGestureRecognizer *)sender
{
    if(ptrCanvas == nullptr)
    {
        LOG_ERROR("ptrCanvas is null \n");
        return;
    }
    
    if (sender.state == UIGestureRecognizerStateBegan) {
//        [controller_ hidePopovers];
        
        CGPoint flipped = [sender locationInView:self];
        
        CGFloat height = self.bounds.size.height;
        CZ2DPoint pt(flipped.x,height-flipped.y);
        ptrCanvas->pinchBegin(pt);
        
        lastTouchCount = sender.numberOfTouches;
                
//        self.isZooming = YES;
//        self.interfaceWasHidden = self.controller.interfaceHidden;
//        self.painting.flattenMode = !(self.controller.replay.isPlaying);
        
//        [self.controller hideInterface];
    }
    else if (sender.state == UIGestureRecognizerStateChanged) {
       
        CGPoint flipped = [sender locationInView:self];
        
        CGFloat height = self.bounds.size.height;
        CZ2DPoint pt(flipped.x,height-flipped.y);
        
        if (sender.numberOfTouches != lastTouchCount){
            ptrCanvas->pinchChanged(pt,true);
            lastTouchCount = sender.numberOfTouches;
        }
        else
            ptrCanvas->pinchChanged(pt);
        
        [self scaleBy:sender.scale / previousScale];
        
    }
    else if (sender.state == UIGestureRecognizerStateEnded) {
//        self.isZooming = NO;
//        [self nixMessageLabel];
//        
//        if (!self.interfaceWasHidden) {
//            [self.controller showInterface];
//        }
//        
//        self.painting.flattenMode = NO;
    }
    
    previousScale = sender.scale;
}

- (void)handlePanGesture:(UIPanGestureRecognizer*)sender
{
    LOG_DEBUG("pan\n");
    
    if(ptrCanvas == nullptr)
    {
        LOG_ERROR("ptrCanvas is null \n");
        return;
    }
    
    CZActiveState *activeState = CZActiveState::getInstance();
    
    if (activeState->colorFillMode || activeState->colorPickMode) return;
    
    if (self.ptrPainting->shouldPreventPaint()) {
        CZLayer *layer = self.ptrPainting->getActiveLayer();
        if (sender.state == UIGestureRecognizerStateBegan || sender.state == UIGestureRecognizerStateChanged) {
            if (!layer->isLocked())          [self.delegate showMessageView:kInvisible];
            else if(layer->isVisible())      [self.delegate showMessageView:kLocked];
            else                             [self.delegate showMessageView:kInvisibleAndLocked];
        }
        else {
            [self.delegate dismissMessageView];
        }

        return;
    }
    
    if (ptrCanvas == nullptr) {
        LOG_ERROR("ptrCanvas is null\n");
        return ;
    }
    
    CGPoint p = [sender locationInView:sender.view];
    CGFloat height = self.bounds.size.height;
    CZ2DPoint pt(p.x,height-p.y);
    pt = ptrCanvas->transformToPainting(pt);
    
    if (sender.state == UIGestureRecognizerStateBegan) {
        activeState->getActiveTool()->moveBegin(pt);
    }
    else if (sender.state == UIGestureRecognizerStateChanged){
        CGPoint velocity = [sender velocityInView:sender.view];
        CZ2DPoint zeroPoint; CZ2DPoint v(velocity.x,velocity.y);
        float   speed = zeroPoint.distanceTo2DPoint(v) / 1000.0f; // pixels/millisecond
        //LOG_DEBUG("speed is %f\n", speed);
        activeState->getActiveTool()->moving(pt, speed);
    }
    else if (sender.state == UIGestureRecognizerStateEnded){
        activeState->getActiveTool()->moveEnd(pt);
        [[NSNotificationCenter defaultCenter] postNotificationName:CZCanvasDirtyNotification object:nil];
    }
    else if (sender.state == UIGestureRecognizerStateCancelled) {
        LOG_DEBUG("gesture canceled!\n");
    }
}

- (void)handleTapGesture:(UITapGestureRecognizer*)sender
{
    LOG_DEBUG("tap\n");
    
    if(ptrCanvas == nullptr)
    {
        LOG_ERROR("ptrCanvas is null \n");
        return;
    }
    
    if (self.ptrPainting->shouldPreventPaint()) {
        CZLayer *layer = self.ptrPainting->getActiveLayer();
        if (!layer->isLocked())          [self.delegate showMessageView:kInvisible];
        else if(layer->isVisible())      [self.delegate showMessageView:kLocked];
        else                             [self.delegate showMessageView:kInvisibleAndLocked];
        if (sender.state == UIGestureRecognizerStateEnded) [self.delegate dismissMessageView];
        return;
    }
    
    if (ptrCanvas == nullptr) {
        LOG_ERROR("ptrCanvas is null\n");
        return ;
    }
    
    CGPoint p = [sender locationInView:sender.view];
    CGFloat height = self.bounds.size.height;
    CZ2DPoint pt(p.x,height-p.y);
    pt = ptrCanvas->transformToPainting(pt);
    
    CZActiveState *activeState = CZActiveState::getInstance();
    
    if (activeState->colorFillMode) {
        CZLayer *layer = self.ptrPainting->getActiveLayer();
        CZColor color = activeState->getPaintColor();
        layer->fill(color, pt);     /// TO DO:
        [self drawView];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:CZCanvasDirtyNotification object:nil];
    }
    else if (activeState->colorPickMode) {
        CZColor pickedColor = self.ptrPainting->pickColor(pt.x, pt.y);
        
        UIColor *pColor = [UIColor colorWithRed:pickedColor.red green:pickedColor.green blue:pickedColor.blue alpha:pickedColor.alpha];
        activeState->setPaintColor(pickedColor);
        NSDictionary *userInfo = @{@"pickedColor": pColor};
        [[NSNotificationCenter defaultCenter] postNotificationName:CZActivePaintColorDidChange object:nil userInfo:userInfo];
    }
    else {
        activeState->getActiveTool()->moveBegin(pt);
        activeState->getActiveTool()->moveEnd(pt);
        
        [[NSNotificationCenter defaultCenter] postNotificationName:CZCanvasDirtyNotification object:nil];
    }
    
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer*)sender
{
    LOG_DEBUG("double tap\n");
    isBarVisible = !isBarVisible;
    [self.delegate displayToolView:isBarVisible];
}

- (void)handleDouble2TapGesture:(UITapGestureRecognizer*)sender
{
    LOG_DEBUG("double 2 tap\n");
    self.trueCanvasScale = 1.f;
    if(ptrCanvas) ptrCanvas->resetTransform();
}

- (void)setPainting:(void*)painting
{
    self.ptrPainting = (CZPainting*)painting;
}

#pragma mark - Draw

- (void) drawView
{
    if (!self.ptrPainting) {
        LOG_ERROR("ptrPainting is NULL!]\n");
        return ;
    }
    
    if (!ptrCanvas) {
        LOG_ERROR("ptrCanvas is null\n");
        return ;
    }
    
    [EAGLContext setCurrentContext:context];
    self.fbo->begin();
    
    ptrCanvas->draw();
    
    glBindRenderbuffer(GL_RENDERBUFFER, self.fbo->getRenderBufferId());
    [context presentRenderbuffer:GL_RENDERBUFFER];
    LOG_DEBUG("drawView\n");
}


#pragma mark Private Method

- (void) scaleBy:(double)scale
{
    if (!self.ptrPainting) {
        LOG_ERROR("ptrPainting is NULL!]\n");
        return ;
    }
    
    float minZoom = [self minimumZoom];
    float canvasScale = ptrCanvas->getScale();
    
    if (scale * canvasScale > kMaxZoom) {
        scale = kMaxZoom / scale;
    } else if (scale * canvasScale < minZoom) {
        scale = minZoom / canvasScale;
    }
    
    float finalScale = self.trueCanvasScale * scale;
    self.trueCanvasScale = finalScale;
    ptrCanvas->rebuildViewTransformAndRedraw(true);
}


- (float) minimumZoom
{
    CZSize paintingSize = self.ptrPainting->getDimensions();
    float maxDimension = MAX(paintingSize.width, paintingSize.height);
    float minBounds = MIN(CGRectGetWidth([UIScreen mainScreen].bounds), CGRectGetHeight([UIScreen mainScreen].bounds));
    
    /// to ensure the maxDimension of Painting occupy at least half of the minBound of Screen
    float scale = ((minBounds / 2.0f) / maxDimension) * [UIScreen mainScreen].scale;
    
    return MIN(scale, 1.0);
}

@end

