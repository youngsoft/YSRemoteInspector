//
//  YSCXXInspector.cpp
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/8/29.
//

#include "YSCXXInspector.hpp"
#include "YSCXXFrontendChannelImpl.hpp"
#include "YSCXXRemoteInspectionTarget.hpp"
#include "YSCInspectorOffset.h"
#include <type_traits>

namespace YSCXX {

RemoteInspector::RemoteInspector(void *target, bool targetIsWebPageProxy, YSCInspectorCallback callback) {
    this->m_debuggable = nullptr;
    if (target != nullptr) {
        if (targetIsWebPageProxy) {
            uint16_t debuggableOffset = YSCGetWebPageProxyDebuggableOffset();
            if (debuggableOffset != 0) {
                this->m_debuggable = *reinterpret_cast<RemoteInspectionTarget **>(((unsigned char *)target) + debuggableOffset);
            }
        } else {
            
            uint16_t debuggableOffset = YSCGetJSGlobalObjectDebuggableOffset();
            if (debuggableOffset != 0) {
                this->m_debuggable = *reinterpret_cast<RemoteInspectionTarget **>(((unsigned char *)target) + debuggableOffset);
            }
        }
    }
    
    this->m_frontendChannel = nullptr;
    if (this->m_debuggable != nullptr) {
        this->m_frontendChannel = new FrontendChannelImpl(callback);
    }
}

RemoteInspector::~RemoteInspector() {
    this->disconnect();
    if (this->m_frontendChannel != nullptr) {
        delete this->m_frontendChannel;
        this->m_frontendChannel = nullptr;
    }
}

bool RemoteInspector::connect() {
    if (this->m_debuggable == nullptr || this->m_frontendChannel == nullptr) {
        return false;
    }
    
    this->m_debuggable->connect(*this->m_frontendChannel);
    return true;
}

void RemoteInspector::disconnect() {
    if (this->m_debuggable == nullptr || this->m_frontendChannel == nullptr) {
        return;
    }
    this->m_debuggable->disconnect(*this->m_frontendChannel);
}

void  RemoteInspector::dispatchMessage(const void *message, unsigned long length) {
    if (this->m_debuggable == nullptr) {
        return;
    }
    
    String strMessage = String::fromUTF8((const char*)message, length);
    this->m_debuggable->dispatchMessageFromRemote(std::move(strMessage));
}


}


using namespace YSCXX;

//对接OC层的接口。。
__BEGIN_DECLS

void *YSCInspectorConnect(void *target, int targetIsWebPageProxy, struct YSCInspectorCallback callback) {
    RemoteInspector *inspector = nullptr;
    if (target != nullptr) {
        inspector = new RemoteInspector(target, targetIsWebPageProxy, callback);
        inspector->connect();
    }
    return inspector;
}

void YSCInspectorDisconnect(void *obj) {
    if (obj != nullptr) {
        RemoteInspector *inspector = reinterpret_cast<RemoteInspector *>(obj);
        inspector->disconnect();
        delete inspector;
    }
}

void YSCInspectorDispatchMessage(void *obj, const void *message, unsigned long length) {
    if (obj != nullptr) {
        RemoteInspector *inspector = reinterpret_cast<RemoteInspector *>(obj);
        inspector->dispatchMessage(message, length);
    }
}

__END_DECLS
