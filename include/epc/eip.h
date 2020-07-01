/*
* Copyright (c) 2020 Sprint
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

#ifndef __eip_h_included
#define __eip_h_included

#include <arpa/inet.h>
#include <netinet/in.h>
#include <list>
#include <unordered_map>
#include <vector>

#include "epctools.h"
#include "ehash.h"

DECLARE_ERROR_ADVANCED2(EIpAddress_UnrecognizedAddressFamily);
inline EIpAddress_UnrecognizedAddressFamily::EIpAddress_UnrecognizedAddressFamily(Int af) {
   setTextf("Unrecognized address family %d", af);
}

class EIpAddress
{
public:
   EIpAddress()
       : family_(AF_UNSPEC)
   {
      memset(&address_, 0, sizeof(address_));
   }
   EIpAddress(cpStr addr)
   {
      *this = addr;
   }
   EIpAddress(const std::string &addr)
   {
      *this = addr.c_str();
   }
   EIpAddress(const EIpAddress &ipaddr)
   {
      *this = ipaddr;
   }
   EIpAddress(const struct sockaddr_storage &saddr)
   {
      *this = saddr;
   }

   EIpAddress &operator=(cpStr addr)
   {
      auto parts = EUtility::split(addr, "/");
      memset(&address_, 0, sizeof(address_));
      if (inet_pton(AF_INET, parts[0], &address_.ipv4) == 1)
      {
         if (family_ != AF_INET)
            family_ = AF_INET;
         maskWidth_ = (parts.size() > 1) ? static_cast<UChar>(std::stoi(parts[1])) : 32;
      }
      else if (inet_pton(AF_INET6, parts[0], &address_.ipv6) == 1)
      {
         if (family_ != AF_INET6)
            family_ = AF_INET6;
         maskWidth_ = (parts.size() > 1) ? static_cast<UChar>(std::stoi(parts[1])) : 128;
      }
      else
      {
         family_ = AF_UNSPEC;
         maskWidth_ = 0;
      }

      return *this;
   }

   EIpAddress &operator=(const EIpAddress &ipaddr)
   {
      family_ = ipaddr.family_;
      maskWidth_ = ipaddr.maskWidth_;
      memset(&address_, 0, sizeof(address_));
      switch (family_)
      {
         case AF_INET:  { address_.ipv4.s_addr = ipaddr.address_.ipv4.s_addr; break; }
         case AF_INET6: { memcpy(address_.ipv6.s6_addr, ipaddr.address_.ipv6.s6_addr, sizeof(address_.ipv6.s6_addr)); break; }
         default:       { break; }
      }
      return *this;
   }

   EIpAddress &operator=(const struct sockaddr_storage &saddr)
   {
      switch (saddr.ss_family)
      {
         case AF_INET:  { return EIpAddress::operator=(((const struct sockaddr_in&)saddr).sin_addr); }
         case AF_INET6: { return EIpAddress::operator=(((const struct sockaddr_in6&)saddr).sin6_addr); }
         default:
         {
            throw EIpAddress_UnrecognizedAddressFamily(saddr.ss_family);
         }
      }
      return *this;
   }

   EIpAddress &operator=(const in_addr &addr)
   {
      if (family_ != AF_INET)
      {
         family_ = AF_INET;
         maskWidth_ = 32;
      }
      address_.ipv4.s_addr = addr.s_addr;
      return *this;
   }

   EIpAddress &operator=(const in6_addr &addr)
   {
      if (family_ != AF_INET6)
      {
         family_ = AF_INET6;
         maskWidth_ = 128;
      }
      memcpy(address_.ipv6.s6_addr, addr.s6_addr, sizeof(address_.ipv6.s6_addr));
      return *this;
   }

   Bool operator==(const EIpAddress &ip) const
   {
      return
         family_ == ip.family_ &&
         maskWidth_ == ip.maskWidth_ &&
         (
            (family_ == AF_INET && address_.ipv4.s_addr == ip.address_.ipv4.s_addr) ||
            (
               family_ == AF_INET6 && 
               address_.ipv6.s6_addr[0] == ip.address_.ipv6.s6_addr[0] &&
               address_.ipv6.s6_addr[1] == ip.address_.ipv6.s6_addr[1] &&
               address_.ipv6.s6_addr[2] == ip.address_.ipv6.s6_addr[2] &&
               address_.ipv6.s6_addr[3] == ip.address_.ipv6.s6_addr[3] &&
               address_.ipv6.s6_addr[4] == ip.address_.ipv6.s6_addr[4] &&
               address_.ipv6.s6_addr[5] == ip.address_.ipv6.s6_addr[5] &&
               address_.ipv6.s6_addr[6] == ip.address_.ipv6.s6_addr[6] &&
               address_.ipv6.s6_addr[7] == ip.address_.ipv6.s6_addr[7] &&
               address_.ipv6.s6_addr[8] == ip.address_.ipv6.s6_addr[8] &&
               address_.ipv6.s6_addr[9] == ip.address_.ipv6.s6_addr[9] &&
               address_.ipv6.s6_addr[10] == ip.address_.ipv6.s6_addr[10] &&
               address_.ipv6.s6_addr[11] == ip.address_.ipv6.s6_addr[11] &&
               address_.ipv6.s6_addr[12] == ip.address_.ipv6.s6_addr[12] &&
               address_.ipv6.s6_addr[13] == ip.address_.ipv6.s6_addr[13] &&
               address_.ipv6.s6_addr[14] == ip.address_.ipv6.s6_addr[14] &&
               address_.ipv6.s6_addr[15] == ip.address_.ipv6.s6_addr[15]
             )
         )
      ;
   }

   std::string address() const
   {
      std::string str;
      
      if (isAny())
      {
         str = "any";
         return str;
      }

      Char buf[INET6_ADDRSTRLEN] = {};

      switch (family_)
      {
         case AF_INET:  { inet_ntop(AF_INET,&address_.ipv4,buf,sizeof(buf)); break; }
         case AF_INET6: { inet_ntop(AF_INET6,&address_.ipv6,buf,sizeof(buf)); break; }
         default:       { break; }
      }

      std::stringstream ss;
      ss << buf << "/" << static_cast<Int>(maskWidth());
      str = ss.str();

      return str;
   }

   const in_addr &ipv4Address() const { return address_.ipv4; }
   const in6_addr &ipv6Address() const { return address_.ipv6; }
   const sa_family_t family() const { return family_; }
   UChar maskWidth() const { return maskWidth_; }

   EIpAddress &setFamily(const sa_family_t family)
   {
      family_ = family;
      return *this;
   }
   EIpAddress &setMaskWidth(UChar maskWidth)
   {
      switch (family_)
      {
         case AF_INET:  { maskWidth_ = (maskWidth > 32) ? 32 : maskWidth; break; }
         case AF_INET6: { maskWidth_ = (maskWidth > 128) ? 128 : maskWidth; break; }
         default:       { break; }
      }
      return *this;
   }

   EIpAddress &setAny(sa_family_t family)
   {
      setFamily(family);
      memset(&address_,0,sizeof(address_));
      maskWidth_ = 0;
      return *this;
   }

   Bool isAny() const
   {
      if (maskWidth_ != 0)
         return False;
      switch (family_)
      {
         case AF_INET:  { if (address_.ipv4.s_addr == 0) return True; break; }
         case AF_INET6: { for (auto b : address_.ipv6.s6_addr) { if (b != 0) return False; } return True; }
         default:       { break; }
      }
      return False;
   }

private:
   sa_family_t family_;
   union {
      in_addr ipv4;
      in6_addr ipv6;
   } address_;
   UChar maskWidth_;
};

namespace std
{
template <>
struct hash<EIpAddress>
{
   std::size_t operator()(const EIpAddress &addr) const noexcept
   {
      size_t iphash = 0xc70f6907UL;
      switch (addr.family())
      {
         case AF_INET:  { iphash = EMurmurHash64::getHash(reinterpret_cast<cpUChar>(&addr.ipv4Address()), sizeof(in_addr)); break; }
         case AF_INET6: { iphash = EMurmurHash64::getHash(reinterpret_cast<cpUChar>(&addr.ipv6Address()), sizeof(in6_addr)); break; }
         default:       { break; }
      }
      size_t mwhash = std::hash<UChar>{}(addr.maskWidth());
      return EMurmurHash64::combine(iphash, mwhash);
   }
};
} // namespace std

DECLARE_ERROR_ADVANCED4(EIpFilterRule_InvalidAction);
DECLARE_ERROR_ADVANCED4(EIpFilterRule_InvalidDirection);
DECLARE_ERROR_ADVANCED4(EIpFilterRule_InvalidProtocol);
DECLARE_ERROR_ADVANCED4(EIpFilterRule_InvalidIpAddress);
DECLARE_ERROR_ADVANCED4(EIpFilterRule_InvalidMaskWidth);
DECLARE_ERROR_ADVANCED4(EIpFilterRule_InvalidPort);
DECLARE_ERROR_ADVANCED4(EIpFilterRule_InvalidOption);
DECLARE_ERROR_ADVANCED4(EIpFilterRule_InvalidOptionMissingSpec);

class EIpFilterRule
{
public:
   enum class Action
   {
      Undefined,
      Permit,
      Deny
   };
   enum class Direction
   {
      Undefined,
      In,
      Out
   };
   enum class OptionType
   {
      Undefined,
      frag,
      ipoptions,
      tcpoptions,
      established,
      setup,
      tcpflags,
      icmptypes
   };
   enum class Protocol
   {
      Undefined = -1,
      ProtoMin = 0,
      IP = 0,         /* dummy for IP */
      HOPOPTS = 0,    /* IP6 hop-by-hop options */
      ICMP = 1,       /* control message protocol */
      IGMP = 2,       /* group mgmt protocol */
      GGP = 3,        /* gateway^2 (deprecated) */
      IPV4 = 4,       /* IPv4 encapsulation */
      IPIP = 4,       /* for compatibility */
      TCP = 6,        /* tcp */
      ST = 7,         /* Stream protocol II */
      EGP = 8,        /* exterior gateway protocol */
      PIGP = 9,       /* private interior gateway */
      RCCMON = 10,    /* BBN RCC Monitoring */
      NVPII = 11,     /* network voice protocol*/
      PUP = 12,       /* pup */
      ARGUS = 13,     /* Argus */
      EMCON = 14,     /* EMCON */
      XNET = 15,      /* Cross Net Debugger */
      CHAOS = 16,     /* Chaos*/
      UDP = 17,       /* user datagram protocol */
      MUX = 18,       /* Multiplexing */
      MEAS = 19,      /* DCN Measurement Subsystems */
      HMP = 20,       /* Host Monitoring */
      PRM = 21,       /* Packet Radio Measurement */
      IDP = 22,       /* xns idp */
      TRUNK1 = 23,    /* Trunk-1 */
      TRUNK2 = 24,    /* Trunk-2 */
      LEAF1 = 25,     /* Leaf-1 */
      LEAF2 = 26,     /* Leaf-2 */
      RDP = 27,       /* Reliable Data */
      IRTP = 28,      /* Reliable Transaction */
      TP = 29,        /* tp-4 w/ class negotiation */
      BLT = 30,       /* Bulk Data Transfer */
      NSP = 31,       /* Network Services */
      INP = 32,       /* Merit Internodal */
      SEP = 33,       /* Sequential Exchange */
      TPC = 34,       /* Third Party Connect */
      IDPR = 35,      /* InterDomain Policy Routing */
      XTP = 36,       /* XTP */
      DDP = 37,       /* Datagram Delivery */
      CMTP = 38,      /* Control Message Transport */
      TPXX = 39,      /* TP++ Transport */
      IL = 40,        /* IL transport protocol */
      IPV6 = 41,      /* IP6 header */
      SDRP = 42,      /* Source Demand Routing */
      ROUTING = 43,   /* IP6 routing header */
      FRAGMENT = 44,  /* IP6 fragmentation header */
      IDRP = 45,      /* InterDomain Routing*/
      RSVP = 46,      /* resource reservation */
      GRE = 47,       /* General Routing Encap. */
      MHRP = 48,      /* Mobile Host Routing */
      BHA = 49,       /* BHA */
      ESP = 50,       /* IP6 Encap Sec. Payload */
      AH = 51,        /* IP6 Auth Header */
      INLSP = 52,     /* Integ. Net Layer Security */
      SWIPE = 53,     /* IP with encryption */
      NHRP = 54,      /* Next Hop Resolution */
      ICMPV6 = 58,    /* ICMP6 */
      NONE = 59,      /* IP6 no next header */
      DSTOPTS = 60,   /* IP6 destination option */
      AHIP = 61,      /* any host internal protocol */
      CFTP = 62,      /* CFTP */
      HELLO = 63,     /* "hello" routing protocol */
      SATEXPAK = 64,  /* SATNET/Backroom EXPAK */
      KRYPTOLAN = 65, /* Kryptolan */
      RVD = 66,       /* Remote Virtual Disk */
      IPPC = 67,      /* Pluribus Packet Core */
      ADFS = 68,      /* Any distributed FS */
      SATMON = 69,    /* Satnet Monitoring */
      VISA = 70,      /* VISA Protocol */
      IPCV = 71,      /* Packet Core Utility */
      CPNX = 72,      /* Comp. Prot. Net. Executive */
      CPHB = 73,      /* Comp. Prot. HeartBeat */
      WSN = 74,       /* Wang Span Network */
      PVP = 75,       /* Packet Video Protocol */
      BRSATMON = 76,  /* BackRoom SATNET Monitoring */
      ND = 77,        /* Sun net disk proto (temp.) */
      WBMON = 78,     /* WIDEBAND Monitoring */
      WBEXPAK = 79,   /* WIDEBAND EXPAK */
      EON = 80,       /* ISO cnlp */
      VMTP = 81,      /* VMTP */
      SVMTP = 82,     /* Secure VMTP */
      VINES = 83,     /* Banyon VINES */
      TTP = 84,       /* TTP */
      IGP = 85,       /* NSFNET-IGP */
      DGP = 86,       /* dissimilar gateway prot. */
      TCF = 87,       /* TCF */
      IGRP = 88,      /* Cisco/GXS IGRP */
      OSPFIGP = 89,   /* OSPFIGP */
      SRPC = 90,      /* Strite RPC protocol */
      LARP = 91,      /* Locus Address Resoloution */
      MTP = 92,       /* Multicast Transport */
      AX25 = 93,      /* AX.25 Frames */
      IPEIP = 94,     /* IP encapsulated in IP */
      MICP = 95,      /* Mobile Int.ing control */
      SCCSP = 96,     /* Semaphore Comm. security */
      ETHERIP = 97,   /* Ethernet IP encapsulation */
      ENCAP = 98,     /* encapsulation header */
      APES = 99,      /* any private encr. scheme */
      GMTP = 100,     /* GMTP*/
      PIM = 103,      /* Protocol Independent Mcast */
      IPCOMP = 108,   /* payload compression (IPComp) */
      PGM = 113,      /* PGM */
      SCTP = 132,     /* SCTP */
      DIVERT = 254,   /* divert pseudo-protocol */
      RAW = 255,      /* raw IP packet */
      ProtoMax = 255
   };

   class Port
   {
   public:
      Port() : first_(0), last_(0) {}
      Port(UShort first = 0, UShort last = 0) : first_(first), last_(!last ? first : last) {}
      Port(const Port &p) : first_(p.first_), last_(p.last_) {}

      UShort first() const { return first_; }
      UShort last() const { return last_; }

      Port &setFirst(UShort first)
      {
         first_ = first;
         if (first_ > last_)
            last_ = first_;
         return *this;
      }
      Port &setLast(UShort last)
      {
         last_ = last;
         if (first_ > last_)
            first_ = last_;
         return *this;
      }
      Port &setFirstLast(UShort first, UShort last)
      {
         setLast(last).setFirst(first);
         return *this;
      }

   private:
      UShort first_;
      UShort last_;
   };
   typedef std::vector<Port> PortVec;

   class Option
   {
   public:
      Option() : type_(OptionType::Undefined) {}
      Option(OptionType t) : type_(t) {}
      Option(const Option &o) : type_(o.type_) {}
      virtual ~Option() {}
      OptionType type() const { return type_; }
      Option &setType(OptionType t)
      {
         type_ = t;
         return *this;
      }
      virtual size_t hash() const = 0;

   private:
      OptionType type_;
   };
   typedef std::shared_ptr<Option> OptionPtr;
   typedef std::vector<OptionPtr> OptionPtrVec;

   class Fragment : public Option
   {
   public:
      Fragment() : Option(OptionType::frag) {}
      virtual ~Fragment() {}

      size_t hash() const
      {
         return std::hash<Int>{}(static_cast<Int>(type()));
      }
   };

   class IpOptions : public Option
   {
   public:
      IpOptions() : Option(OptionType::ipoptions) {}
      IpOptions(cpChar spec) : Option(OptionType::ipoptions), spec_(spec) {}
      IpOptions(const IpOptions &o) : Option(OptionType::ipoptions), spec_(o.spec_) {}
      virtual ~IpOptions() {}
      const EString &spec() { return spec_; }
      IpOptions &setSpec(cpChar spec)
      {
         spec_ = spec;
         return *this;
      }
      IpOptions &setSpec(const EString &spec)
      {
         spec_ = spec;
         return *this;
      }
      IpOptions &setSpec(const std::string &spec)
      {
         spec_ = spec;
         return *this;
      }

      size_t hash() const
      {
         size_t typehash = std::hash<Int>{}(static_cast<Int>(type()));
         size_t spechash = std::hash<std::string>{}(spec_);
         return EMurmurHash64::combine(typehash, spechash);
      }

   private:
      EString spec_;
   };

   class TcpOptions : public Option
   {
   public:
      TcpOptions() : Option(OptionType::tcpoptions) {}
      TcpOptions(cpChar spec) : Option(OptionType::tcpoptions), spec_(spec) {}
      TcpOptions(const TcpOptions &o) : Option(OptionType::tcpoptions), spec_(o.spec_) {}
      virtual ~TcpOptions() {}
      const EString &spec() { return spec_; }
      TcpOptions &setSpec(cpChar spec)
      {
         spec_ = spec;
         return *this;
      }
      TcpOptions &setSpec(const EString &spec)
      {
         spec_ = spec;
         return *this;
      }
      TcpOptions &setSpec(const std::string &spec)
      {
         spec_ = spec;
         return *this;
      }

      size_t hash() const
      {
         size_t typehash = std::hash<Int>{}(static_cast<Int>(type()));
         size_t spechash = std::hash<std::string>{}(spec_);
         return EMurmurHash64::combine(typehash, spechash);
      }

   private:
      EString spec_;
   };

   class Established : public Option
   {
   public:
      Established() : Option(OptionType::established) {}
      virtual ~Established() {}
      size_t hash() const
      {
         return std::hash<Int>{}(static_cast<Int>(type()));
      }
   };
   class Setup : public Option
   {
   public:
      Setup() : Option(OptionType::setup) {}
      virtual ~Setup() {}
      size_t hash() const
      {
         return std::hash<Int>{}(static_cast<Int>(type()));
      }
   };

   class TcpFlags : public Option
   {
   public:
      TcpFlags() : Option(OptionType::tcpflags) {}
      TcpFlags(cpChar flags) : Option(OptionType::tcpflags), flags_(flags) {}
      TcpFlags(const TcpFlags &o) : Option(OptionType::tcpflags), flags_(o.flags_) {}
      virtual ~TcpFlags() {}
      const EString &flags() { return flags_; }
      TcpFlags &setFlags(cpChar flags)
      {
         flags_ = flags;
         return *this;
      }
      TcpFlags &setFlags(const EString &flags)
      {
         flags_ = flags;
         return *this;
      }
      TcpFlags &setFlags(const std::string &flags)
      {
         flags_ = flags;
         return *this;
      }
      size_t hash() const
      {
         size_t typehash = std::hash<Int>{}(static_cast<Int>(type()));
         size_t flagshash = std::hash<std::string>{}(flags_);
         return EMurmurHash64::combine(typehash, flagshash);
      }

   private:
      EString flags_;
   };

   class IcmpTypes : public Option
   {
   public:
      IcmpTypes() : Option(OptionType::icmptypes) {}
      IcmpTypes(cpChar types) : Option(OptionType::icmptypes), types_(types) {}
      IcmpTypes(const IcmpTypes &o) : Option(OptionType::icmptypes), types_(o.types_) {}
      virtual ~IcmpTypes() {}
      const EString &types() { return types_; }
      IcmpTypes &setTypes(cpChar types)
      {
         types_ = types;
         return *this;
      }
      IcmpTypes &setTypes(const EString &types)
      {
         types_ = types;
         return *this;
      }
      IcmpTypes &setTypes(const std::string &types)
      {
         types_ = types;
         return *this;
      }
      size_t hash() const
      {
         size_t typehash = std::hash<Int>{}(static_cast<Int>(type()));
         size_t typeshash = std::hash<std::string>{}(types_);
         return EMurmurHash64::combine(typehash, typeshash);
      }

   private:
      EString types_;
   };

   EIpFilterRule() : action_(Action::Undefined), dir_(Direction::Undefined), proto_(Protocol::Undefined) {}
   EIpFilterRule(cpChar raw) : action_(Action::Undefined), dir_(Direction::Undefined), proto_(Protocol::Undefined) { parse(raw); }
   EIpFilterRule(const EIpFilterRule &r)
       : orig_(r.original()),
         action_(r.action()),
         dir_(r.direction()),
         proto_(r.protocol()),
         src_(r.source()),
         srcPorts_(r.sourcePorts()),
         dst_(r.destination()),
         dstPorts_(r.destinationPorts()),
         options_(options())
   {
   }

   EIpFilterRule &operator=(const EIpFilterRule &r)
   {
      orig_     = r.orig_;
      action_   = r.action_;
      dir_      = r.dir_;
      src_      = r.src_;
      srcPorts_ = r.srcPorts_;
      dst_      = r.dst_;
      dstPorts_ = r.dstPorts_;
      options_  = r.options_;
      return *this;
   }

   EIpFilterRule &operator=(const std::string &r)  { return parse(r); }
   EIpFilterRule &operator=(cpStr r)               { return parse(r); }

   const EString &original()         const { return orig_;     }
   const Action action()             const { return action_;   }
   const Direction direction()       const { return dir_;      }
   const Protocol protocol()         const { return proto_;    }
   const EIpAddress &source()        const { return src_;      }
   const PortVec &sourcePorts()      const { return srcPorts_; }
   const EIpAddress &destination()   const { return dst_;      }
   const PortVec &destinationPorts() const { return dstPorts_; }
   const OptionPtrVec &options()     const { return options_;  }

   EIpFilterRule &setOriginal(cpChar orig)
   {
      orig_ = orig;
      return *this;
   }
   EIpFilterRule &setOriginal(const EString &orig)
   {
      orig_ = orig;
      return *this;
   }
   EIpFilterRule &setOriginal(const std::string &orig)
   {
      orig_ = orig;
      return *this;
   }
   EIpFilterRule &setAction(Action action)
   {
      action_ = action;
      return *this;
   }
   EIpFilterRule &setDirection(Direction dir)
   {
      dir_ = dir;
      return *this;
   }
   EIpFilterRule &setProtocol(Protocol proto)
   {
      proto_ = proto;
      return *this;
   }
   EIpFilterRule &setSource(const EIpAddress &src)
   {
      src_ = src;
      return *this;
   }
   EIpFilterRule &setDestination(const EIpAddress &dst)
   {
      dst_ = dst;
      return *this;
   }

   EIpFilterRule &addSourcePort(UShort first = 0, UShort last = 0)
   {
      srcPorts_.push_back(Port(first, last));
      return *this;
   }
   EIpFilterRule &addSourcePort(const Port &p)
   {
      srcPorts_.push_back(p);
      return *this;
   }
   EIpFilterRule &addDestinationPort(UShort first = 0, UShort last = 0)
   {
      dstPorts_.push_back(Port(first, last));
      return *this;
   }
   EIpFilterRule &addDestinationPort(const Port &p)
   {
      dstPorts_.push_back(p);
      return *this;
   }

   EIpFilterRule &parse(cpChar raw) { return parse(std::string(raw)); }
   EIpFilterRule &parse(const std::string &raw);

   Void dump(cpStr padding="");
   EString final();

