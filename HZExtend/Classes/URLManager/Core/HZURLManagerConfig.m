//
//  HZURLManageConfig.m
//  HZURLManager <https://github.com/GeniusBrother/HZURLManager>
//
//  Created by GeniusBrother on 2016/2/27.
//  Copyright (c) 2016 GeniusBrother. All rights reserved.
//

#import "HZURLManagerConfig.h"
@interface HZURLManagerConfig ()

@property(nonatomic, copy) NSDictionary *urlControllerConfig;

@property(nonatomic, copy) NSDictionary *urlMethodConfig;

@property(nonatomic, strong) NSMutableArray *mutableRewriteRule;

@end

@implementation HZURLManagerConfig
#pragma mark - Initialization
static id _instance;
+ (id)allocWithZone:(struct _NSZone *)zone
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [super allocWithZone:zone];
    });
    return _instance;
}

+ (id)copyWithZone:(struct _NSZone *)zone
{
    return _instance;
}

+ (instancetype)sharedConfig
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [[self alloc] init];
    });
    return _instance;
}

- (instancetype)init
{
    self = [super init];
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _hideBottomWhenPushed = YES;
        _mutableRewriteRule = [NSMutableArray array];
    });
    return self;
}

#pragma mark - Public Method
- (void)loadURLCtrlConfig:(NSString *)ctrlPath urlMethodConfig:(NSString *)methodPath
{
    if ([ctrlPath isKindOfClass:[NSString class]] && ctrlPath.length > 0) {
        if ([[NSFileManager defaultManager] fileExistsAtPath:ctrlPath]) {
            self.urlControllerConfig = [NSDictionary dictionaryWithContentsOfFile:ctrlPath];
        }else {
            NSAssert(NO, @"ctrlPath should be a file path");
        }
    }
    
    if ([methodPath isKindOfClass:[NSString class]] && methodPath.length > 0) {
        if ([[NSFileManager defaultManager] fileExistsAtPath:methodPath]) {
            self.urlMethodConfig = [NSDictionary dictionaryWithContentsOfFile:methodPath];
        }else {
            NSAssert(NO, @"methodPath should be a file path");
        }
    }
}

- (void)addRewriteRules:(NSArray *)rule
{
    if (rule) [self.mutableRewriteRule addObjectsFromArray:rule];
}

#pragma mark - Getter
- (NSArray *)rewriteRule
{
    return [self.mutableRewriteRule copy];
}

@end
