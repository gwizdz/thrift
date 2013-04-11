# Copyright (c) 2013 Lukasz Gwizdz.
# Home at: https://github.com/gwizdz/thrift
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements. See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership. The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.
#
cmake_minimum_required(VERSION 2.8)

project(libthrift CXX)


set(libthrift_SOURCES  src/thrift/Thrift.cpp
                       src/thrift/TApplicationException.cpp
                       src/thrift/VirtualProfiling.cpp )

set(protocol_SOURCES   src/thrift/protocol/TDebugProtocol.cpp
                       src/thrift/protocol/TDenseProtocol.cpp
                       src/thrift/protocol/TJSONProtocol.cpp
                       src/thrift/protocol/TBase64Utils.cpp )

set(transport_SOURCES  src/thrift/transport/TTransportException.cpp
                       src/thrift/transport/TFDTransport.cpp
                       src/thrift/transport/TFileTransport.cpp
                       src/thrift/transport/TSimpleFileTransport.cpp
                       src/thrift/transport/THttpTransport.cpp
                       src/thrift/transport/THttpClient.cpp
                       src/thrift/transport/THttpServer.cpp
                       src/thrift/transport/TSocket.cpp
                       src/thrift/transport/TPipe.cpp
                       src/thrift/transport/TPipeServer.cpp
                       src/thrift/transport/TSSLSocket.cpp
                       src/thrift/transport/TSocketPool.cpp
                       src/thrift/transport/TServerSocket.cpp
                       src/thrift/transport/TSSLServerSocket.cpp
                       src/thrift/transport/TTransportUtils.cpp
                       src/thrift/transport/TBufferTransports.cpp
                       src/thrift/transport/TTCPTransport.cpp
                       src/thrift/transport/TTLSTransport.cpp )

set(transport_tcp_SOURCES  src/thrift/transport/tcp/io_service_access.cpp )

set(server_SOURCES     src/thrift/server/TSimpleServer.cpp
                       src/thrift/server/TThreadPoolServer.cpp
                       src/thrift/server/TThreadedServer.cpp
                       src/thrift/server/TTCPServer.cpp
                       src/thrift/server/TTLSServer.cpp )
if (UNIX)
  set(server_SOURCES ${server_SOURCES} src/thrift/server/TServer.cpp)
endif()

set(server_tcp_SOURCES  src/thrift/server/tcp/detail/io_service_pool.cpp )
set(server_tcp_tls_SOURCES  src/thrift/server/tcp/tls/context.cpp )

set(async_SOURCES      src/thrift/async/TAsyncChannel.cpp )
set(processor_SOURCES  src/thrift/processor/PeekProcessor.cpp )

set(concurrency_SOURCES src/thrift/concurrency/ThreadManager.cpp
                        src/thrift/concurrency/TimerManager.cpp
                        src/thrift/concurrency/Util.cpp
                        src/thrift/concurrency/BoostThreadFactory.cpp
                        src/thrift/concurrency/BoostMonitor.cpp
                        src/thrift/concurrency/BoostMutex.cpp )


set(libthrift_HEADERS    src/thrift/TDispatchProcessor.h
                         src/thrift/Thrift.h
                         src/thrift/TReflectionLocal.h
                         src/thrift/TProcessor.h
                         src/thrift/TApplicationException.h
                         src/thrift/TLogging.h
                         src/thrift/config.hpp
                         src/thrift/output_inserters.hpp )

set(concurrency_HEADERS  src/thrift/concurrency/BoostThreadFactory.h
                         src/thrift/concurrency/Exception.h
                         src/thrift/concurrency/Mutex.h
                         src/thrift/concurrency/Monitor.h
                         src/thrift/concurrency/PlatformThreadFactory.h
                         src/thrift/concurrency/PosixThreadFactory.h
                         src/thrift/concurrency/Thread.h
                         src/thrift/concurrency/ThreadManager.h
                         src/thrift/concurrency/TimerManager.h
                         src/thrift/concurrency/FunctionRunner.h
                         src/thrift/concurrency/Util.h )

set(protocol_HEADERS     src/thrift/protocol/TBinaryProtocol.h
                         src/thrift/protocol/TBinaryProtocol.tcc
                         src/thrift/protocol/TCompactProtocol.h
                         src/thrift/protocol/TCompactProtocol.tcc
                         src/thrift/protocol/TDenseProtocol.h
                         src/thrift/protocol/TDebugProtocol.h
                         src/thrift/protocol/TBase64Utils.h
                         src/thrift/protocol/TJSONProtocol.h
                         src/thrift/protocol/TProtocolTap.h
                         src/thrift/protocol/TProtocolException.h
                         src/thrift/protocol/TVirtualProtocol.h
                         src/thrift/protocol/TProtocol.h )

