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

#include <thrift/server/TTCPServer.h>
#include <thrift/server/tcp/server.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

namespace apache { namespace thrift { namespace server {

boost::shared_ptr<apache::thrift::server::TServer> getTCPServer
(
  const boost::shared_ptr<apache::thrift::TProcessor>& processor,
  const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
  const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
  std::string const& address,
  port_t port
)
{
  try
  {
    return boost::make_shared<tcp::server>(processor, transportFactory,
      protocolFactory, address, boost::lexical_cast<std::string>(port));
  }
  catch (boost::bad_lexical_cast const& e)
  {
    BOOST_THROW_EXCEPTION( apache::thrift::TException(e.what()) );
  }
}

boost::shared_ptr<apache::thrift::server::TServer> getTCPServer
(
  const boost::shared_ptr<apache::thrift::TProcessor>& processor,
  const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
  const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
  std::string const& address,
  port_t p,
  IOConcurrencyStrategy ioConcurrencyStrategy,
  std::size_t concurrencyHint
)
{
  std::string port;
  try
  {
    port = boost::lexical_cast<std::string>(p);
  }
  catch (boost::bad_lexical_cast const& e)
  {
    BOOST_THROW_EXCEPTION( apache::thrift::TException(e.what()) );
  }

  boost::shared_ptr<apache::thrift::server::TServer> server;
  switch ( ioConcurrencyStrategy )
  {
  default:
    BOOST_THROW_EXCEPTION( apache::thrift::TException("Invalid IOConcurrencyStrategy.") );
  case IOServiceInThreadPool:
    boost::make_shared<tcp::server_io_service_in_thread_pool>(processor, transportFactory,
      protocolFactory, address, port, concurrencyHint);
    break;
  case IOServicePerCore:
    boost::make_shared<tcp::server_io_service_per_core>(processor, transportFactory,
      protocolFactory, address, port, concurrencyHint);
    break;
  }

  return server;
}

} // namespace server
} // namespace apache
} // namespace thrift
