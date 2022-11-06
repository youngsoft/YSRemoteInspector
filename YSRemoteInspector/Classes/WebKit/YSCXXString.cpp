//
//  YSCXXString.cpp
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/8/29.
//

#include "YSCXXString.hpp"
#include "YSCInspectorOffset.h"

namespace YSCXX {

RefPtr::~RefPtr() {
    if (m_ptr != nullptr) {
        void(*StringImpldestroy)(void *) =(void(*)(void *))YSCGetStringImpldestroyFunc();
        StringImpldestroy(m_ptr);
        m_ptr = nullptr;
    }
}

RefPtr::operator void*() const {
    void* (*StringImplOperatorNSString)(void*) = (void* (*)(void*))YSCGetStringImplOperatorNSStringFunc();
    return StringImplOperatorNSString(m_ptr);
}

String String::fromUTF8(const char* characters, unsigned long  length) {
    String (*StringfromUTF8)(const char *, unsigned long) = (String (*)(const char *, size_t))YSCGetStringfromUTF8Func();
    return StringfromUTF8(characters, length);
}

String::operator void*() const {
     return m_impl;
}

}
