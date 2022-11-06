//
//  YSCXXFrontendChannelImpl.cpp
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/10/1.
//

#include "YSCXXFrontendChannelImpl.hpp"

namespace YSCXX {

FrontendChannelImpl::FrontendChannelImpl(YSCInspectorCallback callback):m_callback(callback) {
}

FrontendChannelImpl::~FrontendChannelImpl() {
}

unsigned int FrontendChannelImpl::connectionType() const  {
    return 0;
};

void FrontendChannelImpl::sendMessageToFrontend(const String &message) {
    this->m_callback.onMessage(this->m_callback.userInfo, (void *)message, 0);
}

}
