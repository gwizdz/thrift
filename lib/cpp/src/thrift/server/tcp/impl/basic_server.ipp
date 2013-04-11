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

#ifndef _THRIFT_SERVER_TCP_BASIC_SERVER_IPP_
#define _THRIFT_SERVER_TCP_BASIC_SERVER_IPP_

#include <thrift/config.hpp>
#include <thrift/server/tcp/detail/traits.hpp>
#ifdef BOOST_NO_CXX11_LAMBDAS
# include <boost/bind.hpp>
#endif
#ifdef BOOST_NO_CXX11_STATIC_ASSERT
# include <boost/static_assert.hpp>
# define STATIC_ASSERT BOOST_STATIC_ASSERT_MSG
#else
# define STATIC_ASSERT static_assert
#endif

namespace apache { namespace thrift { namespace server { namespace tcp {

template <class Connection, class IOServingPolicy>
basic_server<Connection, IOServingPolicy>::basic_server
(
  const boost::shared_ptr<apache::thrift::TProcessor>& processor,
  const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
  const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
  std::string const& address,
  std::string const& port
) try : inherited( processor ), acceptor( get_io_service() )
{
  STATIC_ASSERT( !detail::is_concurrent_io_serving_policy<IOServingPolicy>::value,
   "This ctor is only acceptable for IOServingPolicy that does not support multithreading. Use appropriate ctor." );

  this->setInputTransportFactory( transportFactory );
  this->setOutputTransportFactory( transportFactory );
  this->setInputProtocolFactory( protocolFactory );
  this->setOutputProtocolFactory( protocolFactory );

  start_listen(address, port);
}
catch ( boost::system::system_error& err )
{
  BOOST_THROW_EXCEPTION( apache::thrift::transport::TTransportException( apache::thrift::transport::TTransportException::INTERNAL_ERROR, err.what() ) );
}

template <class Connection, class IOServingPolicy>
basic_server<Connection, IOServingPolicy>::basic_server
(
  const boost::shared_ptr<apache::thrift::TProcessor>& processor,
  const boost::shared_ptr<apache::thrift::transport::TTransportFactory>& transportFactory,
  const boost::shared_ptr<apache::thrift::protocol::TProtocolFactory>& protocolFactory,
  std::string const& address,
  std::string const& port,
  std::size_t num_threads
) try : inherited( processor ), IOServingPolicy( num_threads ), acceptor( get_io_service() )
{
  STATIC_ASSERT( detail::is_concurrent_io_serving_policy<IOServingPolicy>::value,
   "This ctor is only acceptable for IOServingPolicy that does support multithreading. Use appropriate ctor." );

  this->setInputTransportFactory( transportFactory );
  this->setOutputTransportFactory( transportFactory );
  this->setInputProtocolFactory( protocolFactory );
  this->setOutputProtocolFactory( protocolFactory );

  start_listen(address, port);
}
catch ( boost::system::system_error& err )
{
  BOOST_THROW_EXCEPTION( apache::thrift::transport::TTransportException( apache::thrift::transport::TTransportException::INTERNAL_ERROR, err.what() ) );
}

#undef STATIC_ASSERT

template <class Connection, class IOServingPolicy>
void basic_server<Connection, IOServingPolicy>::start_listen(std::string const& address, std::string const& port)
{
  boost::asio::ip::tcp::resolver resolver( get_io_service() );
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

  acceptor.open( endpoint.protocol() );
  if ( endpoint.protocol() == boost::asio::ip::tcp::v6() )
  {
    boost::system::error_code ec;
    acceptor.set_option( boost::asio::ip::v6_only(false), ec );
    // Call succeeds only on dual stack systems.
  }
  acceptor.set_option( boost::asio::ip::tcp::acceptor::reuse_address(true) );
  acceptor.bind(endpoint);
  acceptor.listen();

  if ( this->eventHandler_ )
    this->eventHandler_->preServe();
}

template <class Connection, class IOServingPolicy>
void basic_server<Connection, IOServingPolicy>::start_accept()
{
#ifndef BOOST_NO_CXX11_AUTO_DECLARATIONS
  auto new_connection = connection_type::create(get_io_service(), *this);
#else
  connection_pointer new_connection = connection_type::create(get_io_service(), *this);
#endif

#ifndef BOOST_NO_CXX11_LAMBDAS
  acceptor.async_accept( new_connection->get_socket(), [this, new_connection](boost::system::error_code const& error) mutable {
    if (!error)
    {
      // Accept succeeded.
      new_connection->start();

      this->start_accept();
    }
  });
#else
  acceptor.async_accept( new_connection->get_socket(), boost::bind(&basic_server::handle_accept, this,
    new_connection, boost::asio::placeholders::error ) );
#endif
}

#ifdef BOOST_NO_CXX11_LAMBDAS
template <class Connection, class IOServingPolicy>
void basic_server<Connection, IOServingPolicy>::handle_accept(connection_pointer c, boost::system::error_code const& error)
{
  if ( !error )
  {
    // Accept succeeded.
    c->start();
  }
  start_accept();
}
#endif

template <class Connection, class IOServingPolicy>
BOOST_FORCEINLINE boost::asio::io_service& basic_server<Connection, IOServingPolicy>::get_io_service()
{
  return IOServingPolicy::get_io_service_impl();
}

template <class Connection, class IOServingPolicy>
BOOST_FORCEINLINE void basic_server<Connection, IOServingPolicy>::start()
{
  start_accept();
  return IOServingPolicy::run_impl();
}

template <class Connection, class IOServingPolicy>
void basic_server<Connection, IOServingPolicy>::serve()
{
  start();
}

template <class Connection, class IOServingPolicy>
void basic_server<Connection, IOServingPolicy>::stop()
{
  return IOServingPolicy::stop_impl();
}

} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_BASIC_SERVER_IPP_
