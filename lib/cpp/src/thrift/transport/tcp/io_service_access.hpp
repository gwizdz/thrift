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

#ifndef _THRIFT_TRANSPORT_TCP_IO_SERVICE_ACCESS_HPP_
#define _THRIFT_TRANSPORT_TCP_IO_SERVICE_ACCESS_HPP_

// Forward declarations.
namespace boost {
namespace asio {
class io_service;
} // namespace asio
template<class T> class shared_ptr;
} // namespace boost

namespace apache { namespace thrift { namespace transport { namespace tcp {

struct io_service_access {
  virtual operator boost::asio::io_service&() = 0;
protected:
  ~io_service_access() {}
};

typedef boost::shared_ptr<io_service_access> io_service_access_ptr;

io_service_access_ptr get_io_service();

} // namespace tcp
} // namespace transport
} // namespace thrift
} // namespace apache

#endif // _THRIFT_TRANSPORT_TCP_IO_SERVICE_ACCESS_HPP_
