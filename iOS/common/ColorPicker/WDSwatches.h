//
//  WDSwatches.h
//  Brushes
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//  Copyright (c) 2009-2013 Steve Sprang
//

#import "WDColorSourceView.h"
#import "WDDragChip.h"

@class WDColor;
@class WDColorPickerController;
@class WDSwatches;

@protocol WDSwatchesDelegate <NSObject>
- (void) setColor:(WDColor *)color;
- (void) doubleTapped:(id)sender;
- (void) setActiveStateSwatchColor:(WDColor*)color atIndex:(NSUInteger)index;
- (void) setActiveStatePaintColorAtIndex:(NSUInteger)index;
- (WDColor*) getColorFromActiveStateSwatchAtIndex:(NSUInteger)index;

@end

@interface WDSwatches : WDColorSourceView <WDColorDragging>

@property (nonatomic, weak) id<WDSwatchesDelegate> delegate;
@property (nonatomic, assign) NSInteger highlightIndex;
@property (nonatomic, assign) NSInteger initialIndex;
@property (nonatomic, strong) WDColor *highlightColor;
@property (nonatomic, strong) WDColor *tappedColor;
@property (nonatomic) UIImageView *shadowOverlay;

@end
