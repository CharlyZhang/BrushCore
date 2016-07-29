//
//  PaintingNameManager.m
//  BrushesCore
//
//  Created by CharlyZhang on 16/5/27.
//  Copyright © 2016年 CharlyZhang. All rights reserved.
//

#import "PaintingNameManager.h"

#define DOCUMENT_DIR            [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"]
#define PAINTING_FILE_EXTENTION @"painting"
#define PAINTING_DEFAULT_NAME   @"Default"

@interface PaintingNameManager()

@property (nonatomic, strong) NSString *newDefaultName;

@end

@implementation PaintingNameManager

+ (PaintingNameManager *) sharedInstance
{
    static PaintingNameManager *paintingNameManager_ = nil;
    
    if (!paintingNameManager_) {
        paintingNameManager_ = [[PaintingNameManager alloc] init];
    }
    
    return paintingNameManager_;
}

- (NSArray*) allNames
{
    return [[[NSFileManager defaultManager] contentsOfDirectoryAtPath:DOCUMENT_DIR error:nil] pathsMatchingExtensions:[NSArray arrayWithObjects:PAINTING_FILE_EXTENTION,nil]];
    
    // TO DO: 排序
}

- (NSString*) newDefaultFilePath
{
    return [self filePathofName:self.newDefaultName];
}

- (NSString*)pathOfDefaultName:(NSString*)name
{
    return [self filePathofName:name];
}

#pragma mark Private

- (NSString*) newDefaultName
{
    NSFileManager *fm = [NSFileManager defaultManager];
    int defaultFileNo = 0;
    _newDefaultName = [NSString stringWithFormat:@"%@%2d.%@",PAINTING_DEFAULT_NAME, defaultFileNo, PAINTING_FILE_EXTENTION];
    BOOL isDir;
    while ([fm fileExistsAtPath: [self filePathofName:_newDefaultName] isDirectory:&isDir]) {
        defaultFileNo ++;
        _newDefaultName = [NSString stringWithFormat:@"%@%2d.%@",PAINTING_DEFAULT_NAME, defaultFileNo, PAINTING_FILE_EXTENTION];
    }
    
    return _newDefaultName;
}

- (NSString*) filePathofName:(NSString*)name
{
    return [DOCUMENT_DIR stringByAppendingPathComponent:name];
}

@end
