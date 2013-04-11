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

#ifndef _THRIFT_SERVER_TCP_DETAIL_TRAITS_HPP_
#define _THRIFT_SERVER_TCP_DETAIL_TRAITS_HPP_

#include <thrift/config.hpp>
#include <boost/type_traits/integral_constant.hpp>

// Forward.
namespace boost {
namespace asio {
namespace ssl {
template <typename Stream>
class stream;
} // namespace ssl
} // namespace asio
} // namespace boost

namespace apache { namespace thrift { namespace server { namespace tcp {

// Forward.
template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
class basic_connection;

namespace detail {

//  is_connection_secured   -----------------------------------------------//
template <class Connection>
struct is_connection_secured : boost::false_type
{};

namespace tls = boost::asio::ssl;
template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
struct is_connection_secured<basic_connection<tls::stream<Stream>, StreamTraits, HandlerPolicy> > : boost::true_type
{};
// is_connection_secured

//  is_concurrent_io_serving_policy   -----------------------------------------------//
template <class IOServingPolicy>
struct is_concurrent_io_serving_policy : boost::false_type
{};

// Forward declarations.
struct io_service_run_in_thread_pool;
struct io_service_per_core;

template <>
struct is_concurrent_io_serving_policy<io_service_run_in_thread_pool> : boost::true_type
{};

template <>
struct is_concurrent_io_serving_policy<io_service_per_core> : boost::true_type
{};
// is_concurrent_io_serving_policy

} // namespace detail
} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_DETAIL_TRAITS_HPP_