set(transport_HEADERS    src/thrift/transport/TFDTransport.h
                         src/thrift/transport/TFileTransport.h
                         src/thrift/transport/TSimpleFileTransport.h
                         src/thrift/transport/TServerSocket.h
                         src/thrift/transport/TSSLServerSocket.h
                         src/thrift/transport/TServerTransport.h
                         src/thrift/transport/THttpTransport.h
                         src/thrift/transport/THttpClient.h
                         src/thrift/transport/THttpServer.h
                         src/thrift/transport/TSocket.h
                         src/thrift/transport/TPipe.h
                         src/thrift/transport/TPipeServer.h
                         src/thrift/transport/TSSLSocket.h
                         src/thrift/transport/TSocketPool.h
                         src/thrift/transport/TVirtualTransport.h
                         src/thrift/transport/TTransport.h
                         src/thrift/transport/TTransportException.h
                         src/thrift/transport/TTransportUtils.h
                         src/thrift/transport/TBufferTransports.h
                         src/thrift/transport/TShortReadTransport.h
                         src/thrift/transport/TZlibTransport.h
                         src/thrift/transport/TTCPTransport.h
                         src/thrift/transport/TTLSTransport.h )

set(transport_tcp_HEADERS  src/thrift/transport/tcp/io_service_access.hpp
                           src/thrift/transport/tcp/basic_transport.hpp
                           src/thrift/transport/tcp/impl/basic_transport.ipp
                           src/thrift/transport/tcp/detail/socket_ops.hpp
                           src/thrift/transport/tcp/transport.hpp
                           src/thrift/transport/tcp/impl/transport.ipp)

set(transport_tcp_tls_HEADERS  src/thrift/transport/tcp/tls/transport.hpp
                               src/thrift/transport/tcp/tls/impl/transport.ipp )

set(server_HEADERS       src/thrift/server/TServer.h
                         src/thrift/server/TSimpleServer.h
                         src/thrift/server/TThreadPoolServer.h
                         src/thrift/server/TThreadedServer.h
                         src/thrift/server/TNonblockingServer.h
                         src/thrift/server/TTCPServer.h
                         src/thrift/server/TTLSServer.h )

set(server_tcp_HEADERS   src/thrift/server/tcp/server.hpp
                         src/thrift/server/tcp/connection.hpp
                         src/thrift/server/tcp/basic_server.hpp
                         src/thrift/server/tcp/impl/basic_server.ipp
                         src/thrift/server/tcp/basic_connection.hpp
                         src/thrift/server/tcp/impl/basic_connection.ipp
                         src/thrift/server/tcp/request_handler.hpp
                         src/thrift/server/tcp/stream_traits.hpp
                         src/thrift/server/tcp/detail/concepts.hpp
                         src/thrift/server/tcp/detail/handler_policies.hpp
                         src/thrift/server/tcp/detail/io_serving_policies.hpp
                         src/thrift/server/tcp/detail/helpers.hpp
                         src/thrift/server/tcp/detail/traits.hpp
                         src/thrift/server/tcp/detail/io_service_pool.hpp )

set(server_tcp_tls_HEADERS  src/thrift/server/tcp/tls/connection.hpp
                            src/thrift/server/tcp/tls/server.hpp
                            src/thrift/server/tcp/tls/context.hpp )

set(processor_HEADERS    src/thrift/processor/PeekProcessor.h
                         src/thrift/processor/StatsProcessor.h )

set(async_HEADERS    src/thrift/async/TAsyncChannel.h
                     src/thrift/async/TAsyncDispatchProcessor.h
                     src/thrift/async/TAsyncProcessor.h
                     src/thrift/async/TAsyncBufferProcessor.h
                     src/thrift/async/TAsyncProtocolProcessor.h
                     src/thrift/async/TEvhttpClientChannel.h
                     src/thrift/async/TEvhttpServer.h )

set(windows_HEADERS         src/thrift/windows/config.h
                            src/thrift/windows/force_inc.h
                            src/thrift/windows/GetTimeOfDay.h
                            src/thrift/windows/Operators.h
                            src/thrift/windows/SocketPair.h
                            src/thrift/windows/StdAfx.h
                            src/thrift/windows/TargetVersion.h
                            src/thrift/windows/TWinsockSingleton.h
                            src/thrift/windows/WinFcntl.h
                            src/thrift/windows/tr1/functional )

