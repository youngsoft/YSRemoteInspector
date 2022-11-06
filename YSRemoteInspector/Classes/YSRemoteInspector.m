//
//  YSRemoteInspector.m
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/8/30.
//

#import "YSRemoteInspector.h"
#include "YSCInspectorAPI.h"
#include <objc/runtime.h>


@interface YSRemoteInspector()

@property (nonatomic, readwrite, nullable) JSGlobalContextRef jsContext;
@property (nonatomic, strong, readwrite, nullable) WKWebView *webView;

@property (nonatomic, assign) void *inspectorImpl;

@property (nonatomic, strong) NSDictionary *currentMessage;
@property (nonatomic, strong) NSString *targetId;

@end


@implementation YSRemoteInspector

+ (BOOL)canInspect {
    return [[UIDevice currentDevice].systemVersion compare:@"13.0"] >= 0;
}

NSInteger _ys_nextId;
+ (NSNumber *)nextId {
    return @(++_ys_nextId);
}


- (instancetype)initWithJSContext:(JSGlobalContextRef)jsContext {
    return [self initWithJSContext:jsContext orWebView:nil];
}

- (instancetype)initWithWebView:(WKWebView *)webView {
    return [self initWithJSContext:nil  orWebView:webView];
}

- (BOOL)connected {
    return self.inspectorImpl != nil;
}

- (BOOL) connect {
    if ([self connected]) {
        return YES;
    }
    
    if (self.jsContext != nil) {
        return [self connectToJSContext:self.jsContext];
    } else if (self.webView != nil) {
        return [self connectToWebView:self.webView];
    } else {
        return NO;
    }
}

- (void)disconnect {
    if (!self.connected) {
        return;
    }
    
    YSCInspectorDisconnect(self.inspectorImpl);
    self.inspectorImpl = NULL;
}

- (NSDictionary *_Nullable)dispatchMessage:(NSDictionary  *)message {
    if (!self.connected) {
        return nil;
    }
    
    NSJSONWritingOptions options = NSJSONWritingPrettyPrinted;
    if (@available(iOS 13.0, *)) {
        options |= NSJSONWritingWithoutEscapingSlashes;
    }
    NSData *messageData = [NSJSONSerialization dataWithJSONObject:message options:options  error:nil];
    if (messageData == nil) {
        return nil;
    }
    
    self.currentMessage = nil;
    YSCInspectorDispatchMessage(self.inspectorImpl, messageData.bytes, messageData.length);
    NSDictionary *retMessage = self.currentMessage;
    self.currentMessage = nil;
    if (![retMessage[@"id"] isEqual:message[@"id"]]) {
        retMessage = nil;
    }
    return retMessage;
}

- (NSDictionary *)dispatchMessage:(NSNumber *)messageId method:(NSString *)method params:(NSDictionary * _Nullable)params {
    NSMutableDictionary *message = [NSMutableDictionary new];
    message[@"id"] = messageId;
    message[@"method"] = method;
    if (params != nil) {
        message[@"params"] = params;
    }
    return [self dispatchMessage:message];
}

- (NSDictionary *_Nullable)sendMessage:(NSDictionary *)message toTarget:(NSNumber *)targetMessageId {
    if (self.targetId == nil){
        return nil;
    }
    
    return [self dispatchMessage:targetMessageId method:@"Target.sendMessageToTarget" params:@{
        @"targetId":self.targetId,
        @"message":[self buildStringFromDictionary:message]
    }];
}

- (void)dealloc {
    [self disconnect];
    self.delegate = nil;
}


#pragma mark -- Private

+ (void *)JSGlobalObjectFrom:(JSGlobalContextRef)jsContext {
    
    void *jsGlobalObject = (void *)jsContext;
    return jsGlobalObject;
}

+ (void *)WebPageProxyFrom:(WKWebView *)webView {
    Ivar pageIvar = class_getInstanceVariable([WKWebView class], "_page");
    void *page = (__bridge void*)object_getIvar(webView, pageIvar);
    return page;
}

- (instancetype) initWithJSContext:(JSGlobalContextRef)jsContext orWebView:(WKWebView *) webView {
    self = [self init];
    if (self != nil) {
        _jsContext = jsContext;
        _webView = webView;
        _inspectorImpl = nil;
        _delegate = nil;
        _currentMessage = nil;
    }
    return self;
}

