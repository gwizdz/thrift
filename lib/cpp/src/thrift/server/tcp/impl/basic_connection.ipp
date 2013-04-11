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

#ifndef _THRIFT_SERVER_TCP_BASIC_CONNECTION_IPP_
#define _THRIFT_SERVER_TCP_BASIC_CONNECTION_IPP_

#include <thrift/config.hpp>
#include <boost/assert.hpp>
#include <boost/make_shared.hpp>
#ifdef BOOST_NO_CXX11_LAMBDAS
# include <boost/asio/placeholders.hpp>
#endif
#include <boost/asio/read.hpp>
#include <boost/asio/io_service.hpp>
#include <thrift/output_inserters.hpp>

namespace apache { namespace thrift { namespace server { namespace tcp {

// Note that this REQUIRES differs from the one defined in .hpp, default parameter is omitted
// because it cannot appears here.
#if !defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) && !defined(BOOST_NO_CXX11_VARIADIC_MACROS)
# define REQUIRES(...) typename boost::enable_if<__VA_ARGS__, bool>::type
#endif

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
#if defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) || defined(BOOST_NO_CXX11_VARIADIC_MACROS)
  template <class ServerReference>
#else
  template <class ServerReference, REQUIRES(which_server<ServerReference, detail::tcp_server_base>)>
#endif
BOOST_FORCEINLINE basic_connection<Stream, StreamTraits, HandlerPolicy>::basic_connection( boost::asio::io_service& io_service, ServerReference& serv ) :
  HandlerPolicy( io_service ), wbuf( boost::make_shared<apache::thrift::transport::TMemoryBuffer>() ),
  rbuf( boost::make_shared<apache::thrift::transport::TMemoryBuffer>() ), socket( io_service ), server( serv ),
  handle_request( server, rbuf, wbuf )
{}

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
#if defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) || defined(BOOST_NO_CXX11_VARIADIC_MACROS)
  template <class ServerReference>
#else
  template <class ServerReference, REQUIRES(which_server<ServerReference, detail::tls_server_base>)>
#endif
BOOST_FORCEINLINE basic_connection<Stream, StreamTraits, HandlerPolicy>::basic_connection( boost::asio::io_service& io_service, boost::asio::ssl::context& ctx, ServerReference& serv ) :
  HandlerPolicy( io_service ), wbuf( boost::make_shared<apache::thrift::transport::TMemoryBuffer>() ),
  rbuf( boost::make_shared<apache::thrift::transport::TMemoryBuffer>() ), socket( io_service, ctx ), server( serv ),
  handle_request( server, rbuf, wbuf )
{}

#ifdef REQUIRES
# undef REQUIRES
#endif

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
BOOST_FORCEINLINE typename basic_connection<Stream, StreamTraits, HandlerPolicy>::socket_reference
basic_connection<Stream, StreamTraits, HandlerPolicy>::get_socket()
{
  return StreamTraits<Stream>::get_underlying_stream( socket );
}

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
BOOST_FORCEINLINE typename basic_connection<Stream, StreamTraits, HandlerPolicy>::stream_reference
basic_connection<Stream, StreamTraits, HandlerPolicy>::get_stream()
{
  return socket;
}

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
void basic_connection<Stream, StreamTraits, HandlerPolicy>::set_socket_options()
{
  get_socket().set_option(boost::asio::ip::tcp::socket::linger(false, 0));
  get_socket().set_option(boost::asio::ip::tcp::socket::keep_alive(true));
  get_socket().set_option(boost::asio::ip::tcp::no_delay(true));
}

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
void basic_connection<Stream, StreamTraits, HandlerPolicy>::process() try
{
  handle_request();

  // schedule sending reply from outputTransport
  write_reply();
}
// If an error occurs then no new asynchronous operations are started. This
// means that all shared_ptr references to the connection object will
// disappear and the object will be destroyed automatically after this
// handler returns. The connection class's destructor closes the socket.
catch ( apache::thrift::transport::TTransportException const& ttx )
{
  apache::thrift::GlobalOutput.printf("Server transport error in process(): %s", ttx.what() );
}
catch ( std::exception const& x )
{
  apache::thrift::GlobalOutput.printf( "Server::process() uncaught exception: %s: %s", typeid(x).name(), x.what() );
}
catch ( ... )
{
  apache::thrift::GlobalOutput.printf( "Server::process() unknown exception" );
}

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
void basic_connection<Stream, StreamTraits, HandlerPolicy>::read_frame_size()
{
#ifndef BOOST_NO_CXX11_AUTO_DECLARATIONS
  const auto len = sizeof( uint32_t );
  auto buff = rbuf->getWritePtr( len );
#else
  const uint32_t len = sizeof( uint32_t );
  uint8_t* buff = rbuf->getWritePtr( len );
#endif
  BOOST_ASSERT( buff );

#ifndef BOOST_NO_CXX11_LAMBDAS
  auto client = this->shared_from_this();

  boost::asio::async_read( socket, boost::asio::buffer(buff, len), this->safe_handler( [ client, buff ]( boost::system::error_code const& error,
    std::size_t bytes_transferred ){
      if ( !error )
      {
        // obtain the frame size
        uint32_t frame_size = *static_cast<const uint32_t*>(static_cast<void*>(buff));
        frame_size = ntohl( frame_size );

        client->rbuf->wroteBytes( bytes_transferred );

        client->read_frame( frame_size );
      }
      // If an error occurs then no new asynchronous operations are started. This
      // means that all shared_ptr references to the connection object will
      // disappear and the object will be destroyed automatically after this
      // handler returns. The connection class's destructor closes the socket.
      //else
      //{
      //  apache::thrift::GlobalOutput << error;
      //}
  } ));
#else
  boost::asio::async_read( socket, boost::asio::buffer(buff, len)
      , this->safe_handler(
       boost::bind( &basic_connection::handle_read_frame_size, this->shared_from_this(), buff
      , boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred
       )
    )
  );
#endif
}

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
void basic_connection<Stream, StreamTraits, HandlerPolicy>::read_frame( std::size_t frame_size )
{
#ifndef BOOST_NO_CXX11_AUTO_DECLARATIONS
  auto buff = rbuf->getWritePtr( frame_size );
#else
  uint8_t* buff = rbuf->getWritePtr( frame_size );
#endif
  BOOST_ASSERT( buff );

#ifndef BOOST_NO_CXX11_LAMBDAS
  auto client = this->shared_from_this();

  boost::asio::async_read(socket, boost::asio::buffer(buff, frame_size),
   this->safe_handler( [client]( boost::system::error_code const& error, std::size_t bytes_transferred ){
      if ( !error )
      {
        client->rbuf->wroteBytes( bytes_transferred );
        client->process();
      }
      // If an error occurs then no new asynchronous operations are started. This
      // means that all shared_ptr references to the connection object will
      // disappear and the object will be destroyed automatically after this
      // handler returns. The connection class's destructor closes the socket.
      else
      {
        apache::thrift::GlobalOutput << error;
      }
  } ));
#else
  boost::asio::async_read(socket, boost::asio::buffer(buff, frame_size),
    this->safe_handler( boost::bind( &basic_connection::handle_read_frame, this->shared_from_this()
    , boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) ));
#endif
}

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
void basic_connection<Stream, StreamTraits, HandlerPolicy>::write_reply()
{
  if ( wbuf->available_read() )
  {
    uint8_t* buffer = nullptr;
    uint32_t length = 0U;
    wbuf->getBuffer(&buffer, &length);

#ifndef BOOST_NO_CXX11_LAMBDAS
    auto client = this->shared_from_this();
    boost::asio::async_write( socket, boost::asio::buffer(buffer, length),
     this->safe_handler( [client]( boost::system::error_code const& error, std::size_t /*bytes_transferred*/ ) {
        if ( !error )
        {
          client->rbuf->resetBuffer();
          client->wbuf->resetBuffer();

          client->read_frame_size();
        }

        // If an error occurs then no new asynchronous operations are started. This
        // means that all shared_ptr references to the connection object will
        // disappear and the object will be destroyed automatically after this
        // handler returns. The connection class's destructor closes the socket.
        else
        {
          apache::thrift::GlobalOutput << error;
        }
    } ));
#else
boost::asio::async_write( socket, boost::asio::buffer(buffer, length),
     this->safe_handler( boost::bind( &basic_connection::handle_write_reply, this->shared_from_this()
    , boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) ) );
#endif
  }
}

#ifdef BOOST_NO_CXX11_LAMBDAS
template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
void basic_connection<Stream, StreamTraits, HandlerPolicy>::handle_read_frame_size
(
  uint8_t* buff, 
  boost::system::error_code const& error,
  std::size_t bytes_transferred
)
{
  if ( !error )
  {
    // obtain the frame size
    uint32_t frame_size = *static_cast<const uint32_t*>(static_cast<void*>(buff));
    frame_size = ntohl( frame_size );

    rbuf->wroteBytes( bytes_transferred );

    read_frame( frame_size );
  }
  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
  //else
  //{
  //  apache::thrift::GlobalOutput << error;
  //}
}

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
void basic_connection<Stream, StreamTraits, HandlerPolicy>::handle_read_frame
(
  boost::system::error_code const& error,
  std::size_t bytes_transferred
)
{
  if ( !error )
  {
    rbuf->wroteBytes( bytes_transferred );
    process();
  }
  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
  else
  {
    apache::thrift::GlobalOutput << error;
  }
}

template <class Stream, template<class> class StreamTraits, class HandlerPolicy>
void basic_connection<Stream, StreamTraits, HandlerPolicy>::handle_write_reply
(
  boost::system::error_code const& error,
  std::size_t bytes_transferred
)
{
  if ( !error )
  {
    rbuf->resetBuffer();
    wbuf->resetBuffer();

    read_frame_size();
  }

  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
  else
  {
    apache::thrift::GlobalOutput << error;
  }
}
#endif

} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_BASIC_CONNECTION_IPP_
