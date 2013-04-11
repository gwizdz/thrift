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

#ifndef _THRIFT_SERVER_TCP_BASIC_CONNECTION_HPP_
#define _THRIFT_SERVER_TCP_BASIC_CONNECTION_HPP_

#include <thrift/config.hpp>
#include <thrift/transport/TBufferTransports.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/ref.hpp>
#include <thrift/server/tcp/detail/helpers.hpp>
#include <thrift/server/tcp/detail/concepts.hpp>
#include <thrift/server/tcp/detail/handler_policies.hpp>
#include <thrift/server/tcp/request_handler.hpp>
#include <thrift/server/tcp/stream_traits.hpp>

#if !defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) && !defined(BOOST_NO_CXX11_VARIADIC_MACROS)
# define REQUIRES(...) typename boost::enable_if<__VA_ARGS__, bool>::type = false
#endif

// Forward declaration.
namespace boost { namespace asio { namespace ssl { class context; } } }

namespace apache { namespace thrift { namespace server { namespace tcp {

#if !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES) && !defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS)\
 && !defined(BOOST_NO_CXX11_VARIADIC_MACROS)
  template <class ServerReference, class ServerBase>
    using which_server = boost::is_same<ServerBase,
      typename boost::remove_reference<
        typename boost::unwrap_reference<ServerReference>::type>::type>;
#endif

template <
    class Stream,
    template <class> class StreamTraits = stream_traits,
    class HandlerPolicy = detail::default_handler_policy
>
class basic_connection : public boost::enable_shared_from_this<basic_connection<Stream, StreamTraits, HandlerPolicy> >
  ,
#ifndef HAS_EXTENDED_FRIEND_DECLARATION
  public
#endif
  HandlerPolicy, boost::noncopyable
  , public StreamTraits<Stream>::template Impl<basic_connection<Stream, StreamTraits, HandlerPolicy> >
{
  BOOST_CONCEPT_ASSERT((detail::concepts::HandlerPolicyConcept<HandlerPolicy>));

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
  template <class Connection, class IOServingPolicy>
  friend class basic_server;
#else
public:
#endif
#ifdef HAS_EXTENDED_FRIEND_DECLARATION
  friend StreamTraits<Stream>;
private:
#else
public:
#endif
  typedef typename StreamTraits<Stream>::value_type socket_type;
  typedef typename StreamTraits<Stream>::reference_type socket_reference;
  typedef typename StreamTraits<Stream>::template Impl<basic_connection>::pointer_type pointer_type;
  typedef typename boost::add_lvalue_reference<Stream>::type stream_reference;

  // Get the underlying socket type.
  socket_reference get_socket();

  // Get stream reference (not underlying socket), in case of ssl::stream<socket>
  // get_socket() returns socket and get_stream() returns ssl::stream<socket>.
  stream_reference get_stream();

  void set_socket_options();

  void process();
  void read_frame_size();
  void read_frame( std::size_t frame_size );
  void write_reply();

#ifdef BOOST_NO_CXX11_LAMBDAS
  void handle_read_frame_size(uint8_t* buff, boost::system::error_code const& error, std::size_t bytes_transferred);
  void handle_read_frame(boost::system::error_code const& error, std::size_t bytes_transferred);
  void handle_write_reply(boost::system::error_code const& error, std::size_t bytes_transferred);
#endif

public:
  typedef typename detail::server_type<basic_connection>::reference_type server_reference;

#if defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) || defined(BOOST_NO_CXX11_VARIADIC_MACROS)
  template <class ServerReference>
#else
  template <class ServerReference, REQUIRES(which_server<ServerReference, detail::tcp_server_base>)>
#endif
  basic_connection( boost::asio::io_service& io_service, ServerReference& serv );

#if defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) || defined(BOOST_NO_CXX11_VARIADIC_MACROS)
  template <class ServerReference>
#else
  template <class ServerReference, REQUIRES(which_server<ServerReference, detail::tls_server_base>)>
#endif
  basic_connection( boost::asio::io_service& io_service, boost::asio::ssl::context&, ServerReference& servref );

private:
  boost::shared_ptr<apache::thrift::transport::TMemoryBuffer> wbuf;
  boost::shared_ptr<apache::thrift::transport::TMemoryBuffer> rbuf;
  socket_type socket;
  server_reference server;
  request_handler handle_request;
};

} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#ifdef REQUIRES
# undef REQUIRES
#endif

#include <thrift/server/tcp/impl/basic_connection.ipp>

#endif // _THRIFT_SERVER_TCP_BASIC_CONNECTION_HPP_