set(windows_SOURCES         src/thrift/windows/GetTimeOfDay.cpp
                            src/thrift/windows/SocketPair.cpp
                            src/thrift/windows/StdAfx.cpp
                            src/thrift/windows/TWinsockSingleton.cpp
                            src/thrift/windows/WinFcntl.cpp )

source_group(async FILES ${async_HEADERS} ${async_SOURCES})
source_group(concurrency FILES ${concurrency_HEADERS} ${concurrency_SOURCES})
source_group(processor FILES ${processor_HEADERS} ${processor_SOURCES})
source_group(protocol FILES ${protocol_HEADERS} ${protocol_SOURCES})
source_group(server FILES ${server_HEADERS} ${server_SOURCES})
source_group(server\\tcp FILES ${server_tcp_HEADERS} ${server_tcp_SOURCES})
source_group(server\\tcp\\tls FILES ${server_tcp_tls_HEADERS} ${server_tcp_tls_SOURCES})
source_group(transport FILES ${transport_HEADERS} ${transport_SOURCES})
source_group(transport\\tcp FILES ${transport_tcp_HEADERS} ${transport_tcp_SOURCES})
source_group(transport\\tcp\\tls FILES ${transport_tcp_tls_HEADERS})
if (WIN32)
  source_group(windows FILES ${windows_SOURCES} ${windows_HEADERS})
endif()

if (MSVC_VERSION LESS 1600)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/stdint.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/stdint.h)
endif()

if (UNIX)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h)
endif()

set(SRC   ${libthrift_SOURCES}
          ${protocol_SOURCES}
          ${transport_SOURCES}
          ${transport_tcp_SOURCES}
          ${server_SOURCES}
          ${server_tcp_SOURCES}
          ${server_tcp_tls_SOURCES}
          ${async_SOURCES}
          ${processor_SOURCES}
          ${concurrency_SOURCES} )
if (WIN32)
  set(SRC ${SRC} ${windows_SOURCES})
endif()

set(HEADERS     ${libthrift_HEADERS}
                ${concurrency_HEADERS}
                ${protocol_HEADERS}
                ${transport_HEADERS}
                ${transport_tcp_HEADERS}
                ${transport_tcp_tls_HEADERS}
                ${server_HEADERS}
                ${processor_HEADERS}
                ${async_HEADERS} )

if (WIN32)
  set(HEADERS ${HEADERS} ${windows_HEADERS})
endif()


add_library(${PROJECT_NAME} STATIC ${SRC} ${HEADERS} )
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${PROJECT_SOURCE_DIR}/src)
if (WIN32)
  include_directories(${PROJECT_SOURCE_DIR}/src/thrift/windows)
  add_definitions(-D_WIN32 -D_LIB)
endif()

add_definitions(-DUSE_BOOST_THREAD)
add_definitions(-DHAVE_CONFIG_H)
set(CMAKE_C_FLAGS_DEBUG , "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")

macro(get_gnu_cxx_version major minor)
  if (CMAKE_COMPILER_IS_GNUCXX)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
    list(GET GCC_VERSION_COMPONENTS 0 ${major})
    list(GET GCC_VERSION_COMPONENTS 1 ${minor})
  endif()
endmacro()

if (CMAKE_COMPILER_IS_GNUCXX)
  get_gnu_cxx_version(gnu_major gnu_minor)
  if ( (gnu_major EQUAL 4 AND gnu_minor EQUAL 3)
    OR (gnu_minor GREATER 3 AND gnu_minor LESS 7 AND gnu_major EQUAL 4))
    set(CMAKE_CXX_FLAGS "-std=c++0x")
  elseif( (gnu_major EQUAL 4 AND gnu_minor EQUAL 7)
    OR (gnu_minor GREATER 7 AND gnu_major EQUAL 4) )
    set(CMAKE_CXX_FLAGS "-std=c++11")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W -pedantic -O2")
else()
  if (UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -W -pedantic -O2")
  endif ()
endif()

if (MSVC)
  string(REGEX REPLACE "/STACK:[0-9]+" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

set(Boost_DEBUG 1)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost)
if (Boost_FOUND)
# message("boost includes at: " ${Boost_INCLUDE_DIR})
# message("boost libraries at:"${Boost_LIBRARY_DIRS})
  include_directories(${Boost_INCLUDE_DIR})
  link_directories(${Boost_LIBRARY_DIRS})
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()

find_package(openssl)
if (OPENSSL_FOUND)
# message("\nOpenSSL " ${OPENSSL_VERSION})
# message("\nOpenSSL includes at: " ${OPENSSL_INCLUDE_DIR})
  include_directories(${OPENSSL_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
endif()

set_target_properties (${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
