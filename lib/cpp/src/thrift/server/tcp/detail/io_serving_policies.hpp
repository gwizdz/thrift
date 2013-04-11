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

#ifndef _THRIFT_SERVER_TCP_DETAIL_IO_SERVING_POLICIES_HPP_
#define _THRIFT_SERVER_TCP_DETAIL_IO_SERVING_POLICIES_HPP_

#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <thrift/server/tcp/detail/io_service_pool.hpp>

namespace apache { namespace thrift { namespace server { namespace tcp { namespace detail {

// single threaded, non-concurrent handlers execution
struct default_io_serving_policy
{
  void run_impl()
  {
    io_service.run();
  }

  void stop_impl()
  {
    return io_service.stop();
  }

  boost::asio::io_service& get_io_service_impl()
  {
    return io_service;
  }

private:
  boost::asio::io_service io_service;
};

struct io_service_run_in_thread_pool
{
  explicit io_service_run_in_thread_pool( std::size_t num_threads ) : thread_pool_size( num_threads )
  {}

  void run_impl()
  {
    boost::thread_group thread_pool;
    for(std::size_t i = 0; i < thread_pool_size; ++i)
    {
      thread_pool.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));
    }
    thread_pool.join_all();
  }

  void stop_impl()
  {
    return io_service.stop();
  }

  boost::asio::io_service& get_io_service_impl()
  {
    return io_service;
  }

private:
  std::size_t thread_pool_size;
  boost::asio::io_service io_service;
};

namespace detail = http::server2;
struct io_service_per_core
{
  explicit io_service_per_core( std::size_t num_threads ) : io_service_pool_( num_threads )
  {}

  void run_impl()
  {
    io_service_pool_.run();
  }

  void stop_impl()
  {
    return io_service_pool_.stop();
  }

  boost::asio::io_service& get_io_service_impl()
  {
    return io_service_pool_.get_io_service();
  }

private:
  detail::io_service_pool io_service_pool_;
};

} // namespace detail
} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_DETAIL_IO_SERVING_POLICIES_HPP_
