//
//  YSRemoteInspector.h
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/8/30.
//

#import <Foundation/Foundation.h>
#import <JavaScriptCore/JavaScriptCore.h>
#import <WebKit/WebKit.h>

NS_ASSUME_NONNULL_BEGIN

@class YSRemoteInspector;


/// Remote Inspector Delegate used to receive the event message from frontend to inspector
@protocol YSRemoteInspectorDelegate <NSObject>

@required

/// Event message from frontend to inspector
/// @param inspector Remote Inspector object
/// @param message Event message. the message form refer to https://github.com/WebKit/WebKit/tree/main/Source/JavaScriptCore/inspector/protocol
- (void)remoteInspector:(YSRemoteInspector *)inspector onMessageReceived:(NSDictionary *)message;

@end


/// Remote Inspector. used to connect a JSContext or WebView.
@interface YSRemoteInspector : NSObject

+ (BOOL)canInspect;

/// automatic Incremental message id.
+ (NSNumber *)nextId;

/// init with a JSGlobalContextRef object
/// @param jsContext JSGlobalContextRef object
- (instancetype)initWithJSContext:(JSGlobalContextRef)jsContext;

/// init with a WKWebView object
/// @param webView WKWebView
- (instancetype)initWithWebView:(WKWebView *)webView;

@property (nonatomic, weak) id<YSRemoteInspectorDelegate> delegate;

- (BOOL)connect;
- (void)disconnect;

@property (nonatomic, assign, readonly, nullable) JSGlobalContextRef jsContext;
@property (nonatomic, strong, readonly, nullable) WKWebView *webView;
@property (nonatomic, readonly) BOOL connected;



/// send message from inspector to frontend.the message format refer to https://github.com/WebKit/WebKit/tree/main/Source/JavaScriptCore/inspector/protocol
/// @param message message content
/// @return the message command response from frontend to inspector.
/// @note
/// The message content consists of three parts:
/// key: id, type: number comment: message id
/// key: method, type: string, comment: message method
/// key: params, type: dictionary, comment: method params
- (NSDictionary *_Nullable )dispatchMessage:(NSDictionary *)message;


/// send message from inspector to frontend.the message format refer to https://github.com/WebKit/WebKit/tree/main/Source/JavaScriptCore/inspector/protocol
/// @param messageId messageId
/// @param method method command
/// @param params method params
/// @return the message command response from frontend to inspector.
- (NSDictionary *_Nullable)dispatchMessage:(NSNumber *)messageId method:(NSString *)method params:(NSDictionary *_Nullable)params;


/**
 This property only used in webview inspector. almost webview messages are sent through Target.sendMessageToTarget, and event message are recevied from Target.dispatchMessageFromTarget. if you set this property to YES, then the delegate of remote inspector can extract the event message from Target.dispatchMessageFromTarget. default is NO
*/
@property (nonatomic) BOOL extractTargetMessage;


/// This method only used in webview inspector. almost webview messages are sent through Target.sendMessageToTarget. you can use this method to reduce the message format.
/// @param message message
/// @param targetMessageId message id
- (NSDictionary *_Nullable)sendMessage:(NSDictionary *)message toTarget:(NSNumber *)targetMessageId;

@end



/// You can use Protocol Monitor to monitor all message from frontend and inspector than use safari
@interface YSRemoteInspector(ProtocolMonitor)


/// open the inspector protocol monitor
/// @param monitor monitor block.
+ (void)enableProtocolMonitor:(void (^)(NSString *origin, NSString *message))monitor;

////close the inspector protocol monitor
+ (void)disableProtocolMonitor;

@end


NS_ASSUME_NONNULL_END
