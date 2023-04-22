//
//  YSCInspectorOffset.c
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/8/29.
//

#include "YSCInspectorOffset.h"
#include <dlfcn.h>
#import <objc/runtime.h>


static void ysc_GetFuncAddrHelper(YSCFuncAddr *funcAddr, const char *funcName) {
    if (*funcAddr == NULL) {
        *funcAddr =(YSCFuncAddr)dlsym(RTLD_DEFAULT, funcName);
    }
}

#if __arm64__

static uint16_t ysc_OffsetFromInstruction(void *insAddr) {
    struct arm64_instruction_ldr {
        uint32_t Rt:5;
        uint32_t Rn:5;
        uint32_t imm12:12;
        uint32_t opc:2;
        uint32_t other:6;
        uint32_t size:2;
    };
    
    struct arm64_instruction_ldr *ins = (struct arm64_instruction_ldr *)insAddr;
    return ins->imm12 << ins->size;
}

#elif __x86_64__

static uint16_t ysc_OffsetFromInstruction(void *insAddr) {
    struct x86_64_instruction_mov {
        uint8_t opc[3];
        uint16_t offset;
        uint8_t other[2];
    }__attribute__((packed,aligned(1)));
    
    struct x86_64_instruction_mov *ins =(struct x86_64_instruction_mov *)insAddr;
    return  ins->offset;
}

#else

static uint16_t ysc_OffsetFromInstruction(void *insAddr) {
    return 0;
}

#endif

uint16_t YSCGetJSGlobalObjectDebuggableOffset() {
    static uint16_t jsglobalobject_debuggable_offset;
    if (jsglobalobject_debuggable_offset == 0) {
        YSCFuncAddr remoteDebuggingEnabledFuncAddr = NULL;
        ysc_GetFuncAddrHelper(&remoteDebuggingEnabledFuncAddr, "_ZNK3JSC14JSGlobalObject14setInspectableEb");
        if (remoteDebuggingEnabledFuncAddr == NULL) {
            ysc_GetFuncAddrHelper(&remoteDebuggingEnabledFuncAddr, "_ZNK3JSC14JSGlobalObject22remoteDebuggingEnabledEv");
        }
        if (remoteDebuggingEnabledFuncAddr != NULL) {
#if __arm64__
            /*
            JavaScriptCore`JSC::JSGlobalObject::remoteDebuggingEnabled:
                0x18c9e4814 <+0>: ldr    x8, [x0, #0x798]   ;this instruction can get debuggable offset
                0x18c9e4818 <+4>: ldrb   w0, [x8, #0xc]
                0x18c9e481c <+8>: ret
             */
            jsglobalobject_debuggable_offset = ysc_OffsetFromInstruction((unsigned char *)remoteDebuggingEnabledFuncAddr + 0);
#elif __x86_64__
            /*
            JavaScriptCore`JSC::JSGlobalObject::remoteDebuggingEnabled:
                0x7fff31634220 <+0>:  pushq  %rbp
                0x7fff31634221 <+1>:  movq   %rsp, %rbp
                0x7fff31634224 <+4>:  movq   0x750(%rdi), %rax   ;this instruction can get debuggable offset
                0x7fff3163422b <+11>: movb   0xc(%rax), %al
                0x7fff3163422e <+14>: popq   %rbp
                0x7fff3163422f <+15>: retq
             */
            jsglobalobject_debuggable_offset = ysc_OffsetFromInstruction((unsigned char *)remoteDebuggingEnabledFuncAddr + 4);
#else
            jsglobalobject_debuggable_offset = ysc_OffsetFromInstruction(remoteDebuggingEnabledFuncAddr);
#endif
        }
    }
    return jsglobalobject_debuggable_offset;
}

uint16_t YSCGetWebPageProxyDebuggableOffset() {
    static uint16_t webpageproxy_debuggable_offset;
    if (webpageproxy_debuggable_offset == 0) {
        Method  method = class_getInstanceMethod(objc_getClass("WKWebView"), sel_registerName("_allowsRemoteInspection"));
        if (method != NULL) {
            IMP _allowsRemoteInspection = method_getImplementation(method);
            if (_allowsRemoteInspection != NULL) {
#if __arm64__
                /*
                WebKit`-[WKWebView(WKPrivate) _allowsRemoteInspection]:
                   0x18f7e6df0 <+0>:  adrp   x8, 295965
                   0x18f7e6df4 <+4>:  ldrsw  x8, [x8, #0xf1c]
                   0x18f7e6df8 <+8>:  ldr    x8, [x0, x8]
                   0x18f7e6dfc <+12>: ldr    x8, [x8, #0xb00]   ;this instruction can get debuggable offset
                   0x18f7e6e00 <+16>: ldrb   w0, [x8, #0xc]
                   0x18f7e6e04 <+20>: ret
                */
                webpageproxy_debuggable_offset = ysc_OffsetFromInstruction((unsigned char *)_allowsRemoteInspection + 12);
#elif __x86_64__
                /*
                WebKit`-[WKWebView(WKPrivate) _allowsRemoteInspection]:
                    0x7fff2877250a <+0>:  pushq  %rbp
                    0x7fff2877250b <+1>:  movq   %rsp, %rbp
                    0x7fff2877250e <+4>:  movq   0x57ea72fb(%rip), %rax    ; WKWebView._page
                    0x7fff28772515 <+11>: movq   (%rdi,%rax), %rax
                    0x7fff28772519 <+15>: movq   0xaa0(%rax), %rax    ;this instruction can get debuggable offset
                    0x7fff28772520 <+22>: movb   0xc(%rax), %al
                    0x7fff28772523 <+25>: popq   %rbp
                    0x7fff28772524 <+26>: retq
                 */
                webpageproxy_debuggable_offset = ysc_OffsetFromInstruction((unsigned char *)_allowsRemoteInspection + 15);
#else
                webpageproxy_debuggable_offset = ysc_OffsetFromInstruction((unsigned char *)_allowsRemoteInspection);
#endif
            }
        }
    }
    return webpageproxy_debuggable_offset;
}


YSCFuncAddr YSCGetStringImplOperatorNSStringFunc(void) {
    static YSCFuncAddr stringimploperatornsstring_func;
    //WTF::StringImpl::operator NSString*()
    ysc_GetFuncAddrHelper(&stringimploperatornsstring_func, "_ZN3WTF10StringImplcvP8NSStringEv");
    return stringimploperatornsstring_func;
}

YSCFuncAddr YSCGetStringfromUTF8Func(void) {
    static YSCFuncAddr stringfromutf8_func;
    //WTF::String::fromUTF8(unsigned char const*, unsigned long)
    ysc_GetFuncAddrHelper(&stringfromutf8_func, "_ZN3WTF6String8fromUTF8EPKhm");
    return stringfromutf8_func;
}

YSCFuncAddr YSCGetStringImpldestroyFunc(void) {
    static YSCFuncAddr stringimpldestroy_func;
    //WTF::StringImpl::destroy(WTF::StringImpl*)
    ysc_GetFuncAddrHelper(&stringimpldestroy_func, "_ZN3WTF10StringImpl7destroyEPS0_");
    return stringimpldestroy_func;
}
