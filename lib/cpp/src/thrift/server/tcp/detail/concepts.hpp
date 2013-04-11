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

#ifndef _THRIFT_SERVER_TCP_DETAIL_CONCEPTS_HPP_
#define _THRIFT_SERVER_TCP_DETAIL_CONCEPTS_HPP_

#include <thrift/config.hpp>
#include <boost/concept_check.hpp>

#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_class.hpp>

#include <boost/concept/detail/concept_def.hpp>

namespace apache { namespace thrift { namespace server { namespace tcp { namespace detail { namespace concepts {

namespace detail {
  // helpers for concepts
  boost::asio::io_service& get_io_service();
} // namespace detail

// Requirements for Connection template parameter of basic_server
BOOST_concept(ConnectionConcept,(TT))
{
  BOOST_CONCEPT_USAGE(ConnectionConcept) {
#ifndef BOOST_NO_CXX11_AUTO_DECLARATIONS
    auto connection = TT::create(detail::get_io_service(), get_server());
#endif
  }
private:
  typename TT::server_reference get_server();
};

// Requirements for IOServingPolicy template parameter of basic_server
BOOST_concept(IOServingConcept,(TT))
{
  BOOST_CONCEPT_USAGE(IOServingConcept) {
    TT& c = make();
    c.run_impl();
    c.stop_impl();
    c.get_io_service_impl();
#if  !defined(BOOST_NO_CXX11_STATIC_ASSERT) && !defined(BOOST_NO_CXX11_DECLTYPE)
    static_assert(boost::is_same< decltype( c.get_io_service_impl() ), boost::asio::io_service&>::value, "IOServingConcept::get_io_service() is required to return boost::asio::io_service&");
#endif
  }
private:
  TT& make(); // helper
};

// Requirements for basic_connection's HandlerPolicy
template<class X>
struct HandlerPolicyConcept : X
{
  BOOST_CONCEPT_USAGE(HandlerPolicyConcept) {
#if !defined(BOOST_NO_CXX11_LAMBDAS) && !defined(BOOST_NO_CXX11_AUTO_DECLARATIONS)
    auto handler = []() {};
    auto h = this->safe_handler( handler);
    h();
#endif
  }
};

} // namespace concepts
} // namespace detail
} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#include <boost/concept/detail/concept_undef.hpp>

#endif // _THRIFT_SERVER_TCP_DETAIL_CONCEPTS_HPP_
