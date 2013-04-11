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

//
// SOCKS Protocol Version 5 - RFC 1928
// http://tools.ietf.org/html/rfc1928
//
// Only CONNECT method and user authentication are implemented
// at this moment.
//

using System;
using System.Text;
using System.Runtime.Serialization;
using System.Net;
using System.Net.Sockets;
using System.Globalization;

namespace SOCKS.Version5
{
	public enum AuthenticationMethod
	{
		NoAuthenticationRequired    = 0x00, // o  X'00' NO AUTHENTICATION REQUIRED
		GSSAPI                      = 0x01, // o  X'01' GSSAPI
		UsernamePassword            = 0x02, // o  X'02' USERNAME/PASSWORD
											// o  X'03' to X'7F' IANA ASSIGNED
											// o  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
		NoAcceptableMethods         = 0xFF  // o  X'FF' NO ACCEPTABLE METHODS
	}

	#region SOCKS Client Exceptions
	/// <summary>
	/// This exception indicates exceptional situation that might occur during proxy logic.
	/// </summary>
	[Serializable()]
	public class ProxyException : Exception
	{
		/// <summary>
		/// Constructor.
		/// </summary>
		public ProxyException()
		{
		}

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="message">Exception message text.</param>
		public ProxyException(string message)
			: base(message)
		{
		}

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="message">Exception message text.</param>
		/// <param name="innerException">The inner exception object.</param>
		public ProxyException(string message, Exception innerException)
			:
			base(message, innerException)
		{
		}

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="info">Serialization information.</param>
		/// <param name="context">Stream context information.</param>
		protected ProxyException(SerializationInfo info,
			StreamingContext context)
			: base(info, context)
		{
		}
	}

	[Serializable()]
	public class AccessDenied : ProxyException
	{
	}
	#endregion

	public interface ProxyAuthentication
	{
		AuthenticationMethod Method { get; }
		bool Authenticate( NetworkStream stream );
	}

	#region ProxyAuthentication Implementations
	/// <summary>
	/// UsernamePasswordProxyAuthentication - authenticate using UsernamePassword method.
	/// Implementation of Username/Password Authentication for SOCKS V5 as defined in RFC 1929.
	/// http://tools.ietf.org/html/rfc1929
	/// </summary>
	class UsernamePasswordProxyAuthentication : ProxyAuthentication
	{
		// The VER field contains the current version of the subnegotiation,
		// which is X'01'.
		private const byte VER = 0x01;
		private const byte SUCCESS = 0x00;

		public UsernamePasswordProxyAuthentication( string user, string passwd )
		{
			username = user;
			password = passwd;
		}

		private byte[] PrepareCredentialsPacket()
		{
			// +----+------+----------+------+----------+
			// |VER | ULEN |  UNAME   | PLEN |  PASSWD  |
			// +----+------+----------+------+----------+
			// | 1  |  1   | 1 to 255 |  1   | 1 to 255 |
			// +----+------+----------+------+----------+
			byte[] credentials = new byte[1 + 1 + username.Length + 1 + password.Length];
			credentials[0] = VER;
			credentials[1] = (byte)username.Length;
			Array.Copy(ASCIIEncoding.ASCII.GetBytes(username), 0, credentials, 2, username.Length);
			credentials[username.Length + 2] = (byte)password.Length;
			Array.Copy(ASCIIEncoding.ASCII.GetBytes(password), 0, credentials, username.Length + 3, password.Length);

			return credentials;
		}

		private bool GetResponse(NetworkStream stream)
		{
			// +----+--------+
			// |VER | STATUS |
			// +----+--------+
			// | 1  |   1    |
			// +----+--------+
			byte[] response = new byte[2];
			stream.Read(response, 0, response.Length);
			return response[1] == SUCCESS;
		}

		public bool Authenticate( NetworkStream stream )
		{
			var credentials = PrepareCredentialsPacket();
			stream.Write(credentials, 0, credentials.Length);
			return GetResponse(stream);
		}

		public AuthenticationMethod Method
		{
			get { return AuthenticationMethod.UsernamePassword; }
		}

		private string username;
		private string password;
	}