private:
   Action parseAction(const EString &str);
   Direction parseDirection(const EString &str);
   Protocol parseProtocol(const EString &str);
   EIpAddress parseIpAddress(const EString &str);
   Void parsePorts(const EString &str, PortVec &ports);
   Void parseOptions(const EString &str);

   EString      orig_;
   Action       action_;
   Direction    dir_;
   Protocol     proto_;
   EIpAddress   src_;
   PortVec      srcPorts_;
   EIpAddress   dst_;
   PortVec      dstPorts_;
   OptionPtrVec options_;
};

namespace std
{
template <>
struct hash<EIpFilterRule::Port>
{
   std::size_t operator()(const EIpFilterRule::Port &port) const noexcept
   {
      size_t firsthash = std::hash<UShort>{}(port.first());
      size_t lasthash = std::hash<UShort>{}(port.last());
      return EMurmurHash64::combine(firsthash, lasthash);
   }
};

template <>
struct hash<EIpFilterRule::PortVec>
{
   std::size_t operator()(const EIpFilterRule::PortVec &pv) const noexcept
   {
      size_t hash = 0xc70f6907UL;
      for (auto port : pv)
      {
         size_t porthash = std::hash<EIpFilterRule::Port>{}(port);
         hash = EMurmurHash64::combine(hash, porthash);
      }
      return hash;
   }
};

template <>
struct hash<EIpFilterRule::Option>
{
   std::size_t operator()(const EIpFilterRule::Option &option) const noexcept
   {
      return option.hash();
   }
};

template <>
struct hash<EIpFilterRule::OptionPtrVec>
{
   std::size_t operator()(const EIpFilterRule::OptionPtrVec &options) const noexcept
   {
      size_t hash = 0xc70f6907UL;
      for (auto option : options)
         hash = EMurmurHash64::combine(hash, option->hash());
      return hash;
   }
};

template <>
struct hash<EIpFilterRule>
{
   std::size_t operator()(const EIpFilterRule &r) const noexcept
   {
      size_t hash = 0xc70f6907UL;
      hash = EMurmurHash64::combine(hash, std::hash<Int>{}(static_cast<Int>(r.action())));
      hash = EMurmurHash64::combine(hash, std::hash<Int>{}(static_cast<Int>(r.direction())));
      hash = EMurmurHash64::combine(hash, std::hash<Int>{}(static_cast<Int>(r.protocol())));
      hash = EMurmurHash64::combine(hash, std::hash<EIpAddress>{}(r.source()));
      hash = EMurmurHash64::combine(hash, std::hash<EIpFilterRule::PortVec>{}(r.sourcePorts()));
      hash = EMurmurHash64::combine(hash, std::hash<EIpAddress>{}(r.destination()));
      hash = EMurmurHash64::combine(hash, std::hash<EIpFilterRule::PortVec>{}(r.destinationPorts()));
      hash = EMurmurHash64::combine(hash, std::hash<EIpFilterRule::OptionPtrVec>{}(r.options()));
      return hash;
   }
};

typedef std::shared_ptr<EIpFilterRule> EIpFilterRulePtr;
typedef std::list<EIpFilterRule> EIpFilterRuleList;
typedef std::list<EIpFilterRulePtr> EIpFilterRulePtrList;

} // namespace std


#endif // #ifndef __eip_h_included
