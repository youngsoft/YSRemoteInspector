//
//  YSCXXInspector.hpp
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/8/29.
//

#ifndef YSCXXInspector_hpp
#define YSCXXInspector_hpp

#include "YSCInspectorAPI.h"

namespace  YSCXX {

class FrontendChannel;
class RemoteInspectionTarget;

class RemoteInspector {
public:
    RemoteInspector(void *target, bool targetIsWebPageProxy, YSCInspectorCallback callback);
    ~RemoteInspector();
    
    bool connect();
    void disconnect();
    void dispatchMessage(const void *message, unsigned long length);
        
private:
    RemoteInspectionTarget *m_debuggable;
    FrontendChannel *m_frontendChannel;
};

}


#endif /* YSCXXInspector_hpp */
