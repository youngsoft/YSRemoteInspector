//
//  YSCInspectorOffset.h
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/8/29.
//

#ifndef YSCInspectorOffset_h
#define YSCInspectorOffset_h

#include <stdlib.h>

__BEGIN_DECLS

uint16_t YSCGetJSGlobalObjectDebuggableOffset(void);
uint16_t YSCGetWebPageProxyDebuggableOffset(void);

typedef void(*YSCFuncAddr)(void);

YSCFuncAddr YSCGetStringImplOperatorNSStringFunc(void);
YSCFuncAddr YSCGetStringfromUTF8Func(void);
YSCFuncAddr YSCGetStringImpldestroyFunc(void);

__END_DECLS

#endif /* YSCInspectorOffset_h */
