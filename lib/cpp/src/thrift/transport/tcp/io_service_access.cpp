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

#include <thrift/config.hpp>
#include <thrift/transport/tcp/io_service_access.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>

namespace apache { namespace thrift { namespace transport { namespace tcp {

io_service_access_ptr get_io_service()
{
  struct IOService : io_service_access {
    static boost::shared_ptr<IOService> get_instance()
    {
      static boost::weak_ptr<IOService> instance_wptr;
      boost::shared_ptr<IOService> instance;
      if ( boost::shared_ptr<IOService> ptr = instance_wptr.lock() )
      {
        return ptr;
      }
      else {
        instance = boost::make_shared<IOService>();
        instance_wptr = instance;
        return instance;
      }
    }

    operator boost::asio::io_service&() OVERRIDE FINAL
    {
      return io_service;
    }
  private:
    boost::asio::io_service io_service;
  };

  return IOService::get_instance();
}

} // namespace tcp
} // namespace transport
} // namespace thrift
} // namespace apache