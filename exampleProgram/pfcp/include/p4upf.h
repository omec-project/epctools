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

#ifndef __P4UPF_H
#define __P4UPF_H

#include <netinet/in.h>

#include <array>
#include <bitset>

#include "eerror.h"
#include "epfcp.h"
#include "eostring.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DECLARE_ERROR_ADVANCED(AccessPointName_InvalidLengthTooLong);
DECLARE_ERROR_ADVANCED(AccessPointName_ParsingErrorInvalidLength);

class AccessPointName
{
public:
   AccessPointName() {}
   AccessPointName(cpStr apn)
   {
      *this = apn;
   }

   AccessPointName &operator=(cpStr apn)
   {
      Int ofs = 0;
      Char label[maxApnLength_ + 1] = {0};
      labels_.clear();
      for (Int idx=0; apn[idx]; idx++)
      {
         if (apn[idx] == '.')
         {
            label[ofs] = 0;
            labels_.push_back(EString(label));
            ofs = 0;
         }
         else
         {
            label[ofs++] = apn[idx];
            if (ofs >= maxApnLength_)
               throw AccessPointName_InvalidLengthTooLong();
         }
      }
      return *this;
   }

   AccessPointName &parse(cpStr apn)
   {
      return *this = apn;
   }

   AccessPointName &parse(cpUChar data, size_t length)
   {
      size_t ofs = 0;
      while (ofs < length)
      {
         UChar len = data[ofs++];
         if (ofs + len > length)
            throw AccessPointName_ParsingErrorInvalidLength();
         addLabel(&data[ofs], len);
         ofs += len;
      }
      return *this;
   }

   const EStringVec &labels()                         { return labels_; }

   AccessPointName &addLabel(cpStr lbl)               { labels_.push_back(EString(lbl)); return *this; }
   AccessPointName &addLabel(cpStr lbl, size_t len)   { labels_.push_back(EString(lbl,len)); return *this; }
   AccessPointName &addLabel(cpUChar lbl, size_t len) { labels_.push_back(EString(reinterpret_cast<cpStr>(lbl),len)); return *this; }

private:
   // max length per 23.003 section 9.1.1
   static const Int maxApnLength_ = 63;
   EStringVec labels_;
};

typedef AccessPointName DataNetworkName;
typedef AccessPointName ApnDnn;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef UShort ConnectionSetId;
typedef ULongLong SessionEndpointId;
typedef ULong TunnelEndpointId;
typedef ULong SdfFilterId;
typedef ULong QerCorrelationId;
typedef Char QosFlowId;
typedef Bool ReflectiveQosIndicator;
typedef UChar TrafficEndpointId;
typedef EOctetString FramedRoute;
typedef std::list<FramedRoute> FramedRouteList;
typedef ULong FramedRouting;
typedef std::list<EOctetString> FramedIpv6RouteList;
typedef ULong Precedence;
typedef ULong MeasurementPeriod;
typedef ULong TimeThreshold;
typedef ULong SubsequentTimeThreshold;
typedef ULong TimeQuota;
typedef ULong SubsequentTimeQuota;
typedef ULong QuotaHoldingTime;
typedef ULong EventId;
typedef ULong EventThreshold;
typedef ULong InactivityDetectionTime;
typedef ULong BaseTimeInterval;
typedef ULong AggregatedUrrId;
typedef ULong EthernetInactivityTimer;
typedef ULong EventQuota;
typedef ULong SubsequentEventQuota;
typedef ULong SubsequentEventThreshold;
typedef EString NetworkInstance;
typedef EString ForwardingPolicy;
typedef ULong UserPlaneInactivityTimer;
typedef EOctetString Nai;
typedef EOctetString TriggeringEvents;
typedef EOctetString TraceReference;
typedef EOctetString TraceInterfaces;

typedef std::array<UChar,2> TosTrafficClass;
typedef std::array<UChar,4> SecurityParameterIndex;
typedef std::array<UChar,3> FlowLabel;
typedef std::array<UChar,2> ServiceClassIndicator;
typedef std::array<UChar,2> EtherType;
typedef std::array<UChar,2> VLanTag;
typedef std::array<UChar,2> ReportingTriggersArray;
typedef std::array<UChar,3> PlmnIdArray;
typedef std::array<Char,4> MccArray;
typedef std::array<Char,4> MncArray;

typedef Char BarId;
typedef ULong FarId;
typedef ULong QerId;
typedef ULong PdrId;
typedef ULong UrrId;
typedef ULong LinkedUrrId;
typedef Char TrafficEndpointId;

typedef std::list<QerId> QerIdList;
typedef std::list<UrrId> UrrIdList;
typedef std::list<LinkedUrrId> LinkedUrrIdList;

// typedef std::vector<UrrId> UrrIdVec;
// typedef std::vector<LinkedUrrId> LinkedUrrIdVec;
// typedef std::vector<QerId> QerIdVec;

typedef struct { UChar value : 6; } Qfi;
typedef std::vector<Qfi> QfiVec;

enum class GateStatusEnum
{
   undefined = -1,
   open = 0,
   closed = 1
};

enum class TimeUnitEnum
{
   undefined = -1,
   minute = 0,
   sixminutes = 1,
   hour = 2,
   day = 3,
   week = 4
};

enum class DestinationInterfaceEnum
{
   undefined = -1,
   access = 0,
   core = 1,
   sgin6lan = 2,
   cpfunction = 3,
   lifunction = 4,

   uplink = core,
   downlink = access
};

enum class SourceInterfaceEnum
{
   undefined = -1,
   access = 0,
   core = 1,
   sgin6lan = 2,
   cpfunction = 3,

   uplink = access,
   downlink = core
};

enum class RedirectAddressTypeEnum
{
   undefined,
   ipv4,
   ipv6,
   url,
   sipuri
};

enum class OuterHeaderCreationEnum
{
   undefined,
   gtpu_udp_ipv4,
   gtpu_udp_ipv6,
   udp_ipv4,
   udp_ipv6
};

enum class OuterHeaderRemovalEnum
{
   undefined,
   gtpu_udp_ipv4,
   gtpu_udp_ipv6,
   udp_ipv4,
   udp_ipv6
};

enum class HeaderTypeEnum
{
   undefined,
   http
};

enum class UeIpAddressSrcDstEnum
{
   undefined = -1,
   source = 0,
   destination = 1
};

enum class BaseTimeIntervalEnum
{
   undefined = -1,
   ctp = 0,
   dtp = 1
};

enum class PdnTypeEnum
{
   undefined = -1,
   ipv4      = 1,
   ipv6      = 2,
   ipv4v6    = 3,
   nonip     = 4,
   ethernet  = 5
};

