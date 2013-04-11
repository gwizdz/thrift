// Copyright (c) 2013 Lukasz Gwizdz.
// Home at: https://github.com/gwizdz/thrift
/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance`
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

#ifndef _THRIFT_SERVER_TCP_STREAM_TRAITS_HPP_
#define _THRIFT_SERVER_TCP_STREAM_TRAITS_HPP_

#include <thrift/config.hpp>
#include <thrift/Thrift.h>
#include <thrift/output_inserters.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/type_traits/add_lvalue_reference.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>

namespace apache { namespace thrift { namespace server { namespace tcp {

template <class Stream>
struct stream_traits;

template <>
struct stream_traits<boost::asio::ip::tcp::socket>
{
  typedef boost::asio::ip::tcp::socket value_type;
  typedef value_type& reference_type;
  typedef value_type& param_type;

  static reference_type get_underlying_stream( param_type s )
  {
    return s;
  }

  // Impl holds implementation that vary on Stream type
  // and has to contribute public interface. Moreover,
  // it does need access to Host class (with usage of CRTP).
  template <class Host>
  struct Impl {
    typedef detail::tcp_server_base server_type;
    typedef boost::shared_ptr<Host> pointer_type;

    // Starts logic after connection has been established.
    void start()
    {
      self().set_socket_options();
      self().read_frame_size();
    }

    // Implementation of connection factory method.
    static pointer_type create(boost::asio::io_service& io_service, server_type& server)
    {
      return boost::make_shared<Host>(boost::ref(io_service), boost::ref(server));
    }
  private:
    Host& self() {
      return static_cast<Host&>(*this);
    }
  };

};

template <class Stream>
struct stream_traits<boost::asio::ssl::stream<Stream> >
{
  typedef boost::asio::ssl::stream<Stream> value_type;

  typedef typename boost::add_lvalue_reference<typename value_type::lowest_layer_type>::type reference_type;
  typedef value_type& param_type;

  static reference_type get_underlying_stream( param_type s )
  {
    return s.lowest_layer();
  }

  // Impl holds implementation that vary on Stream type
  // and has to contribute public interface. Moreover,
  // it does need access to Host class (with usage of CRTP).
  template <class Host>
  struct Impl {
    typedef detail::tls_server_base server_type;
    typedef boost::shared_ptr<Host> pointer_type;

    // Implementation of connection factory method.
    static pointer_type create(boost::asio::io_service& io_service, server_type& server)
    {
      return boost::make_shared<Host>(boost::ref(io_service), boost::ref(server.get_handle()), boost::ref(server));
    }

    // Starts logic after connection has been established.
    void start()
    {
      self().set_socket_options();

#ifndef BOOST_NO_CXX11_LAMBDAS
      auto client = self().shared_from_this();
      self().get_stream().async_handshake(boost::asio::ssl::stream_base::server, self().safe_handler([ client ]( boost::system::error_code const& error ){
        if ( !error )
        {
          client->read_frame_size();
        }
        else
        {
          apache::thrift::GlobalOutput << error;
        }
      } ));
#else
      self().get_stream().async_handshake(boost::asio::ssl::stream_base::server, boost::bind(&Impl::handle_handshake,
          self().shared_from_this(), boost::asio::placeholders::error));
#endif
    }

  private:
    Host& self() {
      return static_cast<Host&>(*this);
    }
#ifdef BOOST_NO_CXX11_LAMBDAS
    void handle_handshake(const boost::system::error_code& error)
    {
      if ( !error )
      {
        self().read_frame_size();
      }
      else
      {
        apache::thrift::GlobalOutput << error;
      }
    }
#endif
  };

};

} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_STREAM_TRAITS_HPP_
