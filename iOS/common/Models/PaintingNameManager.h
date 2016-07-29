//
//  PaintingNameManager.h
//  BrushesCore
//
//  Created by CharlyZhang on 16/5/27.
//  Copyright © 2016年 CharlyZhang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PaintingNameManager : NSObject

@property (nonatomic, strong) NSString *newDefaultFilePath;     //< create a new default path for Painting
@property (nonatomic, strong) NSArray *allNames;                //< the names of all default Paintings

+ (PaintingNameManager*) sharedInstance;

- (NSString*)pathOfDefaultName:(NSString*)name;

@end
