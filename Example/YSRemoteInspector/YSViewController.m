//
//  YSViewController.m
//  YSRemoteInspector
//
//  Created by youngsoft on 09/02/2022.
//  Copyright (c) 2022 youngsoft. All rights reserved.
//

#import "YSViewController.h"

#import "YSRemoteInspector.h"

@interface YSViewController ()<YSRemoteInspectorDelegate>

@property YSRemoteInspector *jsContextInspector;
@property YSRemoteInspector *webViewInspector;

@property JSContext *jsContext;
@property (weak, nonatomic) IBOutlet UITextView *scriptTextView;
@property (weak, nonatomic) IBOutlet WKWebView *webView;

@end

@implementation YSViewController

- (void)viewDidLoad {
    [super viewDidLoad];
	
    self.jsContext = [[JSContext alloc] init];
    self.jsContextInspector = [[YSRemoteInspector alloc] initWithJSContext:self.jsContext.JSGlobalContextRef];
    self.jsContextInspector.delegate = self;
    
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:@"https://www.baidu.com"]];
    [self.webView loadRequest:request];
    self.webViewInspector = [[YSRemoteInspector alloc] initWithWebView:self.webView];
    self.webViewInspector.delegate = self;
    self.webViewInspector.extractTargetMessage = YES;
    
    //[self.webView performSelector:@selector(_allowsRemoteInspection)];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)handleEnableMonitor:(id)sender {
    [YSRemoteInspector enableProtocolMonitor:^(NSString * _Nonnull origin, NSString * _Nonnull message) {
        NSLog(@"%@:%@",origin, message);
    }];
}

- (IBAction)handleDisableMonitor:(id)sender {
    [YSRemoteInspector disableProtocolMonitor];
}

- (IBAction)handleJSContextConnect:(id)sender {
    [self.jsContextInspector connect];
}

- (IBAction)handleJSContextDisconnect:(id)sender {
    [self.jsContextInspector disconnect];
}

- (IBAction)handleJSContextRuntimeCommand:(id)sender {
   NSDictionary *retMsg = [self.jsContextInspector dispatchMessage:[YSRemoteInspector nextId] method:@"Runtime.enable" params:nil];
    NSLog(@"%@",retMsg);
}

- (IBAction)handleJSContextDebuggerCommand:(id)sender {
    NSDictionary *retMsg = [self.jsContextInspector dispatchMessage:[YSRemoteInspector nextId] method:@"Debugger.enable" params:nil];
    NSLog(@"%@",retMsg);
}

- (IBAction)handleJSContextScriptRun:(id)sender {
    [self.jsContext evaluateScript:self.scriptTextView.text withSourceURL:[NSURL URLWithString:@"test Url"]];
}

- (IBAction)handleWebViewConnect:(id)sender {
    [self.webViewInspector connect];
}

- (IBAction)handleWebViewDisconnect:(id)sender {
    [self.webViewInspector disconnect];
}

- (IBAction)handleWebViewPageCommand:(id)sender {
    //https://github.com/WebKit/WebKit/blob/main/Source/JavaScriptCore/inspector/protocol/Target.json
    [self.webViewInspector sendMessage:@{@"id":[YSRemoteInspector nextId],@"method":@"Page.enable"} toTarget:[YSRemoteInspector nextId]];
}

- (IBAction)handleWebViewBrowserCommand:(id)sender {
    [self.webViewInspector dispatchMessage:[YSRemoteInspector nextId] method:@"Browser.enable" params:nil];
    
    [self.webViewInspector sendMessage:@{@"id":[YSRemoteInspector nextId], @"method":@"DOM.getDocument"} toTarget:[YSRemoteInspector nextId]];
}

- (IBAction)handleWebViewCSSCommand:(id)sender {
    [self.webViewInspector sendMessage:@{@"id":[YSRemoteInspector nextId],@"method":@"CSS.enable"} toTarget:[YSRemoteInspector nextId]];
}

- (IBAction)handleWebViewDebuggerCommand:(id)sender {
    [self.webViewInspector sendMessage:@{@"id":[YSRemoteInspector nextId],@"method":@"Debugger.enable"} toTarget:[YSRemoteInspector nextId]];
}

#pragma mark -- YSRemoteInspectorDelegate

-(void)remoteInspector:(YSRemoteInspector *)inspector onMessageReceived:(NSDictionary *)message {
    if (inspector.jsContext != nil) {
        NSLog(@"jsContextMessage:%@",message);
    } else if (inspector.webView != nil) {
        NSLog(@"webViewMessage:%@",message);
    } else {
        NSAssert(0, @"OOPS!!");
    }
}

@end
