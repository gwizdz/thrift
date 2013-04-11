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

#include <thrift/transport/TTCPTransport.h>
#include <thrift/transport/tcp/transport.hpp>
#include <boost/make_shared.hpp>

namespace apache { namespace thrift { namespace transport {

boost::shared_ptr<TTransport> getTCPTransport
(
  std::string const& address,
  unsigned short port
)
{
  return boost::make_shared<tcp::transport<boost::asio::ip::tcp::socket> >
    (address, port);
}

boost::shared_ptr<TTransport> getTCPTransport
(
  std::string const& address,
  std::string const& port
)
{
  return boost::make_shared<tcp::transport<boost::asio::ip::tcp::socket> >
    (address, port);
}

} // namespace transport
} // namespace thrift
} // namespace apache
