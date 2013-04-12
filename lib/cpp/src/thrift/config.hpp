// Copyright (C) 2013 Lukasz Gwizdz.
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

// You can define your BOOST_USER_CONFIG here 
// or even BOOST_COMPILER_CONFI and BOOST_STDLIB_CONFIG.
// See Boost.Config documentation.

#ifndef _THRIFT_CONFIG_HPP_
#define _THRIFT_CONFIG_HPP_

#include <boost/config.hpp>

#if WINVER <= 0x0502 // XP, Server2003
# ifdef poll
#  undef poll
# endif
#endif

#ifdef _WIN32
# include <thrift/windows/config.h>
#endif
/*
#define BOOST_NO_CXX11_LAMBDAS
#define BOOST_NO_CXX11_AUTO_DECLARATIONS
#define BOOST_NO_CXX11_HDR_ARRAY
#define BOOST_NO_CXX11_SMART_PTR
#define BOOST_NO_CXX11_RVALUE_REFERENCES
#define BOOST_NO_CXX11_NULLPTR
#define BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define BOOST_NO_CXX11_VARIADIC_MACROS
#define BOOST_NO_CXX11_STATIC_ASSERT
*/
// Explicit Virtual Overrides
// http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2009/n2928.htm
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3206.htm
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3272.htm
#if _MSC_VER >= 1600 || __GNUC__ >= 4 && __GNUC_MINOR__ >= 7 || \
  defined __clang__ && (__clang_major__ > 3) || (__clang_major__ == 3) && (__clang_minor__ >= 0)
# define OVERRIDE override
# define FINAL final
#else
# define OVERRIDE
# define FINAL
#endif

// Visual C++ 2010 does not support final
#if _MSC_VER == 1600
# undef FINAL
# define FINAL
#endif

#ifdef BOOST_NO_CXX11_NULLPTR
# define nullptr NULL
#endif

// Extended friend declarations N1791
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1791.pdf
#if _MSC_VER >= 1600 || __GNUC__ >= 4 && __GNUC_MINOR__ >= 7 || \
defined __clang__ && (__clang_major__ >= 3) || (__clang_major__ == 2) && (__clang_minor__ >= 9)
# define HAS_EXTENDED_FRIEND_DECLARATION
#endif

// Inheriting Constructors (revision 5) N2540
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2540.htm
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 8
# define HAS_INHERITING_CONSTRUCTORS
#endif

#endif // _THRIFT_CONFIG_HPP_
