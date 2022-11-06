//
//  YSCInspectorAPI.h
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/8/29.
//

#ifndef YSCInspectorAPI_h
#define YSCInspectorAPI_h

struct YSCInspectorCallback {
    void (*onMessage)(void *userInfo, void *message, unsigned long origin);
    void *userInfo;
};

#if defined(__cplusplus)
extern "C" {
#endif

void *YSCInspectorConnect(void *target, int targetIsWebPageProxy, struct YSCInspectorCallback callback);
void  YSCInspectorDisconnect(void *obj);
void  YSCInspectorDispatchMessage(void *obj, const void *message, unsigned long length);
void  YSCInspectorEnableProtocolMonitor(void *jsGlobalObject, void *webPageProxy, struct YSCInspectorCallback callback);
void  YSCInspectorDisableProtocolMonitor(void);

#if defined(__cplusplus)
}
#endif

#endif /* YSCInspectorAPI_h */
