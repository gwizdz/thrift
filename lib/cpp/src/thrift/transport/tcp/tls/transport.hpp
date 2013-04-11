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

#ifndef _THRIFT_TRANSPORT_TCP_TLS_TRANSPORT_HPP_
#define _THRIFT_TRANSPORT_TCP_TLS_TRANSPORT_HPP_

#include <thrift/transport/tcp/transport.hpp>
#include <boost/asio/ssl.hpp>
#include <thrift/server/tcp/tls/context.hpp>
#include <boost/shared_ptr.hpp>

namespace apache { namespace thrift { namespace transport {
namespace tcp { namespace tls {

using apache::thrift::server::tcp::tls::context;
typedef boost::shared_ptr<context> context_ptr;

using apache::thrift::transport::tcp::transport;

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket;

} // namespace tls

template <>
struct transport<tls::socket> : basic_transport<tls::socket, transport>
{
  typedef tls::socket::next_layer_type& socket_reference;

  transport(std::string const& address, port_type port, tls::context_ptr context);
  transport(std::string const& address, std::string const& port, tls::context_ptr context);

  void open();

  socket_reference get_socket();

private:
  tls::socket socket;
  tls::context_ptr context;
};

} // namespace tcp
} // namespace transport
} // namespace thrift
} // namespace apache

#include <thrift/transport/tcp/tls/impl/transport.ipp>

#endif // _THRIFT_TRANSPORT_TCP_TLS_TRANSPORT_HPP_