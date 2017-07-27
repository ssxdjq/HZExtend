//
//  SessionTask.h
//  ZHFramework
//
//  Created by xzh. on 15/8/17.
//  Copyright (c) 2015年 xzh. All rights reserved.
//
/****************     具体的请求任务,配置参数,输出数据     ****************/

#import <Foundation/Foundation.h>
#import "HZNetworkConfig.h"
#import "HZNetworkConst.h"

@class HZSessionTask;

NS_ASSUME_NONNULL_BEGIN

typedef void(^HZSessionTaskDidCompletedBlock)(HZSessionTask * __nullable task);
typedef void(^HZSessionTaskSendingBlock)(HZSessionTask *task);
typedef void(^HZSessionTaskDidCancelBlock)(HZSessionTask *task);
typedef void(^HZSessionTaskUploadProgressBlock)(HZSessionTask *task, NSProgress *progress);
typedef NS_ENUM(NSUInteger, HZSessionTaskState) {   //请求状态
    HZSessionTaskStateRunable = 0,                  //可运行
    HZSessionTaskStateRunning = 1,                  //请求中
    HZSessionTaskStateCancel = 2,                   //请求取消
    HZSessionTaskStateSuccess = 3,                  //请求成功
    HZSessionTaskStateFail = 4,                     //请求失败(业务错误||连接失败)
};

typedef NS_ENUM(NSUInteger, HZSessionTaskCacheImportState) {  //缓存导入状态
    HZSessionTaskCacheImportStateNone = 0,          //未导入过缓存状态,为初始状态
    HZSessionTaskCacheImportStateSuccess = 1,       //缓存导入成功
    HZSessionTaskCacheImportStateFail = 2,          //缓存导入失败状态,可能原因为没有缓存或已经导入过缓存
};


@protocol HZSessionTaskDelegate<NSObject>

/**
 *  task请求完成后调用,此时请求状态可能为:HZSessionTaskStateSuccess 或 HZSessionTaskStateFail
 */
- (void)taskDidCompleted:(HZSessionTask *)task;

/**
 *  task进入请求中调用 此时请求状态为:SessionTaskStateRunning
 */
- (void)taskSending:(HZSessionTask *)task;

/**
 *  task被取消时调用 此时请求状态为:HZSessionTaskStateCancel
 */
- (void)taskDidCancel:(HZSessionTask *)task;

/**
 *  task将要请求时调用,返回不为空,则拦截请求
 */
- (NSString *)taskShouldPerform:(HZSessionTask *)task;

@end

@interface HZSessionTask : NSObject
/**
 *	创建请求任务
 *
 *	@param method  请求类型 目前只支持GET/POST
 *  @param path  URL中的path
 *  @param params  请求参数
 *  @param delegate  代理
 *  @param taskIdentifier 任务标识,必须指定,推荐用把所有的path放到一个文件里用作标识
 *  @param pathKeys 路径参数数组
 */
+ (instancetype)taskWithMethod:(NSString *)method
                          path:(NSString *)path
                        params:(nullable NSMutableDictionary<NSString *, id> *)params
                      delegate:(nullable id<HZSessionTaskDelegate>)delegate
                taskIdentifier:(nullable NSString *)taskIdentifier;

+ (instancetype)taskWithMethod:(NSString *)method
                          path:(NSString *)path
                        params:(nullable NSMutableDictionary<NSString *, id> *)params
                      pathKeys:(NSArray<NSString *> *)keys
                      delegate:(nullable id<HZSessionTaskDelegate>)delegate
                taskIdentifier:(nullable NSString *)taskIdentifier;

/**
 *	创建上传请求任务
 */
+ (instancetype)uploadTaskWithPath:(NSString *)path
                              params:(nullable NSMutableDictionary<NSString *, id> *)params
                            delegate:(nullable id<HZSessionTaskDelegate>)delegate
                      taskIdentifier:(NSString *)taskIdentifier;

/** 任务的唯一标识 */
@property(nonatomic, copy, readonly) NSString *taskIdentifier;

