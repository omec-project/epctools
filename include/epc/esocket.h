/*
* Copyright (c) 2009-2019 Brian Waters
* Copyright (c) 2019 Sprint
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef __esocket_h_included
#define __esocket_h_included

#include <csignal>
#include <cstring>
#include <unordered_map>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "ebase.h"
#include "ecbuf.h"
#include "eerror.h"
#include "estatic.h"
#include "estring.h"
#include "etevent.h"
#include "ehash.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @brief The namespace for all socket related classes.
namespace ESocket
{
   /// @cond DOXYGEN_EXCLUDE
   // (Maximum message length) = (max IP packet size) - (min IP header length) - (udp header length)
   // 65507 = 65535 - 20 - 8
   const Int UPD_MAX_MSG_LENGTH = 65507;

   const Int EPC_INVALID_SOCKET = -1;
   const Int EPC_SOCKET_ERROR = -1;
   typedef Int EPC_SOCKET;
   typedef void *PSOCKETOPT;
   typedef void *PSNDRCVBUFFER;
   typedef socklen_t EPC_SOCKLEN;

   namespace TCP
   {
      template<class TQueue, class TMessage> class Talker;
      template<class TQueue, class TMessage> class Listener;
   }
   template<class TQueue, class TMessage> class UDP;
   template<class TQueue, class TMessage> class Thread;
   /// @endcond

   /// @brief Defines the possible address family values.
   enum class Family
   {
      /// undefined
      Undefined,
      /// IPv4 address
      INET,
      /// IPv6 address
      INET6
   };

   /// @brief Defines the possible socket types.
   enum class SocketType
   {
      /// an undefined socket
      Undefined,
      /// a TCP talker socket
      TcpTalker,
      /// a TCP listener socket
      TcpListener,
      /// a UDP socket
      Udp
   };

   /// @brief The socket connection state.
   enum class SocketState
   {
      /// undefined
      Undefined,
      /// socket is disconnected
      Disconnected,
      /// socket is connecting
      Connecting,
      /// socket is listening
      Listening,
      /// socket is connected
      Connected
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   DECLARE_ERROR_ADVANCED(AddressError_UnknownAddressType);
   DECLARE_ERROR_ADVANCED(AddressError_CannotConvertInet2Inet6);
   DECLARE_ERROR_ADVANCED(AddressError_CannotConvertInet62Inet);
   DECLARE_ERROR_ADVANCED(AddressError_ConvertingToString);
   DECLARE_ERROR_ADVANCED(AddressError_UndefinedFamily);

   DECLARE_ERROR_ADVANCED(BaseError_UnableToCreateSocket);
   DECLARE_ERROR_ADVANCED(BaseError_GetPeerNameError);

   DECLARE_ERROR_ADVANCED(TcpTalkerError_InvalidRemoteAddress);
   DECLARE_ERROR_ADVANCED(TcpTalkerError_UnableToConnect);
   DECLARE_ERROR_ADVANCED(TcpTalkerError_UnableToRecvData);
   DECLARE_ERROR_ADVANCED4(TcpTalkerError_InvalidSendState);
   DECLARE_ERROR_ADVANCED4(TcpTalkerError_InvalidReadState);
   DECLARE_ERROR_ADVANCED4(TcpTalkerError_InvalidWriteState);
   DECLARE_ERROR_ADVANCED4(TcpTalkerError_ReadingWritePacketLength);
   DECLARE_ERROR_ADVANCED(TcpTalkerError_SendingPacket);
   DECLARE_ERROR_ADVANCED(TcpTalkerError_InvalidReceiveState);

   DECLARE_ERROR_ADVANCED(TcpListenerError_UnableToListen);
   DECLARE_ERROR_ADVANCED(TcpListenerError_UnableToBindSocket);
   DECLARE_ERROR_ADVANCED(TcpListenerError_UnableToAcceptSocket);

   DECLARE_ERROR_ADVANCED(UdpError_AlreadyBound);
   DECLARE_ERROR_ADVANCED(UdpError_UnableToBindSocket);
   DECLARE_ERROR_ADVANCED(UdpError_UnableToRecvData);
   DECLARE_ERROR_ADVANCED(UdpError_SendingPacket);
   DECLARE_ERROR_ADVANCED4(UdpError_ReadingWritePacketLength);

   DECLARE_ERROR_ADVANCED(ThreadError_UnableToOpenPipe);
   DECLARE_ERROR_ADVANCED(ThreadError_UnableToReadPipe);
   DECLARE_ERROR_ADVANCED(ThreadError_UnableToWritePipe);
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Encapsulates a sockaddr_storage structure that represents a socket address.
   class Address
   {
   public:
      /// @brief Default constructor.
      Address() : m_addr() {}
      /// @brief Class constructor.
      /// @param addr the IP address string (IPv4 or IPv6).
      /// @param port the IP port.
      Address(cpStr addr, UShort port) { setAddress(addr,port); }
      /// @brief Class constructor.
      /// @param addr the IPv4 address.
      /// @param port the IP port.
      Address(const struct in_addr &addr, UShort port) { setAddress(addr,port); }
      /// @brief Class constructor.
      /// @param addr the IP IPv6 address.
      /// @param port the IP port.
      Address(const struct in6_addr &addr, UShort port) { setAddress(addr,port); }
      /// @brief Class constructor.
      /// @param addr the IPv4 socket address.
      Address(struct sockaddr_in &addr) { memcpy(&m_addr, &addr, sizeof(addr)); }
      /// @brief Class constructor.
      /// @param addr the IPv6 socket address.
      Address(struct sockaddr_in6 &addr) { memcpy(&m_addr, &addr, sizeof(addr)); }
      /// @brief Copy constructor.
      /// @param addr the Address object to copy.
      Address(const Address &addr) { memcpy(&m_addr, &addr, sizeof(addr)); }

      /// @brief Extracts a string object with the printable IP address.
      /// @return EString representation of the IP address.
      operator EString() const
      {
         Char buf[INET6_ADDRSTRLEN];

         if (m_addr.ss_family == AF_INET)
         {
            if (!inet_ntop(m_addr.ss_family,&((struct sockaddr_in*)&m_addr)->sin_addr.s_addr,buf,sizeof(buf)))
               throw AddressError_ConvertingToString();
         }
         else // AF_INET6
         {
            if (!inet_ntop(m_addr.ss_family,&((struct sockaddr_in6*)&m_addr)->sin6_addr.s6_addr,buf,sizeof(buf)))
               throw AddressError_ConvertingToString();
         }
         return EString(buf);
      }

      /// @brief Extracts the port.
      /// @return the port.
      operator UShort() const
      {
         if (m_addr.ss_family == AF_INET)
            return ntohs(((struct sockaddr_in*)&m_addr)->sin_port);
         if (m_addr.ss_family == AF_INET6)
            return ntohs(((struct sockaddr_in6*)&m_addr)->sin6_port);
         throw AddressError_UndefinedFamily();
      }

      /// @brief Retrieves the printable IP address.
      /// @return the printable IP address.
      EString getAddress() const { return *this; }
      /// @brief Retrievs the port.
      /// @return the port.
      UShort getPort() const { return *this; }

      /// @brief Retrieves a sockaddr pointer to the socket address.
      /// @return a sockaddr pointer to the socket address.
      const struct sockaddr_storage &getSockAddrStorage() const
      {
         return m_addr;
      }

      /// @brief Retrieves a sockaddr_storage reference to the socket address.
      /// @return a sockaddr_storage reference to the socket address.
      struct sockaddr *getSockAddr() const
      {
         return (struct sockaddr *)&m_addr;
      }

      /// @brief retrieves the length of the current socket address.
      /// @return the length of the current socket address.
      socklen_t getSockAddrLen() const
      {
         if (m_addr.ss_family == AF_INET)
            return sizeof(struct sockaddr_in);
         if (m_addr.ss_family == AF_INET6)
            return sizeof(struct sockaddr_in6);
         return sizeof(struct sockaddr_storage);
      }

      /// @brief Assignment operator.
      /// @param addr the Address object to copy.
      /// @return a reference to this Address object.
      Address &operator=(const Address& addr)
      {
         memcpy(&m_addr, &addr, sizeof(m_addr));
         return *this;
      }

      /// @brief Assignment operator.
      /// @param addr the IPV4 address object to copy.
      /// @return a reference to this Address object.
      Address &operator=(const sockaddr_in &addr)
      {
         return setAddress(addr);
      }

      /// @brief Assignment operator.
      /// @param addr the IPV6 address object to copy.
      /// @return a reference to this Address object.
      Address &operator=(const sockaddr_in6 &addr)
      {
         return setAddress(addr);
      }

      /// @brief Assigns a port value (allowing IPADDR_ANY).
      /// @param port the port.
      /// @return a reference to this Address object.
      Address &operator=(UShort port)
      {
         return setAddress(port);
      }

      /// @brief Retrieves the address family for this address.
      /// @return the address family for this address.
      Family getFamily() const
      {
         return m_addr.ss_family == AF_INET ? Family::INET :
                m_addr.ss_family == AF_INET6 ? Family::INET6 : Family::Undefined;
      }

      /// @brief Retrieves the sockaddr_storage.
      /// @return a reference to the sockaddr_storage member.
      const struct sockaddr_storage &getStorage() const
      {
         return m_addr;
      }

      /// @brief Retrieves a reference to this address as an IPv4 address.
      /// @return a reference to this address as an IPv4 address.
      const struct sockaddr_in &getInet() const
      {
         if (m_addr.ss_family != AF_INET)
            throw AddressError_CannotConvertInet62Inet();
         return (struct sockaddr_in &)m_addr;
      }

      /// @brief Retrieves a reference to this address as an IPv6 address.
      /// @return a reference to this address as an IPv6 address.
      const struct sockaddr_in6 &getInet6() const
      {
         if (m_addr.ss_family != AF_INET6)
            throw AddressError_CannotConvertInet2Inet6();
         return (struct sockaddr_in6 &)m_addr;
      }

      /// @brief Assigns the socket address.
      /// @param addr the IP address.
      /// @param port the port.
      /// @return a reference to this address object.
      Address &setAddress(cpStr addr, UShort port)
      {
         clear();
         if (inet_pton(AF_INET,addr,&((struct sockaddr_in*)&m_addr)->sin_addr) == 1)
         {
            ((struct sockaddr_in*)&m_addr)->sin_family = AF_INET;
            ((struct sockaddr_in*)&m_addr)->sin_port = htons(port);
            return *this;
         }

         clear();
         if (inet_pton(AF_INET6,addr,&((struct sockaddr_in6*)&m_addr)->sin6_addr) == 1)
         {
            ((struct sockaddr_in6*)&m_addr)->sin6_family = AF_INET6;
            ((struct sockaddr_in6*)&m_addr)->sin6_port = htons(port);
            ((struct sockaddr_in6*)&m_addr)->sin6_flowinfo = 0;
            ((struct sockaddr_in6*)&m_addr)->sin6_scope_id = 0;
            return *this;
         }

         throw AddressError_UnknownAddressType();
      }

      /// @brief Assigns the IPv4 socket address.
      /// @param addr the IPv4 address.
      /// @param port the port.
      /// @return a reference to this address object.
      Address &setAddress(const struct in_addr &addr, UShort port)
      {
         clear();
         std::memcpy(&((struct sockaddr_in*)&m_addr)->sin_addr, &addr, sizeof(((struct sockaddr_in*)&m_addr)->sin_addr));
         ((struct sockaddr_in*)&m_addr)->sin_family = AF_INET;
         ((struct sockaddr_in*)&m_addr)->sin_port = htons(port);
         return *this;
      }

      /// @brief Assigns the IPv6 socket address.
      /// @param addr the IPv6 address.
      /// @param port the port.
      /// @return a reference to this address object.
      Address &setAddress(const struct in6_addr &addr, UShort port)
      {
         clear();
         std::memcpy(&((struct sockaddr_in6*)&m_addr)->sin6_addr, &addr, sizeof(((struct sockaddr_in6*)&m_addr)->sin6_addr));
         ((struct sockaddr_in6*)&m_addr)->sin6_family = AF_INET6;
         ((struct sockaddr_in6*)&m_addr)->sin6_port = htons(port);
         ((struct sockaddr_in6*)&m_addr)->sin6_flowinfo = 0;
         ((struct sockaddr_in6*)&m_addr)->sin6_scope_id = 0;
         return *this;
      }

      /// @brief Assigns the socket address.
      /// @param port the port.
      /// @param fam the address family.
      /// @return a reference to this address object.
      Address &setAddress(UShort port, Family fam = Family::INET6)
      {
         switch (fam)
         {
            case Family::INET:
            {
               ((struct sockaddr_in*)&m_addr)->sin_family = AF_INET;
               ((struct sockaddr_in*)&m_addr)->sin_port = htons(port);
               ((struct sockaddr_in*)&m_addr)->sin_addr.s_addr = INADDR_ANY;
               break;
            }
            case Family::INET6:
            {
               ((struct sockaddr_in6*)&m_addr)->sin6_family = AF_INET6;
               ((struct sockaddr_in6*)&m_addr)->sin6_port = htons(port);
               ((struct sockaddr_in6*)&m_addr)->sin6_flowinfo = 0;
               ((struct sockaddr_in6*)&m_addr)->sin6_scope_id = 0;
               ((struct sockaddr_in6*)&m_addr)->sin6_addr = in6addr_any;
               break;
            }
            default:
            {
               throw AddressError_UndefinedFamily();
            }
         }
         return *this;
      }

      Address &setAddress(const sockaddr_in &addr)
      {
         clear();
         ((struct sockaddr_in*)&m_addr)->sin_family = AF_INET;
         ((struct sockaddr_in*)&m_addr)->sin_port = addr.sin_port;
         ((struct sockaddr_in*)&m_addr)->sin_addr.s_addr = addr.sin_addr.s_addr;
         return *this;
      }

      Address &setAddress(const sockaddr_in6 &addr)
      {
         clear();
         ((struct sockaddr_in6*)&m_addr)->sin6_family = AF_INET6;
         ((struct sockaddr_in6*)&m_addr)->sin6_port = addr.sin6_port;
         ((struct sockaddr_in6*)&m_addr)->sin6_flowinfo = 0;
         ((struct sockaddr_in6*)&m_addr)->sin6_scope_id = 0;
         ((struct sockaddr_in6*)&m_addr)->sin6_addr = addr.sin6_addr;
         return *this;
      }

      /// @brief Clears this address.
      /// @return a reference to this address object.
      Address &clear()
      {
         memset( &m_addr, 0, sizeof(m_addr) );
         return *this;
      }

      /// @brief Determines the address family.
      /// @return the address family.
      static Family getFamily(cpStr addr)
      {
         in_addr ipv4;
         in6_addr ipv6;

         if (inet_pton(AF_INET,addr,&ipv4) == 1)
            return Family::INET;
         if (inet_pton(AF_INET6,addr,&ipv6) == 1)
            return Family::INET6;
         return Family::Undefined;
      }

   private:
      struct sockaddr_storage m_addr;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief The base socket class.
   template <class TQueue, class TMessage>
   class Base
   {
      friend class TCP::Talker<TQueue,TMessage>;
      friend class TCP::Listener<TQueue,TMessage>;
      friend class UDP<TQueue,TMessage>;
      friend class Thread<TQueue,TMessage>;

   public:
      /// @brief Virtual class destructor.
      virtual ~Base()
      {
         close();
      }

      /// @brief Retrieves the socket thread that this socket is associated with.
      /// @return the socket thread that this socket is associated with.
      Thread<TQueue,TMessage> &getThread()
      {
         return m_thread;
      }

      /// @brief Retrieves the socket type.
      /// @return the socket type.
      SocketType getSocketType()
      {
         return m_socktype;
      }

      /// @brief Retrieves the address family.
      /// @return the address family.
      Int getFamily()
      {
         return m_family;
      }

      /// @brief Retrieves the socket type.
      /// @return the address family.
      Int getType()
      {
         return m_type;
      }

      /// @brief Retrieves the protocol.
      /// @return the protocol.
      Int getProtocol()
      {
         return m_protocol;
      }

      /// @brief Retrieves the last error value.
      /// @return the last error value.
      Int getError()
      {
         return m_error;
      }

      cpStr getErrorDescription()
      {
         static Char desc[256];
         return strerror_r(m_error, desc, sizeof(desc));
      }

      /// @brief Closes this socket.
      Void close()
      {
         disconnect();
         onClose();
      }

      /// @brief Disconnects this socket.
      virtual Void disconnect()
      {
         getThread().unregisterSocket(this);
         if (m_handle != EPC_INVALID_SOCKET)
         {
            ::close(m_handle);
            m_handle = EPC_INVALID_SOCKET;
         }
      }

      /// @brief Retrieves the socket file handle.
      /// @return the socket file handle.
      Int getHandle()
      {
         return m_handle;
      }

      /// @brief Retrieves the description of the connection state.
      /// @return the description of the connection state.
      cpStr getStateDescription(SocketState state)
      {
         switch (state)
         {
            case SocketState::Disconnected:  return "DISCONNECTED";
            case SocketState::Connecting:    return "CONNECTING";
            case SocketState::Connected:     return "CONNECTED";
            case SocketState::Listening:     return "LISTENING";
            default:                         return "UNDEFINED";
         }
      }

   protected:
      /// @cond DOXYGEN_EXCLUDE
      Base(Thread<TQueue,TMessage> &thread, SocketType socktype, Int family, Int type, Int protocol)
         : m_thread( thread ),
         m_socktype( socktype ),
         m_family( family ),
         m_type( type ),
         m_protocol( protocol ),
         m_error( 0 ),
         m_handle( EPC_INVALID_SOCKET )
      {
      }
      
      Void createSocket(Int family, Int type, Int protocol)
      {
         m_handle = socket(family, type, protocol);
         if (m_handle == EPC_INVALID_SOCKET)
            throw BaseError_UnableToCreateSocket();

         m_family = family;
         m_type = type;
         m_protocol = protocol;

         setOptions();
      }

      Void assignAddress(cpStr ipaddr, UShort port, Int family, Int socktype,
                        Int flags, Int protocol, struct addrinfo **address);
      Int setError()
      {
         m_error = errno;
         return m_error;
      }

      Void setError(Int error)
      {
         m_error = error;
      }

      Void setHandle(Int handle)
      {
         disconnect();
         m_handle = handle;
         setOptions();
      }

      Base &setFamily(Int family)
      {
         m_family = family;
         return *this;
      }

      Address &setLocalAddress(Address &addr)
      {
         addr.clear();

         socklen_t sockaddrlen = addr.getSockAddrLen();;

         if (getsockname(m_handle, addr.getSockAddr(), &sockaddrlen) < 0)
            throw BaseError_GetPeerNameError();

         return addr;
      }

      Address &setRemoteAddress(Address &addr)
      {
         addr.clear();

         socklen_t sockaddrlen = addr.getSockAddrLen();;

         if (getpeername(m_handle, addr.getSockAddr(), &sockaddrlen) < 0)
            throw BaseError_GetPeerNameError();

         return addr;
      }

      virtual Void onReceive()
      {
      }

      virtual Void onConnect()
      {
      }

      virtual Void onClose()
      {
      }

      virtual Void onError()
      {
      }
      /// @endcond

   private:
      Void setOptions()
      {
         struct linger l;
         l.l_onoff = 1;
         l.l_linger = 0;
         setsockopt(m_handle, SOL_SOCKET, SO_LINGER, (PSOCKETOPT)&l, sizeof(l));

         fcntl(m_handle, F_SETFL, O_NONBLOCK);

         getThread().registerSocket(this);
      }

      Thread<TQueue,TMessage> &m_thread;

      SocketType m_socktype;
      Int m_family;
      Int m_type;
      Int m_protocol;
      Int m_error;

      Int m_handle;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Namespace for TCP related classes.
   namespace TCP
   {
      //////////////////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////

      /// @brief A TCP socket class capabile of sending and receiving data.
      template <class TQueue, class TMessage>
      class Talker : public Base<TQueue,TMessage>
      {
         friend class Thread<TQueue,TMessage>;

      public:
         /// @brief Class constructor.
         /// @param thread the socket thread that this socket is associated with.
         /// @param bufsize the size of the send and receive circular buffers.
         Talker(Thread<TQueue,TMessage> &thread, Int bufsize=2097152)
            : Base<TQueue,TMessage>(thread, SocketType::TcpTalker, AF_INET6, SOCK_STREAM, IPPROTO_TCP),
              m_state( SocketState::Undefined ),
              m_sending(False),
              m_rbuf(bufsize),
              m_wbuf(bufsize)
         {
         }
         /// @brief Class destrucor.
         virtual ~Talker()
         {
         }
         /// @brief Retrieves the local socket address.
         /// @return the local socket address.
         Address &getLocal()
         {
            return m_local;
         }
         /// @brief Retrieves the IP address associated with the local socket.
         /// @return the IP address associated with the local socket.
         EString getLocalAddress() const
         {
            return m_local;
         }
         /// @brief Retrieves the port associated with the local socket.
         /// @brief the port associated with the local socket.
         UShort getLocalPort() const
         {
            return m_local;
         }
         /// @brief Assigns the local socket address.
         /// @param addr the IP address.
         /// @param port the port.
         /// @return a reference to this Talker object.
         Talker &setLocal(cpStr addr, UShort port)
         {
            m_local.setAddress(addr,port);
            return *this;
         }
         /// @brief Assigns the local socket address.
         /// @param addr the address object to copy.
         /// @return a reference to this Talker object.
         Talker &setLocal(const Address &addr)
         {
            m_local = addr;
            return *this;
         }
         /// @brief Retrieves the remote socket address.
         /// @return the remote socket address.
         Address &getRemote()
         {
            return m_remote;
         }
         /// @brief Retrieves the IP address associated with the remote socket.
         /// @return the IP address associated with the remote socket.
         EString getRemoteAddress() const
         {
            return m_remote;
         }
         /// @brief Retrieves the port associated with the remote socket.
         /// @brief the port associated with the remote socket.
         UShort getRemotePort() const
         {
            return m_remote;
         }
         /// @brief Assigns the remote socket address.
         /// @param addr the IP address.
         /// @param port the port.
         /// @return a reference to this Talker object.
         Talker &setRemote(cpStr addr, UShort port)
         {
            m_remote.setAddress(addr,port);
            return *this;
         }
         /// @brief Assigns the remote socket address.
         /// @param addr the address object to copy.
         /// @return a reference to this Talker object.
         Talker &setRemote(const Address &addr)
         {
            m_remote = addr;
            return *this;
         }
         /// @brief Initiates an IP connection with to the previously assigned remote socket address.
         Void connect()
         {
            if (getRemote().getFamily() != Family::INET && getRemote().getFamily() != Family::INET6)
               throw TcpTalkerError_InvalidRemoteAddress();

            Int family = getRemote().getFamily() == Family::INET ? AF_INET : AF_INET6;
            Int type = this->getType();
            Int protocol = this->getProtocol();

            this->createSocket( family, type, protocol );

            int result = ::connect(this->getHandle(), getRemote().getSockAddr(), getRemote().getSockAddrLen());

            if (result == 0)
            {
               setState( SocketState::Connected );
               onConnect();
            }
            else if (result == -1)
            {
               this->setError();
               if (this->getError() != EINPROGRESS && this->getError() != EWOULDBLOCK)
                  throw TcpTalkerError_UnableToConnect();

               setState( SocketState::Connecting );

               this->getThread().bump();



            }
         }
         /// @brief Initiates an IP connection.
         /// @param addr the remote socket address.
         Void connect(Address &addr)
         {
            m_remote = addr;
            connect();
         }
         /// @brief Initiates an IP connection.
         /// @param addr the remote socket IP address.
         /// @param port the remote socket port.
         Void connect(cpStr addr, UShort port)
         {
            m_remote.setAddress( addr, port );
            connect();
         }
         /// @brief Retrieves the number of bytes in the receive buffer.
         /// @return the number of bytes in the receive buffer.
         Int bytesPending()
         {
            return m_rbuf.used();
         }
         /// @brief Rtrieves the specified number of bytes from the receive buffer
         ///    without updating the read position.
         /// @param dest the location to write the data read.
         /// @param len the desired number of bytes to read.
         /// @return the number of actual bytes read.
         Int peek(pUChar dest, Int len)
         {
            return m_rbuf.peekData(dest, 0, len);
         }
         /// @brief Rtrieves the specified number of bytes from the receive buffer.
         /// @param dest the location to write the data read.
         /// @param len the desired number of bytes to read.
         /// @return the number of actual bytes read.
         Int read(pUChar dest, Int len)
         {
            return m_rbuf.readData(dest, 0, len);
         }
         /// @brief Writes data to the socket.  This is a thread safe method.
         /// @param src the location to the data to write.
         /// @param len the desired number of bytes to write.
         Void write(cpUChar src, Int len)
         {
            {
               EMutexLock l(m_wbuf.getMutex());
               m_wbuf.writeData((cpUChar)&len, 0, sizeof(len), True);
               m_wbuf.writeData(src, 0, len, True);
            }

            send();
         }
         /// @brief Retrieves indication if this socket is in the process of sending data.
         /// @return True indicates that data is being sent, otherwise False.
         Bool getSending()
         {
            return m_sending;
         }
         /// @brief Retrieves the connection state.
         /// @return the connection state.
         SocketState getState()
         {
            return m_state;
         }
         /// @brief Retrieves the description of the current connection state.
         /// @return the description of the current connection state.
         cpStr getStateDescription()
         {
            return Base<TQueue,TMessage>::getStateDescription( m_state );
         }
         /// @brief Disconnects this socket.
         Void disconnect()
         {
            Base<TQueue,TMessage>::disconnect();
            m_state = SocketState::Disconnected;
            m_remote.clear();
         }
         /// @brief Called when data has been received.
         virtual Void onReceive()
         {
         }
         /// @brief Called when a connection has been established.
         virtual Void onConnect()
         {
         }
         /// @brief Called when the socket has been closed.
         virtual Void onClose()
         {
            this->close();            
         }
         /// @brief Called when an error is detected on the socket.
         virtual Void onError()
         {
         }

      protected:
         /// @cond DOXYGEN_EXCLUDE
         Talker &setAddresses()
         {
            Base<TQueue,TMessage>::setLocalAddress( m_local );
            Base<TQueue,TMessage>::setRemoteAddress( m_remote );
            return *this;
         }

         Talker &setState(SocketState state)
         {
            m_state = state;
            return *this;
         }

         Int recv()
         {
            //
            // modified this routine to use a buffer allocated from the stack
            // instead of a single buffer allocated from the heap (which had
            // been used for both reading and writing) to avoid between the
            // read and write process
            //
            UChar buf[2048];
            Int totalReceived = 0;

            while (True)
            {
               Int amtReceived = ::recv(this->getHandle(), (PSNDRCVBUFFER)buf, sizeof(buf), 0);
               if (amtReceived > 0)
               {
                  m_rbuf.writeData(buf, 0, amtReceived);
                  totalReceived += amtReceived;
               }
               else if (amtReceived == 0)
               {
                  setState( SocketState::Disconnected );
                  break;
               }
               else
               {
                  this->setError();
                  if (this->getError() == EWOULDBLOCK)
                     break;
                  throw TcpTalkerError_UnableToRecvData();
               }
            }

            return totalReceived;
         }

         Void send(Bool override = False)
         {
            UChar buf[2048];

            EMutexLock lck(m_sendmtx, False);
            if (!lck.acquire(False))
               return;

            if (!override && m_sending)
               return;

            if (m_wbuf.isEmpty())
            {
               m_sending = false;
               return;
            }

            if (getState() != SocketState::Connected)
            {
               // std::raise(SIGINT);
               throw TcpTalkerError_InvalidSendState(getStateDescription());
            }

            m_sending = true;
            while (true)
            {
               if (m_wbuf.isEmpty())
               {
                  m_sending = false;
                  break;
               }

               Int packetLength = 0;
               Int amtRead = m_wbuf.peekData((pUChar)&packetLength, 0, sizeof(packetLength));
               if (amtRead != sizeof(packetLength))
               {
                  EString msg;
                  msg.format("expected %d bytes, read %d bytes", sizeof(packetLength), amtRead);
                  throw TcpTalkerError_ReadingWritePacketLength(msg.c_str());
               }

               Int sentLength = 0;
               while (sentLength < packetLength)
               {
                  Int sendLength = packetLength - sentLength;
                  if (sendLength > (Int)sizeof(buf))
                     sendLength = sizeof(buf);

                  // get data from the circular buffer
                  amtRead = m_wbuf.peekData((pUChar)buf, sizeof(packetLength) + sentLength, sendLength);
                  if (amtRead != sendLength)
                  {
                     EString msg;
                     msg.format("expected %d bytes, read %d bytes", sendLength, amtRead);
                     throw TcpTalkerError_ReadingWritePacketLength(msg.c_str());
                  }

                  // write the data to the socket
                  Int amtWritten = send(buf, sendLength);
                  if (amtWritten == -1) // EWOULDBLOCK
                     break;

                  sentLength += amtWritten;
                  if (amtWritten != sendLength) // only part of the data was written
                     break;
               }

               packetLength -= sentLength;
               m_wbuf.readData(NULL, 0, sentLength + (!packetLength ? sizeof(packetLength) : 0));
               if (packetLength > 0)
               {
                  // need to update the buffer indicating the amount of the
                  // message remaining in the circular buffer
                  //fprintf(stderr,"wrote %d bytes of %d\n", sentLength, packetLength + sentLength);
                  m_wbuf.modifyData((pUChar)&packetLength, 0, (Int)sizeof(packetLength));
                  break;
               }
            }
         }
         /// @endcond

      private:
         Int send(pUChar pData, Int length)
         {
            Int result = ::send(this->getHandle(), (PSNDRCVBUFFER)pData, length, MSG_NOSIGNAL);

            if (result == -1)
            {
               this->setError();
               if (this->getError() != EWOULDBLOCK)
                  throw TcpTalkerError_SendingPacket();
            }

            return result;
         }

         SocketState m_state;
         Address m_local;
         Address m_remote;
         EMutexPrivate m_sendmtx;
         Bool m_sending;

         ECircularBuffer m_rbuf;
         ECircularBuffer m_wbuf;
      };

      //////////////////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////

      /// @brief Listens for incoming TCP/IP connections.
      template <class TQueue, class TMessage>
      class Listener : public Base<TQueue,TMessage>
      {
         friend class Thread<TQueue,TMessage>;

      public:
         /// @brief Class constructor.
         /// @param thread the socket thread that this socket is associated with.
         /// @param family the default address family.
         Listener(Thread<TQueue,TMessage> &thread, Family family = Family::INET6)
            : Base<TQueue,TMessage>(thread, SocketType::TcpListener, 
               family == Family::INET ? AF_INET : AF_INET6,
               SOCK_STREAM, IPPROTO_TCP),
              m_state( SocketState::Undefined ),
              m_backlog( -1 )
         {
         }
         /// @brief Class constructor.
         /// @param thread the socket thread that this socket is associated with.
         /// @param port the port to listen on.
         /// @param family the default address family.
         Listener(Thread<TQueue,TMessage> &thread, UShort port, Family family = Family::INET6)
            : Base<TQueue,TMessage>(thread, SocketType::TcpListener,
               family == Family::INET ? AF_INET : AF_INET6,
               SOCK_STREAM, IPPROTO_TCP),
              m_state( SocketState::Undefined ),
              m_backlog( -1 )
         {
            setPort( port );
         }
         /// @brief Class constructor.
         /// @param thread the socket thread that this socket is associated with.
         /// @param port the port to listen on.
         /// @param backlog the maximum number of "unaccepted" connections.
         /// @param family the default address family.
         Listener(Thread<TQueue,TMessage> &thread, UShort port, Int backlog, Family family = Family::INET6)
            : Base<TQueue,TMessage>(thread, SocketType::TcpListener,
               family == Family::INET ? AF_INET : AF_INET6,
               SOCK_STREAM, IPPROTO_TCP),
              m_state( SocketState::Undefined ),
              m_backlog( backlog )
         {
            setPort( port );
         }
         /// @brief Class destructor.
         virtual ~Listener()
         {
         }
         /// @brief Retrieves the current socket state.
         /// @return the current socket state.
         SocketState getState()
         {
            return m_state;
         }
         /// @brief Retrieves the local listening address.
         /// @return the local listening address.
         Address &getLocalAddress()
         {
            return m_local;
         }
         /// @brief Assigns the port to listen for incoming connections on.
         /// @param port the port to listen for incoming connections on.
         Void setPort(UShort port)
         {
            m_local = port;
            this->setFamily( m_local.getFamily() == Family::INET ? AF_INET : AF_INET6 );
         }
         /// @brief Retrieves the port being listened on for incoming connections.
         /// @return the port being listened on for incoming connections.
         UShort getPort()
         {
            return m_local;
         }
         /// @brief Assigns the maximum number of "unaccepted" connections.
         /// @param backlog the maximum number of "unaccepted" connections.
         Void setBacklog(Int backlog)
         {
            m_backlog = backlog;
         }
         /// @brief Retrieves the maximum number of "unaccepted" connections.
         /// @return the maximum number of "unaccepted" connections.
         Int getBacklog()
         {
            return m_backlog;
         }
         /// @brief Starts listening for incoming connections.
         Void listen()
         {
            bind();
            if (::listen(this->getHandle(), getBacklog()) == EPC_SOCKET_ERROR)
               throw TcpListenerError_UnableToListen();
            setState( SocketState::Listening );
         }
         /// @brief Starts listening for incoming connections.
         /// @param port the port to listen on.
         /// @param backlog the maximum number of "unaccepted" connections.
         Void listen(UShort port, Int backlog)
         {
            setPort(port);
            setBacklog(backlog);
            listen();
         }
         /// @brief Called to create a talking socket when a incoming connection is received.
         /// @param thread the socket thread the talking socket will be associated with.
         /// @return the created talking socket.
         virtual Talker<TQueue,TMessage> *createSocket(Thread<TQueue,TMessage> &thread) = 0;
         /// @brief Called to create a talking socket when a incoming connection is received.
         /// @return the created talking socket.
         Talker<TQueue,TMessage> *createSocket()
         {
            return createSocket(this->getThread());
         }
         /// @brief Called when this socket is closed.
         virtual Void onClose()
         {
            Base<TQueue,TMessage>::onClose();
            setState( SocketState::Undefined );
         }
         /// @brief Called when an error is detected on this socket.
         virtual Void onError()
         {
         }

      private:
         Void bind()
         {
            this->setFamily( m_local.getFamily() == Family::INET ? AF_INET : AF_INET6 );
            
            Base<TQueue,TMessage>::createSocket(this->getFamily(), this->getType(), this->getProtocol());

            int result = ::bind(this->getHandle(), getLocalAddress().getSockAddr(), getLocalAddress().getSockAddrLen());
            if (result == -1)
            {
               TcpListenerError_UnableToBindSocket err;
               this->close();
               throw err;
            }
         }

         Listener<TQueue,TMessage> &setState( SocketState state )
         {
            m_state = state;
            return *this;
         }

         SocketState m_state;
         Address m_local;
         Int m_backlog;
      };
   }

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief A UDP socket class capabile of sending and receiving data.
   template <class TQueue, class TMessage>
   class UDP : public Base<TQueue,TMessage>
   {
      friend class Thread<TQueue,TMessage>;

   public:
      /// @brief Class constructor.
      /// @param thread the socket thread the talking socket will be associated with.
      /// @param bufsize the size of the send and receive circular buffers.
      UDP(Thread<TQueue,TMessage> &thread, Int bufsize=2097152)
         : Base<TQueue,TMessage>(thread, SocketType::Udp, AF_INET6, SOCK_DGRAM, IPPROTO_UDP),
           m_sending(False),
           m_rbuf(bufsize),
           m_wbuf(bufsize),
           m_rcvmsg(NULL),
           m_sndmsg(NULL)
      {
         m_rcvmsg = reinterpret_cast<UDPMessage*>(new UChar[sizeof(UDPMessage) + UPD_MAX_MSG_LENGTH]);
         m_sndmsg = reinterpret_cast<UDPMessage*>(new UChar[sizeof(UDPMessage) + UPD_MAX_MSG_LENGTH]);
      }
      /// @brief Class constructor.
      /// @param thread the socket thread the talking socket will be associated with.
      /// @param port the local port to listen on.
      /// @param bufsize the size of the send and receive circular buffers.
      UDP(Thread<TQueue,TMessage> &thread, UShort port, Int bufsize=2097152)
         : Base<TQueue,TMessage>(thread, SocketType::Udp, AF_INET6, SOCK_DGRAM, IPPROTO_UDP),
           m_sending(False),
           m_rbuf(bufsize),
           m_wbuf(bufsize),
           m_rcvmsg(NULL),
           m_sndmsg(NULL)
      {
         m_local = port;
         this->setFamily( m_local.getFamily() == Family::INET ? AF_INET : AF_INET6 );
         this->bind();
         m_rcvmsg = reinterpret_cast<UDPMessage*>(new UChar[sizeof(UDPMessage) + UPD_MAX_MSG_LENGTH]);
         m_sndmsg = reinterpret_cast<UDPMessage*>(new UChar[sizeof(UDPMessage) + UPD_MAX_MSG_LENGTH]);
      }
      /// @brief Class constructor.
      /// @param thread the socket thread the talking socket will be associated with.
      /// @param ipaddr the local IP address to listen on. 
      /// @param port the local port to listen on.
      /// @param bufsize the size of the send and receive circular buffers.
      UDP(Thread<TQueue,TMessage> &thread, cpStr ipaddr, UShort port, Int bufsize=2097152)
         : Base<TQueue,TMessage>(thread, SocketType::Udp, AF_INET6, SOCK_DGRAM, IPPROTO_UDP),
           m_sending(False),
           m_rbuf(bufsize),
           m_wbuf(bufsize),
           m_rcvmsg(NULL),
           m_sndmsg(NULL)
      {
         m_local.setAddress( ipaddr, port );
         this->setFamily( m_local.getFamily() == Family::INET ? AF_INET : AF_INET6 );
         this->bind();
         m_rcvmsg = reinterpret_cast<UDPMessage*>(new UChar[sizeof(UDPMessage) + UPD_MAX_MSG_LENGTH]);
         m_sndmsg = reinterpret_cast<UDPMessage*>(new UChar[sizeof(UDPMessage) + UPD_MAX_MSG_LENGTH]);
      }
      /// @brief Class constructor.
      /// @param thread the socket thread the talking socket will be associated with.
      /// @param addr the local socket address to listen on. 
      /// @param bufsize the size of the send and receive circular buffers.
      UDP(Thread <TQueue,TMessage>&thread, Address &addr, Int bufsize=2097152)
         : Base<TQueue,TMessage>(thread, SocketType::Udp, AF_INET6, SOCK_DGRAM, IPPROTO_UDP),
           m_sending(False),
           m_rbuf(bufsize),
           m_wbuf(bufsize),
           m_rcvmsg(NULL),
           m_sndmsg(NULL)
      {
         m_local = addr;
         this->setFamily( m_local.getFamily() == Family::INET ? AF_INET : AF_INET6 );
         this->bind();
         m_rcvmsg = reinterpret_cast<UDPMessage*>(new UChar[sizeof(UDPMessage) + UPD_MAX_MSG_LENGTH]);
         m_sndmsg = reinterpret_cast<UDPMessage*>(new UChar[sizeof(UDPMessage) + UPD_MAX_MSG_LENGTH]);
      }
      /// @brief Class destructor.
      virtual ~UDP()
      {
         if (m_rcvmsg)
            delete [] reinterpret_cast<pUChar>(m_rcvmsg);
         if (m_sndmsg)
            delete [] reinterpret_cast<pUChar>(m_sndmsg);
      }
      /// @brief Retrieves the local address for this socket.
      /// @return the local address for this socket.
      const Address &getLocal()
      {
         return m_local;
      }
      /// @brief Retrieves the IP address for this socket.
      /// @return the IP address for this socket.
      EString getLocalAddress()
      {
         return m_local;
      }
      /// @brief Retrieves the port for this socket.
      /// @return the port for this socket.
      UShort getLocalPort()
      {
         return m_local;
      }
      /// @brief Assigns the socket address for this socket.
      /// @param addr the IP address for this socket.
      /// @param port the port for this socket.
      /// @return a reference to this object.
      UDP &setLocal(cpStr addr, UShort port)
      {
         m_local.setAddress(addr,port);
         return *this;
      }
      /// @brief Assigns the socket address for this socket.
      /// @param addr the socket address for this socket.
      /// @return a reference to this object.
      UDP &setLocal(const Address &addr)
      {
         m_local = addr;
         return *this;
      }
      /// @brief Sends data to the specified recipient address.
      /// @param to the address to send the data to.
      /// @param src a pointer to the beginning of the data buffer to send.
      /// @param len the number of bytes to send.
      Void write(const Address &to, cpUChar src, Int len)
      {
         write(Address(), to, src, len);
      }
      /// @brief Sends data to the specified recipient address.
      /// @param from the address the data is sent from.
      /// @param to the address to send the data to.
      /// @param src a pointer to the beginning of the data buffer to send.
      /// @param len the number of bytes to send.
      Void write(const Address &from, const Address &to, cpUChar src, Int len)
      {
         UDPMessage msg;
         msg.total_length = sizeof(msg) + len;
         msg.data_length = len;
         msg.local = from;
         msg.remote = to;

         {
            EMutexLock l(m_wbuf.getMutex());
            m_wbuf.writeData(reinterpret_cast<cpUChar>(&msg), 0, sizeof(msg), True);
            m_wbuf.writeData(src, 0, len, True);
         }

         send();
      }
      /// @brief Retrieves indication if this socket is in the process of sending data.
      /// @return True indicates that data is being sent, otherwise False.
      Bool getSending()
      {
         return m_sending;
      }
      /// @brief Binds this socket to a local port and IPADDR_ANY.
      /// @param port the port.
      Void bind(UShort port)
      {
         if (this->getHandle() != EPC_INVALID_SOCKET)
            throw UdpError_AlreadyBound();
         m_local = port;
         this->setFamily( m_local.getFamily() == Family::INET ? AF_INET : AF_INET6 );
         bind();
      }
      /// @brief Binds this socket to a local address.
      /// @param ipaddr the IP address.
      /// @param port the port.
      Void bind(cpStr ipaddr, UShort port)
      {
         if (this->getHandle() != EPC_INVALID_SOCKET)
            throw UdpError_AlreadyBound();
         m_local.setAddress( ipaddr, port );
         this->setFamily( m_local.getFamily() == Family::INET ? AF_INET : AF_INET6 );
         bind();
      }
      /// @brief Binds this socket to a local address.
      /// @param addr the local socket address.
      Void bind(const Address &addr)
      {
         if (this->getHandle() != EPC_INVALID_SOCKET)
            throw UdpError_AlreadyBound();
         m_local = addr;
         this->setFamily( m_local.getFamily() == Family::INET ? AF_INET : AF_INET6 );
         bind();
      }
      /// @brief Disconnects the socket.
      Void disconnect()
      {
         Base<TQueue,TMessage>::disconnect();
         m_local.clear();
      }
      /// @brief Called for each message that is received.
      /// @param from the socket address that the data was received from.
      /// @param to the socket address that the message was sent to.
      /// @param msg pointer to the received data.
      /// @param len number of bytes received.
      virtual Void onReceive(const Address &from, const Address &to, cpUChar msg, Int len)
      {
      }
      /// @brief Called when an error is detected on this socket.
      virtual Void onError()
      {
      }

   protected:
      /// @cond DOXYGEN_EXCLUDE
      UDP &setAddresses()
      {
         Base<TQueue,TMessage>::setLocalAddress( m_local );
      }

      Int recv()
      {
         union ControlData
         {
            struct cmsghdr header;
            struct in_pktinfo pktinfo;
            struct in6_pktinfo pktinfo6;
         };

         Int totalReceived = 0;
         Address local;
         Address remote;
         // socklen_t addrlen;
         Int flags = 0;
         UChar cmsgdata[CMSG_SPACE(sizeof(ControlData))];
         struct iovec iov[1];
         struct msghdr mh;

         std::memset(&mh, 0, sizeof(mh));
         mh.msg_control = cmsgdata;
         mh.msg_controllen = sizeof(cmsgdata);
         mh.msg_iov = iov;
         mh.msg_iovlen = 1;
         iov[0].iov_base = m_rcvmsg->data;
         iov[0].iov_len = UPD_MAX_MSG_LENGTH;
         mh.msg_name = (pVoid)&remote.getStorage();
         mh.msg_namelen = sizeof(remote.getStorage());

         while (True)
         {
            // addrlen = addr.getSockAddrLen();
            // Int amtReceived = ::recvfrom(this->getHandle(), m_rcvmsg->data, UPD_MAX_MSG_LENGTH, flags, addr.getSockAddr(), &addrlen);
            Int amtReceived = ::recvmsg(this->getHandle(), &mh, flags);
            if (amtReceived >= 0)
            {
               m_rcvmsg->total_length = sizeof(UDPMessage) + amtReceived;
               m_rcvmsg->data_length = amtReceived;
               m_rcvmsg->local = getLocal();
               m_rcvmsg->remote = remote;

               for (struct cmsghdr *cp = CMSG_FIRSTHDR(&mh); cp != NULL; cp = CMSG_NXTHDR(&mh,cp))
               {
                  if (cp->cmsg_level == IPPROTO_IP && cp->cmsg_type == IP_PKTINFO)
                  {
                     struct in_pktinfo *p = (struct in_pktinfo *)CMSG_DATA(cp);
                     sockaddr_in ipv4;
                     std::memset(&ipv4, 0, sizeof(ipv4));
                     ipv4.sin_family = AF_INET;
                     ipv4.sin_port = ((struct sockaddr_in&)getLocal().getStorage()).sin_port;
                     ipv4.sin_addr.s_addr = p->ipi_spec_dst.s_addr;
                     m_rcvmsg->local = ipv4;
                     break;
                  }
                  if (cp->cmsg_level == IPPROTO_IPV6 && cp->cmsg_type == IPV6_PKTINFO)
                  {
                     struct in6_pktinfo *p = (struct in6_pktinfo *)CMSG_DATA(cp);
                     sockaddr_in6 ipv6;
                     std::memset(&ipv6, 0, sizeof(ipv6));
                     ipv6.sin6_family = AF_INET6;
                     ipv6.sin6_port = ((struct sockaddr_in6&)getLocal().getStorage()).sin6_port;
                     ipv6.sin6_flowinfo = 0;
                     ipv6.sin6_scope_id = 0;
                     ipv6.sin6_addr = p->ipi6_addr;
                     m_rcvmsg->local = ipv6;
                     break;
                  }
               }

               m_rbuf.writeData( reinterpret_cast<pUChar>(m_rcvmsg), 0, m_rcvmsg->total_length);
               totalReceived += amtReceived;
            }
            else
            {
               this->setError();
               if (this->getError() == EWOULDBLOCK)
                  break;
               throw UdpError_UnableToRecvData();
            }
         }

         return totalReceived;
      }

      Void send(Bool override = False)
      {
         EMutexLock lck(m_sendmtx, False);
         if (!lck.acquire(False))
            return;

         if (!override && m_sending)
            return;

         if (m_wbuf.isEmpty())
         {
            m_sending = false;
            return;
         }

         m_sending = true;
         while (true)
         {
            if (m_wbuf.isEmpty())
            {
               m_sending = false;
               break;
            }

            size_t packetLength = 0;
            Int amtRead = m_wbuf.peekData(reinterpret_cast<pUChar>(&packetLength), 0, sizeof(packetLength));
            if ((size_t)amtRead != sizeof(packetLength))
            {
               EString msg;
               msg.format("expected %d bytes, read %d bytes", sizeof(packetLength), amtRead);
               throw UdpError_ReadingWritePacketLength(msg.c_str());
            }

            amtRead = m_wbuf.peekData(reinterpret_cast<pUChar>(m_sndmsg), 0, packetLength);
            if ((size_t)amtRead != packetLength)
            {
               EString msg;
               msg.format("expected %d bytes, read %d bytes", sizeof(packetLength), amtRead);
               throw UdpError_ReadingWritePacketLength(msg.c_str());
            }

            if (send(m_sndmsg->local, m_sndmsg->remote, m_sndmsg->data, m_sndmsg->data_length) == -1)
            {
               // unable to send this message so get out, it will be sent when the socket is ready for writing
               break;
            }

            m_wbuf.readData(NULL, 0, m_sndmsg->total_length);
         }
      }
      /// @endcond

   private:
      #pragma pack(push,1)
      struct UDPMessage
      {
         size_t total_length;
         size_t data_length;
         Address local;
         Address remote;
         UChar data[0];
      };
      #pragma pack(pop)

      Void onConnect()
      {
      }

      Void onClose()
      {
      }

      Void onReceive()
      {
         while (readMessage(*m_rcvmsg))
         {
            onReceive(m_rcvmsg->remote, m_rcvmsg->local, m_rcvmsg->data, m_rcvmsg->data_length);
         }
      }

      Void bind()
      {
         if (this->getHandle() != EPC_INVALID_SOCKET)
            throw UdpError_AlreadyBound();

         Base<TQueue,TMessage>::createSocket(this->getFamily(), this->getType(), this->getProtocol());

         int result;
         int sockopt = 1;
         result = setsockopt(this->getHandle(), IPPROTO_IP, IP_PKTINFO, &sockopt, sizeof(sockopt));
         if (result == -1)
         {
            UdpError_UnableToBindSocket err;
            err.appendTextf(" - %s:%u", getLocal().getAddress().c_str(), getLocal().getPort());
            this->close();
            throw err;
         }
         if (getLocal().getFamily() == Family::INET6)
         {
            result = setsockopt(this->getHandle(), IPPROTO_IPV6, IPV6_RECVPKTINFO, &sockopt, sizeof(sockopt));
            if (result == -1)
            {
               UdpError_UnableToBindSocket err;
               err.appendTextf(" - %s:%u", getLocal().getAddress().c_str(), getLocal().getPort());
               this->close();
               throw err;
            }
         }

         result = ::bind(this->getHandle(), getLocal().getSockAddr(), getLocal().getSockAddrLen());
         if (result == -1)
         {
            UdpError_UnableToBindSocket err;
            err.appendTextf(" - %s:%u", getLocal().getAddress().c_str(), getLocal().getPort());
            this->close();
            throw err;
         }
      }

      Bool readMessage(UDPMessage &msg)
      {
         if (m_rbuf.peekData(reinterpret_cast<pUChar>(&msg), 0, sizeof(msg)))
         {
            m_rbuf.readData(reinterpret_cast<pUChar>(&msg), 0, msg.total_length);
            return True;
         }

         return False;
      }

      Int send(Address &from, Address &to, cpVoid pData, Int length)
      {
         Int flags = MSG_NOSIGNAL;
         Int result = sendto(this->getHandle(), pData, length, flags, to.getSockAddr(), to.getSockAddrLen());

         if (result == -1)
         {
            this->setError();
            if (this->getError() != EMSGSIZE)
               throw UdpError_SendingPacket();
         }

         return result;
      }

      Address m_local;
      EMutexPrivate m_sendmtx;
      Bool m_sending;

      ECircularBuffer m_rbuf;
      ECircularBuffer m_wbuf;
      UDPMessage *m_rcvmsg;
      UDPMessage *m_sndmsg;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief The socket thread base class. An event based thread class capable of surfacing socket events.
   template <class TQueue, class TMessage>
   class Thread : public EThreadEvent<TQueue,TMessage>
   {
      friend class TCP::Talker<TQueue,TMessage>;
      friend class TCP::Listener<TQueue,TMessage>;
      friend class UDP<TQueue,TMessage>;

   public:
      /// @brief Default constructor.
      Thread()
      {
         int *pipefd = this->getBumpPipe();

         m_error = 0;

         int result = pipe(pipefd);
         if (result == -1)
            throw ThreadError_UnableToOpenPipe();
         fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

         FD_ZERO(&m_master);

         getMaxFileDescriptor(True);
      }
      /// @brief Class destructor.
      virtual ~Thread()
      {
         int *pipefd = this->getBumpPipe();
         close(pipefd[0]);
         close(pipefd[1]);
      }
      /// @brief Called by the framework to register a Base derived socket object with this thread.
      /// @param socket the socket to register.
      Void registerSocket(Base<TQueue,TMessage>* socket)
      {
         m_socketmap.insert(std::make_pair(socket->getHandle(), socket));
         FD_SET(socket->getHandle(), &m_master);
         getMaxFileDescriptor(True);
         bump();
      }
      /// @brief Called by the framework to unregister a Base derived socket object with this thread.
      /// @param socket the socket to unregister.
      Void unregisterSocket(Base<TQueue,TMessage>* socket)
      {
         if (m_socketmap.erase(socket->getHandle()))
         {
            FD_CLR(socket->getHandle(), &m_master);
            getMaxFileDescriptor(True);
            bump();
         }
      }
      /// @brief Called when an error is detected.
      Int getError() { return m_error; }

   protected:
      /// @cond DOXYGEN_EXCLUDE
      virtual Void pumpMessages()
      {
         int maxfd, fd, fdcnt;
         fd_set readworking, writeworking, errorworking;

         onInit();

         while (true)
         {
            {
               memcpy(&readworking, &m_master, sizeof(m_master));
               FD_SET(this->getBumpPipe()[0], &readworking);

               FD_ZERO(&writeworking);
               for (auto it = m_socketmap.begin(); it != m_socketmap.end(); it++)
               {
                  Base<TQueue,TMessage> *pSocket = it->second;
                  if ((pSocket->getSocketType() == SocketType::TcpTalker &&
                        ((static_cast<TCP::Talker<TQueue,TMessage>*>(pSocket))->getSending() ||
                        (static_cast<TCP::Talker<TQueue,TMessage>*>(pSocket))->getState() == SocketState::Connecting)) ||
                     (pSocket->getSocketType() == SocketType::Udp && (static_cast<UDP<TQueue,TMessage>*>(pSocket))->getSending()))
                  {
                     FD_SET(it->first, &writeworking);
                  }
               }

               memcpy(&errorworking, &m_master, sizeof(m_master));

               maxfd = getMaxFileDescriptor() + 1;
            }

            fdcnt = select(maxfd, &readworking, &writeworking, &errorworking, NULL);
            if (fdcnt == -1)
            {
               if (errno == EINTR || errno == 514 /*ERESTARTNOHAND*/)
               {
                  if (!pumpMessagesInternal())
                     break;
               }
               else
               {
                  onError();
               }
               continue;
            }

            ////////////////////////////////////////////////////////////////////////
            // Process any thread messages
            ////////////////////////////////////////////////////////////////////////
            if (FD_ISSET(this->getBumpPipe()[0], &readworking))
            {
               --fdcnt;
               if (!pumpMessagesInternal())
                  break;
            }

            ////////////////////////////////////////////////////////////////////////
            // Process any socket messages
            ////////////////////////////////////////////////////////////////////////
            for (fd = 0; fd < maxfd && fdcnt > 0; fd++)
            {
               if (FD_ISSET(fd, &errorworking))
               {
                  auto socket_it = m_socketmap.find(fd);
                  if (socket_it != m_socketmap.end())
                  {
                     Base<TQueue,TMessage> *pSocket = socket_it->second;
                     if (pSocket)
                     {
                        int error;
                        socklen_t optlen = sizeof(error);
                        getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &optlen);
                        pSocket->setError(error);
                        processSelectError(pSocket);
                     }
                  }
                  fdcnt--;
               }

               Bool result = True;

               if (fdcnt > 0 && FD_ISSET(fd, &readworking))
               {
                  auto socket_it = m_socketmap.find(fd);
                  if (socket_it != m_socketmap.end())
                  {
                     Base<TQueue,TMessage> *pSocket = socket_it->second;
                     if (pSocket)
                        result = processSelectRead(pSocket);
                  }
                  fdcnt--;
               }

               if (fdcnt > 0 && FD_ISSET(fd, &writeworking))
               {
                  auto socket_it = m_socketmap.find(fd);
                  if (result && socket_it != m_socketmap.end())
                  {
                     Base<TQueue,TMessage> *pSocket = socket_it->second;
                     if (pSocket)
                        processSelectWrite(pSocket);
                  }
                  fdcnt--;
               }
            }

            ////////////////////////////////////////////////////////////////////////
            // Process any thread messages that may have been posted while
            //   processing the socket events
            ////////////////////////////////////////////////////////////////////////
            if (!pumpMessagesInternal())
               break;

            clearBump();
         }

         while (true)
         {
            auto it = m_socketmap.begin();
            if (it == m_socketmap.end())
               break;
            Base<TQueue,TMessage> *psocket = it->second;
            m_socketmap.erase(it);
            delete psocket;
         }
      }

      virtual Void errorHandler(EError &err, Base<TQueue,TMessage> *psocket) = 0;
      virtual Void onSocketClosed(Base<TQueue,TMessage> *psocket)
      {
      }
      virtual Void onSocketError(Base<TQueue,TMessage> *psocket)
      {
      }
      
      virtual Void onInit()
      {
         EThreadEvent<TQueue,TMessage>::onInit();
      }

      virtual Void onQuit()
      {
         EThreadEvent<TQueue,TMessage>::onQuit();
      }

      virtual Void onMessageQueued(const TMessage &msg)
      {
         EThreadEvent<TQueue,TMessage>::onMessageQueued(msg);
         bump();
      }

      virtual Void onError()
      {
      }

      Void bump()
      {
         if (write(this->getBumpPipe()[1], "~", 1) == -1)
            throw ThreadError_UnableToWritePipe();
      }
      
      Void clearBump()
      {
         char buf[1];
         while (true)
         {
            if (read(this->getBumpPipe()[0], buf, 1) == -1)
            {
               if (errno == EWOULDBLOCK)
                  break;
               throw ThreadError_UnableToReadPipe();
            }
         }
      }

      virtual const typename EThreadEvent<TQueue,TMessage>::msgmap_t *GetMessageMap() const
      {
         return GetThisMessageMap();
      }

      static const typename EThreadEvent<TQueue,TMessage>::msgmap_t *GetThisMessageMap()
      {
         static const typename EThreadEvent<TQueue,TMessage>::msgentry_t _msgEntries[] =
         {
            {0, (typename EThreadEvent<TQueue,TMessage>::msgfxn_t)NULL}
         };
         static const typename EThreadEvent<TQueue,TMessage>::msgmap_t msgMap =
            {&EThreadEvent<TQueue,TMessage>::GetThisMessageMap, &_msgEntries[0]};
         return &msgMap;
      }
      /// @endcond

   private:
      Void setError(Int error) { m_error = error; }

      Bool pumpMessagesInternal()
      {
         TMessage msg;

         try
         {
            while (True)
            {
               if (!EThreadEvent<TQueue,TMessage>::pumpMessage(msg, false) || msg.getMessageId() == EM_QUIT)
                  break;
            }
         }
         catch (...)
         {
            throw;
         }

         ////////////////////////////////////////////////////////////////////
         // get out if the thread has been told to stop
         ////////////////////////////////////////////////////////////////////
         //return (keepGoing() && msg.getMsgId() != EM_QUIT);
         return msg.getMessageId() != EM_QUIT;
      }

      Void processSelectAccept(Base<TQueue,TMessage> *psocket)
      {
         if (psocket->getSocketType() == SocketType::TcpListener)
         {
            bool more = true;
            while (more)
            {
               try
               {
                  struct sockaddr ipaddr;
                  socklen_t ipaddrlen = sizeof(ipaddr);

                  EPC_SOCKET handle = ::accept((static_cast<TCP::Listener<TQueue,TMessage>*>(psocket))->getHandle(), &ipaddr, &ipaddrlen);
                  if (handle == EPC_INVALID_SOCKET)
                  {
                     Int err = errno;
                     if (err == EWOULDBLOCK)
                        break;
                     throw TcpListenerError_UnableToAcceptSocket();
                  }

                  TCP::Talker<TQueue,TMessage> *pnewsocket = (static_cast<TCP::Listener<TQueue,TMessage>*>(psocket))->createSocket(*this);
                  if (pnewsocket)
                  {
                     pnewsocket->setHandle(handle);
                     pnewsocket->setAddresses();
                     pnewsocket->setState( SocketState::Connected );
                     registerSocket(pnewsocket);
                     pnewsocket->onConnect();
                  }
                  else
                  {
                     // the connection is being refused, so close the handle
                     close(handle);
                  }
               }
               catch (EError &err)
               {
                  if (err.getLastOsError() != EWOULDBLOCK)
                  {
                     //printf("errorHandler() 1 %d\n", err->getLastOsError());
                     errorHandler(err, NULL);
                  }
                  more = false;
               }
            }
         }
      }

      Void processSelectConnect(Base<TQueue,TMessage> *psocket)
      {
         if (psocket->getSocketType() == SocketType::TcpTalker)
            ((TCP::Talker<TQueue,TMessage>*)psocket)->onConnect();
      }

      Bool processSelectRead(Base<TQueue,TMessage> *psocket)
      {
         if (psocket->getSocketType() == SocketType::TcpListener)
         {
            processSelectAccept(psocket);
         }
         else if (psocket->getSocketType() == SocketType::TcpTalker)
         {
            switch ((static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->getState())
            {
               case SocketState::Connecting:
               {
                  Int result, socketError;
                  socklen_t socketErrorLen = sizeof(socketError);

                  result = getsockopt(psocket->getHandle(), SOL_SOCKET, SO_ERROR, &socketError, &socketErrorLen);

                  try
                  {
                     if (result == -1 || socketError != 0)
                     {
                        psocket->setError(socketError);
                        TcpTalkerError_UnableToConnect ex;
                        ex.appendTextf(" ESocket::Thread<TQueue,TMessage>::processSelectRead() socketError=%d (%s)",
                           psocket->getError(), psocket->getErrorDescription());
                        throw ex;
                     }

                     (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->setState( SocketState::Connected );
                     (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->setAddresses();
                     (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->onConnect();
                  }
                  catch (EError &ex)
                  {
                     (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->setState( SocketState::Undefined );
                     processSelectError(psocket);
                     return False;
                  }
                  // fall thru
               }
               case SocketState::Connected:
               {
                  while (true)
                  {
                     try
                     {
                        Int amtRead = (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->recv();
                        if (amtRead <= 0)
                           break;
                     }
                     catch (EError &err)
                     {
                        //printf("errorHandler() 2\n");
                        errorHandler(err, psocket);
                     }
                  }

                  ((TCP::Talker<TQueue,TMessage>*)psocket)->onReceive();

                  if ((static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->getState() == SocketState::Disconnected)
                     processSelectClose(psocket);

                  break;
               }
               default:
               {
                  // throw TcpTalkerError_InvalidReadState(
                  //    static_cast<TCP::Talker<TQueue,TMessage>*>(psocket)->getStateDescription());
                  (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->setState( SocketState::Undefined );
                  processSelectError(psocket);
                  return False;
               }
            }
         }
         else if (psocket->getSocketType() == SocketType::Udp)
         {
            while (true)
            {
               try
               {
                  Int amtRead = (static_cast<UDP<TQueue,TMessage>*>(psocket))->recv();
                  if (amtRead <= 0)
                     break;
               }
               catch (EError &err)
               {
                  //printf("errorHandler() 2\n");
                  errorHandler(err, psocket);
               }
            }

            (reinterpret_cast<UDP<TQueue,TMessage>*>(psocket))->onReceive();
         }

         return True;
      }

      Void processSelectWrite(Base<TQueue,TMessage> *psocket)
      {
         if (psocket->getSocketType() == SocketType::TcpTalker)
         {
            switch ((static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->getState())
            {
               case SocketState::Connecting:
               {
                  Int result, socketError;
                  socklen_t socketErrorLen = sizeof(socketError);

                  result = getsockopt(psocket->getHandle(), SOL_SOCKET, SO_ERROR, &socketError, &socketErrorLen);

                  try
                  {
                     if (result == -1 || socketError != 0)
                     {
                        psocket->setError(socketError);
                        TcpTalkerError_UnableToConnect ex;
                        ex.appendTextf(" ESocket::Thread<TQueue,TMessage>::processSelectWrite() socketError=%d (%s)",
                           psocket->getError(), psocket->getErrorDescription());
                        throw ex;
                     }

                     (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->setState( SocketState::Connected );
                     (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->setAddresses();
                     (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->onConnect();
                  }
                  catch (EError &ex)
                  {
                     (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->setState( SocketState::Undefined );
                     processSelectError(psocket);
                     return;
                  }
                  break;
               }
               case SocketState::Connected:
               {
                  try
                  {
                     (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->send(True);
                  }
                  catch (EError &err)
                  {
                     // errorHandler(err, psocket);
                     processSelectError(psocket);
                  }
                  break;
               }
               default:
               {
                  // throw TcpTalkerError_InvalidWriteState(
                  //    (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->getStateDescription());
                  (static_cast<TCP::Talker<TQueue,TMessage>*>(psocket))->setState( SocketState::Undefined );
                  processSelectError(psocket);
                  return;
               }
            }
         }
         else if (psocket->getSocketType() == SocketType::Udp)
         {
            try
            {
               (static_cast<UDP<TQueue,TMessage>*>(psocket))->send(True);
            }
            catch (EError &err)
            {
               //printf("errorHandler() 3\n");
               errorHandler(err, psocket);
            }
         }
      }

      Void processSelectError(Base<TQueue,TMessage> *psocket)
      {
         psocket->onError();
         onSocketError(psocket);
      }

      Void processSelectClose(Base<TQueue,TMessage> *psocket)
      {
         psocket->onClose();
         onSocketClosed(psocket);
      }

      int getMaxFileDescriptor(Bool calc=False)
      {
         if (calc)
         {
            m_maxfd = this->getBumpPipe()[0];
            
            for (auto entry : m_socketmap)
               if (entry.second->getHandle() > m_maxfd)
                  m_maxfd = entry.second->getHandle();
         }
         
         return m_maxfd;
      }

      Int m_error;
      std::unordered_map<Int,Base<TQueue,TMessage>*> m_socketmap;
      fd_set m_master;
      Int m_maxfd;
   };

   typedef Base<EThreadQueuePublic<EThreadMessage>,EThreadMessage> BasePublic;
   typedef Base<EThreadQueuePrivate<EThreadMessage>,EThreadMessage> BasePrivate;
   typedef Thread<EThreadQueuePublic<EThreadMessage>,EThreadMessage> ThreadPublic;
   typedef Thread<EThreadQueuePrivate<EThreadMessage>,EThreadMessage> ThreadPrivate;
   namespace TCP
   {
      typedef Talker<EThreadQueuePublic<EThreadMessage>,EThreadMessage> TalkerPublic;
      typedef Talker<EThreadQueuePrivate<EThreadMessage>,EThreadMessage> TalkerPrivate;
      typedef Listener<EThreadQueuePublic<EThreadMessage>,EThreadMessage> ListenerPublic;
      typedef Listener<EThreadQueuePrivate<EThreadMessage>,EThreadMessage> ListenerPrivate;
   }
   typedef UDP<EThreadQueuePublic<EThreadMessage>,EThreadMessage> UdpPublic;
   typedef UDP<EThreadQueuePrivate<EThreadMessage>,EThreadMessage> UdpPrivate;
}

namespace std
{
   template<>
   struct hash<ESocket::Address>
   {
      std::size_t operator()(const ESocket::Address &addr) const noexcept
      {
         size_t addrhash = EMurmurHash64::getHash(reinterpret_cast<cpUChar>(addr.getSockAddr()), addr.getSockAddrLen());
         size_t porthash = std::hash<UShort>{}(addr.getPort());
         return EMurmurHash64::combine(addrhash, porthash);
      }
   };
}

#endif // #define __esocket_h_included
