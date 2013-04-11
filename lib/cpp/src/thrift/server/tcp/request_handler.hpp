// Copyright (c) 2013 Lukasz Gwizdz.
// Home at: https://github.com/gwizdz/thrift
/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _THRIFT_SERVER_TCP_REQUEST_HANDLER_HPP_
#define _THRIFT_SERVER_TCP_REQUEST_HANDLER_HPP_

#include <thrift/config.hpp>
#include <thrift/server/TServer.h>

namespace apache { namespace thrift { namespace server { namespace tcp {

struct request_handler
{
  request_handler
  (
    apache::thrift::server::TServer& server,
    boost::shared_ptr<apache::thrift::transport::TTransport> input,
    boost::shared_ptr<apache::thrift::transport::TTransport> output
  ) : event_handler( server.getEventHandler() )
  {
    inputTransport = server.getInputTransportFactory()->getTransport( input );
    outputTransport = server.getOutputTransportFactory()->getTransport( output );
    inputProtocol = server.getInputProtocolFactory()->getProtocol( inputTransport );
    outputProtocol = server.getOutputProtocolFactory()->getProtocol( outputTransport );

    if ( event_handler )
      conn_ctx = event_handler->createContext( inputProtocol, outputProtocol );
    else
#ifdef BOOST_NO_CXX11_NULLPTR
      conn_ctx = 0;
#else
      conn_ctx = nullptr;
#endif

    processor = get_processor( server, inputProtocol, outputProtocol, inputTransport );
  }

  void operator()()
  {
    if ( event_handler )
      event_handler->processContext( conn_ctx, inputTransport );

    processor->process( inputProtocol, outputProtocol, conn_ctx );
  }

  ~request_handler()
  {
    if ( event_handler )
      event_handler->deleteContext( conn_ctx, inputProtocol, outputProtocol );

    inputTransport->close();
    outputTransport->close();
  }

private:
  boost::shared_ptr<apache::thrift::TProcessor> get_processor
  (
    apache::thrift::server::TServer& server,
    boost::shared_ptr<apache::thrift::protocol::TProtocol> inputProtocol,
    boost::shared_ptr<apache::thrift::protocol::TProtocol> outputProtocol,
    boost::shared_ptr<apache::thrift::transport::TTransport> transport
  )
  {
    apache::thrift::TConnectionInfo connInfo;
    connInfo.input = inputProtocol;
    connInfo.output = outputProtocol;
    connInfo.transport = transport;
    return server.getProcessorFactory()->getProcessor(connInfo);
  }

  boost::shared_ptr<apache::thrift::server::TServerEventHandler> event_handler;

  boost::shared_ptr<apache::thrift::transport::TTransport> inputTransport;
  boost::shared_ptr<apache::thrift::transport::TTransport> outputTransport;
  boost::shared_ptr<apache::thrift::protocol::TProtocol> inputProtocol;
  boost::shared_ptr<apache::thrift::protocol::TProtocol> outputProtocol;

  boost::shared_ptr<apache::thrift::TProcessor> processor;
  void* conn_ctx;
};

} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_REQUEST_HANDLER_HPP_
