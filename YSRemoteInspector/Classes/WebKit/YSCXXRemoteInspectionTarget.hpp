//
//  YSCXXRemoteInspectionTarget.hpp
//  YSRemoteInspector
//
//  Created by youngsoft on 2022/8/29.
//

/*
 * Copyright (C) 2013, 2015 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef YSCXXRemoteInspectionTarget_h
#define YSCXXRemoteInspectionTarget_h

namespace YSCXX {

class FrontendChannel;

class  RemoteControllableTarget {
public:
    virtual ~RemoteControllableTarget() = 0;

    virtual void connect(FrontendChannel&, bool isAutomaticConnection = false, bool immediatelyPause = false) = 0;
    virtual void disconnect(FrontendChannel&) = 0;

    virtual unsigned int type() const = 0;
    virtual bool remoteControlAllowed() const = 0;
    virtual void dispatchMessageFromRemote(String&& message) = 0;
};


class  RemoteInspectionTarget : public RemoteControllableTarget {
public:
    virtual String name() const = 0; // ITML JavaScript Page ServiceWorker WebPage
    virtual String url() const = 0; // Page ServiceWorker WebPage
    virtual bool hasLocalDebugger() const = 0;

    virtual void setIndicating(bool) = 0;// Default is to do nothing.

    virtual bool automaticInspectionAllowed()  = 0;
    virtual void pauseWaitingForAutomaticInspection() = 0;
    virtual void unpauseForInitializedInspector() = 0;
};

}

#endif /* YSCXXRemoteInspectionTarget_h */
