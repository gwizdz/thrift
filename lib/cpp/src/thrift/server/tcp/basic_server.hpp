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

#ifndef _THRIFT_SERVER_TCP_BASIC_SERVER_HPP_
#define _THRIFT_SERVER_TCP_BASIC_SERVER_HPP_

#include <thrift/config.hpp>
#include <thrift/server/tcp/detail/helpers.hpp>
#include <thrift/server/tcp/detail/io_serving_policies.hpp>
#include <thrift/server/tcp/detail/concepts.hpp>
#include <thrift/server/TServer.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/noncopyable.hpp>

namespace apache { namespace thrift { namespace server { namespace tcp {

template <class Connection, class IOServingPolicy = detail::default_io_serving_policy>
class basic_server : public detail::security_policy_chooser<Connection>, private IOServingPolicy, private boost::noncopyable
{
  BOOST_CONCEPT_ASSERT((detail::concepts::ConnectionConcept<Connection>));
  BOOST_CONCEPT_ASSERT((detail::concepts::IOServingConcept<IOServingPolicy>));

  typedef detail::security_policy_chooser<Connection> inherited;
  typedef Connection connection_type;
  typedef typename Connection::pointer_type connection_pointer;
public:

  basic_server
  (
    const boost::shared_ptr<apache::thrift::TProcessor>& processor,
    const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
    const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
    std::string const& address,
    std::string const& port
  );

  basic_server
  (
    const boost::shared_ptr<apache::thrift::TProcessor>& processor,
    const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
    const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
    std::string const& address,
    std::string const& port,
    std::size_t num_threads 
  );

  virtual void serve() OVERRIDE;
  virtual void stop() OVERRIDE;

private:
  void start();
  void start_listen(std::string const& address, std::string const& port);
  void start_accept();
#ifdef BOOST_NO_CXX11_LAMBDAS
  void handle_accept(connection_pointer, boost::system::error_code const&);
#endif
  boost::asio::io_service& get_io_service();

  boost::asio::ip::tcp::acceptor acceptor;
};

} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#include <thrift/server/tcp/impl/basic_server.ipp>

#endif // _THRIFT_SERVER_TCP_BASIC_SERVER_HPP_