- (BOOL)connectToJSContext:(JSGlobalContextRef)jsContext {
    self.inspectorImpl = YSCInspectorConnect([YSRemoteInspector JSGlobalObjectFrom:jsContext], false, [self makeCallback]);
    return self.inspectorImpl != nil;
}

- (BOOL)connectToWebView:(WKWebView *)webView {
    self.targetId = nil;
    self.inspectorImpl = YSCInspectorConnect([YSRemoteInspector WebPageProxyFrom:webView], true, [self makeCallback]);
    return self.inspectorImpl != nil;
}

-(NSDictionary *)buildDictionaryFromString:(NSString *)string {
    if (string.length == 0) {
        return nil;
    }
    
    NSData *data = [string dataUsingEncoding:NSUTF8StringEncoding];
    return [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
}

- (NSString *) buildStringFromDictionary:(NSDictionary *)dictionary {
    if (dictionary == nil) {
        return nil;
    }
    
    NSJSONWritingOptions options = NSJSONWritingPrettyPrinted;
    if (@available(iOS 13.0, *)) {
        options |= NSJSONWritingWithoutEscapingSlashes;
    }
    NSData *data = [NSJSONSerialization dataWithJSONObject:dictionary options:options  error:nil];
    if (data == nil) {
        return nil;
    }
    return [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
}

-(NSDictionary*)handleTargetMessage:(NSDictionary*)dictMessage {
    if (self.jsContext != NULL) {
        return dictMessage;
    }
    
    NSString *method = dictMessage[@"method"];
    if (self.extractTargetMessage && [method isEqualToString:@"Target.dispatchMessageFromTarget"]) {
        return [self buildDictionaryFromString:dictMessage[@"params"][@"message"]];
    }
    
    if ([method isEqualToString:@"Target.targetCreated"]) {
        self.targetId = dictMessage[@"params"][@"targetInfo"][@"targetId"];
    }
    
    if ([method isEqualToString:@"Target.targetDestroyed"]) {
        self.targetId = nil;
    }
    
    return dictMessage;
}

-(void)handleMessageRecived:(NSString *)message {
    NSDictionary *dictMessage = [self buildDictionaryFromString:message];
    if (dictMessage == nil) {
        return;
    }
    
    if (dictMessage[@"id"] != nil) {
        self.currentMessage = dictMessage;
    } else {
        dictMessage = [self handleTargetMessage:dictMessage];
        self.currentMessage = nil;
        if (self.delegate != nil) {
            [self.delegate remoteInspector:self onMessageReceived:dictMessage];
        }
    }
}

static void YSOnMessage_(void *userInfo, void *message, unsigned long origin) {
    if (userInfo == nil) {
        return;
    }
    
    YSRemoteInspector *remoteInspector = (__bridge YSRemoteInspector *)userInfo;
    NSString *strMessage = (__bridge NSString *)(message);
    [remoteInspector handleMessageRecived:strMessage];
}

- (struct YSCInspectorCallback)makeCallback {
    struct YSCInspectorCallback callback;
    callback.userInfo = (__bridge void*)self;
    callback.onMessage = (void(*)(void *, void *, unsigned long))YSOnMessage_;
    return callback;
}

@end



@implementation YSRemoteInspector(ProtocolMonitor)

BOOL _ys_hasMonitor = NO;

static void YSOnMonitorMessage_(void *userInfo, void *message, unsigned long origin) {
    void (^monitor)(NSString *, NSString *) = (__bridge void(^)(NSString*, NSString*))userInfo;
    NSString *orgin = (origin == 0)? @"from":@"to";
    NSString *strMessage = (__bridge NSString *)message;
    if (monitor != nil) {
        monitor(orgin, strMessage);
    }
}

+ (void)enableProtocolMonitor:(void (^)(NSString *, NSString *))monitor {
    if (!_ys_hasMonitor) {
        _ys_hasMonitor = YES;

        JSContext *jsContext = [[JSContext alloc] init];
        WKWebView *webView = [[WKWebView alloc] init];
        
        struct YSCInspectorCallback callback;
        callback.userInfo = (__bridge void *)monitor;
        callback.onMessage = YSOnMonitorMessage_;
        YSCInspectorEnableProtocolMonitor([self JSGlobalObjectFrom:jsContext.JSGlobalContextRef], [self WebPageProxyFrom:webView], callback);
    }
}

+ (void)disableProtocolMonitor {
    if (_ys_hasMonitor) {
        _ys_hasMonitor = NO;
        YSCInspectorDisableProtocolMonitor();
    }
}

@end