	/// <summary>
	/// NoAuthenticationRequiredProxyAuthentication - proxy server does not require user authentication.
	/// </summary>
	class NoAuthenticationRequiredProxyAuthentication : ProxyAuthentication
	{
		public AuthenticationMethod Method
		{
			get { return AuthenticationMethod.NoAuthenticationRequired; }
		}

		public bool Authenticate(NetworkStream stream)
		{
			return true;
		}
	}
	#endregion

	public class ProxyClient : System.IDisposable
	{
		private string proxyHost;
		private ushort proxyPort;
		private TcpClient tcpClient;
		private ProxyAuthentication auth = new NoAuthenticationRequiredProxyAuthentication();

		#region ProxyProperties
		/// <summary>
		/// Gets or sets host name or IP address of the proxy server.
		/// </summary>
		public string ProxyHost
		{
			get { return proxyHost; }
			set { proxyHost = value; }
		}

		/// <summary>
		/// Gets or sets port used to connect to proxy server.
		/// </summary>
		public ushort ProxyPort
		{
			get { return proxyPort; }
			set { proxyPort = value; }
		}

		/// <summary>
		/// Gets or sets the TcpClient object. Existing connection to the proxy server.
		/// </summary>
		public TcpClient TcpClient
		{
			get { return tcpClient; }
			set { tcpClient = value;  }
		}

		/// <summary>
		/// Injects proxy authentication logic.
		/// </summary>
		public ProxyAuthentication ProxyAuthentication
		{
			private get { return auth; }
			set { auth = value; }
		}
		#endregion

		public TcpClient Connect(string destinationHost, ushort destinationPort)
		{
			if (String.IsNullOrEmpty(destinationHost))
				throw new ArgumentNullException("destinationHost");

			if (destinationPort <= 0 || destinationPort > 65535)
				throw new ArgumentOutOfRangeException("destinationPort", "port must be greater than zero and less than 65535");

			// create the connection if there were not passed any
			if (tcpClient == null)
			{
				if (String.IsNullOrEmpty(proxyHost))
					throw new ProxyException("Proxy server address was not specified. ProxyHost must contain a value.");

				if (proxyPort <= 0 || proxyPort > 65535)
					throw new ProxyException("Invalid proxy port number. ProxyPort value must be greater than zero and less than 65535.");

				tcpClient = new TcpClient();
				tcpClient.Connect(proxyHost, proxyPort);
			}

			HandleProxyAuthentication();
			SendRequest(Command.CONNECT, destinationHost, destinationPort);
			var status = GetReply();
			if (status != Status.Succeeded)
				throw new ProxyException(StatusMessage(status));

			return tcpClient;
		}

		#region SOCKSProtocolConstants
		private const byte VER = 0x05;
		private const byte RSV = 0x00;
		#endregion

		#region AuthenticationMethod
		private void SendPreferredAuthMethod()
		{
			// precondition check
			System.Diagnostics.Debug.Assert(tcpClient.Connected, "Not connected to Proxy server.");

			// The client connects to the server, and sends a version
			// identifier/method selection message:
			//
			// +----+----------+----------+
			// |VER | NMETHODS | METHODS  |
			// +----+----------+----------+
			// | 1  |    1     | 1 to 255 |
			// +----+----------+----------+
			byte[] verIdMethSelMsg = new byte[3];
			verIdMethSelMsg[0] = VER;
			verIdMethSelMsg[1] = 1; // currently we handle only one authentication method at the time
			verIdMethSelMsg[2] = (byte)ProxyAuthentication.Method;

			var stream = tcpClient.GetStream();
			stream.Write(verIdMethSelMsg, 0, verIdMethSelMsg.Length);
		}