enum class TraceDepthEnum
{
   undefined = -1,
   minimum   = 0,
   medium    = 1,
   maximum   = 2,
   minimumWithoutVendorSpecificExtension = 3,
   mediumWithoutVendorSpecificExtension = 4,
   maximumWithoutVendorSpecificExtension = 5
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class FqCsid
{
public:
   FqCsid()
      : ipAddress_{},
        csid_(0)
   {      
   }
   FqCsid(const FqCsid &fqcsid)
      : ipAddress_( fqcsid.ipAddress_ ),
        csid_( fqcsid.csid_ )
   {
   }
   FqCsid(const EIpAddress &ipaddr, const ConnectionSetId csid)
      : ipAddress_( ipaddr ),
        csid_( csid )
   {
   }

   FqCsid &operator=(const FqCsid &fqcsid)
   {
      ipAddress_ = fqcsid.ipAddress_;
      csid_ = fqcsid.csid_;
      return *this;
   }

   FqCsid &operator=(const EIpAddress &ipaddr)
   {
      ipAddress_ = ipaddr;
      return *this;
   }

   FqCsid &operator=(const ConnectionSetId csid)
   {
      csid_ = csid;
      return *this;
   }

   const EIpAddress &ipAddress() const { return ipAddress_; }
   const ConnectionSetId csid() const { return csid_; }

private:
   EIpAddress ipAddress_;
   ConnectionSetId csid_;
};

namespace std
{
   template <>
   struct hash<FqCsid>
   {
      std::size_t operator()(const FqCsid &fqcsid) const noexcept
      {
         size_t hIpAddress = std::hash<EIpAddress>{}(fqcsid.ipAddress());
         size_t hCsid = EMurmurHash64::getHash(fqcsid.csid());
         return EMurmurHash64::combine(hIpAddress,hCsid);
      }
   };
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class FqSeid
{
public:
   FqSeid()
      : ipAddress_{},
        seid_(0)
   {      
   }
   FqSeid(const FqSeid &fqseid)
      : ipAddress_( fqseid.ipAddress_ ),
        seid_( fqseid.seid_ )
   {
   }
   FqSeid(const EIpAddress &ipaddr, const SessionEndpointId seid)
      : ipAddress_( ipaddr ),
        seid_( seid )
   {
   }

   FqSeid &operator=(const FqSeid &fqseid)
   {
      ipAddress_ = fqseid.ipAddress_;
      seid_ = fqseid.seid_;
      return *this;
   }

   FqSeid &operator=(const EIpAddress &ipaddr)
   {
      ipAddress_ = ipaddr;
      return *this;
   }

   FqSeid &operator=(const SessionEndpointId seid)
   {
      seid_ = seid;
      return *this;
   }

   const EIpAddress &ipAddress() const { return ipAddress_; }
   const SessionEndpointId seid() const { return seid_; }

private:
   EIpAddress ipAddress_;
   SessionEndpointId seid_;
};

namespace std
{
   template <>
   struct hash<FqSeid>
   {
      std::size_t operator()(const FqSeid &fqseid) const noexcept
      {
         size_t iphash = std::hash<EIpAddress>{}(fqseid.ipAddress());
         size_t seidhash = std::hash<SessionEndpointId>{}(fqseid.seid());
         return EMurmurHash64::combine(iphash,seidhash);
      }
   };
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class FqTeid
{
public:
   FqTeid()
      : ipAddress_{},
        teid_(0)
   {      
   }
   FqTeid(const FqTeid &fteid)
      : ipAddress_( fteid.ipAddress_ ),
        teid_( fteid.teid_ )
   {
   }
   FqTeid(const EIpAddress &ipaddr, const TunnelEndpointId teid)
      : ipAddress_( ipaddr ),
        teid_( teid )
   {
   }

   FqTeid &operator=(const FqTeid &fteid)
   {
      ipAddress_ = fteid.ipAddress_;
      teid_ = fteid.teid_;
      return *this;
   }

   FqTeid &operator=(const EIpAddress &ipaddr)
   {
      ipAddress_ = ipaddr;
      return *this;
   }

   FqTeid &operator=(const TunnelEndpointId teid)
   {
      teid_ = teid;
      return *this;
   }

   const EIpAddress &ipAddress() const { return ipAddress_; }
   const TunnelEndpointId teid() const { return teid_; }

private:
   EIpAddress ipAddress_;
   TunnelEndpointId teid_;
};

namespace std
{
   template <>
   struct hash<FqTeid>
   {
      std::size_t operator()(const FqTeid &fqteid) const noexcept
      {
         size_t iphash = std::hash<EIpAddress>{}(fqteid.ipAddress());
         size_t teidhash = std::hash<TunnelEndpointId>{}(fqteid.teid());
         return EMurmurHash64::combine(iphash,teidhash);
      }
   };
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Imsi
{
public:
   Imsi()
      : length_(0),
        imsi_{}
   {
   }
   Imsi(const Imsi &imsi)
   {
      *this = imsi;
   }

   Imsi &operator=(const Imsi &imsi)
   {
      length_ = imsi.length_;
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(imsi_, imsi.imsi_, length_);
      imsi_[length_] = 0;
      return *this;
   }
   Imsi &operator=(cpChar imsi)
   {
      length_ = strlen(imsi);
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(imsi_, imsi, length_);
      imsi_[length_] = 0;
      return *this;
   }

   operator cpChar() const { return imsi_; }

   Imsi &setImsi(const Imsi &imsi)
   {
      return *this = imsi;
   }
   cpChar imsi() { return imsi_; }

   Imsi &setImsi(cpChar imsi)
   {
      return *this = imsi;
   }
   Imsi &setImsi(cpChar imsi, size_t len)
   {
      length_ = len;
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(imsi_, imsi, length_);
      imsi_[length_] = 0;
      return *this;
   }

   size_t length() const { return length_; }

private:
   static const size_t maxLength = 15;

   size_t length_;
   Char imsi_[maxLength + 1];
};

namespace std
{
   template <>
   struct hash<Imsi>
   {
      std::size_t operator()(const Imsi &imsi) const noexcept
      {
         return EMurmurHash64::getHash(imsi, imsi.length());
      }
   };
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Imei
{
public:
   Imei()
      : length_(0),
        imei_{}
   {
   }
   Imei(const Imei &imei)
   {
      *this = imei;
   }

   Imei &operator=(const Imei &imei)
   {
      length_ = imei.length_;
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(imei_, imei.imei_, length_);
      imei_[length_] = 0;
      return *this;
   }
   Imei &operator=(cpChar imei)
   {
      length_ = strlen(imei);
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(imei_, imei, length_);
      imei_[length_] = 0;
      return *this;
   }

   operator cpChar() const { return imei_; }

   Imei &setImei(const Imei &imei)
   {
      return *this = imei;
   }
   cpChar imei() const { return imei_; }

   Imei &setImei(cpChar imei)
   {
      return *this = imei;
   }
   Imei &setImei(cpChar imei, size_t len)
   {
      length_ = len;
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(imei_, imei, length_);
      imei_[length_] = 0;
      return *this;
   }

private:
   static const size_t maxLength = 16;

   size_t length_;
   Char imei_[maxLength + 1];
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Msisdn
{
public:
   Msisdn()
      : length_(0),
        msisdn_{}
   {
   }
   Msisdn(const Msisdn &msisdn)
   {
      *this = msisdn;
   }

   Msisdn &operator=(const Msisdn &msisdn)
   {
      length_ = msisdn.length_;
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(msisdn_, msisdn.msisdn_, length_);
      msisdn_[length_] = 0;
      return *this;
   }
   Msisdn &operator=(cpChar msisdn)
   {
      length_ = strlen(msisdn);
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(msisdn_, msisdn, length_);
      msisdn_[length_] = 0;
      return *this;
   }

   operator cpChar() { return msisdn_; }

   Msisdn &setImsi(const Msisdn &msisdn)
   {
      return *this = msisdn;
   }
   cpChar msisdn() { return msisdn_; }

   Msisdn &setMsisdn(cpChar msisdn)
   {
      return *this = msisdn;
   }
   Msisdn &setMsisdn(cpChar msisdn, size_t len)
   {
      length_ = len;
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(msisdn_, msisdn, length_);
      msisdn_[length_] = 0;
      return *this;
   }

private:
   static const size_t maxLength = 15;

   size_t length_;
   Char msisdn_[maxLength + 1];
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Nai
{
public:
   Nai()
      : length_(0),
        nai_{}
   {
   }
   Nai(const Nai &nai)
   {
      *this = nai;
   }

   Nai &operator=(const Nai &nai)
   {
      length_ = nai.length_;
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(nai_, nai.nai_, length_);
      nai_[length_] = 0;
      return *this;
   }
   Nai &operator=(cpChar nai)
   {
      length_ = strlen(nai);
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(nai_, nai, length_);
      nai_[length_] = 0;
      return *this;
   }

   operator cpChar() { return nai_; }

   Nai &setNai(const Nai &nai)
   {
      return *this = nai;
   }
   cpChar nai() { return nai_; }

   Nai &setImsi(cpChar nai)
   {
      return *this = nai;
   }
   Nai &setNai(cpChar nai, size_t len)
   {
      length_ = len;
      if (length_ > maxLength)
         length_ = maxLength;
      memcpy(nai_, nai, length_);
      nai_[length_] = 0;
      return *this;
   }

private:
   // TODO: Need to rework this class to allocate the NAI buffer from a global store
   //       the local value should be a pointer and set to NULL unless/until a value
   //       is assigned.
   static const size_t maxLength = 253;

   size_t length_;
   Char nai_[maxLength + 1];
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class SdfFilter
{
public:
   SdfFilter() : presence_(0) {}
   SdfFilter(const SdfFilter &f)
      : presence_      (f.presence_),
        flowDesc_      (f.flowDesc_),
        tosTrafficCls_ (f.tosTrafficCls_),
        secParmIdx_    (f.secParmIdx_),
        flowLbl_       (f.flowLbl_),
        id_            (f.id_)
   {
   }

   SdfFilter &operator=(const SdfFilter &f)
   {
      presence_      = f.presence_;
      flowDesc_      = f.flowDesc_;
      tosTrafficCls_ = f.tosTrafficCls_;
      secParmIdx_    = f.secParmIdx_;
      flowLbl_       = f.flowLbl_;
      id_            = f.id_;
      return *this;
   }

   const EIpFilterRule &flowDescription()                 const { return flowDesc_; }
   const TosTrafficClass &tosTrafficCls()                 const { return tosTrafficCls_; }
   const SecurityParameterIndex &securityParameterIndex() const { return secParmIdx_; }
   const FlowLabel &flowLabel()                           const { return flowLbl_; }
   const SdfFilterId sdfFilterId()                        const { return id_; }

   SdfFilter &setFlowDescription(cpStr data)                               { presence_.flowDesc_ = 1;       flowDesc_ = data;     return *this; }
   SdfFilter &setFlowDescription(const EIpFilterRule &fd)                  { presence_.flowDesc_ = 1;       flowDesc_ = fd;       return *this; }
   SdfFilter &setTosTrafficCls(const TosTrafficClass &ttc)                 { presence_.tosTrafficCls_ = 1;  tosTrafficCls_ = ttc; return *this; }
   SdfFilter &setSecurityParameterIndex(const SecurityParameterIndex &spi) { presence_.secParmIdx_ = 1;     secParmIdx_ = spi;    return *this; }
   SdfFilter &setFlowLabel(const FlowLabel &fl)                            { presence_.flowLbl_ = 1;        flowLbl_ = fl;        return *this; }
   SdfFilter &setSdfFilterId(const SdfFilterId id)                         { presence_.id_ = 1;             id_ = id;             return *this; }

   Bool flowDescriptionPresent()        const { return presence_.flowDesc_; }
   Bool tosTrafficClsPresent()          const { return presence_.tosTrafficCls_; }
   Bool securityParameterIndexPresent() const { return presence_.secParmIdx_; }
   Bool flowLabelPresent()              const { return presence_.flowLbl_; }
   Bool sdfFilterIdPresent()            const { return presence_.id_; }

private:
   struct
   {
      UChar flowDesc_      : 1;
      UChar tosTrafficCls_ : 1;
      UChar secParmIdx_    : 1;
      UChar flowLbl_       : 1;
      UChar id_            : 1;
   } presence_;
   EIpFilterRule          flowDesc_;
   TosTrafficClass        tosTrafficCls_;
   SecurityParameterIndex secParmIdx_;
   FlowLabel              flowLbl_;
   SdfFilterId            id_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class DlFlowLevelMarking
{
public:
   DlFlowLevelMarking() : presence_{0}
   {
   }
   DlFlowLevelMarking(const DlFlowLevelMarking &dlflm)
      : presence_      (dlflm.presence_),
        tosTrafficCls_ (dlflm.tosTrafficCls_),
        svcClsInd_     (dlflm.svcClsInd_)
   {
   }

   DlFlowLevelMarking &operator=(const DlFlowLevelMarking &dlflm)
   {
      presence_      = dlflm.presence_;
      tosTrafficCls_ = dlflm.tosTrafficCls_;
      svcClsInd_     = dlflm.svcClsInd_;
      return *this;
   }
   
   const TosTrafficClass &tosTrafficCls()   const { return tosTrafficCls_; }
   const ServiceClassIndicator &svcClsInd() const { return svcClsInd_; }

   DlFlowLevelMarking &setTosTrafficCls(const TosTrafficClass &ttc) { presence_.tosTrafficCls_ = 1; tosTrafficCls_ = ttc; return *this; }
   DlFlowLevelMarking &setSvcClsInd(const ServiceClassIndicator &sci) { presence_.svcClsInd_ = 1;   svcClsInd_ = sci;     return *this; }

   Bool tosTrafficClsPresent() const { return presence_.tosTrafficCls_; }
   Bool svcClsIndPresent()     const { return presence_.svcClsInd_; }

private:
   struct {
      UChar tosTrafficCls_ : 1;
      UChar svcClsInd_     : 1;
   } presence_;
   TosTrafficClass       tosTrafficCls_;
   ServiceClassIndicator svcClsInd_;
};

class GateStatus
{
public:
   GateStatus()
      : ul_(GateStatusEnum::undefined),
        dl_(GateStatusEnum::undefined)
   {
   }
   GateStatus(const GateStatus &gs)
      : ul_(gs.ul_),
        dl_(gs.dl_)
   {
   }

   GateStatus &operator=(const GateStatus &gs)
   {
      ul_ = gs.ul_;
      dl_ = gs.dl_;
      return *this;
   }

   const GateStatusEnum uplink()   const { return ul_; }
   const GateStatusEnum downlink() const { return dl_; }

   GateStatus &setUplink(const GateStatusEnum gs)   { ul_ = gs; return *this; }
   GateStatus &setDownlink(const GateStatusEnum gs) { dl_ = gs; return *this; }

private:
   GateStatusEnum ul_;
   GateStatusEnum dl_;
};

class MaximumBitRate
{
public:
   MaximumBitRate() : ul_(0), dl_(0) {}
   MaximumBitRate(const MaximumBitRate &mbr)
      : ul_ (mbr.ul_),
        dl_ (mbr.dl_)
   {
   }

   MaximumBitRate(const MaximumBitRate &mbr)
   {
      ul_ = mbr.ul_;
      dl_ = mbr.dl_;
   }

   ULongLong uplinkMbr()   const { return ul_; }
   ULongLong downlinkMbr() const { return dl_; }

   MaximumBitRate &setUplinkMbr(ULongLong mbr)   { ul_ = mbr; return *this; }
   MaximumBitRate &setDownlinkMbr(ULongLong mbr) { dl_ = mbr; return *this; }

private:
   ULongLong ul_;
   ULongLong dl_;
};

class GuaranteedBitRate
{
public:
   GuaranteedBitRate() : ul_(0), dl_(0) {}
   GuaranteedBitRate(const GuaranteedBitRate &gbr)
      : ul_ (gbr.ul_),
        dl_ (gbr.dl_)
   {
   }

   GuaranteedBitRate(const GuaranteedBitRate &gbr)
   {
      ul_ = gbr.ul_;
      dl_ = gbr.dl_;
   }

   GuaranteedBitRate &operator=(const GuaranteedBitRate &gbr)
   {
      ul_ = gbr.ul_;
      dl_ = gbr.dl_;
      return *this;
   }

   ULongLong uplinkGbr()  const { return ul_; }
   ULongLong downlikGbr() const { return dl_; }

   GuaranteedBitRate &setUplinkGbr(const ULongLong gbr)   { ul_ = gbr; return *this; }
   GuaranteedBitRate &setDownlinkGbr(const ULongLong gbr) { dl_ = gbr; return *this; }

private:
   ULongLong ul_;
   ULongLong dl_;
};

class PacketRate
{
public:
   PacketRate()
      : presence_{0},
        ultu_(TimeUnitEnum::undefined),
        dltu_(TimeUnitEnum::undefined),
        ulpr_(0),
        dlpr_(0)
   {
   }
   PacketRate(const PacketRate &pr)
      : presence_(pr.presence_),
        ultu_(pr.ultu_),
        dltu_(pr.dltu_),
        ulpr_(pr.ulpr_),
        dlpr_(pr.dlpr_)
   {
   }

   PacketRate &operator=(const PacketRate &pr)
   {
      presence_ = pr.presence_;
      ultu_     = pr.ultu_;
      dltu_     = pr.dltu_;
      ulpr_     = pr.ulpr_;
      dlpr_     = pr.dlpr_;
      return *this;
   }

   TimeUnitEnum uplinkTimeUnit()   const { return ultu_; }
   TimeUnitEnum downlinkTimeUnit() const { return dltu_; }
   UShort uplinkPacketRate()       const { return ulpr_; }
   UShort donwlinkPacketRate()     const { return dlpr_; }

   PacketRate &setUplinkTimeUnit(TimeUnitEnum tu)   { presence_.ul_ = 1; ultu_ = tu; return *this; }
   PacketRate &setDownlinkTimeUnit(TimeUnitEnum tu) { presence_.dl_ = 1; dltu_ = tu; return *this; }
   PacketRate &setUplinkPacketRate(UShort pr)       { presence_.ul_ = 1; ulpr_ = pr; return *this; }
   PacketRate &setDownlinkPacketRate(UShort pr)     { presence_.dl_ = 1; dlpr_ = pr; return *this; }
   
   Bool uplinkPresent()   const { return presence_.ul_; }
   Bool downlinkPresent() const { return presence_.dl_; }
private:
   struct
   {
      UChar ul_ : 1;
      UChar dl_ : 1;
   } presence_;
   
   TimeUnitEnum ultu_;
   TimeUnitEnum dltu_;
   UShort ulpr_;
   UShort dlpr_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class ApplyAction
{
public:
   ApplyAction()
      : drop_(0),
        forw_(0),
        buff_(0),
        nocp_(0),
        dupl_(0)
   {
   }

   ApplyAction(const ApplyAction & aa)
      : drop_(aa.drop_),
        forw_(aa.forw_),
        buff_(aa.buff_),
        nocp_(aa.nocp_),
        dupl_(aa.dupl_)
   {
   }

   ApplyAction &operator=(const ApplyAction &aa)
   {
      drop_ = aa.drop_;
      forw_ = aa.forw_;
      buff_ = aa.buff_;
      nocp_ = aa.nocp_;
      dupl_ = aa.dupl_;
      return *this;
   }

   const Bool drop()      const { return drop_; }
   const Bool forward()   const { return forw_; }
   const Bool buffer()    const { return buff_; }
   const Bool notifyCp()  const { return nocp_; }
   const Bool duplicate() const { return dupl_; }

   ApplyAction &setDrop(Bool v)      { drop_ = (v ? 1 : 0); return *this; }
   ApplyAction &setForward(Bool v)   { forw_ = (v ? 1 : 0); return *this; }
   ApplyAction &setBuffer(Bool v)    { buff_ = (v ? 1 : 0); return *this; }
   ApplyAction &setNotifyCp(Bool v)  { nocp_ = (v ? 1 : 0); return *this; }
   ApplyAction &setDuplicate(Bool v) { dupl_ = (v ? 1 : 0); return *this; }

private:
   UChar drop_ : 1;
   UChar forw_ : 1;
   UChar buff_ : 1;
   UChar nocp_ : 1;
   UChar dupl_ : 1;
};

class RedirectInformation
{
public:
   RedirectInformation()
      : addrType_(RedirectAddressTypeEnum::undefined)
   {
   }
   RedirectInformation(const RedirectInformation &ri)
      : addrType_(ri.addrType_),
        addr_(ri.addr_)
   {
   }

   RedirectInformation &operator=(const RedirectInformation &ri)
   {
      addrType_ = ri.addrType_;
      addr_ = ri.addr_;
      return *this;
   }

   const RedirectAddressTypeEnum addressType() { return addrType_; }
   const EString &address() { return addr_; }

   RedirectInformation &setAddressType(RedirectAddressTypeEnum t) { addrType_ = t; return *this; }
   RedirectInformation &setAddress(cpStr addr) { addr_ = addr; return *this; }
   RedirectInformation &setAddress(cpStr addr, size_t len) { addr_.assign(addr, len); return *this; }
   RedirectInformation &setAddress(const EString &addr) { addr_ = addr; return *this; }

private:
   RedirectAddressTypeEnum addrType_;
   EString addr_;
};

class OuterHeaderCreation
{
public:
   OuterHeaderCreation()
      : desc_(OuterHeaderCreationDescriptionEnum::undefined),
        teid_(0),
        port_(0)
   {
   }
   OuterHeaderCreation(const OuterHeaderCreation &ohc)
      : desc_(ohc.desc_),
        teid_(ohc.teid_),
        addr_(ohc.addr_),
        port_(ohc.port_)
   {
   }

   OuterHeaderCreation &operator=(const OuterHeaderCreation &ohc)
   {
      desc_ = ohc.desc_;
      teid_ = ohc.teid_;
      addr_ = ohc.addr_;
      port_ = ohc.port_;
      return *this;
   }

   const OuterHeaderCreationDescriptionEnum description() const { return desc_; }
   const TunnelEndpointId teid()                          const { return teid_; }
   const EIpAddress &address()                            const { return addr_; }
   const UShort port()                                    const { return port_; }

   OuterHeaderCreation &setDescription(const OuterHeaderCreationDescriptionEnum d) { desc_ = d; return *this; }
   OuterHeaderCreation &setTeid(const TunnelEndpointId teid)                       { teid_ = teid; return *this; }
   EIpAddress &setAddress()                                                        { return addr_; }
   OuterHeaderCreation &setPort(UShort port)                                       { port_ = port; return *this; }

private:
   OuterHeaderCreationDescriptionEnum desc_;
   TunnelEndpointId teid_;
   EIpAddress addr_;
   UShort port_;
};

class HeaderEnrichment
{
public:
   HeaderEnrichment()
      : type_(HeaderTypeEnum::undefined)
   {
   }

   HeaderEnrichment(const HeaderEnrichment &he)
      : type_(he.type_),
        name_(he.name_),
        value_(he.value_)
   {
   }

   HeaderEnrichment &operator=(const HeaderEnrichment &he)
   {
      type_ = he.type_;
      name_ = he.name_;
      value_ = he.value_;
      return *this;
   }

   const HeaderTypeEnum type() const { return type_; }
   const EString &name()       const { return name_; }
   const EString &value()      const { return value_; }

   HeaderEnrichment &setType(const HeaderTypeEnum type) { type_ = type; return *this; }
   HeaderEnrichment &setName(cpStr name)                { name_ = name; return *this; }
   HeaderEnrichment &setName(const std::string &name)   { name_ = name; return *this; }
   HeaderEnrichment &setName(const EString &name)       { name_ = name; return *this; }
   HeaderEnrichment &setValue(cpStr value)              { value_ = value; return *this; }
   HeaderEnrichment &setValue(const std::string &value) { value_ = value; return *this; }
   HeaderEnrichment &setValue(const EString &value)     { value_ = value; return *this; }

private:
   HeaderTypeEnum type_;
   EString name_;
   EString value_;
};

class Proxying
{
public:
   Proxying()
      : arp_(False),
        ins_(False)
   {
   }
   Proxying(const Proxying &p)
      : arp_(p.arp_),
        ins_(p.ins_)
   {
   }

   Proxying &operator=(const Proxying &p)
   {
      arp_ = p.arp_;
      ins_ = p.ins_;
      return *this;
   }

   const Bool arp() { return arp_; }
   const Bool ins() { return ins_; }

   Proxying &setArp(Bool arp) { arp_ = arp; return *this; }
   Proxying &setIns(Bool ins) { ins_ = ins; return *this; }

private:
   Bool arp_;
   Bool ins_;
};

class ForwardingParameters
{
public:
   ForwardingParameters() {}

   ForwardingParameters(const ForwardingParameters &fp)
      : di_(fp.di_),
        ni_(fp.ni_),
        ri_(fp.ri_),
        ohc_(fp.ohc_),
        tlm_(fp.tlm_),
        fp_(fp.fp_),
        he_(fp.he_),
        ltei_(fp.ltei_),
        p_(fp.p_)
      {
      }

   ForwardingParameters &operator=(const ForwardingParameters &fp)
   {
      di_   = fp.di_;
      ni_   = fp.ni_;
      ri_   = fp.ri_;
      ohc_  = fp.ohc_;
      tlm_  = fp.tlm_;
      fp_   = fp.fp_;
      he_   = fp.he_;
      ltei_ = fp.ltei_;
      p_    = fp.p_;
      return *this;
   }

   const DestinationInterfaceEnum destIntfc()     const { return di_; }
   const AccessPointName &networkInstc()          const { return ni_; }
   const RedirectInformation &redirInfo()         const { return ri_; }
   const OuterHeaderCreation &outerHdrCreation()  const { return ohc_; }
   const TosTrafficClass &xportLvlMarking()       const { return tlm_; }
   const ForwardingPolicy &forwardingPolicy()     const { return fp_; }
   const HeaderEnrichment &headerEnrichment()     const { return he_; }
   const TrafficEndpointId linkedTrafficEndptId() const { return ltei_; }
   const Proxying &proxying()                     const { return p_; }

   ForwardingParameters &setDestIntfc(const DestinationInterfaceEnum di)       { di_ = di; return *this; }
   AccessPointName &setNetworkInstc()                                          { return ni_; }
   RedirectInformation &setRedirInfo()                                         { return ri_; }
   OuterHeaderCreation &setOuterHdrCreation()                                  { return ohc_; }
   ForwardingParameters &setXportLvlMarking(const TosTrafficClass &tlm)        { tlm_ = tlm; return *this; }
   ForwardingPolicy &setForwardingPolicy()                                     { return fp_; }
   HeaderEnrichment &setHeaderEnrichment()                                     { return he_; }
   ForwardingParameters &setLinkedTrafficEndptId(const TrafficEndpointId ltei) { ltei_ = ltei; return *this; }
   Proxying &setProxying()                                                     { return p_; }

private:
   DestinationInterfaceEnum di_;
   AccessPointName          ni_;
   RedirectInformation      ri_;
   OuterHeaderCreation      ohc_;
   TosTrafficClass          tlm_;
   ForwardingPolicy         fp_;
   HeaderEnrichment         he_;
   TrafficEndpointId        ltei_;
   Proxying                 p_;
};

class DuplicatingParameters
{
public:
   DuplicatingParameters()
      : di_(DestinationInterfaceEnum::undefined)
   {
   }
   DuplicatingParameters(const DuplicatingParameters &dp)
      : di_(dp.di_),
        ohc_(dp.ohc_),
        tlm_(dp.tlm_),
        fp_(dp.fp_)
   {
   }

   DuplicatingParameters &operator=(const DuplicatingParameters &dp)
   {
      di_ = dp.di_;
      ohc_ = dp.ohc_;
      tlm_ = dp.tlm_;
      fp_ = dp.fp_;
      return *this;
   }

   const DestinationInterfaceEnum destIntfc()     const { return di_; }
   const OuterHeaderCreation &outerHdrCreation()  const { return ohc_; }
   const TosTrafficClass &transportLevelMarking() const { return tlm_; }
   const ForwardingPolicy &forwardingPolicy()     const { return fp_; }

   DuplicatingParameters &setDestIntfc(const DestinationInterfaceEnum di)      { di_ = di; return *this; }
   OuterHeaderCreation &setOuterHdrCreation()                                  { return ohc_; }
   DuplicatingParameters &setTransportLevelMarking(const TosTrafficClass &tlm) { tlm_ = tlm; return *this; }
   ForwardingPolicy &setForwardingPolicy()                                     { return fp_; }

private:
   DestinationInterfaceEnum di_;
   OuterHeaderCreation ohc_;
   TosTrafficClass tlm_;
   ForwardingPolicy fp_;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class UeIpAddress
{
public:
   UeIpAddress()
      : sd_(UeIpAddressSrcDstEnum::undefined)
   {
   }
   UeIpAddress(const UeIpAddress &ue)
      : addr_(ue.addr_),
        sd_(ue.sd_)
   {
   }

   UeIpAddress &operator=(const UeIpAddress &ue)
   {
      addr_ = ue.addr_;
      sd_ = ue.sd_;
      return *this;
   }

   const EIpAddress &address() const { return addr_; }
   const UeIpAddressSrcDstEnum srcDst() const { return sd_; }

   EIpAddress &setAddress()                               { return addr_; }
   UeIpAddress &setSrcDst(const UeIpAddressSrcDstEnum sd) { sd_ = sd; return *this; }

private:
   EIpAddress addr_;
   UeIpAddressSrcDstEnum sd_;
};

class MacAddress
{
public:
   typedef std::array<UChar,6> AddressValue;

   MacAddress() : presence_{0} {}
   MacAddress(const MacAddress &mac)
      : presence_(mac.presence_),
        src_(mac.src_),
        dest_(mac.dest_),
        upperSrc_(mac.upperSrc_),
        upperDest_(mac.upperDest_)
   {
   }

   MacAddress &operator=(const MacAddress &mac)
   {
      presence_ = mac.presence_;
      src_ = mac.src_;
      dest_ = mac.dest_;
      upperSrc_ = mac.upperSrc_;
      upperDest_ = mac.upperDest_;
      return *this;
   }

   const AddressValue &source()           const { return src_; }
   const AddressValue &destination()      const { return dest_; }
   const AddressValue &upperSource()      const { return upperSrc_; }
   const AddressValue &upperDestination() const { return upperDest_; }

   MacAddress &setSource(const AddressValue &val)            { presence_.src_ = 1; src_ = val; return *this; }
   MacAddress &setDestination(const AddressValue &val)       { presence_.dest_ = 1; dest_ = val; return *this; }
   MacAddress &setUpperSource(const AddressValue &val)       { presence_.upperSrc_ = 1; upperSrc_ = val; return *this; }
   MacAddress &setUppertDestination(const AddressValue &val) { presence_.upperDest_ = 1; upperDest_ = val; return *this; }

   MacAddress &setSourcePresent(Bool present)           { presence_.src_ = present ? 1 : 0; return *this; }
   MacAddress &setDestinationPresent(Bool present)      { presence_.dest_ = present ? 1 : 0; return *this; }
   MacAddress &setUpperSourcePresent(Bool present)      { presence_.upperSrc_ = present ? 1 : 0; return *this; }
   MacAddress &setUpperDestinationPresent(Bool present) { presence_.upperDest_ = present ? 1 : 0; return *this; }

private:
   struct {
      UChar src_ : 1;
      UChar dest_ : 1;
      UChar upperSrc_ : 1;
      UChar upperDest_ : 1;
   } presence_;
   AddressValue src_;
   AddressValue dest_;
   AddressValue upperSrc_;
   AddressValue upperDest_;
};

class EthernetPduSessionInfo
{
public:
   EthernetPduSessionInfo() : ethi_(False) {}
   EthernetPduSessionInfo(const EthernetPduSessionInfo &v) : ethi_(v.ethi_) {}

   EthernetPduSessionInfo &operator=(const EthernetPduSessionInfo &v)
   {
      ethi_ = v.ethi_;
      return *this;
   }

   Bool ethernetIndication() const { return ethi_; }

   EthernetPduSessionInfo &setEthernetIndication(Bool ethi) { ethi_ = ethi; return *this; 
   }
private:
   Bool ethi_;
};


class EthernetPacketFilter
{
public:
   EthernetPacketFilter()
      : presence_{0},
        filterId_(0),
        bide_(False)
   {
   }
   EthernetPacketFilter(const EthernetPacketFilter &epf)
      : presence_(epf.presence_),
        filterId_(epf.filterId_),
        bide_(epf.bide_),
        mac_(epf.mac_),
        et_(epf.et_),
        ctag_(epf.ctag_),
        stag_(epf.stag_),
        sdf_(epf.sdf_)
   {
   }

   EthernetPacketFilter &operator=(const EthernetPacketFilter &epf)
   {
      presence_ = epf.presence_;
      filterId_ = epf.filterId_;
      bide_     = epf.bide_;
      mac_      = epf.mac_;
      et_       = epf.et_;
      ctag_     = epf.ctag_;
      stag_     = epf.stag_;
      sdf_      = epf.sdf_;
      return *this;
   }

   ULong filterId()                 const { return filterId_; }
   Bool biDirFilter()               const { return bide_; }
   const MacAddress &macAddress()   const { return mac_; }
   const EtherType &etherType()     const { return et_; }
   const VLanTag &cTag()            const { return ctag_; }
   const VLanTag &sTag()            const { return stag_; }
   const EIpFilterRule &sdfFilter() const { return sdf_; }

   EthernetPacketFilter &setFilterId(ULong fid)    { presence_.filterId_ = 1; filterId_ = fid;  return *this; }
   EthernetPacketFilter &setBiDirFilter(Bool bide) { presence_.bide_ = 1; bide_ = bide;         return *this; }
   MacAddress &setMacAddress()                     { presence_.mac_ = 1;                        return mac_; }
   EtherType &setEtherType()                       { presence_.et_ = 1;                         return et_; }
   VLanTag &setCTag()                              { presence_.ctag_ = 1;                       return ctag_; }
   VLanTag &setSTag()                              { presence_.stag_ = 1;                       return stag_; }
   EIpFilterRule &setSdfFilter()                   { presence_.sdf_ = 1;                        return sdf_; }

   Bool filterIdPresent()    const { return presence_.filterId_; }
   Bool biDirFilterPresent() const { return presence_.bide_; }
   Bool macAddressPresent()  const { return presence_.mac_; }
   Bool etherTypePresent()   const { return presence_.et_; }
   Bool cTagPresent()        const { return presence_.ctag_; }
   Bool stagPresent()        const { return presence_.stag_; }
   Bool sdfFilterPresent()   const { return presence_.sdf_; }

   EthernetPacketFilter &setFilterIdPresent(Bool present)    { presence_.filterId_ = present; return *this; }
   EthernetPacketFilter &setBiDirFilterPresent(Bool present) { presence_.bide_ = present;     return *this; }
   EthernetPacketFilter &setMacAddressPresent(Bool present)  { presence_.mac_ = present;      return *this; }
   EthernetPacketFilter &setEtherTypePresent(Bool present)   { presence_.et_ = present;       return *this; }
   EthernetPacketFilter &setCTagPresent(Bool present)        { presence_.ctag_ = present;     return *this; }
   EthernetPacketFilter &setStagPresent(Bool present)        { presence_.stag_ = present;     return *this; }
   EthernetPacketFilter &setSdfFilterPresent(Bool present)   { presence_.sdf_ = present;      return *this; }

private:
   struct {
      UChar filterId_ : 1;
      UChar bide_     : 1;
      UChar mac_      : 1;
      UChar et_       : 1;
      UChar ctag_     : 1;
      UChar stag_     : 1;
      UChar sdf_      : 1;
   } presence_;
   ULong filterId_;
   Bool bide_; // Ethernet Filter Properties
   MacAddress mac_;
   EtherType et_;
   VLanTag ctag_;
   VLanTag stag_;
   EIpFilterRule sdf_;
};

typedef std::vector<EthernetPacketFilter> EthernetPacketFilterVec;

class PacketDetectionInformation
{
public:
   PacketDetectionInformation()
      : presence_ {0},
        si_       (SourceInterfaceEnum::undefined),
        fteid_    (),
        ni_       (),
        ue_       (),
        tei_      (0),
        sdf_      (),
        appid_    (),
        epsi_     (),
        epf_      (),
        qfi_      (),
        fr_       (),
        fring_    (0),
        fripv6_   ()
   {
   }

   PacketDetectionInformation(const PacketDetectionInformation &pdi)
      : presence_ (pdi.presence_),
        si_       (pdi.si_),
        fteid_    (pdi.fteid_),
        ni_       (pdi.ni_),
        ue_       (pdi.ue_),
        tei_      (pdi.tei_),
        sdf_      (pdi.sdf_),
        appid_    (pdi.appid_),
        epsi_     (pdi.epsi_),
        epf_      (pdi.epf_),
        qfi_      (pdi.qfi_),
        fr_       (pdi.fr_),
        fring_    (pdi.fring_),
        fripv6_   (pdi.fripv6_)
   {
   }

   PacketDetectionInformation &operator=(const PacketDetectionInformation &pdi)
   {
      presence_ = pdi.presence_;
      si_       = pdi.si_;
      fteid_    = pdi.fteid_;
      ni_       = pdi.ni_;
      ue_       = pdi.ue_;
      tei_      = pdi.tei_;
      sdf_      = pdi.sdf_;
      appid_    = pdi.appid_;
      epsi_     = pdi.epsi_;
      epf_      = pdi.epf_;
      qfi_      = pdi.qfi_;
      fr_       = pdi.fr_;
      fring_    = pdi.fring_;
      fripv6_   = pdi.fripv6_;
      return *this;
   }

   SourceInterfaceEnum srcIntfc()                         const { return si_; }
   const FqTeid &localFteid()                             const { return fteid_; }
   const NetworkInstance &netInstc()                      const { return ni_; }
   const UeIpAddress &ueIpAddress()                       const { return ue_; }
   TrafficEndpointId trafficEndpointId()                  const { return tei_; }
   const EIpFilterRuleList &sdfFilters()                  const { return sdf_; }
   const EOctetString &applicationId()                    const { return appid_; }
   const EthernetPduSessionInfo &ethernetPduSessionInfo() const { return epsi_; }
   const EthernetPacketFilterVec &ethernetPacketFilters() const { return epf_; }
   const QfiVec &qfis()                                   const { return qfi_; }
   const FramedRouteList &framedRoutes()                  const { return fr_; }
   FramedRouting framedRouting()                          const { return fring_; }
   const FramedIpv6RouteList &framedIpv6Routes()          const { return fripv6_; }

   PacketDetectionInformation &setSrcIntfc(SourceInterfaceEnum si)         { presence_.si_     = 1; si_ = si;       return *this;}
   FqTeid &setLocalFteid()                                                 { presence_.fteid_  = 1;                 return fteid_; }
   NetworkInstance &setNetInstc()                                          { presence_.ni_     = 1;                 return ni_; }
   UeIpAddress &setUeIpAddress()                                           { presence_.ue_     = 1;                 return ue_; }
   PacketDetectionInformation &setTrafficEndpointId(TrafficEndpointId tei) { presence_.tei_    = 1; tei_ = tei;     return *this; }
   EIpFilterRuleList &setSdfFilters()                                      { presence_.sdf_    = 1;                 return sdf_; }
   EOctetString &setApplicationId()                                        { presence_.appid_  = 1;                 return appid_; }
   EthernetPduSessionInfo &setEthernetPduSessionInfo()                     { presence_.epsi_   = 1;                 return epsi_; }
   EthernetPacketFilterVec &setEthernetPacketFilters()                     { presence_.epf_    = 1;                 return epf_; }
   QfiVec &setQfis()                                                       { presence_.qfi_    = 1;                 return qfi_; }
   FramedRouteList &setFramedRoutes()                                      { presence_.fr_     = 1;                 return fr_; }
   PacketDetectionInformation &setFramedRouting(FramedRouting fring)       { presence_.fring_  = 1; fring_ = fring; return *this; }
   FramedIpv6RouteList &setFramedIpv6Routes()                              { presence_.fripv6_ = 1;                 return fripv6_; }

   PacketDetectionInformation &addSdfFilter(const EIpFilterRule &sdf)                   { presence_.sdf_    = 1; sdf_.push_back(sdf);       return *this; }
   PacketDetectionInformation &addEthernetPacketFilter(const EthernetPacketFilter &epf) { presence_.epf_    = 1; epf_.push_back(epf);       return *this; }
   PacketDetectionInformation &addFramedRoute(const EOctetString &fr)                   { presence_.fr_     = 1; fr_.push_back(fr);         return *this; }
   PacketDetectionInformation &addFramedIpv6Route(const EOctetString &fripv6)           { presence_.fripv6_ = 1; fripv6_.push_back(fripv6); return *this; }

   Bool srcIntfcPresent()               const { return presence_.si_; }
   Bool localFteidPresent()             const { return presence_.fteid_; }
   Bool netInstcPresent()               const { return presence_.ni_; }
   Bool ueIpAddressPresent()            const { return presence_.ue_; }
   Bool trafficEndpointIdPresent()      const { return presence_.tei_; }
   Bool sdfFiltersPresent()             const { return presence_.sdf_; }
   Bool applicationIdPresent()          const { return presence_.appid_; }
   Bool ethernetPduSessionInfoPresent() const { return presence_.epsi_; }
   Bool ethernetPacketFiltersPresent()  const { return presence_.epf_; }
   Bool qfisPresent()                   const { return presence_.qfi_; }
   Bool framedRoutesPresent()           const { return presence_.fr_; }
   Bool framedRoutingPresent()          const { return presence_.fring_; }
   Bool framedIpv6RoutesPresent()       const { return presence_.fripv6_; }

private:
   struct {
      UChar si_     : 1;
      UChar fteid_  : 1;
      UChar ni_     : 1;
      UChar ue_     : 1;
      UChar tei_    : 1;
      UChar sdf_    : 1;
      UChar appid_  : 1;
      UChar epsi_   : 1;
      UChar epf_    : 1;
      UChar qfi_    : 1;
      UChar fr_     : 1;
      UChar fring_  : 1;
      UChar fripv6_ : 1;
   } presence_;
   SourceInterfaceEnum si_;
   FqTeid fteid_;
   NetworkInstance ni_;
   UeIpAddress ue_;
   TrafficEndpointId tei_;
   EIpFilterRuleList sdf_;
   EOctetString appid_;
   EthernetPduSessionInfo epsi_;
   EthernetPacketFilterVec epf_;
   QfiVec qfi_;
   FramedRouteList fr_;
   FramedRouting fring_;
   FramedIpv6RouteList fripv6_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class MeasurementMethod
{
public:
   MeasurementMethod()
      : dur_(0),
        vol_(0),
        evt_(0)
   {
   }
   MeasurementMethod(const MeasurementMethod &mm)
      : dur_(mm.dur_),
        vol_(mm.vol_),
        evt_(mm.evt_)
   {
   }

   MeasurementMethod &operator=(const MeasurementMethod &mm)
   {
      dur_ = mm.dur_;
      vol_ = mm.vol_;
      evt_ = mm.evt_;
      return *this;
   }

   Bool duration() const { return dur_; }
   Bool volume()   const { return vol_; }
   Bool event()    const { return evt_; }

   MeasurementMethod &setDuration(Bool b) { dur_ = b ? 1 : 0; return *this; }
   MeasurementMethod &setVolume(Bool b)   { vol_ = b ? 1 : 0; return *this; }
   MeasurementMethod &setEvent(Bool b)    { evt_ = b ? 1 : 0; return *this; }

private:
   UChar dur_ : 1;
   UChar vol_ : 1;
   UChar evt_ : 1;
};

class ReportingTriggers
{
public:
   ReportingTriggers() : data_{0} {}
   ReportingTriggers(const ReportingTriggers &rt)
      : data_(rt.data_)
   {
   }

   ReportingTriggers &operator=(const ReportingTriggers &rt)
   {
      data_ = rt.data_;
      return *this;
   }

   Bool periodicReporting()         const { return data_.bits_.perio_; }
   Bool volumeThreshold()           const { return data_.bits_.volth_; }
   Bool timeThreshold()             const { return data_.bits_.timth_; }
   Bool quotaHoldingTime()          const { return data_.bits_.quhti_; }
   Bool trafficStart()              const { return data_.bits_.start_; }
   Bool trafficStop()               const { return data_.bits_.stopt_; }
   Bool droppedDlTrafficThreshold() const { return data_.bits_.droth_; }
   Bool linkedUsageReporting()      const { return data_.bits_.liusa_; }
   Bool volumeQuota()               const { return data_.bits_.volqu_; }
   Bool timeQuota()                 const { return data_.bits_.timqu_; }
   Bool envelopeClosure()           const { return data_.bits_.envcl_; }
   Bool macAddressesReporting()     const { return data_.bits_.macar_; }
   Bool eventThreshold()            const { return data_.bits_.eveth_; }

   const ReportingTriggersArray &bytes() { return data_.bytes_; }

   ReportingTriggers &setOctet5(UChar byte) { data_.bytes_[0] = byte; return *this; }
   ReportingTriggers &setOctet6(UChar byte) { data_.bytes_[1] = byte; return *this; }

private:
   union {
      struct {
         UChar perio_ : 1;
         UChar volth_ : 1;
         UChar timth_ : 1;
         UChar quhti_ : 1;
         UChar start_ : 1;
         UChar stopt_ : 1;
         UChar droth_ : 1;
         UChar liusa_ : 1;

         UChar volqu_ : 1;
         UChar timqu_ : 1;
         UChar envcl_ : 1;
         UChar macar_ : 1;
         UChar eveth_ : 1;
      } bits_;
      std::array<UChar,2> bytes_;
   } data_;
};

class VolumeThreshold
{
public:
   VolumeThreshold()
      : presence_{0},
        total_(0),
        ul_(0),
        dl_(0)
   {
   }
   VolumeThreshold(const VolumeThreshold &vt)
      : presence_(vt.presence_),
        total_(vt.total_),
        ul_(vt.ul_),
        dl_(vt.dl_)
   {
   }

   VolumeThreshold &operator=(const VolumeThreshold &vt)
   {
      presence_ = vt.presence_;
      total_ = vt.total_;
      ul_ = vt.ul_;
      dl_ = vt.dl_;
      return *this;
   }

   ULongLong totalVolume()    const { return total_; }
   ULongLong uplinkVolume()   const { return ul_; }
   ULongLong downlinkVolume() const { return dl_; }

   VolumeThreshold &setTotalVolume(ULongLong total) { presence_.total_; total_ = total; return *this; }
   VolumeThreshold &setUplinkVolume(ULongLong ul)   { presence_.ul_; ul_ = ul; return *this; }
   VolumeThreshold &setDownlinkVolume(ULongLong dl) { presence_.dl_; dl_ = dl; return *this; }

   Bool totalVolumePresent()    const { return presence_.total_; }
   Bool uplinkVolumePresent()   const { return presence_.ul_; }
   Bool downlinkVolumePresent() const { return presence_.dl_; }

   VolumeThreshold &totalVolumePresent(Bool b)    { presence_.total_ = b; return *this; }
   VolumeThreshold &uplinkVolumePresent(Bool b)   { presence_.ul_ = b; return *this; }
   VolumeThreshold &downlinkVolumePresent(Bool b) { presence_.dl_ = b; return *this; }

private:
   struct {
      UChar total_ : 1;
      UChar ul_ : 1;
      UChar dl_ : 1;
   } presence_;
   ULongLong total_;
   ULongLong ul_;
   ULongLong dl_;
};

class SubsequentVolumeThreshold
{
public:
   SubsequentVolumeThreshold()
      : presence_{0},
        total_(0),
        ul_(0),
        dl_(0)
   {
   }
   SubsequentVolumeThreshold(const SubsequentVolumeThreshold &vt)
      : presence_(vt.presence_),
        total_(vt.total_),
        ul_(vt.ul_),
        dl_(vt.dl_)
   {
   }

   SubsequentVolumeThreshold &operator=(const SubsequentVolumeThreshold &vt)
   {
      presence_ = vt.presence_;
      total_ = vt.total_;
      ul_ = vt.ul_;
      dl_ = vt.dl_;
      return *this;
   }

   ULongLong totalVolume()    const { return total_; }
   ULongLong uplinkVolume()   const { return ul_; }
   ULongLong downlinkVolume() const { return dl_; }

   VolumeThreshold &setTotalVolume(ULongLong total) { presence_.total_; total_ = total; return *this; }
   VolumeThreshold &setUplinkVolume(ULongLong ul)   { presence_.ul_; ul_ = ul; return *this; }
   VolumeThreshold &setDownlinkVolume(ULongLong dl) { presence_.dl_; dl_ = dl; return *this; }

   Bool totalVolumePresent()    const { return presence_.total_; }
   Bool uplinkVolumePresent()   const { return presence_.ul_; }
   Bool downlinkVolumePresent() const { return presence_.dl_; }

   VolumeThreshold &totalVolumePresent(Bool b)    { presence_.total_ = b; return *this; }
   VolumeThreshold &uplinkVolumePresent(Bool b)   { presence_.ul_ = b; return *this; }
   VolumeThreshold &downlinkVolumePresent(Bool b) { presence_.dl_ = b; return *this; }

private:
   struct {
      UChar total_ : 1;
      UChar ul_ : 1;
      UChar dl_ : 1;
   } presence_;
   ULongLong total_;
   ULongLong ul_;
   ULongLong dl_;
};

class VolumeQuota
{
public:
   VolumeQuota()
      : presence_{0},
        total_(0),
        ul_(0),
        dl_(0)
   {
   }
   VolumeQuota(const VolumeQuota &vt)
      : presence_(vt.presence_),
        total_(vt.total_),
        ul_(vt.ul_),
        dl_(vt.dl_)
   {
   }

   VolumeQuota &operator=(const VolumeQuota &vt)
   {
      presence_ = vt.presence_;
      total_ = vt.total_;
      ul_ = vt.ul_;
      dl_ = vt.dl_;
      return *this;
   }

   ULongLong totalVolume()    const { return total_; }
   ULongLong uplinkVolume()   const { return ul_; }
   ULongLong downlinkVolume() const { return dl_; }

   VolumeQuota &setTotalVolume(ULongLong total) { presence_.total_; total_ = total; return *this; }
   VolumeQuota &setUplinkVolume(ULongLong ul)   { presence_.ul_; ul_ = ul; return *this; }
   VolumeQuota &setDownlinkVolume(ULongLong dl) { presence_.dl_; dl_ = dl; return *this; }

   Bool totalVolumePresent()    const { return presence_.total_; }
   Bool uplinkVolumePresent()   const { return presence_.ul_; }
   Bool downlinkVolumePresent() const { return presence_.dl_; }

   VolumeQuota &totalVolumePresent(Bool b)    { presence_.total_ = b; return *this; }
   VolumeQuota &uplinkVolumePresent(Bool b)   { presence_.ul_ = b; return *this; }
   VolumeQuota &downlinkVolumePresent(Bool b) { presence_.dl_ = b; return *this; }

private:
   struct {
      UChar total_ : 1;
      UChar ul_ : 1;
      UChar dl_ : 1;
   } presence_;
   ULongLong total_;
   ULongLong ul_;
   ULongLong dl_;
};

class SubsequentVolumeQuota
{
public:
   SubsequentVolumeQuota()
      : presence_{0},
        total_(0),
        ul_(0),
        dl_(0)
   {
   }
   SubsequentVolumeQuota(const SubsequentVolumeQuota &vt)
      : presence_(vt.presence_),
        total_(vt.total_),
        ul_(vt.ul_),
        dl_(vt.dl_)
   {
   }

   SubsequentVolumeQuota &operator=(const SubsequentVolumeQuota &vt)
   {
      presence_ = vt.presence_;
      total_ = vt.total_;
      ul_ = vt.ul_;
      dl_ = vt.dl_;
      return *this;
   }

   ULongLong totalVolume()    const { return total_; }
   ULongLong uplinkVolume()   const { return ul_; }
   ULongLong downlinkVolume() const { return dl_; }

   SubsequentVolumeQuota &setTotalVolume(ULongLong total) { presence_.total_; total_ = total; return *this; }
   SubsequentVolumeQuota &setUplinkVolume(ULongLong ul)   { presence_.ul_; ul_ = ul; return *this; }
   SubsequentVolumeQuota &setDownlinkVolume(ULongLong dl) { presence_.dl_; dl_ = dl; return *this; }

   Bool totalVolumePresent()    const { return presence_.total_; }
   Bool uplinkVolumePresent()   const { return presence_.ul_; }
   Bool downlinkVolumePresent() const { return presence_.dl_; }

   SubsequentVolumeQuota &totalVolumePresent(Bool b)    { presence_.total_ = b; return *this; }
   SubsequentVolumeQuota &uplinkVolumePresent(Bool b)   { presence_.ul_ = b; return *this; }
   SubsequentVolumeQuota &downlinkVolumePresent(Bool b) { presence_.dl_ = b; return *this; }

private:
   struct {
      UChar total_ : 1;
      UChar ul_ : 1;
      UChar dl_ : 1;
   } presence_;
   ULongLong total_;
   ULongLong ul_;
   ULongLong dl_;
};

class DroppedDlTrafficThreshold
{
public:
   DroppedDlTrafficThreshold()
      : presence_{0},
        pa_(0),
        by_(0)
   {
   }
   DroppedDlTrafficThreshold(const DroppedDlTrafficThreshold &dlt)
      : presence_(dlt.presence_),
        pa_(dlt.pa_),
        by_(dlt.by_)
   {
   }

   DroppedDlTrafficThreshold &operator=(const DroppedDlTrafficThreshold &dlt)
   {
      presence_ = dlt.presence_;
      pa_ = dlt.pa_;
      by_ = dlt.by_;
      return *this;
   }

   ULongLong dlPackets() const { return pa_; }
   ULongLong dlBytes() const { return by_; }

   DroppedDlTrafficThreshold &setDlPackets(ULongLong pa) { presence_.pa_ = 1; pa_ = pa; return *this; }
   DroppedDlTrafficThreshold &setDlBytes(ULongLong by)   { presence_.by_ = 1; by_ = by; return *this; }

   Bool dlPacketsPresent() const { return presence_.pa_; }
   Bool dlBytesPresent()   const { return presence_.by_; }

   DroppedDlTrafficThreshold &setDlPacketsPresent(Bool val) { presence_.pa_ = val ? 1 : 0; return *this; }
   DroppedDlTrafficThreshold &setDlBytesPresent(Bool val)   { presence_.by_ = val ? 1 : 0; return *this; }

private:
   struct {
      UChar pa_ : 1;
      UChar by_ : 1;
   } presence_;
   ULongLong pa_;
   ULongLong by_;
};

class EventInformation
{
public:
   EventInformation()
      : id_(0),
        th_(0)
   {
   }
   EventInformation(const EventInformation &ei)
      : id_(ei.id_),
        th_(ei.th_)
   {
   }

   EventInformation &operator=(const EventInformation &ei)
   {
      id_ = ei.id_;
      th_ = ei.th_;
      return *this;
   }

   EventId eventId() const { return id_; }
   EventThreshold eventThreshold() const { return th_; }

   EventInformation &setEventId(EventId id) { id_ = id; return *this; }
   EventInformation &setEventThreshold(EventThreshold th) { th_ = th; return *this; }

private:
   EventId id_;
   EventThreshold th_;
};

class MeasurementInformation
{
public:
   MeasurementInformation() : data_{0} {}
   MeasurementInformation(const MeasurementInformation &mi)
      : data_(mi.data_)
   {
   }

   MeasurementInformation &operator=(const MeasurementInformation &mi)
   {
      data_ = mi.data_;
      return *this;
   }

   Bool mbqe() const { return data_.bits_.mbqe_; }
   Bool inam() const { return data_.bits_.inam_; }
   Bool radi() const { return data_.bits_.radi_; }
   Bool istm() const { return data_.bits_.istm_; }

   MeasurementInformation &setMbqe(Bool b) { data_.bits_.mbqe_ = b ? 1 : 0; return *this; }
   MeasurementInformation &setInam(Bool b) { data_.bits_.inam_ = b ? 1 : 0; return *this; }
   MeasurementInformation &setRadi(Bool b) { data_.bits_.radi_ = b ? 1 : 0; return *this; }
   MeasurementInformation &setIstm(Bool b) { data_.bits_.istm_ = b ? 1 : 0; return *this; }

   MeasurementInformation &clear()
   {
      data_.byte_ = 0;
      return *this;
   }

private:
   union {
      struct {
         UChar mbqe_ : 1;
         UChar inam_ : 1;
         UChar radi_ : 1;
         UChar istm_ : 1;
      } bits_;
      UChar byte_;
   } data_;
};

class TimeQuotaMechanism
{
public:
   TimeQuotaMechanism()
      : btit_(BaseTimeIntervalEnum::undefined),
        bti_(0)
   {
   }
   TimeQuotaMechanism(const TimeQuotaMechanism &tqm)
      : btit_(tqm.btit_),
        bti_(tqm.bti_)
   {
   }

   TimeQuotaMechanism &operator=(const TimeQuotaMechanism &tqm)
   {
      btit_ = tqm.btit_;
      bti_ = tqm.bti_;
      return *this;
   }

   BaseTimeIntervalEnum baseTimeIntervalType() const { return btit_; }
   BaseTimeInterval baseTimeInterval()         const { return bti_; }

   TimeQuotaMechanism &setBaseTimeIntervalType(BaseTimeIntervalEnum btit) { btit_ = btit; return *this; }
   TimeQuotaMechanism &setBaseTimeInterval(BaseTimeInterval bti)          { bti_ = bti; return *this; }

private:
   BaseTimeIntervalEnum btit_;
   BaseTimeInterval bti_;
};

class Multiplier
{
public:
   Multiplier()
      : valueDigits_(0),
        exponent_(0)
   {
   }
   Multiplier(const Multiplier &m)
      : valueDigits_(m.valueDigits_),
        exponent_(m.exponent_)
   {
   }

   Multiplier &operator=(const Multiplier &m)
   {
      valueDigits_ = m.valueDigits_;
      exponent_ = m.exponent_;
      return *this;
   }

   LongLong valueDigits() const { return valueDigits_; }
   Long exponent() const { return exponent_; }

   Multiplier &setValueDigits(const LongLong vd) { valueDigits_ = vd; return *this; }
   Multiplier &setExponent(const Long exp) { exponent_ = exp; return *this; }

private:
   LongLong valueDigits_;
   Long exponent_;
};

class AggregatedUrr
{
public:
   AggregatedUrr()
      : urrid_(0),
        multiplier_()
   {
   }
   AggregatedUrr(const AggregatedUrr &urrs)
      : urrid_(urrs.urrid_),
        multiplier_(urrs.multiplier)
   {
   }

   AggregatedUrr &operator=(const AggregatedUrr &urrs)
   {
      urrid_ = urrs.urrid_;
      multiplier_ = urrs.multiplier;
      return *this;
   }

   AggregatedUrrId urrId() const { return urrid_; }
   const Multiplier &multiplier() const { return multiplier_; }

   AggregatedUrr &setUrrId(const AggregatedUrrId urrid) { urrid_ = urrid; return *this; }
   Multiplier &setMultiplier() { return multiplier_; }

private:
   AggregatedUrrId urrid_;
   Multiplier multiplier_;
};

typedef std::list<AggregatedUrr> AggregatedUrrList;

class AdditionalMonitoringTime
{
public:
   AdditionalMonitoringTime()
      : mt_(),
        svt_(),
        stt_(0),
        svq_(),
        stq_(0),
        set_(0),
        seq_(0)
   {
   }
   AdditionalMonitoringTime(const AdditionalMonitoringTime &amt)
      : mt_(amt.mt_),
        svt_(amt.svt_),
        stt_(amt.stt_),
        svq_(amt.svq_),
        stq_(amt.stq_),
        set_(amt.set_),
        seq_(amt.eq_)
   {
   }

   AdditionalMonitoringTime &operator=(const AdditionalMonitoringTime &amt)
   {
      mt_ = amt.mt_;
      svt_ = amt.svt_;
      stt_ = amt.stt_;
      svq_ = amt.svq_;
      stq_ = amt.stq_;
      set_ = amt.set_;
      seq_ = amt.seq_;
      return *this;
   }

   const ETime &monitoringTime()                                const { return mt_; }
   const SubsequentVolumeThreshold &subsequentVolumeThreshold() const { return svt_; }
   const SubsequentTimeThreshold subsequentTimeThreshold()      const { return stt_; }
   const SubsequentVolumeQuota& subsequentVolumeQuota()         const { return svq_; }
   const SubsequentTimeQuota subsequentTimeQuota()              const { return stq_; }
   const EventThreshold subsequentEventThreshold()              const { return set_; }
   const EventQuota subsequentEventQuota()                      const { return seq_; }

   ETime setMonitoringTime()                                                               { return mt_; }
   SubsequentVolumeThreshold setSubsequentVolumeThreshold()                                { presence_.svt_ = 1; return svt_; }
   AdditionalMonitoringTime &setSubsequentTimeThreshold(const SubsequentTimeThreshold stt) { presence_.stt_; stt_ = stt; return *this; }
   SubsequentVolumeQuota &setSubsequentVolumeQuota()                                       { presence_.svq_ = 1; return svq_; }
   AdditionalMonitoringTime &setSubsequentTimeQuota(const SubsequentTimeQuota stq)         { presence_.stq_ = 1; stq_ = stq; return *this; }
   AdditionalMonitoringTime &setEventThreshold(const EventThreshold set)                   { presence_.set_ = 1; set_ = set; return *this; }
   AdditionalMonitoringTime &setEventQuota(const EventQuota seq)                           { presence_.seq_ = 1; seq_ = seq; return *this; }

   Bool subsequentVolumeThresholdPresent() { return presence_.svt_; }
   Bool subsequentTimeThresholdPresent()   { return presence_.stt_; }
   Bool subsequentVolumeQuotaPresent()     { return presence_.svq_; }
   Bool subsequentTimeQuotaPresent()       { return presence_.stq_; }
   Bool subsequentEventThresholdPresent()  { return presence_.set_; }
   Bool subsequentEventQuotaPresent()      { return presence_.seq_; }

   AdditionalMonitoringTime &setSubsequentVolumeThresholdPresent(Bool p) { presence_.svt_ = p; return *this; }
   AdditionalMonitoringTime &setSubsequentTimeThresholdPresent(Bool p)   { presence_.stt_ = p; return *this; }
   AdditionalMonitoringTime &setSubsequentVolumeQuotaPresent(Bool p)     { presence_.svq_ = p; return *this; }
   AdditionalMonitoringTime &setSubsequentTimeQuotaPresent(Bool p)       { presence_.stq_ = p; return *this; }
   AdditionalMonitoringTime &setSubsequentEventThresholdPresent(Bool p)  { presence_.set_ = p; return *this; }
   AdditionalMonitoringTime &setSubsequentEventQuotaPresent(Bool p)      { presence_.seq_ = p; return *this; }

   AdditionalMonitoringTime &clearPresence()      { presence_ = {0}; return *this; }

private:
   struct {
      UChar svt_ : 1;
      UChar stt_ : 1;
      UChar svq_ : 1;
      UChar stq_ : 1;
      UChar set_ : 1;
      UChar seq_ : 1;
   } presence_;
   ETime mt_;
   SubsequentVolumeThreshold svt_;
   SubsequentTimeThreshold stt_;
   SubsequentVolumeQuota svq_;
   SubsequentTimeQuota stq_;
   EventThreshold set_;
   EventQuota seq_;
};

typedef std::list<AdditionalMonitoringTime> AdditionalMonitoringTimeList;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class UserId
{
public:
   UserId()
      : presence_{0}
   {
   }
   UserId(const UserId &uid)
      : presence_(uid.presence_),
        imsi_(uid.imsi_),
        imei_(uid.imei_),
        msisdn_(uid.msisdn_),
        nai_(uid.nai_)
   {
   }

   UserId &operator=(const UserId &uid)
   {
      presence_ = uid.presence_;
      imsi_ = uid.imsi_;
      imei_ = uid.imei_;
      msisdn_ = uid.msisdn_;
      nai_ = uid.nai_;
      return *this;
   }

   const ETbcdString &imsi() const { return imsi_; }
   const ETbcdString &imei() const { return imei_; }
   const ETbcdString &msisdn() const { return msisdn_; }
   const EOctetString &nai() const { return nai_; }

   ETbcdString &setImsi()   { presence_.imsi_ = 1; return imsi_; }
   ETbcdString &setImei()   { presence_.imei_ = 1; return imei_; }
   ETbcdString &setMsisdn() { presence_.msisdn_ = 1; return msisdn_; }
   EOctetString &setNai()   { presence_.nai_ = 1; return nai_; }

   Bool imsiPresent() const { return presence_.imsi_; }
   Bool imeiPresent() const { return presence_.imei_; }
   Bool msisdnPresent() const { return presence_.msisdn_; }
   Bool naiPresent() const { return presence_.nai_; }

   UserId &setImsiPresent(Bool b)   { presence_.imsi_;   return *this; }
   UserId &setImeiPresent(Bool b)   { presence_.imei_;   return *this; }
   UserId &setMsisdnPresent(Bool b) { presence_.msisdn_; return *this; }
   UserId &setNaiPresent(Bool b)    { presence_.nai_;    return *this; }

   UserId &clearPresence() { presence_ = {0}; return *this; }

private:
   struct {
      UChar imsi_ : 1;
      UChar imei_ : 1;
      UChar msisdn_ : 1;
      UChar nai_ : 1;
   } presence_;
   ETbcdString imsi_;
   ETbcdString imei_;
   ETbcdString msisdn_;
   EOctetString nai_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DECLARE_ERROR_ADVANCED(PlmnId_InvalidPlmnIdLength);
DECLARE_ERROR_ADVANCED(PlmnId_InvalidMccLength);
DECLARE_ERROR_ADVANCED(PlmnId_InvalidMncLength);
DECLARE_ERROR_ADVANCED(PlmnId_MccIsNotNumeric);
DECLARE_ERROR_ADVANCED(PlmnId_MncIsNotNumeric);

class PlmnId
{
public:
   PlmnId()
      : plmnid_{0},
        mcc_{0},
        mnc_{0}
   {
   }
   PlmnId(const PlmnId &pid)
      : plmnid_(pid.plmnid_),
        mcc_(pid.mcc_),
        mnc_(pid.mnc_)
   {
   }

   PlmnId &operator=(const PlmnId &pid)
   {
      plmnid_ = pid.plmnid_;
      mcc_ = pid.mcc_;
      mnc_ = pid.mnc_;
      return *this;
   }

   const PlmnIdArray &plmnId() const { return plmnid_; }
   const MccArray &mcc() const { return mcc_; }
   const MncArray &mnc() const { return mnc_; }

   PlmnId &setPlmnId(cpUChar plmnid, size_t len)
   {
      if (len != plmnid_.max_size())
         throw PlmnId_InvalidPlmnIdLength();
      
      for (size_t idx=0; idx<plmnid_.max_size(); idx++)
         plmnid_[idx] = plmnid[idx];
      
      // mcc
      mcc_[0] = LOW_NIBBLE(plmnid_[0]) + '0';
      mcc_[1] = HIGH_NIBBLE(plmnid_[0]) + '0';
      mcc_[2] = LOW_NIBBLE(plmnid_[1]) + '0';
      mcc_[3] = 0;

      //mnc
      mnc_[0] = LOW_NIBBLE(plmnid_[2]) + '0';
      mnc_[1] = HIGH_NIBBLE(plmnid_[2]) + '0';
      mnc_[2] = (HIGH_NIBBLE(plmnid_[1]) == 0x0f) ? 0 : (HIGH_NIBBLE(plmnid_[1]) + '0');
      mnc_[3] = 0;

      return *this;
   }

   PlmnId &setMcc(cpStr mcc)
   {
      if (strlen(mcc) != 3)
         throw PlmnId_InvalidMccLength();

      for (size_t idx=0; idx<3; idx++)
         if (!std::isdigit(mcc[idx]))
            throw PlmnId_MccIsNotNumeric();
      
      plmnid_[0] = ((((UChar)mcc[0]) - '0') & 0x0f) | (((((UChar)mcc[1]) - '0') << 4) & 0xf0);
      plmnid_[1] = (plmnid_[1] & 0xf0) | ((((UChar)mcc[2]) - '0') & 0x0f);

      return *this;
   }

   PlmnId &setMnc(cpStr mnc)
   {
      size_t len = strlen(mnc);

      if (len < 2 || len > 3)
         throw PlmnId_InvalidMncLength();

      for (size_t idx=0; mnc[idx]; idx++)
         if (!std::isdigit(mnc[idx]))
            throw PlmnId_MncIsNotNumeric();
      
      plmnid_[2] = ((((UChar)mnc[0]) - '0') & 0x0f) | (((((UChar)mnc[1]) - '0') << 4) & 0xf0);
      if (mnc[2])
         plmnid_[1] = (plmnid_[1] & 0x0f) | (((((UChar)mnc[2]) - '0') & 0x0f) << 4);
      else
         plmnid_[1] |= 0xf0;

      return *this;
   }

private:
   PlmnIdArray plmnid_;
   MccArray mcc_;
   MncArray mnc_;
};

class TraceInformation
{
public:
   TraceInformation()
      : plmnid_(),
        tid_(),
        te_(),
        std_(TraceDepthEnum::undefined),
        ifcs_(),
        ip_()
   {
   }
   TraceInformation(const TraceInformation &ti)
      : plmnid_(ti.plmnid_),
        tid_(ti.tid_),
        te_(ti.te_),
        std_(ti.std_),
        ifcs_(ti.ifcs_),
        ip_(ti.ip_)
   {
   }

   TraceInformation &operator=(const TraceInformation &ti)
   {   
      plmnid_ = ti.plmnid_;
      tid_    = ti.tid_;
      te_     = ti.te_;
      std_    = ti.std_;
      ifcs_   = ti.ifcs_;
      ip_     = ti.ip_;
      return *this;
   }

   const PlmnId &plmnId()                        const { return plmnid_; }
   const TraceReference &traceId()               const { return tid_; }
   const TriggeringEvents &triggeringEvents()    const { return te_; }
   const TraceDepthEnum sessionTraceDepth()      const { return std_; }
   const TraceInterfaces &traceInterfaces()      const { return ifcs_; }
   const EIpAddress &collectionEntityIpAddress() const { return ip_; }

   PlmnId &setPlmnId()                                               { return plmnid_; }
   TraceReference &setTraceId()                                      { return tid_; }
   TriggeringEvents &setTriggeringEvents()                           { return te_; }
   TraceInformation &setSessionTraceDepth(const TraceDepthEnum std)  { std_ = std; return *this; }
   TraceInterfaces &setTraceInterfaces()                             { return ifcs_; }
   EIpAddress &setCollectionEntityIpAddress()                        { return ip_; }

private:
   PlmnId plmnid_;
   TraceReference tid_;
   TriggeringEvents te_;
   TraceDepthEnum std_;
   TraceInterfaces ifcs_;
   EIpAddress ip_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class BufferingActionRule
{
public:
   BufferingActionRule(BarId id=0, UChar packetCount=0, LongLong ddnDelay=0)
      : id_(id),
        packetCount_(packetCount),
        ddnDelay_(ddnDelay)
   {
   }
   BufferingActionRule(const BufferingActionRule &bar)
      : id_(bar.id_),
        packetCount_(bar.packetCount_),
        ddnDelay_(bar.ddnDelay_)
   {
   }

   BufferingActionRule &operator=(const BufferingActionRule &bar)
   {
      id_ = bar.id_;
      packetCount_ = bar.packetCount_;
      ddnDelay_ = bar.ddnDelay_;
      return *this;
   }

   BarId id()          const { return id_; }
   UChar packetCount() const { return packetCount_; }
   LongLong ddnDelay() const { return ddnDelay_; }

   BufferingActionRule &setId(BarId id)                   { id_ = id; return *this; }
   BufferingActionRule &setPacketCount(UChar packetCount) { packetCount_ = packetCount; return *this; }
   BufferingActionRule &setDdnDelay(LongLong ddnDelay)    { ddnDelay_ = ddnDelay; return *this; }
   
private:
   BarId id_;
   UChar packetCount_;
   LongLong ddnDelay_;
};

typedef std::shared_ptr<BufferingActionRule> BufferingActionRulePtr;

////////////////////////////////////////////////////////////////////////////////

class PacketDetectionRule
{
public:

   const PdrId id()                                  const { return id_; }
   const Precedence precedence()                     const { return precedence_; }
   const PacketDetectionInformation &pdi()           const { return pdi_; }
   const OuterHeaderRemovalEnum outerHeaderRemoval() const { return ohr_; }
   const FarId farId()                               const { return farid_; }
   const UrrIdList &urrId()                          const { return urrids_; }
   const QerIdList &qerId()                          const { return qerids_; }
   const EOctetStringVec &activePredefinedRules()    const { return aprs_; }

   PacketDetectionRule &setId(const PdrId id)                                    {                        id_ = id;        return *this;   }
   PacketDetectionRule &setPrecedence(const Precedence p)                        {                        precedence_ = p; return *this;   }
   PacketDetectionInformation &setPdi()                                          {                                         return pdi_;    }
   PacketDetectionRule &setOuterHeaderRemoval(const OuterHeaderRemovalEnum ohr)  { presence_.ohr_ = 1;    ohr_ = ohr;      return *this;   }
   PacketDetectionRule &setFarId(const FarId id)                                 { presence_.farid_ = 1;  farid_ = id;     return *this;   }
   UrrIdList &setUrrId()                                                         { presence_.urrids_ = 1;                  return urrids_; }
   QerIdList &setQerId()                                                         { presence_.qerids_ = 1;                  return qerids_; }
   EOctetStringVec &setActivePredefinedRules()                                   { presence_.aprs_ = 1;                    return aprs_;   }

   Bool outerHeaderRemovalPresent()    const { return presence_.ohr_; }
   Bool farIdPresent()                 const { return presence_.farid_; }
   Bool urrIdPresent()                 const { return presence_.urrids_; }
   Bool qerIdPresent()                 const { return presence_.qerids_; }
   Bool activePredefinedRulesPresent() const { return presence_.aprs_; }

   PacketDetectionRule &setOuterHeaderRemovalPresent(Bool p)    { presence_.ohr_ = p;    return *this; }
   PacketDetectionRule &setFarIdPresent(Bool p)                 { presence_.farid_ = p;  return *this; }
   PacketDetectionRule &setUrrIdPresent(Bool p)                 { presence_.urrids_ = p; return *this; }
   PacketDetectionRule &setQerIdPresent(Bool p)                 { presence_.qerids_ = p; return *this; }
   PacketDetectionRule &setActivePredefinedRulesPresent(Bool p) { presence_.aprs_ = p;   return *this; }

   PacketDetectionRule &clearPresence()                         { presence_ = {0};       return *this; }

private:
   struct {
      UChar ohr_ : 1;
      UChar farid_ : 1;
      UChar urrids_ : 1;
      UChar qerids_ : 1;
      UChar aprs_ : 1;
   }  presence_;
   PdrId id_;
   Precedence precedence_;
   PacketDetectionInformation pdi_;
   OuterHeaderRemovalEnum ohr_;
   FarId farid_;
   UrrIdList urrids_;
   QerIdList qerids_;
   EOctetStringVec aprs_;
};

typedef std::unordered_map<PdrId,PacketDetectionRule> PacketDetectionRuleUMap;

// typedef std::shared_ptr<PacketDetectionRule> PacketDetectionRulePtr;
// typedef std::unordered_map<PdrId,PacketDetectionRulePtr> PacketDetectionRulePtrUMap;

////////////////////////////////////////////////////////////////////////////////

class UsageReportingRule
{
public:
   UsageReportingRule()
      : presence_ {0},
        id_       (0),
        mm_       (),
        rt_       (),
        mp_       (0),
        vt_       (),
        vq_       (),
        et_       (0),
        eq_       (0),
        tt_       (0),
        tq_       (0),
        qht_      (0),
        ddtt_     (),
        mt_       (),
        svt_      (),
        stt_      (0),
        svq_      (),
        stq_      (0),
        set_      (0),
        seq_      (0),
        idt_      (0),
        lid_      (),
        mi_       (),
        tqm_      (),
        aus_      (),
        fqa_      (0),
        eit_      (0),
        amt_      ()
   {
   }
   UsageReportingRule(const UsageReportingRule &urr)
      : presence_ (urr.presence_),
        id_       (urr.id_),
        mm_       (urr.mm_),
        rt_       (urr.rt_),
        mp_       (urr.mp_),
        vt_       (urr.vt_),
        vq_       (urr.vq_),
        et_       (urr.et_),
        eq_       (urr.eq_),
        tt_       (urr.tt_),
        tq_       (urr.tq_),
        qht_      (urr.qht_),
        ddtt_     (urr.ddtt_),
        mt_       (urr.mt_),
        svt_      (urr.svt_),
        stt_      (urr.stt_),
        svq_      (urr.svq_),
        stq_      (urr.stq_),
        set_      (urr.set_),
        seq_      (urr.seq_),
        idt_      (urr.idt_),
        lid_      (urr.lid_),
        mi_       (urr.mi_),
        tqm_      (urr.tqm_),
        aus_      (urr.aus_),
        fqa_      (urr.fqa_),
        eit_      (urr.eit_),
        amt_      (urr.amt_)
   {
   }

   UsageReportingRule &operator=(const UsageReportingRule &urr)
   {
      presence_ = urr.presence_;
      id_       = urr.id_;
      mm_       = urr.mm_;
      rt_       = urr.rt_;
      mp_       = urr.mp_;
      vt_       = urr.vt_;
      vq_       = urr.vq_;
      et_       = urr.et_;
      eq_       = urr.eq_;
      tt_       = urr.tt_;
      tq_       = urr.tq_;
      qht_      = urr.qht_;
      ddtt_     = urr.ddtt_;
      mt_       = urr.mt_;
      svt_      = urr.svt_;
      stt_      = urr.stt_;
      svq_      = urr.svq_;
      stq_      = urr.stq_;
      set_      = urr.set_;
      seq_      = urr.seq_;
      idt_      = urr.idt_;
      lid_      = urr.lid_;
      mi_       = urr.mi_;
      tqm_      = urr.tqm_;
      aus_      = urr.aus_;
      fqa_      = urr.fqa_;
      eit_      = urr.eit_;
      amt_      = urr.amt_;
      return *this;
   }

   const UrrId urrId()                                            const { return id_;   }
   const MeasurementMethod &measurementMethod()                   const { return mm_;   }
   const ReportingTriggers &reportingTriggers()                   const { return rt_;   }
   const MeasurementPeriod measurementPeriod()                    const { return mp_;   }
   const VolumeThreshold &volumeThreshold()                       const { return vt_;   }
   const VolumeQuota &volumeQuota()                               const { return vq_;   }
   const EventThreshold eventThreshold()                          const { return et_;   }
   const EventQuota eventQuota()                                  const { return eq_;   }
   const TimeThreshold timeThreshold()                            const { return tt_;   }
   const TimeQuota timeQuota()                                    const { return tq_;   }
   const QuotaHoldingTime quotaHoldingTime()                      const { return qht_;  }
   const DroppedDlTrafficThreshold &droppedDlTrafficThreshold()   const { return ddtt_; }
   const ETime &monitoringTime()                                  const { return mt_;   }
   const SubsequentVolumeThreshold &subsequentVolumeThreshold()   const { return svt_;  }
   const SubsequentTimeThreshold subsequentTimeThreshold()        const { return stt_;  }
   const SubsequentVolumeQuota &subsequentVolumeQuota()           const { return svq_;  }
   const SubsequentTimeQuota subsequentTimeQuota()                const { return stq_;  }
   const SubsequentEventThreshold subsequentEventThreshold()      const { return set_;  }
   const SubsequentEventQuota subsequentEventQuota()              const { return seq_;  }
   const InactivityDetectionTime inactivityDetectionTime()        const { return idt_;  }
   const LinkedUrrIdList &linkedUrrIds()                           const { return lid_;  }
   const MeasurementInformation &measurementInformation()          const { return mi_;   }
   const TimeQuotaMechanism &timeQuotaMechanism()                  const { return tqm_;  }
   const AggregatedUrrList &aggregatedUrrs()                       const { return aus_;  }
   const FarId farIdQuotaAction()                                  const { return fqa_;  }
   const EthernetInactivityTimer ethernetInactivityTimer()         const { return eit_;  }
   const AdditionalMonitoringTimeList &additionalMonitoringTimes() const { return amt_;  }

   UsageReportingRule &setUrrId(const UrrId id)                                        {                      id_ = id;   return *this; }
   MeasurementMethod &setMeasurementMethod()                                           {                                  return mm_;   }
   ReportingTriggers &setReportingTriggers()                                           {                                  return rt_;   }
   UsageReportingRule &setMeasurementPeriod(const MeasurementPeriod mp)                { presence_.mp_   = 1; mp_ = mp;   return *this; }
   VolumeThreshold &setVolumeThreshold()                                               { presence_.vt_   = 1;             return vt_;   }
   VolumeQuota &setVolumeQuota()                                                       { presence_.vq_   = 1;             return vq_;   }
   UsageReportingRule &setEventThreshold(const EventThreshold et)                      { presence_.et_   = 1; et_ = et;   return *this; }
   UsageReportingRule &setEventQuota(const EventQuota eq)                              { presence_.eq_   = 1; eq_ = eq;   return *this; }
   UsageReportingRule &setTimeThreshold(const TimeThreshold tt)                        { presence_.tt_   = 1; tt_ = tt;   return *this; }
   UsageReportingRule &setTimeQuota(const TimeQuota tq)                                { presence_.tq_   = 1; tq_ = tq;   return *this; }
   UsageReportingRule &setQuotaHoldingTime(const QuotaHoldingTime qht)                 { presence_.qht_  = 1; qht_ = qht; return *this; }
   DroppedDlTrafficThreshold &setDroppedDlTrafficThreshold()                           { presence_.ddtt_ = 1;             return ddtt_; }
   ETime &setMonitoringTime()                                                          { presence_.mt_   = 1;             return mt_;   }
   SubsequentVolumeThreshold &setSubsequentVolumeThreshold()                           { presence_.svt_  = 1;             return svt_;  }
   UsageReportingRule &setSubsequentTimeThreshold(const SubsequentTimeThreshold stt)   { presence_.stt_  = 1; stt_ = stt; return *this; }
   SubsequentVolumeQuota &setSubsequentVolumeQuota()                                   { presence_.svq_  = 1;             return svq_;  }
   UsageReportingRule &setSubsequentTimeQuota(const SubsequentTimeQuota stq)           { presence_.stq_  = 1; stq_ = stq; return *this; }
   UsageReportingRule &setSubsequentEventThreshold(const SubsequentEventThreshold set) { presence_.set_  = 1; set_ = set; return *this; }
   UsageReportingRule &setSubsequentEventQuota(const SubsequentEventQuota seq)         { presence_.seq_  = 1; seq_ = seq; return *this; }
   UsageReportingRule &setInactivityDetectionTime(const InactivityDetectionTime idt)   { presence_.idt_  = 1; idt_ = idt; return *this; }
   LinkedUrrIdList &setLinkedUrrIds()                                                  { presence_.lid_  = 1;             return lid_;  }
   MeasurementInformation &setMeasurementInformation()                                 { presence_.mi_   = 1;             return mi_;   }
   TimeQuotaMechanism &setTimeQuotaMechanism()                                         { presence_.tqm_  = 1;             return tqm_;  }
   AggregatedUrrList &setAggregatedUrrs()                                              { presence_.aus_  = 1;             return aus_;  }
   UsageReportingRule &setFarIdQuotaAction(const FarId fqa)                            { presence_.fqa_  = 1; fqa_ = fqa; return *this; }
   UsageReportingRule &setEthernetInactivityTimer(const EthernetInactivityTimer eit)   { presence_.eit_  = 1; eit_ = eit; return *this; }
   AdditionalMonitoringTimeList &setAdditionalMonitoringTimes()                        { presence_.amt_  = 1;             return amt_;  }

   Bool measurementPeriodPresent()         const { return presence_.mp_;   }
   Bool volumeThresholdPresent()           const { return presence_.vt_;   }
   Bool volumeQuotaPresent()               const { return presence_.vq_;   }
   Bool eventThresholdPresent()            const { return presence_.et_;   }
   Bool eventQuotaPresent()                const { return presence_.eq_;   }
   Bool timeThresholdPresent()             const { return presence_.tt_;   }
   Bool timeQuotaPresent()                 const { return presence_.tq_;   }
   Bool quotaHoldingTimePresent()          const { return presence_.qht_;  }
   Bool droppedDlTrafficThresholdPresent() const { return presence_.ddtt_; }
   Bool monitoringTimePresent()            const { return presence_.mt_;   }
   Bool subsequentVolumeThresholdPresent() const { return presence_.svt_;  }
   Bool subsequentTimeThresholdPresent()   const { return presence_.stt_;  }
   Bool subsequentVolumeQuotaPresent()     const { return presence_.svq_;  }
   Bool subsequentTimeQuotaPresent()       const { return presence_.stq_;  }
   Bool subsequentEventThresholdPresent()  const { return presence_.set_;  }
   Bool subsequentEventQuotaPresent()      const { return presence_.seq_;  }
   Bool inactivityDetectionTimePresent()   const { return presence_.idt_;  }
   Bool linkedUrrIdsPresent()              const { return presence_.lid_;  }
   Bool measurementInformationPresent()    const { return presence_.mi_;   }
   Bool timeQuotaMechanismPresent()        const { return presence_.tqm_;  }
   Bool aggregatedUrrsPresent()            const { return presence_.aus_;  }
   Bool farIdQuotaActionPresent()          const { return presence_.fqa_;  }
   Bool ethernetInactivityTimerPresent()   const { return presence_.eit_;  }
   Bool additionalMonitoringTimesPresent() const { return presence_.amt_;  }

   UsageReportingRule &setMeasurementPeriodPresent(Bool p)         { presence_.mp_ = p;   return *this; }
   UsageReportingRule &setVolumeThresholdPresent(Bool p)           { presence_.vt_ = p;   return *this; }
   UsageReportingRule &setVolumeQuotaPresent(Bool p)               { presence_.vq_ = p;   return *this; }
   UsageReportingRule &setEventThresholdPresent(Bool p)            { presence_.et_ = p;   return *this; }
   UsageReportingRule &setEventQuotaPresent(Bool p)                { presence_.eq_ = p;   return *this; }
   UsageReportingRule &setTimeThresholdPresent(Bool p)             { presence_.tt_ = p;   return *this; }
   UsageReportingRule &setTimeQuotaPresent(Bool p)                 { presence_.tq_ = p;   return *this; }
   UsageReportingRule &setQuotaHoldingTimePresent(Bool p)          { presence_.qht_ = p;  return *this; }
   UsageReportingRule &setDroppedDlTrafficThresholdPresent(Bool p) { presence_.ddtt_ = p; return *this; }
   UsageReportingRule &setMonitoringTimePresent(Bool p)            { presence_.mt_ = p;   return *this; }
   UsageReportingRule &setSubsequentVolumeThresholdPresent(Bool p) { presence_.svt_ = p;  return *this; }
   UsageReportingRule &setSubsequentTimeThresholdPresent(Bool p)   { presence_.stt_ = p;  return *this; }
   UsageReportingRule &setSubsequentVolumeQuotaPresent(Bool p)     { presence_.svq_ = p;  return *this; }
   UsageReportingRule &setSubsequentTimeQuotaPresent(Bool p)       { presence_.stq_ = p;  return *this; }
   UsageReportingRule &setSubsequentEventThresholdPresent(Bool p)  { presence_.set_ = p;  return *this; }
   UsageReportingRule &setSubsequentEventQuotaPresent(Bool p)      { presence_.seq_ = p;  return *this; }
   UsageReportingRule &setInactivityDetectionTimePresent(Bool p)   { presence_.idt_ = p;  return *this; }
   UsageReportingRule &setLinkedUrrIdsPresent(Bool p)              { presence_.lid_ = p;  return *this; }
   UsageReportingRule &setMeasurementInformationPresent(Bool p)    { presence_.mi_ = p;   return *this; }
   UsageReportingRule &setTimeQuotaMechanismPresent(Bool p)        { presence_.tqm_ = p;  return *this; }
   UsageReportingRule &setAggregatedUrrsPresent(Bool p)            { presence_.aus_ = p;  return *this; }
   UsageReportingRule &setFarIdQuotaActionPresent(Bool p)          { presence_.fqa_ = p;  return *this; }
   UsageReportingRule &setEthernetInactivityTimerPresent(Bool p)   { presence_.eit_ = p;  return *this; }
   UsageReportingRule &setAdditionalMonitoringTimesPresent(Bool p) { presence_.amt_ = p;  return *this; }

   UsageReportingRule &clearPresence()                             { presence_ = {0};     return *this; }

private:
   struct {
      UChar mp_   : 1;
      UChar vt_   : 1;
      UChar vq_   : 1;
      UChar et_   : 1;
      UChar eq_   : 1;
      UChar tt_   : 1;
      UChar tq_   : 1;
      UChar qht_  : 1;
      UChar ddtt_ : 1;
      UChar mt_   : 1;
      UChar svt_  : 1;
      UChar stt_  : 1;
      UChar svq_  : 1;
      UChar stq_  : 1;
      UChar set_  : 1;
      UChar seq_  : 1;
      UChar idt_  : 1;
      UChar lid_  : 1;
      UChar mi_   : 1;
      UChar tqm_  : 1;
      UChar aus_  : 1;
      UChar fqa_  : 1;
      UChar eit_  : 1;
      UChar amt_  : 1;
   } presence_;
   UrrId                        id_;
   MeasurementMethod            mm_;
   ReportingTriggers            rt_;
   MeasurementPeriod            mp_;
   VolumeThreshold              vt_;
   VolumeQuota                  vq_;
   EventThreshold               et_;
   EventQuota                   eq_;
   TimeThreshold                tt_;
   TimeQuota                    tq_;
   QuotaHoldingTime             qht_;
   DroppedDlTrafficThreshold    ddtt_;
   ETime                        mt_;
   SubsequentVolumeThreshold    svt_;
   SubsequentTimeThreshold      stt_;
   SubsequentVolumeQuota        svq_;
   SubsequentTimeQuota          stq_;
   SubsequentEventThreshold     set_;
   SubsequentEventQuota         seq_;
   InactivityDetectionTime      idt_;
   LinkedUrrIdList              lid_;
   MeasurementInformation       mi_;
   TimeQuotaMechanism           tqm_;
   AggregatedUrrList            aus_;
   FarId                        fqa_;
   EthernetInactivityTimer      eit_;
   AdditionalMonitoringTimeList amt_;
};

typedef std::unordered_map<UrrId,UsageReportingRule> UsageReportingRuleUMap;
// typedef std::shared_ptr<UsageReportingRule> UsageReportingRulePtr;
// typedef std::unordered_map<UrrId,UsageReportingRulePtr> UsageReportingRulePtrUMap;

////////////////////////////////////////////////////////////////////////////////

class QosEnforcementRule
{
public:
   QosEnforcementRule()
      : id_(0),
        cid_(0)
   {
   }

   const QerId id()                             const { return id_; }
   const QerCorrelationId correlationId()       const { return cid_; }
   const GateStatus &gateStatus()               const { return gs_; }
   const MaximumBitRate &maxBitrate()           const { return mbr_; }
   const GuaranteedBitRate &guaranteedBitrate() const { return gbr_; }
   const PacketRate &packetRate()               const { return pr_; }
   const DlFlowLevelMarking &dlFlowLvlMarking() const { return dlflm_; }
   const QosFlowId qfi()                        const { return qfi_; }
   const ReflectiveQosIndicator rqi()           const { return rqi_; }

   QosEnforcementRule &setId(QerId id)                        { id_ = id;                       return *this;  }
   QosEnforcementRule &setCorrelationid(QerCorrelationId cid) { presence_.cid_ = 1; cid_ = cid; return *this;  }
   MaximumBitRate &setMaxBitrate()                            { presence_.mbr_ = 1;             return mbr_;   }
   GuaranteedBitRate &setGuaranteedBitrate()                  { presence_.gbr_ = 1;             return gbr_;   }
   PacketRate &setPacketRate()                                { presence_.pr_ = 1;              return pr_;    }
   DlFlowLevelMarking &setDlFlowLvlMarking()                  { presence_.dlflm_ = 1;           return dlflm_; }
   QosEnforcementRule &setQfi(QosFlowId qfi)                  { presence_.qfi_ = 1; qfi_ = qfi; return *this;  }
   QosEnforcementRule &setRqi(ReflectiveQosIndicator rqi)     { presence_.rqi_ = 1; rqi_ = rqi; return *this;  }
               
   Bool correlationIdPresent()     const { return presence_.cid_; }
   Bool maxBitratePresent()        const { return presence_.mbr_; }
   Bool guaranteedBitratePresent() const { return presence_.gbr_; }
   Bool packerRatePresent()        const { return presence_.pr_; }
   Bool dlFlowLvlMarkingPresent()  const { return presence_.dlflm_; }
   Bool qfiPresent()               const { return presence_.qfi_; }
   Bool rqiPresent()               const { return presence_.rqi_; }

private:
   struct
   {
      UChar cid_ : 1;
      UChar mbr_ : 1;
      UChar gbr_ : 1;
      UChar pr_ : 1;
      UChar dlflm_ : 1;
      UChar qfi_ : 1;
      UChar rqi_ : 1;
   } presence_;
   
   QerId                  id_;
   QerCorrelationId       cid_;
   GateStatus             gs_;
   MaximumBitRate         mbr_;
   GuaranteedBitRate      gbr_;
   PacketRate             pr_;
   DlFlowLevelMarking     dlflm_;
   QosFlowId              qfi_;
   ReflectiveQosIndicator rqi_;
};

typedef std::unordered_map<QerId,QosEnforcementRule> QosEnforcementRuleUMap;
// typedef std::unique_ptr<QosEnforcementRule> QosEnforcementRulePtr;
// typedef std::unordered_map<QerId,QosEnforcementRulePtr> QosEnforcementRulePtrUMap;

////////////////////////////////////////////////////////////////////////////////

class ForwardingActionRule
{
public:
   ForwardingActionRule()
      : presence_{0},
        id_(0),
        bid_(0)
   {
   }
   ForwardingActionRule(const ForwardingActionRule &far)
      : presence_{far.presence_},
        id_(far.id_),
        aa_(far.aa_),
        fp_(far.fp_),
        dp_(far.dp_),
        bid_(far.bid_)
   {
   }

   ForwardingActionRule &operator=(const ForwardingActionRule &far)
   {
      presence_ = far.presence_;
      id_ = far.id_;
      aa_ = far.aa_;
      fp_ = far.fp_;
      dp_ = far.dp_;
      bid_ = far.bid_;
      return *this;
   }

   const FarId id()                                const { return id_; }
   const ApplyAction &applyAction()                const { return aa_; }
   const ForwardingParameters &forwardingParms()   const { return fp_; }
   const DuplicatingParameters &duplicatingParms() const { return dp_; }
   const BarId barId()                             const { return bid_; }

   ForwardingActionRule &setId(FarId id)        { id_ = id; return *this; }
   ApplyAction &setApplyAction()                { return aa_; }
   ForwardingParameters &setForwardingParms()   { return fp_; }
   DuplicatingParameters &setDuplicatingParms() { return dp_; }
   ForwardingActionRule &setBarId(BarId bid)    { presence_.bid_ = 1; bid_ = bid; return *this; }

   Bool forwardingParmsPresent() const    { return presence_.fp_; }
   Bool duplicatingParmsPresent() const   { return presence_.dp_; }
   Bool barIdPresent() const              { return presence_.bid_; }

   ForwardingActionRule &setForwardingParmsPresent(Bool present)  { presence_.fp_ = present ? 1 : 0; return *this; }
   ForwardingActionRule &setDuplicatingParmsPresent(Bool present) { presence_.dp_ = present ? 1 : 0; return *this; }
   ForwardingActionRule &setBarIdPresent(Bool present)            { presence_.bid_ = present ? 1 : 0; return *this; }

private:
   struct {
      UChar fp_ : 1;
      UChar dp_ : 1;
      UChar bid_ : 1;
   } presence_;
   FarId id_;
   ApplyAction aa_;
   ForwardingParameters fp_;
   DuplicatingParameters dp_;
   BufferingActionRulePtr bar_;
   BarId bid_;
};

typedef std::unordered_map<FarId,ForwardingActionRule> ForwardingActionRuleUMap;
// typedef std::unique_ptr<ForwardingActionRule> ForwardingActionRulePtr;
// typedef std::unordered_map<FarId,ForwardingActionRulePtr> ForwardingActionRulePtrUMap;

////////////////////////////////////////////////////////////////////////////////

class TrafficEndpoint
{
public:
   TrafficEndpoint()
      : presence_ {0},
        tid_      (0),
        lfteid_   (),
        ni_       (),
        ueip_     (),
        epsi_     (),
        fr_       (),
        fring_    (0),
        fripv6_   ()
   {
   }
   TrafficEndpoint(const TrafficEndpoint &te)
      : presence_ (te.presence_),
        tid_      (te.tid_),
        lfteid_   (te.lfteid_),
        ni_       (te.ni_),
        ueip_     (te.ueip_),
        epsi_     (te.epsi_),
        fr_       (te.fr_),
        fring_    (te.fring_),
        fripv6_   (te.fripv6_)
   {
   }

   TrafficEndpoint &operator=(const TrafficEndpoint &te)
   {
      presence_ = te.presence_;
      tid_      = te.tid_;
      lfteid_   = te.lfteid_;
      ni_       = te.ni_;
      ueip_     = te.ueip_;
      epsi_     = te.epsi_;
      fr_       = te.fr_;
      fring_    = te.fring_;
      fripv6_   = te.fripv6_;
      return *this;
   }

   const TrafficEndpointId id()                           const { return tid_; }
   const FqTeid &fteid()                                  const { return lfteid_; }
   const NetworkInstance &networkInstance()               const { return ni_; }
   const UeIpAddress &ueIpAddress()                       const { return ueip_; }
   const EthernetPduSessionInfo &ethernetPduSessionInfo() const { return epsi_; }
   const FramedRouteList &framedRoutes()                  const { return fr_; }
   const FramedRouting framedRouting()                    const { return fring_; }
   const FramedIpv6RouteList &framedIpv6Routes()          const { return fripv6_; }

   TrafficEndpoint &setId(const TrafficEndpointId id)           {                        tid_ = id;      return *this;   }
   FqTeid &setFteid()                                           { presence_.lfteid_ = 1;                 return lfteid_; }
   NetworkInstance &setNetworkInstance()                        { presence_.ni_     = 1;                 return ni_;     }
   UeIpAddress &setUeIpAddress()                                { presence_.ueip_   = 1;                 return ueip_;   }
   EthernetPduSessionInfo &setEthernetPduSessionInfo()          { presence_.epsi_   = 1;                 return epsi_;   }
   FramedRouteList &setFramedRoutes()                           { presence_.fr_     = 1;                 return fr_;     }
   TrafficEndpoint &setFramedRouting(const FramedRouting fring) { presence_.fring_  = 1; fring_ = fring; return *this;   }
   FramedIpv6RouteList &setFramedIpv6Routes()                   { presence_.fripv6_ = 1;                 return fripv6_; }

   Bool fteidPresent()                  const { return presence_.lfteid_; }
   Bool networkInstancePresent()        const { return presence_.ni_;     }
   Bool ueIpAddressPresent()            const { return presence_.ueip_;   }
   Bool ethernetPduSessionInfoPresent() const { return presence_.epsi_;   }
   Bool framedRoutesPresent()           const { return presence_.fr_;     }
   Bool framedRoutingPresent()          const { return presence_.fring_;  }
   Bool framedIpv6RoutesPresent()       const { return presence_.fripv6_; }

   TrafficEndpoint &setFteidPresent(Bool p)                  { presence_.lfteid_ = p; return *this; }
   TrafficEndpoint &setNetworkInstancePresent(Bool p)        { presence_.ni_     = p; return *this; }
   TrafficEndpoint &setUeIpAddressPresent(Bool p)            { presence_.ueip_   = p; return *this; }
   TrafficEndpoint &setEthernetPduSessionInfoPresent(Bool p) { presence_.epsi_   = p; return *this; }
   TrafficEndpoint &setFramedRoutesPresent(Bool p)           { presence_.fr_     = p; return *this; }
   TrafficEndpoint &setFramedRoutingPresent(Bool p)          { presence_.fring_  = p; return *this; }
   TrafficEndpoint &setFramedIpv6RoutesPresent(Bool p)       { presence_.fripv6_ = p; return *this; }

   TrafficEndpoint &clearPresence()                          { presence_ = {0};       return *this; }

private:
   struct {
      UChar lfteid_ : 1;
      UChar ni_     : 1;
      UChar ueip_   : 1;
      UChar epsi_   : 1;
      UChar fr_     : 1;
      UChar fring_  : 1;
      UChar fripv6_ : 1;
   } presence_;
   TrafficEndpointId      tid_;
   FqTeid                 lfteid_;
   NetworkInstance        ni_;
   UeIpAddress            ueip_;
   EthernetPduSessionInfo epsi_;
   FramedRouteList        fr_;
   FramedRouting          fring_;
   FramedIpv6RouteList    fripv6_;
};

typedef std::unordered_map<TrafficEndpointId,TrafficEndpoint> TrafficEndpointUMap;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class UpfSlice;
typedef std::shared_ptr<UpfSlice> upfsliceptr_t;

class Session
{
public:
   FqSeid &setCpSeid() { return cpSeid_; }
   FqSeid &setUpSeid() { return upSeid_; }


private:
   FqSeid                   cpSeid_;
   FqSeid                   upSeid_;
   PacketDetectionRuleUMap  pdrs_;
   ForwardingActionRuleUMap fars_;
   UsageReportingRuleUMap   urrs_;
   QosEnforcementRuleUMap   qers_;
   BufferingActionRule      bar_;
   TrafficEndpointUMap      te_;
   PdnTypeEnum              pt_;
   FqCsid                   sgwcsid_;
   FqCsid                   mmecsid_;
   FqCsid                   pgwcsid_;
   FqCsid                   epdgcsid_;
   FqCsid                   twancsid_;
   UserPlaneInactivityTimer upit_;
   UserId                   uid_;
   TraceInformation         ti_;
   DataNetworkName          apndnn_;                  
};


class UpfSlice
{
public:

   PFCP::Interface &interface() { return interface_; }

   static Bool getSlice(const ESocket::Address &addr, UpfSlicePtr &ptr)
   {
      ERDLock lck(sliceLock_);
      auto it = slices_.find(addr);
      if (it != slices_.end())
      {
         ptr = it->second;
         return True;
      }
      return False;
   }

   static Bool addSlice(UpfSlicePtr &ptr)
   {
      EWRLock lck(sliceLock_);
      auto it = slices_.find(ptr->interface().address());
      if (it == slices_.end())
      {
         slices_[ptr->interface().address()] = ptr;
         return True;
      }
      return False;
   }

   static Bool removeSlice(UpfSlicePtr &ptr)
   {
      EWRLock lck(sliceLock_);
      return slices_.erase(ptr->interface().address()) == 1;
   }

   PFCP::Interface &interface() { return interface_; }

private:
   static ERWLock sliceLock_;
   static std::unordered_map<const ESocket::Address&,UpfSlicePtr> slices_;

   PFCP::Interface interface_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#endif // #ifndef __P4UPF_H