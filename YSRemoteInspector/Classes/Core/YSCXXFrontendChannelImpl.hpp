//
//  YSCXXFrontendChannelImpl.hpp
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/10/1.
//

#ifndef YSCXXFrontendChannelImpl_hpp
#define YSCXXFrontendChannelImpl_hpp

#include "YSCInspectorAPI.h"
#include "YSCXXFrontendChannel.hpp"

namespace YSCXX {

class FrontendChannelImpl : public FrontendChannel {
public:
    FrontendChannelImpl(YSCInspectorCallback callback);
    
    virtual ~FrontendChannelImpl();
    virtual unsigned int connectionType() const;
    virtual void sendMessageToFrontend(const String &message);
    
private:
    YSCInspectorCallback m_callback;
};

}

#endif /* YSCXXFrontendChannelImpl_hpp */
