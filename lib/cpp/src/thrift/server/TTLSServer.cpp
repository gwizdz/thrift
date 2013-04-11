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

#include <thrift/server/TTLSServer.h>
#include <thrift/server/tcp/tls/server.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

namespace apache { namespace thrift { namespace server {

namespace {
  void update_context( tcp::tls::context& ctx, TLSContext const& inCtx )
  {
    ctx.certificate_authority(inCtx.certificateAuthority);
    ctx.certificate(inCtx.certificate);
    ctx.private_key(inCtx.privateKey);
    ctx.password(inCtx.password);
    ctx.ciphers(inCtx.ciphers);
  }
}

boost::shared_ptr<apache::thrift::server::TServer> getTLSServer
(
  const boost::shared_ptr<apache::thrift::TProcessor>& processor,
  const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
  const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
  std::string const& address,
  port_t port,
  TLSContext const& tlsContext
)
{
  boost::shared_ptr<tcp::tls::server> server;
  try
  {
    server = boost::make_shared<tcp::tls::server>(processor, transportFactory,
      protocolFactory, address, boost::lexical_cast<std::string>(port));
    update_context(*server, tlsContext);
  }
  catch (boost::bad_lexical_cast const& e)
  {
    BOOST_THROW_EXCEPTION( apache::thrift::TException(e.what()) );
  }
  return server;
}

boost::shared_ptr<apache::thrift::server::TServer> getTLSServer
(
  const boost::shared_ptr<apache::thrift::TProcessor>& processor,
  const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
  const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
  std::string const& address,
  port_t p,
  TLSContext const& tlsContext,
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

  switch (ioConcurrencyStrategy)
  {
  default:
    BOOST_THROW_EXCEPTION( apache::thrift::TException("Invalid IOConcurrencyStrategy.") );
  case IOServiceInThreadPool:
    {
      boost::shared_ptr<tcp::tls::server_io_service_in_thread_pool> server =
        boost::make_shared<tcp::tls::server_io_service_in_thread_pool>(processor, transportFactory,
        protocolFactory, address, port, concurrencyHint);
      update_context(*server, tlsContext);
      return server;
    }
  case IOServicePerCore:
    {
      boost::shared_ptr<tcp::tls::server_io_service_per_core> server =
        boost::make_shared<tcp::tls::server_io_service_per_core>(processor, transportFactory,
        protocolFactory, address, port, concurrencyHint);
      update_context(*server, tlsContext);
      return server;
    }
  }
}

} // namespace server
} // namespace apache
} // namespace thrift
