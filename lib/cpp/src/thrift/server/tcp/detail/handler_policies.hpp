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

#ifndef _THRIFT_SERVER_TCP_DETAIL_HANDLER_POLICIES_HPP_
#define _THRIFT_SERVER_TCP_DETAIL_HANDLER_POLICIES_HPP_

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

namespace apache { namespace thrift { namespace server { namespace tcp { namespace detail {

// for non-concurrent handlers execution
struct default_handler_policy
{
#ifndef _MSC_VER
protected:
#endif
  explicit default_handler_policy( boost::asio::io_service& ) {}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES  // has r-value references
  template <class Handler>
  Handler&& safe_handler(Handler&& handler)
  {
    return std::forward<Handler&&>(handler);
  }
#else
  template <class Handler>
  Handler safe_handler(Handler handler)
  {
    return handler;
  }
#endif
};

struct concurrent_handler_execution_policy
{
#ifndef _MSC_VER
protected:
#endif
  explicit concurrent_handler_execution_policy( boost::asio::io_service& io_service ) : strand_( io_service )
  {}

  template <class Handler>
  boost::asio::detail::wrapped_handler<boost::asio::io_service::strand, Handler> safe_handler(Handler handler)
  {
    return strand_.wrap(handler);
  }

private:  
  /// Strand to ensure the connection's handlers are not called concurrently.
  boost::asio::io_service::strand strand_;
};

} // namespace detail
} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_DETAIL_HANDLER_POLICIES_HPP_
