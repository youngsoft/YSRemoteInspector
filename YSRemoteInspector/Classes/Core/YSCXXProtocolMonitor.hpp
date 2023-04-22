//
//  YSCXXProtocolMonitor.hpp
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/9/23.
//

#ifndef YSCXXProtocolMonitor_hpp
#define YSCXXProtocolMonitor_hpp


#include "YSCInspectorAPI.h"
#include "YSCXXFrontendChannelImpl.hpp"
#include "YSCXXRemoteInspectionTarget.hpp"

namespace YSCXX {

#if DEBUG

class ProtocolMonitor {
public:
    
    using ConnectFuncAddr = void (*)(RemoteInspectionTarget *, FrontendChannel &, bool, bool);
    using DispatchMessageFromRemoteFuncAddr = void (*)(RemoteInspectionTarget *, const String &);
    using SendMessageToFrontendFuncAddr = void (*)(FrontendChannel *, const String &);

    ProtocolMonitor(YSCInspectorCallback callback);
    void messageCallback(const String &message, unsigned long origin);
    void alterWebPageProxyDebuggableVTable(void *debuggable);
    void alterJSGlobalObjectDebuggableVTable(void *debuggable);
    void alterFrontendChannelVTable(FrontendChannel &channel);
                
    static void JSGlobalObjectDebuggableConnectHook(RemoteInspectionTarget *pThis, FrontendChannel &channel, bool isAutomaticConnection, bool immediatelyPause);
    static void WebPageProxyDebuggableConnectHook(RemoteInspectionTarget *pThis, FrontendChannel &channel, bool isAutomaticConnection, bool immediatelyPause);
    static void JSGlobalObjectDebuggableDispatchMessageFromRemoteHook(RemoteInspectionTarget *pThis, const String &message);
    static void WebPageProxyDebuggableDispatchMessageFromRemoteHook(RemoteInspectionTarget *pThis, const String &message);
    static void FrontendChannelSendMessageToFrontendHook(FrontendChannel *pThis, const String &message);
    static void LocalFrontendChannelSendMessageToFrontendHook(FrontendChannel *pThis, const String &message);

    static ConnectFuncAddr WebPageProxyDebuggableConnectOrigin;
    static ConnectFuncAddr JSGlobalObjectDebuggableConnectOrigin;
    static DispatchMessageFromRemoteFuncAddr WebPageProxyDebuggableDispatchMessageFromRemoteOrigin;
    static DispatchMessageFromRemoteFuncAddr JSGlobalObjectDebuggableDispatchMessageFromRemoteOrigin;
    static SendMessageToFrontendFuncAddr FrontendChannelSendMessageToFrontendOrigin;
    static SendMessageToFrontendFuncAddr LocalFrontendChannelSendMessageToFrontendOrigin;

private:
    bool changePageMemoryWritable(void *addr, int protectFlag);

    YSCInspectorCallback m_callback;
    FrontendChannelImpl m_localChannel;
    
};

#endif

}


#endif /* YSCXXProtocolMonitor_hpp */
