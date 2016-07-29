//
//  WDColorPickerController.m
//  Brushes
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//  Copyright (c) 2011-2013 Steve Sprang
//

#import "WDColor.h"
#import "WDColorComparator.h"
#import "WDColorPickerController.h"
#import "WDColorSlider.h"
#import "WDColorSquare.h"
#import "WDColorWheel.h"
#import "WDUtilities.h"
#import "HYBrushCore.h"

@implementation WDColorPickerController

@synthesize color = color_;
@synthesize colorComparator = colorComparator_;
@synthesize colorSquare = colorSquare_;
@synthesize colorWheel = colorWheel_;
@synthesize swatches = swatches_;
@synthesize alphaSlider = alphaSlider_;
@synthesize delegate;

- (IBAction)dismiss:(id)sender
{
    [[self presentingViewController] dismissViewControllerAnimated:NO completion:nil];
}

- (void) doubleTapped:(id)sender
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(dismissViewController:)]) {
        [self.delegate performSelector:@selector(dismissViewController:) withObject:self];
    }
}

- (void) setActiveStateSwatchColor:(WDColor*)color atIndex:(NSUInteger)index
{
    [[HYBrushCore sharedInstance]setActiveStateSwatchColor:[color UIColor]atIndex:index];
}

- (void) setActiveStatePaintColorAtIndex:(NSUInteger)index
{
    [[HYBrushCore sharedInstance] setActiveStatePaintColorAtIndex:index];
}

- (WDColor*) getColorFromActiveStateSwatchAtIndex:(NSUInteger)index
{
    UIColor *color = [[HYBrushCore sharedInstance]getColorFromActiveStateSwatchAtIndex:index];
    CGFloat r,g,b,a;
    [color getRed:&r green:&g blue:&b alpha:&a];
    return [WDColor colorWithRed:r green:g blue:b alpha:a];
}

- (void) takeColorFromComparator:(id)sender
{
    [self setColor:(WDColor *) [sender color]];
}

- (void) takeHueFrom:(id)sender
{
    float hue = [(WDColorWheel *)sender hue];
    WDColor *newColor = [WDColor colorWithHue:hue
                                   saturation:[color_ saturation]
                                   brightness:[color_ brightness]
                                        alpha:[color_ alpha]];
    
    [self setColor:newColor];
}

- (void) takeBrightnessAndSaturationFrom:(id)sender
{
    float saturation = [(WDColorSquare *)sender saturation];
    float brightness = [(WDColorSquare *)sender brightness];
    
    WDColor *newColor = [WDColor colorWithHue:[color_ hue]
                                   saturation:saturation
                                   brightness:brightness
                                        alpha:[color_ alpha]];
    
    [self setColor:newColor];
}

- (void) takeAlphaFrom:(WDColorSlider *)slider
{
    float alpha = slider.floatValue;
    
    WDColor *newColor = [WDColor colorWithHue:[color_ hue]
                                   saturation:[color_ saturation]
                                   brightness:[color_ brightness]
                                        alpha:alpha];
    [self setColor:newColor];
}

- (void) takeBrightnessFrom:(WDColorSlider *)slider
{
    float brightness = slider.floatValue;
    
    WDColor *newColor = [WDColor colorWithHue:[color_ hue]
                                   saturation:[color_ saturation]
                                   brightness:brightness
                                        alpha:[color_ alpha]];
    
    [self setColor:newColor];
}

- (void) takeSaturationFrom:(WDColorSlider *)slider
{
    float saturation = slider.floatValue;
    
    WDColor *newColor = [WDColor colorWithHue:[color_ hue]
                                   saturation:saturation
                                   brightness:[color_ brightness]
                                        alpha:[color_ alpha]];
    
    [self setColor:newColor];
}

- (void) setColor_:(WDColor *)color
{
    color_ = color;
    
    [self.colorWheel setColor:color_];
    [self.colorComparator setCurrentColor:color_];
    [self.colorSquare setColor:color_];
    [self.alphaSlider setColor:color_];
}

- (void) setInitialColor:(WDColor*)iColor;
{
    [self.colorComparator setInitialColor:iColor];
    [self setColor_:iColor];
}

- (void) setColor:(WDColor *)color
{
    [self setColor_:color];
    [self.delegate setActiveStateColor:color from:self];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor colorWithWhite:0.95 alpha:1];
    self.preferredContentSize = self.view.frame.size;
    
    // set up color comparator
    self.colorComparator.target = self;
	self.colorComparator.action = @selector(takeColorFromComparator:);
    
    // set up color wheel
    UIControlEvents dragEvents = (UIControlEventTouchDown | UIControlEventTouchDragInside | UIControlEventTouchDragOutside);
    self.colorWheel.backgroundColor = nil;
    [self.colorWheel addTarget:self action:@selector(takeHueFrom:) forControlEvents:dragEvents];
    
    // set up color square
    [self.colorSquare addTarget:self action:@selector(takeBrightnessAndSaturationFrom:) forControlEvents:dragEvents];
    
    // set up swatches
    self.swatches.delegate = self;
    
    self.alphaSlider.mode = WDColorSliderModeAlpha;
    [alphaSlider_ addTarget:self action:@selector(takeAlphaFrom:) forControlEvents:dragEvents];
    
    UIColor *activeColor = [[HYBrushCore sharedInstance] getActiveStatePaintColor];
    CGFloat r,g,b,a;
    [activeColor getRed:&r green:&g blue:&b alpha:&a];
    self.initialColor = [WDColor colorWithRed:r green:g blue:b alpha:a];
}

- (void) viewWillAppear:(BOOL)animated
{
    self.navigationController.navigationBarHidden = YES;
    
    [self willRotateToInterfaceOrientation:self.interfaceOrientation duration:0];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    return YES;
}

- (BOOL) prefersStatusBarHidden
{
    return YES;
}

@end
