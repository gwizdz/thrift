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

#ifndef _THRIFT_SERVER_TTLSSERVER_H_
#define _THRIFT_SERVER_TTLSSERVER_H_

#include <thrift/server/TServer.h>
#include <thrift/server/TTCPServer.h>
#include <boost/shared_ptr.hpp>
#include <string>

namespace apache { namespace thrift { namespace server {

typedef unsigned short port_t;

struct TLSContext
{
  std::string certificateAuthority;
  std::string certificate;
  std::string privateKey;
  std::string password;
  std::string ciphers;
};

boost::shared_ptr<apache::thrift::server::TServer> getTLSServer
(
  const boost::shared_ptr<apache::thrift::TProcessor>& processor,
  const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
  const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
  std::string const& address,
  port_t port,
  TLSContext const& tlsContext
);

boost::shared_ptr<apache::thrift::server::TServer> getTLSServer
(
  const boost::shared_ptr<apache::thrift::TProcessor>& processor,
  const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
  const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
  std::string const& address,
  port_t port,
  TLSContext const& tlsContext,
  IOConcurrencyStrategy ioConcurrencyStrategy,
  std::size_t concurrencyHint = 2
);

} // namespace server
} // namespace apache
} // namespace thrift

#endif // _THRIFT_SERVER_TTLSSERVER_H_
