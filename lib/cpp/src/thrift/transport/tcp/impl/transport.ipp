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

#ifndef _THRIFT_TRANSPORT_TCP_TRANSPORT_IPP_
#define _THRIFT_TRANSPORT_TCP_TRANSPORT_IPP_

#include <thrift/transport/tcp/transport.hpp>

namespace apache { namespace thrift { namespace transport { namespace tcp {

#if !defined(HAS_INHERITING_CONSTRUCTORS)
BOOST_FORCEINLINE
transport<boost::asio::ip::tcp::socket>::transport(std::string const& address, port_type port)
  : basic_transport(address, port), socket(*io_service)
{}

BOOST_FORCEINLINE 
transport<boost::asio::ip::tcp::socket>::transport(std::string const& address, std::string const& port)
  : basic_transport(address, port), socket(*io_service)
{}
#endif

transport<boost::asio::ip::tcp::socket>::socket_reference
  transport<boost::asio::ip::tcp::socket>::get_socket()
{
  return socket;
}

} // namespace tcp
} // namespace transport
} // namespace thrift
} // namespace apache

#endif // _THRIFT_TRANSPORT_TCP_TRANSPORT_IPP_