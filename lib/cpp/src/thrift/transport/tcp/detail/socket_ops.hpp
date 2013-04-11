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

#ifndef _THRIFT_TRANSPORT_TCP_DETAIL_SOCKET_OPS_HPP_
#define _THRIFT_TRANSPORT_TCP_DETAIL_SOCKET_OPS_HPP_

#include <thrift/config.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <vector>

namespace apache { namespace thrift { namespace transport {
namespace tcp { namespace detail {

// Windows Sockets API has its own specific code to ECONNRESET
#if defined(__linux__)
  static int const econnreset = ECONNRESET;
#elif defined(WIN32)
  static int const econnreset = WSAECONNRESET;
#endif

void set_result(boost::optional<boost::system::error_code>& a, boost::system::error_code b)
{
  a.reset(b);
}

template <class Operation, class Stream>
typename Operation::result_type operation_with_timeout
(
  Stream& s,
  typename Operation::argument_type arg,
  boost::asio::deadline_timer::duration_type timeout
)
{
  using boost::system::error_code;
  using boost::system::system_error;
  using boost::asio::deadline_timer;
  using boost::optional;

  optional<error_code> timer_result;
  boost::asio::io_service& io_service = s.get_io_service();
  deadline_timer timer(io_service);
  timer.expires_from_now(timeout);

  timer.async_wait(boost::bind(set_result, boost::ref(timer_result), _1));

  optional<error_code> operation_result;
  Operation async_op;
  async_op(s, arg, operation_result);

  io_service.reset();
  while (io_service.run_one())
  {
    if (operation_result)
      timer.cancel();
    else if (timer_result)
      s.cancel();
  }

  if (*operation_result)
    BOOST_THROW_EXCEPTION(system_error(*operation_result));

  return async_op.result();
}

//  async_connect_op   -----------------------------------------------//
struct async_connect_op : private boost::noncopyable {
  typedef void result_type;
  typedef std::vector<boost::asio::ip::tcp::endpoint> const& argument_type;

  template <class Stream, class Result>
  void operator()(Stream& s, argument_type endpoints, Result& op_result)
  {
    boost::asio::async_connect(s, endpoints.begin(), endpoints.end(), boost::bind(set_result, boost::ref(op_result), _1));
  }

  result_type result() const {}
}; // async_connect_op

//  async_rw_op_base   -----------------------------------------------//
struct async_rw_op_base : private boost::noncopyable {
  typedef std::size_t result_type;

  async_rw_op_base() : bytes_transferred_(0UL)
  {}

  void set_result(boost::optional<boost::system::error_code>& a,
    boost::system::error_code b, std::size_t bytes_transferred)
  {
    a.reset(b);
    this->bytes_transferred_ = bytes_transferred;
  }

  result_type result() const
  {
    return bytes_transferred_;
  }

protected:
  std::size_t bytes_transferred_;
  ~async_rw_op_base() {}
}; // async_rw_op_base

//  async_read_op   -----------------------------------------------//
template <class MutableBufferSequence>
struct async_read_op : async_rw_op_base
{
  using async_rw_op_base::result_type;
  typedef MutableBufferSequence const& argument_type;

  template <class Stream, class Result>
  void operator()(Stream& s, argument_type buff, Result& op_result)
  {
    boost::asio::async_read(s, buff, boost::bind(&async_read_op::set_result
      , this, boost::ref(op_result), _1, boost::asio::placeholders::bytes_transferred)
      );
  }
}; // async_read_op

//  async_write_op   -----------------------------------------------//
template <class ConstBufferSequence>
struct async_write_op : async_rw_op_base
{
  using async_rw_op_base::result_type;
  typedef ConstBufferSequence const& argument_type;

  template <class Stream, class Result>
  void operator()(Stream& s, argument_type buff, Result& op_result)
  {
    boost::asio::async_write(s, buff, boost::bind(&async_write_op::set_result
      , this, boost::ref(op_result), _1, boost::asio::placeholders::bytes_transferred)
      );
  }
}; // async_write_op

} // namespace detail
} // namespace tcp
} // namespace transport
} // namespace thrift
} // namespace apache

#endif // _THRIFT_TRANSPORT_TCP_DETAIL_SOCKET_OPS_HPP_
