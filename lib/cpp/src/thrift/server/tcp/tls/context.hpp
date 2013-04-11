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

#ifndef _THRIFT_SERVER_TCP_TLS_CONTEXT_HPP_
#define _THRIFT_SERVER_TCP_TLS_CONTEXT_HPP_

#include <thrift/config.hpp>

#ifdef BOOST_NO_CXX11_SMART_PTR
# include <boost/scoped_ptr.hpp>
#else
# include <memory>
#endif

namespace boost {
namespace asio {
namespace ssl {

  class context;

} // namespace ssl
} // namespace asio
} // namespace boost



namespace apache { namespace thrift { namespace server { namespace tcp { namespace tls {

struct context {

  context();
  ~context();

  operator boost::asio::ssl::context&();

  void certificate_authority(std::string const& ca);

  void certificate(std::string const& cert);

  void private_key(std::string const& priv_key);

  void password( std::string const& p );

  void ciphers(std::string const& ciphers);

  boost::asio::ssl::context& get_handle();

private:
  struct context_impl_;
#ifdef BOOST_NO_CXX11_SMART_PTR
  boost::scoped_ptr<context_impl_> pimpl;
#else
  std::unique_ptr<context_impl_> pimpl;
#endif
};

} // namespace tls
} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_TLS_CONTEXT_HPP_
