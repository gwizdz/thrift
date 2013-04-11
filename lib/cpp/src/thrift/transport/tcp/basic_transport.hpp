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

#ifndef _THRIFT_TRANSPORT_TCP_BASIC_TRANSPORT_HPP_
#define _THRIFT_TRANSPORT_TCP_BASIC_TRANSPORT_HPP_

#include <string>
#include <thrift/config.hpp>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/tcp/io_service_access.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace apache { namespace thrift { namespace transport { namespace tcp {

namespace detail
{
  typedef apache::thrift::transport::TTransport dynamic_binding;
  typedef struct {} static_binding;
}

typedef unsigned short port_type;

template <class Stream, template<class> class TransportImpl, class BindingMode = detail::dynamic_binding>
class basic_transport : public BindingMode
{
public:
  bool isOpen();
  bool peek();
  void open();
  void close();
  uint32_t read_virt(uint8_t* buf, uint32_t len);
  uint32_t readAll_virt(uint8_t* buf, uint32_t len);
  void write_virt(const uint8_t* buf, uint32_t len);

  // Just to mimic some part of the TSocket class interface if needed
  std::string getHost() const;
  port_type getPort() const;
  void setLinger(bool on, int linger);
  void setNoDelay(bool noDelay);
  void setConnTimeout(int ms);
  void setRecvTimeout(int ms);
  void setSendTimeout(int ms);
  int getSocketFD();

protected:
  basic_transport(std::string const& address, port_type port);
  basic_transport(std::string const& address, std::string const& port);

  io_service_access_ptr io_service;
private:
  TransportImpl<Stream>& self();
  void io_op_precond_check();

private:
  boost::posix_time::time_duration send_timeout;
  boost::posix_time::time_duration recv_timeout;
  boost::posix_time::time_duration connect_timeout;

  std::string address, port;
};

} // namespace tcp
} // namespace transport
} // namespace thrift
} // namespace apache

#include <thrift/transport/tcp/impl/basic_transport.ipp>

#endif // _THRIFT_TRANSPORT_TCP_BASIC_TRANSPORT_HPP_