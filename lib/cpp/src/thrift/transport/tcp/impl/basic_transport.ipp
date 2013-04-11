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

#ifndef _THRIFT_TRANSPORT_TCP_BASIC_TRANSPORT_IPP_
#define _THRIFT_TRANSPORT_TCP_BASIC_TRANSPORT_IPP_

#include <thrift/config.hpp>
#include <vector>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/lexical_cast.hpp>
#include <thrift/transport/TTransportException.h>
#include <thrift/transport/tcp/detail/socket_ops.hpp>

namespace apache { namespace thrift { namespace transport { namespace tcp {

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE basic_transport<Stream, TransportImpl, BindingMode>::
basic_transport(std::string const& addr, port_type p) try
  : io_service(get_io_service()), address(addr), port(boost::lexical_cast<std::string>(p))
{}
catch (boost::bad_lexical_cast const&)
{
  BOOST_THROW_EXCEPTION( apache::thrift::transport::TTransportException\
  ( ::apache::thrift::transport::TTransportException::UNKNOWN, "Invalid port." ) );
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE basic_transport<Stream, TransportImpl, BindingMode>::
basic_transport(std::string const& addr, std::string const& p)
  : io_service(get_io_service()), address(addr), port(p)
{}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE
TransportImpl<Stream>& basic_transport<Stream, TransportImpl, BindingMode>::self()
{
  return static_cast<TransportImpl<Stream>&>(*this);
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
bool basic_transport<Stream, TransportImpl, BindingMode>::isOpen()
{
  return self().get_socket().is_open();
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
bool basic_transport<Stream, TransportImpl, BindingMode>::peek()
{
  io_op_precond_check();

  uint8_t buff = 0U;
  boost::system::error_code ec;
  try
  {
    const size_t r = self().get_socket().receive(boost::asio::buffer(&buff, 1),
      boost::asio::socket_base::message_peek, ec);
    if (ec > 0)
      return false;
    return r > 0;
  }
  catch (boost::system::system_error const& e)
  {
    BOOST_THROW_EXCEPTION(apache::thrift::transport::TTransportException\
      (apache::thrift::transport::TTransportException::UNKNOWN, e.what()));
  }
}

#if defined(BOOST_NO_CXX11_LAMBDAS)
namespace detail {

bool is_ipv4(boost::asio::ip::tcp::endpoint const& endpoint)
{
  return endpoint.protocol() == boost::asio::ip::tcp::v4();
}

} // detail
#endif

#define IS_TIMEOUT(tv) tv > boost::posix_time::microseconds(0)

template <class Stream, template<class> class TransportImpl, class BindingMode>
void basic_transport<Stream, TransportImpl, BindingMode>::open() try
{
  boost::asio::ip::tcp::resolver resolver(*io_service);
  boost::asio::ip::tcp::resolver::query query(address, port);
  std::vector<boost::asio::ip::tcp::endpoint> endpoints(resolver.resolve(query),
    boost::asio::ip::tcp::resolver::iterator());

#if !defined(BOOST_NO_CXX11_LAMBDAS) && !defined(BOOST_NO_CXX11_AUTO_DECLARATIONS)
  auto is_ipv4 = [](boost::asio::ip::tcp::endpoint const& endpoint) {
    return endpoint.protocol() == boost::asio::ip::tcp::v4();
  };
#else
// local classes can be template arguments in C++11,
// but lambda expressions are part of C++11 as well
// so this predicate is defined outside, in detail
// namespace scope due to conformance with C++98/C++03.
// for the same reasons non-member versions of begin()/end()
// aren't used.
  using detail::is_ipv4;
#endif
  std::stable_partition( endpoints.begin(), endpoints.end(), is_ipv4 );

  using detail::async_connect_op;

  if (IS_TIMEOUT(connect_timeout))
    detail::operation_with_timeout<async_connect_op>(self().get_socket(),
      endpoints, connect_timeout);
  else
    boost::asio::connect(self().get_socket(), endpoints.begin(), endpoints.end());
}
catch (boost::system::system_error const& e)
{
  BOOST_THROW_EXCEPTION(apache::thrift::transport::TTransportException(e.what()));
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
void basic_transport<Stream, TransportImpl, BindingMode>::close()
{
  // Initiate graceful connection closure.
  boost::system::error_code ignored_ec;
  self().get_socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
uint32_t basic_transport<Stream, TransportImpl, BindingMode>::read_virt(uint8_t* buf, uint32_t len)
{
  io_op_precond_check();

  try
  {
    using detail::async_read_op;

    if (IS_TIMEOUT(recv_timeout))
      return detail::operation_with_timeout<async_read_op<boost::asio::mutable_buffers_1>
        >(self().get_socket(), boost::asio::buffer(buf, len), recv_timeout);
    else
      return boost::asio::read(self().get_socket(), boost::asio::buffer(buf, len));
  }
  catch (boost::system::system_error const& e)
  {
    if (e.code() == boost::asio::error::eof || e.code().value() == detail::econnreset)
      return 0;
    BOOST_THROW_EXCEPTION(apache::thrift::transport::TTransportException(e.what()));
  }
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
uint32_t basic_transport<Stream, TransportImpl, BindingMode>::readAll_virt(uint8_t* buf, uint32_t len)
{
  return apache::thrift::transport::readAll(*this, buf, len);
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
void basic_transport<Stream, TransportImpl, BindingMode>::write_virt(const uint8_t* buf, uint32_t len)
{
  io_op_precond_check();

  try
  {
    using detail::async_write_op;

    if (IS_TIMEOUT(send_timeout))
      detail::operation_with_timeout<async_write_op<boost::asio::const_buffers_1>
        >(self().get_socket(), boost::asio::buffer(buf, len), send_timeout);
    else
      boost::asio::write(self().get_socket(), boost::asio::buffer(buf, len));
  }
  catch (boost::system::system_error const& e)
  {
    BOOST_THROW_EXCEPTION(apache::thrift::transport::TTransportException(e.what()));
  }
}
#undef IS_TIMEOUT

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE void basic_transport<Stream, TransportImpl, BindingMode>::setLinger(bool on, int linger) try
{
  boost::asio::socket_base::linger option(on, linger);
  self().get_socket().set_option(option);
}
catch (boost::system::system_error const& e)
{
  const int errno_copy = errno;
  apache::thrift::GlobalOutput.perror(e.what(), errno_copy);
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE void basic_transport<Stream, TransportImpl, BindingMode>::setNoDelay(bool noDelay) try
{
  boost::asio::ip::tcp::no_delay nodelay(noDelay);
  self().get_socket().set_option(nodelay);
}
catch (boost::system::system_error const& e)
{
  const int errno_copy = errno;
  apache::thrift::GlobalOutput.perror(e.what(), errno_copy);
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE void basic_transport<Stream, TransportImpl, BindingMode>::setConnTimeout(int ms)
{
  connect_timeout = boost::posix_time::milliseconds( ms );
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE void basic_transport<Stream, TransportImpl, BindingMode>::setRecvTimeout(int ms)
{
  recv_timeout = boost::posix_time::milliseconds( ms );
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE void basic_transport<Stream, TransportImpl, BindingMode>::setSendTimeout(int ms)
{
  send_timeout = boost::posix_time::milliseconds( ms );
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE void basic_transport<Stream, TransportImpl, BindingMode>::io_op_precond_check()
{
  BOOST_ASSERT( io_service );
  if ( !self().get_socket().is_open() )
    BOOST_THROW_EXCEPTION( apache::thrift::transport::TTransportException\
    (apache::thrift::transport::TTransportException::NOT_OPEN,\
       "Cannot perform IO operation on not open socket.") );
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE std::string basic_transport<Stream, TransportImpl, BindingMode>::getHost() const
{
  return address;
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE port_type basic_transport<Stream, TransportImpl, BindingMode>::getPort() const
try {
  return boost::lexical_cast<port_type>(port);
}
catch (boost::bad_lexical_cast const&)
{
  BOOST_THROW_EXCEPTION( apache::thrift::transport::TTransportException\
  (::apache::thrift::transport::TTransportException::UNKNOWN, "Invalid port."));
}

template <class Stream, template<class> class TransportImpl, class BindingMode>
BOOST_FORCEINLINE int basic_transport<Stream, TransportImpl, BindingMode>::getSocketFD()
{
  io_op_precond_check();
  return self().get_socket().native_handle();
}

} // namespace tcp
} // namespace transport
} // namespace thrift
} // namespace apache

#endif // _THRIFT_TRANSPORT_TCP_BASIC_TRANSPORT_IPP_
