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

#ifndef _THRIFT_OUTPUT_INSERTERS_HPP_
#define _THRIFT_OUTPUT_INSERTERS_HPP_

#include <thrift/config.hpp>
#include <thrift/Thrift.h>

namespace apache { namespace thrift {

// Formatted output

// INSERTERS
BOOST_FORCEINLINE TOutput& operator<<(TOutput& o, std::string const& s)
{
  o(s.c_str());
  return o;
}

BOOST_FORCEINLINE TOutput& operator<<(TOutput& o, boost::system::error_code const& ec)
{
  o.printf("%s : %ld - %s", ec.category().name(), ec.value(), ec.message().c_str());
  return o;
}

BOOST_FORCEINLINE TOutput& operator<<( TOutput& o, TOutput& (*pfn)(TOutput&) )
{
  BOOST_ASSERT(pfn);
  return ((*pfn)(o));
}

// MANIPULATORS
BOOST_FORCEINLINE TOutput& endl(TOutput& o)
{
  o.printf("\r\n");
  return o;
}

} // namespace thrift
} // namespace apache

#endif // _THRIFT_OUTPUT_INSERTERS_HPP_
