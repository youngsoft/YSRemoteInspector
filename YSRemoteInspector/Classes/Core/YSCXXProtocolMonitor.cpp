//
//  YSCXXProtocolMonitor.cpp
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/9/23.
//

#include <sys/mman.h>
#include "YSCXXProtocolMonitor.hpp"
#include "YSCInspectorOffset.h"

namespace YSCXX {

#if DEBUG

#define YSCPAGESIZE_SHIFT 14
#define YSCPAGESIZE (1<<YSCPAGESIZE_SHIFT)
#define YSCVTABLE_CONNECT_INDEX 2
#define YSCVTABLE_DISPATCHMESSAGEFROMREMOTE_INDEX 6
#define YSCVTABLE_SENDMESSAGETOFRONTEND_INDEX 3

static ProtocolMonitor *_ys_monitor = nullptr;

ProtocolMonitor::ConnectFuncAddr ProtocolMonitor::JSGlobalObjectDebuggableConnectOrigin = nullptr;
void ProtocolMonitor::JSGlobalObjectDebuggableConnectHook(RemoteInspectionTarget *pThis, FrontendChannel &channel, bool isAutomaticConnection, bool immediatelyPause) {
    if (_ys_monitor != nullptr) {
        _ys_monitor->alterFrontendChannelVTable(channel);
    }
    JSGlobalObjectDebuggableConnectOrigin(pThis, channel, isAutomaticConnection, immediatelyPause);
}

ProtocolMonitor::ConnectFuncAddr ProtocolMonitor::WebPageProxyDebuggableConnectOrigin = nullptr;
void ProtocolMonitor::WebPageProxyDebuggableConnectHook(RemoteInspectionTarget *pThis, FrontendChannel &channel, bool isAutomaticConnection, bool immediatelyPause) {
    if (_ys_monitor != nullptr) {
        _ys_monitor->alterFrontendChannelVTable(channel);
    }
    WebPageProxyDebuggableConnectOrigin(pThis, channel, isAutomaticConnection, immediatelyPause);
}

ProtocolMonitor::DispatchMessageFromRemoteFuncAddr ProtocolMonitor::JSGlobalObjectDebuggableDispatchMessageFromRemoteOrigin = nullptr;
void ProtocolMonitor::JSGlobalObjectDebuggableDispatchMessageFromRemoteHook(RemoteInspectionTarget *pThis, const String &message) {
    if (_ys_monitor != nullptr) {
        _ys_monitor->messageCallback(message, 0);
    }
    JSGlobalObjectDebuggableDispatchMessageFromRemoteOrigin(pThis, message);
}

ProtocolMonitor::DispatchMessageFromRemoteFuncAddr ProtocolMonitor::WebPageProxyDebuggableDispatchMessageFromRemoteOrigin = nullptr;
void ProtocolMonitor::WebPageProxyDebuggableDispatchMessageFromRemoteHook(RemoteInspectionTarget *pThis, const String &message) {
    if (_ys_monitor != nullptr) {
        _ys_monitor->messageCallback(message, 0);
    }
    WebPageProxyDebuggableDispatchMessageFromRemoteOrigin(pThis, message);
}

ProtocolMonitor::SendMessageToFrontendFuncAddr ProtocolMonitor::FrontendChannelSendMessageToFrontendOrigin = nullptr;
void ProtocolMonitor::FrontendChannelSendMessageToFrontendHook(FrontendChannel *pThis, const String &message) {
    if (_ys_monitor != nullptr) {
        _ys_monitor->messageCallback(message, 1);
    }
    FrontendChannelSendMessageToFrontendOrigin(pThis, message);
}

ProtocolMonitor::SendMessageToFrontendFuncAddr ProtocolMonitor::LocalFrontendChannelSendMessageToFrontendOrigin = nullptr;
void ProtocolMonitor::LocalFrontendChannelSendMessageToFrontendHook(FrontendChannel *pThis, const String &message) {
    if (_ys_monitor != nullptr) {
        _ys_monitor->messageCallback(message, 1);
    }
    LocalFrontendChannelSendMessageToFrontendOrigin(pThis, message);
}

ProtocolMonitor::ProtocolMonitor(YSCInspectorCallback callback)
:m_callback(callback)
,m_localChannel({}){
}

bool ProtocolMonitor::changePageMemoryWritable(void *addr, int protectFlag) {
    if (addr == nullptr) {
        return false;
    }
    
    unsigned long  pageAddr = reinterpret_cast<unsigned long>(addr);
    pageAddr &= ~(YSCPAGESIZE - 1);
    return  mprotect(reinterpret_cast<void*>(pageAddr), YSCPAGESIZE, protectFlag) == 0;
}

void ProtocolMonitor::messageCallback(const String &message, unsigned long origin) {
    if (m_callback.onMessage != nullptr && m_callback.userInfo != nullptr) {
        m_callback.onMessage(m_callback.userInfo, (void*)message, origin);
    }
}

void ProtocolMonitor::alterFrontendChannelVTable(FrontendChannel &channel) {
    auto hook = FrontendChannelSendMessageToFrontendHook;
    auto origin = &FrontendChannelSendMessageToFrontendOrigin;
    
    void **virtualTable = *(void ***)&channel;
    void *localVirtualTable = *(void ***)&m_localChannel;
    int protectFlag = PROT_WRITE|PROT_READ;
    if (localVirtualTable == virtualTable) {
        hook = LocalFrontendChannelSendMessageToFrontendHook;
        origin = &LocalFrontendChannelSendMessageToFrontendOrigin;
        protectFlag |= PROT_EXEC;
    }
    
    void *currentSendMessageToFrontend =  virtualTable[YSCVTABLE_SENDMESSAGETOFRONTEND_INDEX];
    if (currentSendMessageToFrontend != hook) {
        if (changePageMemoryWritable(virtualTable, protectFlag)) {
            *origin = (SendMessageToFrontendFuncAddr)currentSendMessageToFrontend;
            virtualTable[YSCVTABLE_SENDMESSAGETOFRONTEND_INDEX] = (void*)hook;
        }
    }
}

void ProtocolMonitor::alterJSGlobalObjectDebuggableVTable(void *debuggable) {
    if (debuggable != nullptr) {
        void **virtualTable = *(void ***)debuggable;
        void *currentConnect = virtualTable[YSCVTABLE_CONNECT_INDEX];
        if (currentConnect != JSGlobalObjectDebuggableConnectHook) {
            if (changePageMemoryWritable(virtualTable, PROT_WRITE|PROT_READ)) {
                JSGlobalObjectDebuggableConnectOrigin = (ConnectFuncAddr) virtualTable[YSCVTABLE_CONNECT_INDEX];
                virtualTable[YSCVTABLE_CONNECT_INDEX] = (void*)JSGlobalObjectDebuggableConnectHook;
                
                JSGlobalObjectDebuggableDispatchMessageFromRemoteOrigin = (DispatchMessageFromRemoteFuncAddr) virtualTable[YSCVTABLE_DISPATCHMESSAGEFROMREMOTE_INDEX];
                virtualTable[YSCVTABLE_DISPATCHMESSAGEFROMREMOTE_INDEX] = (void*)JSGlobalObjectDebuggableDispatchMessageFromRemoteHook;
            }
        }
    }
}

void ProtocolMonitor::alterWebPageProxyDebuggableVTable(void *debuggable) {
    if (debuggable != nullptr) {
        void **virtualTable = *(void ***)debuggable;
        void *currentConnect = virtualTable[YSCVTABLE_CONNECT_INDEX];
        if (currentConnect != WebPageProxyDebuggableConnectHook) {
            if (changePageMemoryWritable(virtualTable,PROT_WRITE|PROT_READ)) {
                WebPageProxyDebuggableConnectOrigin = (ConnectFuncAddr) virtualTable[YSCVTABLE_CONNECT_INDEX];
                virtualTable[YSCVTABLE_CONNECT_INDEX] = (void*)WebPageProxyDebuggableConnectHook;
                
                WebPageProxyDebuggableDispatchMessageFromRemoteOrigin = (DispatchMessageFromRemoteFuncAddr) virtualTable[YSCVTABLE_DISPATCHMESSAGEFROMREMOTE_INDEX];
                virtualTable[YSCVTABLE_DISPATCHMESSAGEFROMREMOTE_INDEX] = (void*)WebPageProxyDebuggableDispatchMessageFromRemoteHook;
            }
        }
    }
}


#endif

}


//对接OC层的接口。。
using namespace YSCXX;
__BEGIN_DECLS

void YSCInspectorEnableProtocolMonitor(void *jsGlobalObject, void *webPageProxy, struct YSCInspectorCallback callback) {
#if DEBUG
    if (_ys_monitor != nullptr) {
        return;
    }
    _ys_monitor = new YSCXX::ProtocolMonitor(callback);
    
    uint16_t jsGlobalObjectDebuggableOffset = YSCGetJSGlobalObjectDebuggableOffset();
    _ys_monitor->alterJSGlobalObjectDebuggableVTable(*(void **)((unsigned long)jsGlobalObject + jsGlobalObjectDebuggableOffset));
            
    uint16_t webPageProxyDebuggableOffset = YSCGetWebPageProxyDebuggableOffset();
    _ys_monitor->alterWebPageProxyDebuggableVTable(*(void **)((unsigned long)webPageProxy + webPageProxyDebuggableOffset));
#endif
}

void YSCInspectorDisableProtocolMonitor() {
#if DEBUG
    if (_ys_monitor != nullptr) {
        delete _ys_monitor;
        _ys_monitor = nullptr;
    }
#endif
}

__END_DECLS
