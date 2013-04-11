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

#include <sstream>
#include <thrift/server/tcp/tls/context.hpp>
#include <thrift/transport/TTransportException.h>
#include <boost/asio/ssl/context.hpp>
#include <boost/bind.hpp>

#ifndef BOOST_NO_CXX11_HDR_ARRAY
#  include <array>
#else
#  include <boost/array.hpp>
#endif

namespace {
  bool verify_cert( bool preverified, boost::asio::ssl::verify_context& ctx )
  {
    if ( !preverified )
    {
#ifndef BOOST_NO_CXX11_AUTO_DECLARATIONS
      auto store = ctx.native_handle();
#else
      boost::asio::ssl::verify_context::native_handle_type store = ctx.native_handle();
#endif
      X509* cert = X509_STORE_CTX_get_current_cert( store );
      const int depth = X509_STORE_CTX_get_error_depth( store );
      const int err = X509_STORE_CTX_get_error( store );

      std::ostringstream s;
      s << "-Error with certificate at depth: " << depth;

#ifndef BOOST_NO_CXX11_HDR_ARRAY
      std::array<char, 256> data;
#else
      boost::array<char, 256> data;
#endif
      X509_NAME_oneline( X509_get_issuer_name( cert ), data.data(), data.size() );
      s << " issuer = " << std::string( data.data() );
      X509_NAME_oneline( X509_get_subject_name( cert ), data.data(), data.size() );
      s << " subject = " << std::string( data.data() ) << " err " << err << ":" << X509_verify_cert_error_string( err ) << std::endl;

      apache::thrift::GlobalOutput( s.str().c_str() );
    }

    return preverified;
  }
}

namespace apache { namespace thrift { namespace server { namespace tcp { namespace tls {

struct context::context_impl_ : private boost::noncopyable
{
private:
  friend struct context;
  context_impl_() : ctx(boost::asio::ssl::context::sslv23)
  {
    ctx.set_options(boost::asio::ssl::context_base::default_workarounds | boost::asio::ssl::context_base::no_sslv2 | boost::asio::ssl::context_base::single_dh_use );
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    ctx.set_verify_callback( verify_cert );
  }

  operator boost::asio::ssl::context&()
  {
    return ctx;
  }

  boost::asio::ssl::context& get_context()
  {
    return ctx;
  }

  void certificate_authority(std::string const& ca)
  {
    try
    {
      ctx.load_verify_file(ca.c_str());
      ctx.set_default_verify_paths();
    }
    catch (boost::system::system_error const& e)
    {
      BOOST_THROW_EXCEPTION(apache::thrift::transport::TTransportException(e.what()));
    }
  }

  void certificate(std::string const& cert)
  {
    try
    {
      ctx.use_certificate_chain_file( cert.c_str() );
    }
    catch (boost::system::system_error const& e)
    {
      BOOST_THROW_EXCEPTION(apache::thrift::transport::TTransportException(e.what()));
    }
  }

  void private_key(std::string const& priv_key)
  {
    try
    {
      ctx.use_private_key_file(priv_key.c_str(), boost::asio::ssl::context_base::pem);
      ctx.set_password_callback(boost::bind(&context_impl_::get_passwd, this));
    }
    catch (boost::system::system_error const& e)
    {
      BOOST_THROW_EXCEPTION(apache::thrift::transport::TTransportException(e.what()));
    }
  }

  void password( std::string const& p )
  {
    pwd = p;
  }

  void ciphers(std::string const& ciphers)
  {
#ifndef BOOST_NO_CXX11_AUTO_DECLARATIONS    
    auto ret = SSL_CTX_set_cipher_list(ctx.native_handle(), ciphers.c_str());
#else
    int ret = SSL_CTX_set_cipher_list(ctx.native_handle(), ciphers.c_str());
#endif
    if ( 1 != ret )
      BOOST_THROW_EXCEPTION(apache::thrift::transport::TTransportException("SSL_CTX_set_cipher_list error."));
  }

  std::string get_passwd()
  {
    return pwd;
  }
  std::string pwd;
  boost::asio::ssl::context ctx;
};

void context::password( std::string const& p )
{
  BOOST_ASSERT( pimpl );
  return pimpl->password(p);
}

void context::ciphers( std::string const& ciphers )
{
  BOOST_ASSERT( pimpl );
  return pimpl->ciphers(ciphers);
}

void context::private_key( std::string const& priv_key )
{
  BOOST_ASSERT( pimpl );
  return pimpl->private_key(priv_key);
}

void context::certificate( std::string const& cert )
{
  BOOST_ASSERT( pimpl );
  return pimpl->certificate(cert);
}

void context::certificate_authority( std::string const& ca )
{
  BOOST_ASSERT( pimpl );
  return pimpl->certificate_authority(ca);
}

context::operator boost::asio::ssl::context&()
{
  BOOST_ASSERT( pimpl );
  return static_cast<boost::asio::ssl::context&>(*pimpl);
}

context::context() : pimpl( new context_impl_ )
{
}

context::~context()
{
}

boost::asio::ssl::context& context::get_handle()
{
  BOOST_ASSERT( pimpl );
  return pimpl->get_context();
}

} // namespace tls
} // namespace tcp
} // namespace server
} // namespace thrift
} // namespace apache