/** 请求状态 */
@property(nonatomic, assign, readonly) HZSessionTaskState state;

/** 缓存导入状态 */
@property(nonatomic, assign, readonly) HZSessionTaskCacheImportState cacheImportState;

/** 是否为第一次执行 */
@property(nonatomic, assign, readonly) BOOL isFirstRequest;


@property(nonatomic, weak) id<HZSessionTaskDelegate> delegate;

/** task完成时调用 */
@property(nonatomic, copy) HZSessionTaskDidCompletedBlock taskDidCompletedBlock;

/** task进入请求中调用 */
@property(nonatomic, copy) HZSessionTaskSendingBlock taskSendingBlock;

/** task取消请求时调用 */
@property(nonatomic, copy) HZSessionTaskDidCancelBlock taskDidCancelBlock;

/** 上传过程中会调用 */
@property(nonatomic, copy) HZSessionTaskUploadProgressBlock uploadProgressBlock;

/** 不设置则每个session默认为HZNetworkConfig的baseURL为基本的路径 */
@property(nullable, nonatomic, copy) NSString *baseURL;

/** URL中的path */
@property(nonatomic, copy) NSString *path;

/** 请求参数 */
@property(nonatomic, strong) NSMutableDictionary<NSString *, id> *params;

/** 文件参数 */
@property(nullable, nonatomic, strong) NSMutableDictionary<NSString *, id> *fileParams;

/** 请求方法 */
@property(nonatomic, copy, readonly) NSString *method;

/** 请求头 */
@property(nullable, nonatomic, readonly) NSDictionary <NSString *, NSString *> *requestHeader;

/** pathKeys不为空则URL为http://abc/a/value1/value2/...的格式。pathkeys=@[key1,key2....] value1,value2通过params设置*/
@property(nullable, nonatomic, copy) NSArray *pathkeys;

/** 是否对正确返回数据缓存 默认为为YES,HZUploadSessionTask默认为NO */
@property(nonatomic, assign, getter=isCached) BOOL cached;

/** 设置每次只导入缓存一次，默认为YES,分页时应设置成NO来解决上拉加载来导入缓存 */
@property(nonatomic, assign) BOOL importCacheOnce;

/** 服务器返回的json对象 */
@property(nullable, nonatomic, strong, readonly) NSDictionary *responseObject;

@property(nullable, nonatomic, strong, readonly) NSError *error;

/** 服务器反馈消息 */
@property(nullable, nonatomic, copy, readonly) NSString *message;

/** 请求的绝对路径 */
@property(nonatomic, copy, readonly) NSString *absoluteURL;

/**
 *	设置请求头，或者在HZNetworkConfig设置通用的请求头
 */
- (void)setValue:(NSString *)value forHeaderField:(NSString *)key;


/**
 *	开始请求
 *
 *	@param completionCallBack  task完成时的回调
 *	@param sendingCallBack  task进入请求中状态时回调
 *	@param lostCallBack task无法连接时的回调
 *	@param uploadCallBack task上传过程中的回调
 */
- (void)start;
- (void)startWithCompletion:(HZSessionTaskDidCompletedBlock)completion;
- (void)startWithCompletionCallBack:(HZSessionTaskDidCompletedBlock)completionCallBack
                    sendingCallBack:(nullable HZSessionTaskSendingBlock)sendingCallBack;

/**
 *	开始请求
 *
 *	@param handler  将要执行请求任务时调用,如果error不为nil,说明请求被拦截
 */
- (void)startWithHandler:(nullable void(^)(HZSessionTask *task, NSError  * _Nullable error))handler;

/**
 *	开始上传请求
 *  上传请求任务请用该方法启动
 *	@param completionCallBack  task完成时的回调
 *  @param uploadCallBack task上传过程中的回调
 */
- (void)startUploadWithCompletionCallBack:(HZSessionTaskDidCompletedBlock)completionCallBack
                     uploadCallBack:(HZSessionTaskUploadProgressBlock)uploadCallBack;

/**
 *	取消请求任务
 */
- (void)cancel;

@end

NS_ASSUME_NONNULL_END