		private AuthenticationMethod GetServerSelectionAuthMethod()
		{
			// precondition check
			System.Diagnostics.Debug.Assert(tcpClient.Connected, "Not connected to Proxy server.");

			// The server selects from one of the methods given in METHODS, and
			// sends a METHOD selection message:

			//                      +----+--------+
			//                      |VER | METHOD |
			//                      +----+--------+
			//                      | 1  |   1    |
			//                      +----+--------+

			//If the selected METHOD is X'FF', none of the methods listed by the
			//client are acceptable, and the client MUST close the connection.

			//The values currently defined for METHOD are:

			//       o  X'00' NO AUTHENTICATION REQUIRED
			//       o  X'01' GSSAPI
			//       o  X'02' USERNAME/PASSWORD
			//       o  X'03' to X'7F' IANA ASSIGNED
			//       o  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
			//       o  X'FF' NO ACCEPTABLE METHODS

			//The client and server then enter a method-specific sub-negotiation.

			byte[] response = new byte[2];
			var stream = tcpClient.GetStream();
			stream.Read(response, 0, response.Length);

			return (AuthenticationMethod)response[1];
		}

		private AuthenticationMethod NegotiateAuthenticationMethod()
		{
			SendPreferredAuthMethod();
			return GetServerSelectionAuthMethod();
		}

		void HandleProxyAuthentication()
		{
			var serverAuthMethod = NegotiateAuthenticationMethod();
			if (serverAuthMethod == ProxyAuthentication.Method)
			{
				if (!ProxyAuthentication.Authenticate(TcpClient.GetStream()))
					throw new AccessDenied();
			}
		}
		#endregion

		//o  ATYP   address type of following address
		private enum AddressType
		{
			IP_V4_address   = 0x01,
			DOMAINNAME      = 0x03,
			IP_V6_address   = 0x04
		}

		//o  CMD
		private enum Command
		{
			CONNECT         = 0x01,     // o  CONNECT X'01'
			BIND            = 0x02,     // o  BIND X'02'
			UDP_ASSOCIATE   = 0x03      // o  UDP ASSOCIATE X'03'
		}

		void SendRequest(Command cmd, string destinationHost, ushort destinationPort)
		{
			// precondition check
			System.Diagnostics.Debug.Assert(tcpClient.Connected, "Not connected to Proxy server.");

			var addressType = GetAddressType(destinationHost);
			byte[] destAddress = GetAddressBytes(addressType, destinationHost);
			byte[] destPort = GetPortBytes(destinationPort);

			// The SOCKS request is formed as follows:
			//
			//       +----+-----+-------+------+----------+----------+
			//       |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
			//       +----+-----+-------+------+----------+----------+
			//       | 1  |  1  | X'00' |  1   | Variable |    2     |
			//       +----+-----+-------+------+----------+----------+
			//
			//    Where:
			//
			//         o  VER    protocol version: X'05'
			//         o  CMD
			//            o  CONNECT X'01'
			//            o  BIND X'02'
			//            o  UDP ASSOCIATE X'03'
			//         o  RSV    RESERVED
			//         o  ATYP   address type of following address
			//            o  IP V4 address: X'01'
			//            o  DOMAINNAME: X'03'
			//            o  IP V6 address: X'04'
			//         o  DST.ADDR       desired destination address
			//         o  DST.PORT desired destination port in network octet
			//            order
			//
			//  The SOCKS server will typically evaluate the request based on source
			//  and destination addresses, and return one or more reply messages, as
			//  appropriate for the request type.

			byte[] request = new byte[4 + destAddress.Length + 2];
			request[0] = VER;
			request[1] = (byte)cmd;
			request[2] = RSV;
			request[3] = (byte)addressType;
			destAddress.CopyTo(request, 4);
			destPort.CopyTo(request, 4 + destAddress.Length);

			var stream = tcpClient.GetStream();
			stream.Write(request, 0, request.Length);
		}

		// reply status
		private enum Status
		{
			Succeeded                       = 0x00,
			GeneralSOCKSServerFailure       = 0x01,
			ConnectionNotAllowedByRuleset   = 0x02,
			NetworkUnreachable              = 0x03,
			HostUnreachable                 = 0x04,
			ConnectionRefused               = 0x05,
			TTLExpired                      = 0x06,
			CommandNotSupported             = 0x07,
			AddressTypeNotSupported         = 0x08,
			// o  X'09' to X'FF' unassigned
		}

