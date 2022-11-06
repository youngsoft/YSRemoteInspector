# YSRemoteInspector

[![CI Status](https://img.shields.io/travis/oubaiquan/YSRemoteInspector.svg?style=flat)](https://travis-ci.org/oubaiquan/YSRemoteInspector)
[![Version](https://img.shields.io/cocoapods/v/YSRemoteInspector.svg?style=flat)](https://cocoapods.org/pods/YSRemoteInspector)
[![License](https://img.shields.io/cocoapods/l/YSRemoteInspector.svg?style=flat)](https://cocoapods.org/pods/YSRemoteInspector)
[![Platform](https://img.shields.io/cocoapods/p/YSRemoteInspector.svg?style=flat)](https://cocoapods.org/pods/YSRemoteInspector)

## Example

To run the example project, clone the repo, and run `pod install` from the Example directory first.

在调试iOS上的JSContext和WebView时，我们只能通过Safari中的“开发”菜单进行。 这个开源库的目的就是用来打破这些限制，我们可以不依赖于Safari，甚至是通过无线网络的方式来实现调试工作。
对于JSContext和WebView来说，系统内部实现了一个可调试对象(Debuggable)。Debuggable对象实现了一个RemoteControllableTarget接口。我们可以通过这个接口使用WIP协议来获取内部的调试数据。iOS的WebKit并未公开这些能力。


对于YSRemoteInspector来说，它实现了两个功能。一个是用来调试JSContext和WebKit。一个是用来监视Safari和本应用进行通信的所有WIP协议数据。

对于第一个功能来说，为了调试JSContext和WebView。你需要创建一个YSRemoteInspector对象，这个是一个远程检查器对象。你需要用JSContext或者WebView来初始化它，然后调用connect方法连接上调试目标，最后通过dispatchMessage方法来进行通信。通信数据是一个NSDictionary格式的数据。这个字典由三部分组成：
```
id: 一个自增的数字唯一标识。
method:  一个字符串表示的方法，它由”Domain.Command”组成，用来指定要获取的信息方法 
params:   用来指定method所附带的参数。
```
dispatchMessage方法会返回method调用指定的信息，格式也是一个NSDictionary。

当然具体的消息格式以及内容规约你可以参考：[https://github.com/WebKit/WebKit/tree/main/Source/JavaScriptCore/inspector/protocol](https://github.com/WebKit/WebKit/tree/main/Source/JavaScriptCore/inspector/protocol) 中的内容。


对于第二个功能来说。你可以通过调用类方法enableProtocolMonitor来开启消息的监听，当开启消息监听后，所有通过Safari来进行调试的通信数据都可以被监控到。你可以通过这些监控数据来分析和查看Safari是如何对JSContext/WebView进行调试的。


那么YSRemoteInspector到底有什么用呢？

我想应该有两个用处：
  1.你可以不依赖Safari来实现JSContext/WebView 对象的调试。
   2.你可以通过其他调试工具，比如google的CDT工具来实现对JSContext/WebKit的调试。

## Requirements

## Installation

YSRemoteInspector is available through [CocoaPods](https://cocoapods.org). To install
it, simply add the following line to your Podfile:

```ruby
pod 'YSRemoteInspector'
```

## Author

youngsoft, obq0387_cn@sina.com

## License

YSRemoteInspector is available under the MIT license. See the LICENSE file for more info.
