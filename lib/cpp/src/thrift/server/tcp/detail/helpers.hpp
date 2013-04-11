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

#ifndef _THRIFT_SERVER_TCP_DETAIL_HELPERS_HPP_
#define _THRIFT_SERVER_TCP_DETAIL_HELPERS_HPP_

#include <boost/type_traits/add_lvalue_reference.hpp>
#include <boost/mpl/if.hpp>
#include <thrift/server/TServer.h>
#include <thrift/server/tcp/tls/context.hpp>
#include <thrift/server/tcp/detail/traits.hpp>

namespace apache { namespace thrift { namespace server { namespace tcp { namespace detail {

//  tcp_server_base   -----------------------------------------------//
typedef apache::thrift::server::TServer tcp_server_base;

//  tls_server_base   -----------------------------------------------//
struct tls_server_base : apache::thrift::server::TServer, apache::thrift::server::tcp::tls::context
{
protected:
  explicit tls_server_base(boost::shared_ptr<apache::thrift::TProcessor> const& processor) : apache::thrift::server::TServer(processor)
  {}
};

//  server_type   -----------------------------------------------//
template <class Connection>
struct server_type {
  typedef typename boost::mpl::if_<is_connection_secured<Connection>, tls_server_base, tcp_server_base>::type value_type;
  typedef typename boost::add_lvalue_reference<value_type>::type reference_type;
};

//  security_policy_chooser   -----------------------------------------------//
template <class Connection>
struct security_policy_chooser : server_type<Connection>::value_type
{
  typedef typename server_type<Connection>::value_type inherited;
protected:
  explicit security_policy_chooser(boost::shared_ptr<apache::thrift::TProcessor> const& processor) : inherited(processor)
  {}
};

} // namespace detail
} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_DETAIL_HELPERS_HPP_