		Status GetReply()
		{
			// precondition check
			System.Diagnostics.Debug.Assert(tcpClient.Connected, "Not connected to Proxy server.");

			// The SOCKS request information is sent by the client as soon as it has
			// established a connection to the SOCKS server, and completed the
			// authentication negotiations.  The server evaluates the request, and
			// returns a reply formed as follows:
			//
			//      +----+-----+-------+------+----------+----------+
			//      |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
			//      +----+-----+-------+------+----------+----------+
			//      | 1  |  1  | X'00' |  1   | Variable |    2     |
			//      +----+-----+-------+------+----------+----------+
			//
			//   Where:
			//
			//        o  VER    protocol version: X'05'
			//        o  REP    Reply field:
			//           o  X'00' succeeded
			//           o  X'01' general SOCKS server failure
			//           o  X'02' connection not allowed by ruleset
			//           o  X'03' Network unreachable
			//           o  X'04' Host unreachable
			//           o  X'05' Connection refused
			//           o  X'06' TTL expired
			//           o  X'07' Command not supported
			//           o  X'08' Address type not supported
			//           o  X'09' to X'FF' unassigned
			//        o  RSV    RESERVED
			//        o  ATYP   address type of following address
			//           o  IP V4 address: X'01'
			//           o  DOMAINNAME: X'03'
			//           o  IP V6 address: X'04'
			//        o  BND.ADDR       server bound address
			//        o  BND.PORT       server bound port in network octet order
			//
			// Fields marked RESERVED (RSV) must be set to X'00'.
			//
			// If the chosen method includes encapsulation for purposes of
			// authentication, integrity and/or confidentiality, the replies are
			// encapsulated in the method-dependent encapsulation.
			byte[] response = new byte[255];

			var stream = tcpClient.GetStream();
			stream.Read(response, 0, response.Length);

			return (Status)response[1];
		}

		#region utils
		private string StatusMessage(Status status)
		{
			switch (status)
			{
				case Status.Succeeded: return "succeeded";
				case Status.GeneralSOCKSServerFailure: return "general SOCKS server failure";
				case Status.ConnectionNotAllowedByRuleset: return "connection not allowed by ruleset";
				case Status.NetworkUnreachable: return "Network unreachable";
				case Status.HostUnreachable: return "Host unreachable";
				case Status.ConnectionRefused: return "Connection refused";
				case Status.TTLExpired: return "TTL expired";
				case Status.CommandNotSupported: return "Command not supported";
				case Status.AddressTypeNotSupported: return "Address type not supported";
				default: return "unassigned";
			}
		}

		private AddressType GetAddressType(string host)
		{
			IPAddress ip = null;
			var result = IPAddress.TryParse(host, out ip);

			if (!result)
				return AddressType.DOMAINNAME;

			switch (ip.AddressFamily)
			{
			case AddressFamily.InterNetwork:
					return AddressType.IP_V4_address;
			case AddressFamily.InterNetworkV6:
				return AddressType.IP_V6_address;
			default:
				throw new ProxyException(String.Format(CultureInfo.InvariantCulture, "The host addess {0} of type '{1}' is not a supported address type.", host));
			}
		}

		private byte[] GetPortBytes(ushort value)
		{
			byte[] array = new byte[2];
			array[0] = Convert.ToByte(value / 256);
			array[1] = Convert.ToByte(value % 256);
			return array;
		}

		private byte[] GetAddressBytes(AddressType addrType, string host)
		{
			switch (addrType)
			{
				case AddressType.IP_V4_address:
				case AddressType.IP_V6_address:
					return IPAddress.Parse(host).GetAddressBytes();
				case AddressType.DOMAINNAME:
					//  create a byte array to hold the host name bytes plus one byte to store the length
					byte[] bytes = new byte[host.Length + 1];
					//  if the address field contains a fully-qualified domain name.  The first
					//  octet of the address field contains the number of octets of name that
					//  follow, there is no terminating NUL octet.
					bytes[0] = Convert.ToByte(host.Length);
					Encoding.ASCII.GetBytes(host).CopyTo(bytes, 1);
					return bytes;
				default:
					return null;
			}
		}

		public void Dispose()
		{
			tcpClient.Close();
		}
		#endregion

	}
}
