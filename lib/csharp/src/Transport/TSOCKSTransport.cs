// Copyright (C) 2012 - 2013 Lukasz Gwizdz.
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

// Transport that utilizes SOCKS Protocol Version 5
// in order to connect to the services behind SOCKS proxy.
// At this moment, only CONNECT command request and user 
// authentication are implemented.
//
// SOCKS Protocol Version 5 - RFC 1928
// http://tools.ietf.org/html/rfc1928
//
// Because of using System.Tuple requires at least .NET Framework 4.0.
//

using System;

namespace Thrift.Transport
{
	/// <summary>
	/// TAccessDenied - exception signals access denied due to authentication failure.
	/// </summary>
	public class TAccessDenied : TTransportException
	{
		public TAccessDenied(string message) : base(message)
		{
		}
	}

	/// <summary>
	/// TSOCKSTransport - transport that uses SOCKS5 Proxy server.
	/// </summary>
	public class TSOCKSTransport : TStreamTransport
	{
		#region " TSOCKSTransport Constructors "
		/// <summary>
		/// Ctor
		/// </summary>
		/// <param name="destEndpointAddress">Service endpoint address.</param>
		/// <param name="destEndpointPort">Service endpoint port number.</param>
		/// <param name="proxyHost">Proxy server address.</param>
		/// <param name="proxyPort">Proxy server port number.</param>
		public TSOCKSTransport(string destEndpointAddress, ushort destEndpointPort, string proxyHost, ushort proxyPort)
		{
			proxyClient.ProxyHost = proxyHost;
			proxyClient.ProxyPort = proxyPort;
			destinationEndpoint = Tuple.Create(destEndpointAddress, destEndpointPort);
		}

		/// <summary>
		/// Ctor
		/// </summary>
		/// <param name="tcpClient">Existing connection to the Proxy server used to establish connection between proxy and destination service on behalf of the client.</param>
		/// <param name="destEndpointAddress">Service endpoint address.</param>
		/// <param name="destEndpointPort">Service endpoint port number.</param>
		public TSOCKSTransport(System.Net.Sockets.TcpClient tcpClient, string destEndpointAddress, ushort destEndpointPort)
		{
			proxyClient.TcpClient = tcpClient;
			destinationEndpoint = Tuple.Create(destEndpointAddress, destEndpointPort);
		}

		/// <summary>
		/// Ctor
		/// </summary>
		/// <param name="destEndpointAddress">Service endpoint address.</param>
		/// <param name="destEndpointPort">Service endpoint port number.</param>
		/// <param name="proxyHost">Proxy server address.</param>
		/// <param name="proxyPort">Proxy server port number.</param>
		/// <param name="proxyAuthentication">Authentication logic - implementation of SOCKS.Version5.ProxyAuthentication interface.</param>
		public TSOCKSTransport(string destEndpointAddress, ushort destEndpointPort, string proxyHost, ushort proxyPort, SOCKS.Version5.ProxyAuthentication proxyAuthentication)
		{
			proxyClient.ProxyHost = proxyHost;
			proxyClient.ProxyPort = proxyPort;
			destinationEndpoint = Tuple.Create(destEndpointAddress, destEndpointPort);
			ProxyAuthentication = proxyAuthentication;
		}

		/// <summary>
		/// Ctor
		/// </summary>
		/// <param name="tcpClient">Existing connection to the Proxy server used to establish connection between proxy and destination service on behalf of the client.</param>
		/// <param name="destEndpointAddress">Service endpoint address.</param>
		/// <param name="destEndpointPort">Service endpoint port number.</param>
		/// <param name="proxyAuthentication">Authentication logic - implementation of SOCKS.Version5.ProxyAuthentication interface.</param>
		public TSOCKSTransport(System.Net.Sockets.TcpClient tcpClient, string destEndpointAddress, ushort destEndpointPort, SOCKS.Version5.ProxyAuthentication proxyAuthentication)
		{
			proxyClient.TcpClient = tcpClient;
			destinationEndpoint = Tuple.Create(destEndpointAddress, destEndpointPort);
			ProxyAuthentication = proxyAuthentication;
		}
		#endregion

		/// <summary>
		/// ProxyAuthentication - injects proxy authentication logic.
		/// </summary>
		public SOCKS.Version5.ProxyAuthentication ProxyAuthentication
		{
			set { proxyClient.ProxyAuthentication = value; }
		}

		public override bool IsOpen
		{
			get
			{
				if (proxyClient != null)
				{
					if (proxyClient.TcpClient != null)
					{
						if (proxyClient.TcpClient.Connected)
							return true;
					}
				}
				return false;
			}
		}

		public override void Open()
		{
			try
			{
				proxyClient.Connect(destinationEndpoint.Item1, destinationEndpoint.Item2);
				inputStream = proxyClient.TcpClient.GetStream();
				outputStream = proxyClient.TcpClient.GetStream();
			}
			catch (SOCKS.Version5.AccessDenied ex)
			{
				throw new Thrift.Transport.TAccessDenied(ex.Message);
			}
			catch (SOCKS.Version5.ProxyException ex)
			{
				throw new Thrift.Transport.TTransportException(ex.Message);
			}
		}

		public override void Close()
		{
			base.Close();
			proxyClient.Dispose();
		}

		public int Timeout
		{
			set
			{
				proxyClient.TcpClient.ReceiveTimeout = proxyClient.TcpClient.SendTimeout = value;
			}
		}

		private SOCKS.Version5.ProxyClient proxyClient = new SOCKS.Version5.ProxyClient();
		private Tuple<string, ushort> destinationEndpoint;
	}
}
