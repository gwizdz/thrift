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

#ifndef _THRIFT_SERVER_TCP_SERVER_HPP_
#define _THRIFT_SERVER_TCP_SERVER_HPP_

#include <thrift/server/tcp/basic_server.hpp>
#include <thrift/server/tcp/connection.hpp>

namespace apache { namespace thrift { namespace server { namespace tcp {

typedef basic_server<connection> server;
typedef basic_server<connection, detail::io_service_per_core> server_io_service_per_core;
typedef basic_server<concurrent_connection, detail::io_service_run_in_thread_pool> server_io_service_in_thread_pool;

} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache

#endif // _THRIFT_SERVER_TCP_SERVER_HPP_
