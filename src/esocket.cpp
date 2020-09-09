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

#include <sys/fcntl.h>
#include <unistd.h>
#include <csignal>

#include "esocket.h"

// #define epc_closesocket(a) ::close(a)
// #define EPC_LASTERROR errno
// #define EPC_INVALID_SOCKET -1
// #define EPC_EINPROGRESS EINPROGRESS
// #define EPC_EWOULDBLOCK EWOULDBLOCK
// #define EPC_EMSGSIZE EMSGSIZE
// #define EPC_SOCKET_ERROR -1
// typedef Int EPC_SOCKET;
// typedef void *PSOCKETOPT;
// typedef void *PSNDRCVBUFFER;
// typedef socklen_t EPC_SOCKLEN;

////////////////////////////////////////////////////////////////////////////////
// ESocketError
////////////////////////////////////////////////////////////////////////////////

namespace ESocket
{

/// @cond DOXYGEN_EXCLUDE

AddressError_UnknownAddressType::AddressError_UnknownAddressType()
{
   setSevere();
   setTextf("%s: Unknown address type", Name());
}

AddressError_CannotConvertInet2Inet6::AddressError_CannotConvertInet2Inet6()
{
   setSevere();
   setTextf("%s: Cannot convert an INET address to an INET6 address", Name());
}

AddressError_CannotConvertInet62Inet::AddressError_CannotConvertInet62Inet()
{
   setSevere();
   setTextf("%s: Cannot convert an INEt6 address to an INET address", Name());
}

AddressError_ConvertingToString::AddressError_ConvertingToString()
{
   setSevere();
   setTextf("%s: Error converting address to a string", Name());
}

AddressError_UndefinedFamily::AddressError_UndefinedFamily()
{
   setSevere();
   setTextf("%s: Undefined family", Name());
}

BaseError_UnableToCreateSocket::BaseError_UnableToCreateSocket()
{
   setSevere();
   setTextf("%s: Error creating socket - ", Name());
   appendLastOsError();
}

BaseError_GetPeerNameError::BaseError_GetPeerNameError()
{
   setSevere();
   setTextf("%s: Error executing getpeername() - ", Name());
   appendLastOsError();
}

TcpListenerError_UnableToBindSocket::TcpListenerError_UnableToBindSocket()
{
   setSevere();
   setTextf("%s: Error binding socket in bind() - ", Name());
   appendLastOsError();
}

TcpListenerError_UnableToAcceptSocket::TcpListenerError_UnableToAcceptSocket()
{
   setSevere();
   setTextf("%s: Error accepting new connection in accept() - ", Name());
   appendLastOsError();
}

TcpListenerError_UnableToListen::TcpListenerError_UnableToListen()
{
   setSevere();
   setTextf("%s: Error executing listen - ", Name());
   appendLastOsError();
}

TcpTalkerError_InvalidRemoteAddress::TcpTalkerError_InvalidRemoteAddress()
{
   setSevere();
   setTextf("%s: Invalid remote address", Name());
}

TcpTalkerError_UnableToConnect::TcpTalkerError_UnableToConnect()
{
   setSevere();
   setTextf("%s: Unable to connect - ", Name());
   appendLastOsError();
}

TcpTalkerError_UnableToRecvData::TcpTalkerError_UnableToRecvData()
{
   setSevere();
   setTextf("%s: Error while executing recv() - ", Name());
   appendLastOsError();
}

TcpTalkerError_InvalidSendState::TcpTalkerError_InvalidSendState(cpStr msg)
{
   setSevere();
   setTextf("%s: Invalid state while sending: %s", Name(), msg);
}

TcpTalkerError_InvalidReadState::TcpTalkerError_InvalidReadState(cpStr msg)
{
   setSevere();
   setTextf("%s: Invalid state while reading: %s", Name(), msg);
}

TcpTalkerError_InvalidWriteState::TcpTalkerError_InvalidWriteState(cpStr msg)
{
   setSevere();
   setTextf("%s: Invalid state while writing: %s", Name(), msg);
}

TcpTalkerError_ReadingWritePacketLength::TcpTalkerError_ReadingWritePacketLength(cpStr msg)
{
   setSevere();
   setTextf("%s: Unable to read the write packet length - %s", Name(), msg);
}

TcpTalkerError_SendingPacket::TcpTalkerError_SendingPacket()
{
   setSevere();
   setTextf("%s: Error while attempting to send a packet of data via send() - ", Name());
   appendLastOsError();
}

UdpError_AlreadyBound::UdpError_AlreadyBound()
{
   setSevere();
   setTextf("%s: Socket is already bound", Name());
}

UdpError_UnableToBindSocket::UdpError_UnableToBindSocket()
{
   setSevere();
   setTextf("%s: Error while attempting to bind socket - ", Name());
   appendLastOsError();
}

UdpError_UnableToRecvData::UdpError_UnableToRecvData()
{
   setSevere();
   setTextf("%s: Error receiving data - ", Name());
   appendLastOsError();
}

UdpError_SendingPacket::UdpError_SendingPacket()
{
   setSevere();
   setTextf("%s: Error while attempting to send a packet of data via sendto() - ", Name());
   appendLastOsError();
}

UdpError_ReadingWritePacketLength::UdpError_ReadingWritePacketLength(cpStr msg)
{
   setSevere();
   setTextf("%s: Unable to read the write packet length - %s", Name(), msg);
}

ThreadError_UnableToOpenPipe::ThreadError_UnableToOpenPipe()
{
   setSevere();
   setTextf("%s: Error while opening select pipe - ", Name());
   appendLastOsError();
}

ThreadError_UnableToReadPipe::ThreadError_UnableToReadPipe()
{
   setSevere();
   setTextf("%s: Error while reading select pipe - ", Name());
   appendLastOsError();
}

ThreadError_UnableToWritePipe::ThreadError_UnableToWritePipe()
{
   setSevere();
   setTextf("%s: Error while writing select pipe - ", Name());
   appendLastOsError();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @endcond

}