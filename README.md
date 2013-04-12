Extensions for Apache Thrift
============================
Copyright (C) 2012 - 2013 Lukasz Gwizdz.  
Home at: https://github.com/gwizdz/thrift  

In order to use my extensions you need to merge them with original Thrift source code.   
Provided extensions were tested with Thrift version 0.9.0 using g++ 4.7.2, clang++ 3.2   
(under Linux) and Visual Studio 2010, Visual Studio 2008 (under Windows). Provided   
source code is also compatible with Visual Studio 2008, however there are problems   
with Thrift source code under this version of Visual Studio (lack of stdint.h, which   
is solved by CMake script and missed includes in several Thrift headers). To build   
Thrift in Visual Studio 2008 you need to manually modify several files.

For more information, documentation, sources see http://thrift.apache.org/ .   


TCP & Generic SSL/TLS High Performance Server C++
-------------------------------------------------
Written using boost::asio and utilizes Proactor pattern. All io_service handling strategies are   
implemented and can be easily configured (see source code). It also supports Generic SSL/TLS   
connections, so can be seen as a better alternative for TNonblockingServer.  

###Modyfing Thrift tutorial sample:

####CppClient side:
```#include <thrift/transport/TTCPTransport.h>```

```// ...```

```shared_ptr<TTransport> socket = getTCPTransport("localhost", 9090);```  
```shared_ptr<TTransport> transport(new TFramedTransport(socket)); // <- TFramedTransport is required```  
```shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));```  
```CalculatorClient client(protocol);```  

####CppServer side:  

```#include <thrift/server/TTCPServer.h>```

```// ...```

```shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());```  
```shared_ptr<CalculatorHandler> handler(new CalculatorHandler());```  
```shared_ptr<TProcessor> processor(new CalculatorProcessor(handler));```  
```shared_ptr<TServerTransport> serverTransport(new TServerSocket(9090));```  

```// TFramedTransport is required```  
```shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());```  

```boost::shared_ptr<TServer> server_ptr = getTCPServer(processor, transportFactory, protocolFactory, "0.0.0.0", 9090);```  
```server->serve();```  


SOCKSv5 Transport C#
--------------------
Transport that utilizes SOCKS Protocol Version 5 in order to connect to the services behind   
SOCKS proxy. At this moment, only CONNECT command request and user authentication are  
implemented. The current version requires at least .NET Framework 4.0 (probably will be  
changed, so .NET Framework 3.5 will be sufficient).
