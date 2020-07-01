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

#ifndef __PFCPR15INL_H
#define __PFCPR15INL_H

#include "pfcpr15.h"

namespace PFCP_R15
{
////////////////////////////////////////////////////////////////////////////////
//// IE Classes
////////////////////////////////////////////////////////////////////////////////

union AliasPointer {
   in_addr *in_addr_ptr;
   in_addr_t *in_addr_t_ptr;
   in6_addr *in6_addr_ptr;
   uint32_t *uint32_t_ptr;
   uint16_t *uint16_t_ptr;
   uint8_t *uint8_t_ptr;
};

inline CauseIE::CauseIE(pfcp_cause_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_CAUSE, parent),
      ie_(ie)
{
}

inline CauseEnum CauseIE::cause() const
{
   return static_cast<CauseEnum>(ie_.cause_value);
}

inline CauseIE &CauseIE::cause(CauseEnum val)
{
   ie_.cause_value = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline pfcp_cause_ie_t &CauseIE::data()
{
   return ie_;
}

inline uint16_t CauseIE::calculateLength()
{
   return sizeof(pfcp_cause_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline SourceInterfaceIE::SourceInterfaceIE(pfcp_src_intfc_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SRC_INTFC, parent),
      ie_(ie)
{
}

inline SourceInterfaceEnum SourceInterfaceIE::interface_value() const
{
   return static_cast<SourceInterfaceEnum>(ie_.interface_value);
}

inline SourceInterfaceIE &SourceInterfaceIE::interface_value(SourceInterfaceEnum val)
{
   ie_.interface_value = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline pfcp_src_intfc_ie_t &SourceInterfaceIE::data()
{
   return ie_;
}

inline uint16_t SourceInterfaceIE::calculateLength()
{
   return sizeof(pfcp_src_intfc_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool FTeidIE::chid() const
{
   return ie_.chid;
}

inline Bool FTeidIE::ch() const
{
   return ie_.ch;
}

inline Bool FTeidIE::v6() const
{
   return ie_.v6;
}

inline Bool FTeidIE::v4() const
{
   return ie_.v4;
}

inline uint32_t FTeidIE::teid() const
{
   return ie_.teid;
}

inline const in_addr &FTeidIE::ipv4_address() const
{
   AliasPointer u;
   u.uint32_t_ptr = &ie_.ipv4_address;
   return *u.in_addr_ptr;
}

inline const in6_addr &FTeidIE::ipv6_address() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.ipv6_address;
   return *u.in6_addr_ptr;
}

inline uint8_t FTeidIE::choose_id() const
{
   return ie_.choose_id;
}

inline FTeidIE &FTeidIE::teid(uint32_t val)
{
   ie_.chid = 0;
   ie_.ch = 0;
   ie_.teid = val;
   setLength();
   return *this;
}

inline FTeidIE &FTeidIE::ip_address(const ESocket::Address &val)
{
   if (val.getFamily() == ESocket::Family::INET)
      return ip_address(val.getInet().sin_addr);
   else if (val.getFamily() == ESocket::Family::INET6)
      return ip_address(val.getInet6().sin6_addr);
   return *this;
}

inline FTeidIE &FTeidIE::ip_address(const EIpAddress &val)
{
   if (val.family() == AF_INET)
      return ip_address(val.ipv4Address());
   else if (val.family() == AF_INET6)
      return ip_address(val.ipv6Address());
   return *this;
}

inline FTeidIE &FTeidIE::ip_address(const in_addr &val)
{
   ie_.chid = 0;
   ie_.ch = 0;
   ie_.v4 = 1;
   ie_.ipv4_address = val.s_addr;
   setLength();
   return *this;
}

inline FTeidIE &FTeidIE::ip_address(const in6_addr &val)
{
   ie_.chid = 0;
   ie_.ch = 0;
   ie_.v6 = 1;
   std::memcpy(ie_.ipv6_address, val.s6_addr, sizeof(ie_.ipv6_address));
   setLength();
   return *this;
}

inline FTeidIE &FTeidIE::ch(Bool ch)
{
   if (ch)
   {
      ie_.v4 = 0;
      ie_.v6 = 0;
      ie_.ch = 1;
   }
   else
   {
      ie_.ch = 0;
      ie_.chid = 0;
   }
   setLength();
   return *this;      
}

inline FTeidIE &FTeidIE::choose_id(uint8_t val)
{
   ie_.v4 = 0;
   ie_.v6 = 0;
   ie_.ch = 1;
   ie_.chid = 1;
   ie_.choose_id = val;
   setLength();
   return *this;
}

inline pfcp_fteid_ie_t &FTeidIE::data()
{
   return ie_;
}

inline FTeidIE::FTeidIE(pfcp_fteid_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FTEID, parent),
      ie_(ie)
{
}

inline uint16_t FTeidIE::calculateLength()
{
   return
      (ie_.ch ? 0 : sizeof(ie_.teid)) +
      (ie_.chid ? 1 : 0) +
      (ie_.v6 ? sizeof(in6_addr) : 0) +
      (ie_.v4 ? sizeof(in_addr) : 0);
}

////////////////////////////////////////////////////////////////////////////////

inline NetworkInstanceIE::NetworkInstanceIE(pfcp_ntwk_inst_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_NTWK_INST, parent),
      ie_(ie)
{
}

inline const uint8_t *NetworkInstanceIE::ntwk_inst() const
{
   return ie_.ntwk_inst;
}

inline const uint16_t NetworkInstanceIE::ntwk_inst_len() const
{
   return ie_.header.len;
}

inline NetworkInstanceIE &NetworkInstanceIE::ntwk_inst(const EOctetString &val)
{
   return ntwk_inst(val.data(), val.length());
}

inline NetworkInstanceIE &NetworkInstanceIE::ntwk_inst(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.ntwk_inst))
      len = sizeof(ie_.ntwk_inst);
   ie_.header.len = len;
   std::memcpy(ie_.ntwk_inst, val, len);
   // no need to call setLength() since we just set the header length
   return *this;
}

inline pfcp_ntwk_inst_ie_t &NetworkInstanceIE::data()
{
   return ie_;
}

inline uint16_t NetworkInstanceIE::calculateLength()
{
   return ie_.header.len;
}

////////////////////////////////////////////////////////////////////////////////

inline SdfFilterIE::SdfFilterIE(pfcp_sdf_filter_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SDF_FILTER, parent),
      ie_(ie)
{
}

inline Bool SdfFilterIE::fd() const
{
   return ie_.fd;
}

inline Bool SdfFilterIE::ttc() const
{
   return ie_.ttc;
}

inline Bool SdfFilterIE::spi() const
{
   return ie_.spi;
}

inline Bool SdfFilterIE::fl() const
{
   return ie_.fl;
}

inline Bool SdfFilterIE::bid() const
{
   return ie_.bid;
}

inline const uint8_t *SdfFilterIE::flow_desc() const
{
   return ie_.flow_desc;
}

inline uint16_t SdfFilterIE::len_of_flow_desc() const
{
   return ie_.len_of_flow_desc;
}

inline const uint8_t *SdfFilterIE::tos_traffic_cls() const
{
   return ie_.tos_traffic_cls;
}

inline const uint8_t *SdfFilterIE::secur_parm_idx() const
{
   return ie_.secur_parm_idx;
}

inline const uint8_t *SdfFilterIE::flow_label() const
{
   return ie_.flow_label;
}

inline uint32_t SdfFilterIE::sdf_filter_id() const
{
   return ie_.sdf_filter_id;
}

inline SdfFilterIE &SdfFilterIE::bid(Bool bid)
{
   ie_.bid = bid ? 1 : 0;
   setLength();
   return *this;
}

inline SdfFilterIE &SdfFilterIE::flow_desc(const uint8_t *flowDesc, uint16_t flowDescLen)
{
   if (flowDescLen > sizeof(ie_.flow_desc))
      flowDescLen = sizeof(ie_.flow_desc);
   ie_.fd = 1;
   ie_.len_of_flow_desc = flowDescLen;
   std::memcpy(ie_.flow_desc, flowDesc, flowDescLen);
   setLength();
   return *this;
}

inline SdfFilterIE &SdfFilterIE::tos_traffic_cls(const uint8_t *tosTrafficClass)
{
   ie_.ttc = 1;
   std::memcpy(ie_.tos_traffic_cls, tosTrafficClass, sizeof(ie_.tos_traffic_cls));
   setLength();
   return *this;
}

inline SdfFilterIE &SdfFilterIE::secur_parm_idx(const uint8_t *securityParameterIndex)
{
   ie_.spi = 1;
   std::memcpy(ie_.secur_parm_idx, securityParameterIndex, sizeof(ie_.secur_parm_idx));
   setLength();
   return *this;
}

inline SdfFilterIE &SdfFilterIE::flow_label(const uint8_t *flowLabel)
{
   ie_.fl = 1;
   std::memcpy(ie_.flow_label, flowLabel, sizeof(ie_.flow_label));
   setLength();
   return *this;
}

inline SdfFilterIE &SdfFilterIE::sdf_filter_id(const uint32_t sdfFilterId)
{
   ie_.sdf_filter_id = sdfFilterId;
   setLength();
   return *this;
}

inline pfcp_sdf_filter_ie_t &SdfFilterIE::data()
{
   return ie_;
}

inline uint16_t SdfFilterIE::calculateLength()
{
   return
      1 /* octet 5, bits */ +
      sizeof(ie_.sdf_fltr_spare2) +
      (ie_.fd ? (sizeof(ie_.len_of_flow_desc) + ie_.len_of_flow_desc) : 0) +
      (ie_.ttc ? sizeof(ie_.tos_traffic_cls) : 0) +
      (ie_.spi ? sizeof(ie_.secur_parm_idx) : 0) +
      (ie_.fl ? 3 /* should be sizeof(ie_flow_label) */ : 0) +
      sizeof(ie_.sdf_filter_id)
      ;
}

////////////////////////////////////////////////////////////////////////////////

inline ApplicationIdIE::ApplicationIdIE(pfcp_application_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_APPLICATION_ID, parent),
      ie_(ie)
{
}

inline const uint8_t *ApplicationIdIE::app_ident() const
{
   return ie_.app_ident;
}

inline ApplicationIdIE &ApplicationIdIE::app_ident(const uint8_t *app_id)
{
   std::memcpy(ie_.app_ident, app_id, sizeof(ie_.app_ident));
   setLength();
   return *this;
}

inline pfcp_application_id_ie_t &ApplicationIdIE::data()
{
   return ie_;
}

inline uint16_t ApplicationIdIE::calculateLength()
{
   return sizeof(pfcp_application_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline GateStatusIE::GateStatusIE(pfcp_gate_status_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_GATE_STATUS, parent),
      ie_(ie)
{
}

inline DownlinkGateEnum GateStatusIE::dl_gate() const
{
   return static_cast<DownlinkGateEnum>(ie_.dl_gate);
}

inline UplinkGateEnum GateStatusIE::ul_gate() const
{
   return static_cast<UplinkGateEnum>(ie_.ul_gate);
}

inline GateStatusIE &GateStatusIE::dl_gate(DownlinkGateEnum dl)
{
   switch (dl)
   {
      case DownlinkGateEnum::Closed:
      case DownlinkGateEnum::Future1:
      case DownlinkGateEnum::Future2:
      {
         ie_.dl_gate = static_cast<uint8_t>(DownlinkGateEnum::Closed);
         break;
      }
      default:
      {
         ie_.dl_gate = static_cast<uint8_t>(dl);
         break;
      }
   }
   setLength();
   return *this;
}

inline GateStatusIE &GateStatusIE::ul_gate(UplinkGateEnum ul)
{
   switch (ul)
   {
      case UplinkGateEnum::Closed:
      case UplinkGateEnum::Future1:
      case UplinkGateEnum::Future2:
      {
         ie_.ul_gate = static_cast<uint8_t>(UplinkGateEnum::Closed);
         break;
      }
      default:
      {
         ie_.ul_gate = static_cast<uint8_t>(ul);
         break;
      }
   }
   setLength();
   return *this;
}

inline pfcp_gate_status_ie_t &GateStatusIE::data()
{
   return ie_;
}

inline uint16_t GateStatusIE::calculateLength()
{
   return sizeof(pfcp_gate_status_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline MbrIE::MbrIE(pfcp_mbr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_MBR, parent),
     ie_(ie)
{
}

inline uint64_t MbrIE::ul_mbr() const
{
   return ie_.ul_mbr;
}

inline uint64_t MbrIE::dl_mbr() const
{
   return ie_.dl_mbr;
}

inline MbrIE &MbrIE::ul_mbr(uint64_t mbr)
{
   ie_.ul_mbr = mbr & 0x000000ffffffffff;
   setLength();
   return *this;
}

inline MbrIE &MbrIE::dl_mbr(uint64_t mbr)
{
   ie_.dl_mbr = mbr & 0x000000ffffffffff;
   setLength();
   return *this;
}

inline pfcp_mbr_ie_t &MbrIE::data()
{
   return ie_;
}

inline uint16_t MbrIE::calculateLength()
{
   return 10; /* ul mbr + dl mbr --> 5+5=10 */
}

////////////////////////////////////////////////////////////////////////////////

inline GbrIE::GbrIE(pfcp_gbr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_GBR, parent),
     ie_(ie)
{
}

inline uint64_t GbrIE::ul_gbr() const
{
   return ie_.ul_gbr;
}

inline uint64_t GbrIE::dl_gbr() const
{
   return ie_.dl_gbr;
}

inline GbrIE &GbrIE::ul_gbr(uint64_t mbr)
{
   ie_.ul_gbr = mbr & 0x000000ffffffffff;
   setLength();
   return *this;
}

inline GbrIE &GbrIE::dl_gbr(uint64_t mbr)
{
   ie_.dl_gbr = mbr & 0x000000ffffffffff;
   setLength();
   return *this;
}

inline pfcp_gbr_ie_t &GbrIE::data()
{
   return ie_;
}

inline uint16_t GbrIE::calculateLength()
{
   return 10; /* ul gbr + dl gbr --> 5+5=10 */
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t QerCorrelationIdIE::qer_corr_id_val() const
{
   return ie_.qer_corr_id_val;
}

inline QerCorrelationIdIE &QerCorrelationIdIE::qer_corr_id_val(uint32_t id)
{
   ie_.qer_corr_id_val = id;
   setLength();
   return *this;
}

inline pfcp_qer_corr_id_ie_t &QerCorrelationIdIE::data()
{
   return ie_;
}

inline QerCorrelationIdIE::QerCorrelationIdIE(pfcp_qer_corr_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_QER_CORR_ID, parent),
      ie_(ie)
{
}

inline uint16_t QerCorrelationIdIE::calculateLength()
{
   return sizeof(pfcp_qer_corr_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t PrecedenceIE::prcdnc_val() const
{
   return ie_.prcdnc_val;
}

inline PrecedenceIE &PrecedenceIE::prcdnc_val(uint32_t val)
{
   ie_.prcdnc_val = val;
   setLength();
   return *this;
}

inline pfcp_precedence_ie_t &PrecedenceIE::data()
{
   return ie_;
}

inline PrecedenceIE::PrecedenceIE(pfcp_precedence_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_PRECEDENCE, parent),
      ie_(ie)
{
}

inline uint16_t PrecedenceIE::calculateLength()
{
   return sizeof(pfcp_precedence_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline const uint8_t *TransportLevelMarkingIE::tostraffic_cls() const
{
   return ie_.tostraffic_cls;
}

inline TransportLevelMarkingIE &TransportLevelMarkingIE::tostraffic_cls(const uint8_t *val)
{
   std::memcpy(ie_.tostraffic_cls, val, sizeof(ie_.tostraffic_cls));
   setLength();
   return *this;
}

inline pfcp_trnspt_lvl_marking_ie_t &TransportLevelMarkingIE::data()
{
   return ie_;
}

inline TransportLevelMarkingIE::TransportLevelMarkingIE(pfcp_trnspt_lvl_marking_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_TRNSPT_LVL_MARKING, parent),
     ie_(ie)
{
}

inline uint16_t TransportLevelMarkingIE::calculateLength()
{
   return sizeof(pfcp_trnspt_lvl_marking_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool VolumeThresholdIE::tovol() const
{
   return ie_.tovol;
}

inline Bool VolumeThresholdIE::ulvol() const
{
   return ie_.ulvol;
}

inline Bool VolumeThresholdIE::dlvol() const
{
   return ie_.dlvol;
}

inline uint64_t VolumeThresholdIE::total_volume() const
{
   return ie_.total_volume;
}

inline uint64_t VolumeThresholdIE::uplink_volume() const
{
   return ie_.uplink_volume;
}

inline uint64_t VolumeThresholdIE::downlink_volume() const
{
   return ie_.downlink_volume;
}

inline VolumeThresholdIE &VolumeThresholdIE::total_volume(uint64_t val)
{
   ie_.tovol = 1;
   ie_.total_volume = val;
   setLength();
   return *this;
}

inline VolumeThresholdIE &VolumeThresholdIE::uplink_volume(uint64_t val)
{
   ie_.ulvol = 1;
   ie_.uplink_volume = val;
   setLength();
   return *this;
}

inline VolumeThresholdIE &VolumeThresholdIE::downlink_volume(uint64_t val)
{
   ie_.dlvol = 1;
   ie_.downlink_volume = val;
   setLength();
   return *this;
}

inline pfcp_vol_thresh_ie_t &VolumeThresholdIE::data()
{
   return ie_;
}

inline VolumeThresholdIE::VolumeThresholdIE(pfcp_vol_thresh_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_VOL_THRESH, parent),
      ie_(ie)
{
}

inline uint16_t VolumeThresholdIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.tovol ? sizeof(ie_.total_volume) : 0) +
      (ie_.ulvol ? sizeof(ie_.uplink_volume) : 0) +
      (ie_.dlvol ? sizeof(ie_.downlink_volume) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline TimeThresholdIE::TimeThresholdIE(pfcp_time_threshold_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_TIME_THRESHOLD, parent),
   ie_(ie)
{
}

inline uint32_t TimeThresholdIE::time_threshold() const
{
   return ie_.time_threshold;
}

inline TimeThresholdIE &TimeThresholdIE::time_threshold(uint32_t val)
{
   ie_.time_threshold = val;
   setLength();
   return *this;
}

inline pfcp_time_threshold_ie_t &TimeThresholdIE::data()
{
   return ie_;
}

inline uint16_t TimeThresholdIE::calculateLength()
{
   return sizeof(pfcp_time_threshold_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline MonitoringTimeIE::MonitoringTimeIE(pfcp_monitoring_time_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_MONITORING_TIME, parent),
     ie_(ie)
{
}

inline const ETime MonitoringTimeIE::monitoring_time() const
{
   ETime t;
   t.setNTPTime(ie_.monitoring_time);
   return t;
}

inline MonitoringTimeIE &MonitoringTimeIE::monitoring_time(uint32_t val)
{
   ie_.monitoring_time = val;
   setLength();
   return *this;
}

inline MonitoringTimeIE &MonitoringTimeIE::monitoring_time(const ETime &val)
{
   ie_.monitoring_time = val.getNTPTimeSeconds();
   setLength();
   return *this;
}

inline pfcp_monitoring_time_ie_t &MonitoringTimeIE::data()
{
   return ie_;
}

inline uint16_t MonitoringTimeIE::calculateLength()
{
   return sizeof(pfcp_monitoring_time_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline SubsequentVolumeThresholdIE::SubsequentVolumeThresholdIE(pfcp_sbsqnt_vol_thresh_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SBSQNT_VOL_THRESH, parent),
     ie_(ie)
{
}

inline Bool SubsequentVolumeThresholdIE::tovol() const
{
   return ie_.tovol;
}

inline Bool SubsequentVolumeThresholdIE::ulvol() const
{
   return ie_.ulvol;
}

inline Bool SubsequentVolumeThresholdIE::dlvol() const
{
   return ie_.dlvol;
}

inline uint64_t SubsequentVolumeThresholdIE::total_volume() const
{
   return ie_.total_volume;
}

inline uint64_t SubsequentVolumeThresholdIE::uplink_volume() const
{
   return ie_.uplink_volume;
}

inline uint64_t SubsequentVolumeThresholdIE::downlink_volume() const
{
   return ie_.downlink_volume;
}

inline SubsequentVolumeThresholdIE &SubsequentVolumeThresholdIE::total_volume(uint64_t val)
{
   ie_.tovol = 1;
   ie_.total_volume = val;
   setLength();
   return *this;
}

inline SubsequentVolumeThresholdIE &SubsequentVolumeThresholdIE::uplink_volume(uint64_t val)
{
   ie_.ulvol = 1;
   ie_.uplink_volume = val;
   setLength();
   return *this;
}

inline SubsequentVolumeThresholdIE &SubsequentVolumeThresholdIE::downlink_volume(uint64_t val)
{
   ie_.dlvol = 1;
   ie_.downlink_volume = val;
   setLength();
   return *this;
}
   
inline pfcp_sbsqnt_vol_thresh_ie_t &SubsequentVolumeThresholdIE::data()
{
   return ie_;
}

inline uint16_t SubsequentVolumeThresholdIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.tovol ? sizeof(ie_.total_volume) : 0) +
      (ie_.ulvol ? sizeof(ie_.uplink_volume) : 0) +
      (ie_.dlvol ? sizeof(ie_.downlink_volume) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline SubsequentTimeThresholdIE::SubsequentTimeThresholdIE(pfcp_sbsqnt_time_thresh_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SBSQNT_TIME_THRESH, parent),
     ie_(ie)
{
}

inline uint32_t SubsequentTimeThresholdIE::sbsqnt_time_thresh() const
{
   return ie_.sbsqnt_time_thresh;
}

inline SubsequentTimeThresholdIE &SubsequentTimeThresholdIE::sbsqnt_time_thresh(uint32_t val)
{
   ie_.sbsqnt_time_thresh = val;
   setLength();
   return *this;
}

inline pfcp_sbsqnt_time_thresh_ie_t &SubsequentTimeThresholdIE::data()
{
   return ie_;
}

inline uint16_t SubsequentTimeThresholdIE::calculateLength()
{
   return sizeof(pfcp_sbsqnt_time_thresh_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t InactivityDetectionTimeIE::inact_det_time() const
{
   return ie_.inact_det_time;
}

inline InactivityDetectionTimeIE &InactivityDetectionTimeIE::inact_det_time(uint32_t val)
{
   ie_.inact_det_time = val;
   setLength();
   return *this;
}

inline pfcp_inact_det_time_ie_t &InactivityDetectionTimeIE::data()
{
   return ie_;
}

inline InactivityDetectionTimeIE::InactivityDetectionTimeIE(pfcp_inact_det_time_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_INACT_DET_TIME, parent),
     ie_(ie)
{
}

inline uint16_t InactivityDetectionTimeIE::calculateLength()
{
   return sizeof(pfcp_inact_det_time_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline ReportingTriggersIE::ReportingTriggersIE(pfcp_rptng_triggers_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_RPTNG_TRIGGERS, parent),
     ie_(ie)
{
}

inline Bool ReportingTriggersIE::perio() const
{
   return ie_.perio;
}

inline Bool ReportingTriggersIE::volth() const
{
   return ie_.volth;
}

inline Bool ReportingTriggersIE::timth() const
{
   return ie_.timth;
}

inline Bool ReportingTriggersIE::quhti() const
{
   return ie_.quhti;
}

inline Bool ReportingTriggersIE::start() const
{
   return ie_.start;
}

inline Bool ReportingTriggersIE::stopt() const
{
   return ie_.stopt;
}

inline Bool ReportingTriggersIE::droth() const
{
   return ie_.droth;
}

inline Bool ReportingTriggersIE::liusa() const
{
   return ie_.liusa;
}

inline Bool ReportingTriggersIE::volqu() const
{
   return ie_.volqu;
}

inline Bool ReportingTriggersIE::timqu() const
{
   return ie_.timqu;
}

inline Bool ReportingTriggersIE::envcl() const
{
   return ie_.envcl;
}

inline Bool ReportingTriggersIE::macar() const
{
   return ie_.macar;
}

inline Bool ReportingTriggersIE::eveth() const
{
   return ie_.eveth;
}

inline Bool ReportingTriggersIE::evequ() const
{
   return ie_.evequ;
}

inline ReportingTriggersIE &ReportingTriggersIE::setPerio(Bool val)
{
   ie_.perio = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setVolth(Bool val)
{
   ie_.volth = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setTimth(Bool val)
{
   ie_.timth = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setQuhti(Bool val)
{
   ie_.quhti = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setStart(Bool val)
{
   ie_.start = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setStopt(Bool val)
{
   ie_.stopt = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setDroth(Bool val)
{
   ie_.droth = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setLiusa(Bool val)
{
   ie_.liusa = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setVolqu(Bool val)
{
   ie_.volqu = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setTimqu(Bool val)
{
   ie_.timqu = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setEnvcl(Bool val)
{
   ie_.envcl = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setMacar(Bool val)
{
   ie_.macar = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setEveth(Bool val)
{
   ie_.eveth = val ? 1 : 0;
   setLength();
   return *this;
}

inline ReportingTriggersIE &ReportingTriggersIE::setEvequ(Bool val)
{
   ie_.evequ = val ? 1 : 0;
   setLength();
   return *this;
}

inline pfcp_rptng_triggers_ie_t &ReportingTriggersIE::data()
{
   return ie_;
}

inline uint16_t ReportingTriggersIE::calculateLength()
{
   return sizeof(pfcp_rptng_triggers_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline RedirectInformationIE::RedirectInformationIE(pfcp_redir_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_REDIR_INFO, parent),
      ie_(ie)
{
}

inline RedirectAddressTypeEnum RedirectInformationIE::redir_addr_type() const
{
   return static_cast<RedirectAddressTypeEnum>(ie_.redir_addr_type);
}

inline uint16_t RedirectInformationIE::redir_svr_addr_len() const
{
   return ie_.redir_svr_addr_len;
}

inline const uint8_t *RedirectInformationIE::redir_svr_addr() const
{
   return ie_.redir_svr_addr;
}

inline uint16_t RedirectInformationIE::other_redir_svr_addr_len() const
{
   return ie_.other_redir_svr_addr_len;
}

inline const uint8_t *RedirectInformationIE::other_redir_svr_addr() const
{
   return ie_.other_redir_svr_addr;
}

inline RedirectInformationIE &RedirectInformationIE::redir_addr_type(RedirectAddressTypeEnum val)
{
   ie_.redir_addr_type = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline RedirectInformationIE &RedirectInformationIE::redir_svr_addr(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.redir_svr_addr))
      len = sizeof(ie_.redir_svr_addr);
   ie_.redir_svr_addr_len = len;
   std::memcpy(ie_.redir_svr_addr, val, len);
   setLength();
   return *this;
}

inline RedirectInformationIE &RedirectInformationIE::other_redir_svr_addr(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.other_redir_svr_addr))
      len = sizeof(ie_.other_redir_svr_addr);
   ie_.other_redir_svr_addr_len = len;
   std::memcpy(ie_.other_redir_svr_addr, val, len);
   setLength();
   return *this;
}

inline pfcp_redir_info_ie_t &RedirectInformationIE::data()
{
   return ie_;
}

inline uint16_t RedirectInformationIE::calculateLength()
{
   return 
      1 + /* octet 5 */
      sizeof(ie_.redir_svr_addr_len) +
      ie_.redir_svr_addr_len +
      sizeof(ie_.other_redir_svr_addr_len) +
      ie_.other_redir_svr_addr_len
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline ReportTypeIE::ReportTypeIE(pfcp_report_type_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_REPORT_TYPE, parent),
     ie_(ie)
{
}

inline Bool ReportTypeIE::dldr() const
{
   return ie_.dldr;
}

inline Bool ReportTypeIE::usar() const
{
   return ie_.usar;
}

inline Bool ReportTypeIE::erir() const
{
   return ie_.erir;
}

inline Bool ReportTypeIE::upir() const
{
   return ie_.upir;
}

inline ReportTypeIE &ReportTypeIE::dldr(Bool val)
{
   ie_.dldr = val ? 1 : 0; setLength();
   return *this;
}

inline ReportTypeIE &ReportTypeIE::usar(Bool val)
{
   ie_.usar = val ? 1 : 0; setLength();
   return *this;
}

inline ReportTypeIE &ReportTypeIE::erir(Bool val)
{
   ie_.erir = val ? 1 : 0; setLength();
   return *this;
}

inline ReportTypeIE &ReportTypeIE::upir(Bool val)
{
   ie_.upir = val ? 1 : 0; setLength();
   return *this;
}

inline pfcp_report_type_ie_t &ReportTypeIE::data()
{
   return ie_;
}

inline uint16_t ReportTypeIE::calculateLength()
{
   return sizeof(pfcp_report_type_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint16_t OffendingIeIE::type_of_the_offending_ie() const
{
   return ie_.type_of_the_offending_ie;
}

inline OffendingIeIE &OffendingIeIE::type_of_the_offending_ie(uint16_t val)
{
   ie_.type_of_the_offending_ie = val;
   setLength();
   return *this;
}
 
inline pfcp_offending_ie_ie_t &OffendingIeIE::data()
{
   return ie_;
}

inline OffendingIeIE::OffendingIeIE(pfcp_offending_ie_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_OFFENDING_IE, parent),
     ie_(ie)
{
}

inline uint16_t OffendingIeIE::calculateLength()
{
   return sizeof(pfcp_offending_ie_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint8_t ForwardingPolicyIE::frwdng_plcy_ident_len() const
{
   return ie_.frwdng_plcy_ident_len;
}

inline const uint8_t *ForwardingPolicyIE::frwdng_plcy_ident() const
{
   return ie_.frwdng_plcy_ident;
}

inline ForwardingPolicyIE &ForwardingPolicyIE::frwdng_plcy_ident(const uint8_t *val, uint8_t len)
{
   if (len > sizeof(ie_.frwdng_plcy_ident))
      len = sizeof(ie_.frwdng_plcy_ident);
   ie_.frwdng_plcy_ident_len = len;
   std::memcpy(ie_.frwdng_plcy_ident, val, len);
   setLength();
   return *this;
}

inline pfcp_frwdng_plcy_ie_t &ForwardingPolicyIE::data()
{
   return ie_;
}

inline ForwardingPolicyIE::ForwardingPolicyIE(pfcp_frwdng_plcy_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FRWDNG_PLCY, parent),
     ie_(ie)
{
}

inline uint16_t ForwardingPolicyIE::calculateLength()
{
   return sizeof(ie_.frwdng_plcy_ident_len) + ie_.frwdng_plcy_ident_len;
}

////////////////////////////////////////////////////////////////////////////////

inline DestinationInterfaceIE::DestinationInterfaceIE(pfcp_dst_intfc_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DST_INTFC, parent),
      ie_(ie)
{
}

inline DestinationInterfaceEnum DestinationInterfaceIE::interface_value() const
{
   return static_cast<DestinationInterfaceEnum>(ie_.interface_value);
}

inline DestinationInterfaceIE &DestinationInterfaceIE::interface_value(DestinationInterfaceEnum val)
{
   ie_.interface_value = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline pfcp_dst_intfc_ie_t &DestinationInterfaceIE::data()
{
   return ie_;
}

inline uint16_t DestinationInterfaceIE::calculateLength()
{
   return sizeof(pfcp_dst_intfc_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline UpFunctionFeaturesIE::UpFunctionFeaturesIE(pfcp_up_func_feat_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_UP_FUNC_FEAT, parent),
     ie_(ie)
{
}

inline Bool UpFunctionFeaturesIE::bucp() const
{
   return ie_.sup_feat.bucp;
}

inline Bool UpFunctionFeaturesIE::ddnd() const
{
   return ie_.sup_feat.ddnd;
}

inline Bool UpFunctionFeaturesIE::dlbd() const
{
   return ie_.sup_feat.dlbd;
}

inline Bool UpFunctionFeaturesIE::trst() const
{
   return ie_.sup_feat.trst;
}

inline Bool UpFunctionFeaturesIE::ftup() const
{
   return ie_.sup_feat.ftup;
}

inline Bool UpFunctionFeaturesIE::pfdm() const
{
   return ie_.sup_feat.pfdm;
}

inline Bool UpFunctionFeaturesIE::heeu() const
{
   return ie_.sup_feat.heeu;
}

inline Bool UpFunctionFeaturesIE::treu() const
{
   return ie_.sup_feat.treu;
}

inline Bool UpFunctionFeaturesIE::empu() const
{
   return ie_.sup_feat.empu;
}

inline Bool UpFunctionFeaturesIE::pdiu() const
{
   return ie_.sup_feat.pdiu;
}

inline Bool UpFunctionFeaturesIE::udbc() const
{
   return ie_.sup_feat.udbc;
}

inline Bool UpFunctionFeaturesIE::quoac() const
{
   return ie_.sup_feat.quoac;
}

inline Bool UpFunctionFeaturesIE::trace() const
{
   return ie_.sup_feat.trace;
}

inline Bool UpFunctionFeaturesIE::frrt() const
{
   return ie_.sup_feat.frrt;
}

inline Bool UpFunctionFeaturesIE::pfde() const
{
   return ie_.sup_feat.pfde;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::bucp(Bool val)
{
   ie_.sup_feat.bucp = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::ddnd(Bool val)
{
   ie_.sup_feat.ddnd = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::dlbd(Bool val)
{
   ie_.sup_feat.dlbd = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::trst(Bool val)
{
   ie_.sup_feat.trst = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::ftup(Bool val)
{
   ie_.sup_feat.ftup = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::pfdm(Bool val)
{
   ie_.sup_feat.pfdm = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::heeu(Bool val)
{
   ie_.sup_feat.heeu = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::treu(Bool val)
{
   ie_.sup_feat.treu = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::empu(Bool val)
{
   ie_.sup_feat.empu = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::pdiu(Bool val)
{
   ie_.sup_feat.pdiu = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::udbc(Bool val)
{
   ie_.sup_feat.udbc = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::quoac(Bool val)
{
   ie_.sup_feat.quoac = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::trace(Bool val)
{
   ie_.sup_feat.trace = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::frrt(Bool val)
{
   ie_.sup_feat.frrt = val ? 1 : 0;
   setLength();
   return *this;
}

inline UpFunctionFeaturesIE &UpFunctionFeaturesIE::pfde(Bool val)
{
   ie_.sup_feat.pfde = val ? 1 : 0;
   setLength();
   return *this;
}

inline pfcp_up_func_feat_ie_t &UpFunctionFeaturesIE::data()
{
   return ie_;
}

inline uint16_t UpFunctionFeaturesIE::calculateLength()
{
   return sizeof(pfcp_up_func_feat_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline ApplyActionIE::ApplyActionIE(pfcp_apply_action_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_APPLY_ACTION, parent),
     ie_(ie)
{
}

inline Bool ApplyActionIE::drop() const
{
   return ie_.drop;
}

inline Bool ApplyActionIE::forw() const
{
   return ie_.forw;
}

inline Bool ApplyActionIE::buff() const
{
   return ie_.buff;
}

inline Bool ApplyActionIE::nocp() const
{
   return ie_.nocp;
}

inline Bool ApplyActionIE::dupl() const
{
   return ie_.dupl;
}

inline ApplyActionIE &ApplyActionIE::drop(Bool val)
{
   ie_.drop = val ? 1 : 0;
   setLength();
   return *this;
}

inline ApplyActionIE &ApplyActionIE::forw(Bool val)
{
   ie_.forw = val ? 1 : 0;
   setLength();
   return *this;
}

inline ApplyActionIE &ApplyActionIE::buff(Bool val)
{
   ie_.buff = val ? 1 : 0;
   setLength();
   return *this;
}

inline ApplyActionIE &ApplyActionIE::nocp(Bool val)
{
   ie_.nocp = val ? 1 : 0;
   setLength();
   return *this;
}

inline ApplyActionIE &ApplyActionIE::dupl(Bool val)
{
   ie_.dupl = val ? 1 : 0;
   setLength();
   return *this;
}

inline pfcp_apply_action_ie_t &ApplyActionIE::data()
{
   return ie_;
}

inline uint16_t ApplyActionIE::calculateLength()
{
   return sizeof(pfcp_apply_action_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline DownlinkDataServiceInformationIE::DownlinkDataServiceInformationIE(pfcp_dnlnk_data_svc_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DNLNK_DATA_SVC_INFO, parent),
     ie_(ie)
{
}

inline Bool DownlinkDataServiceInformationIE::ppi() const
{
   return ie_.ppi;
}

inline Bool DownlinkDataServiceInformationIE::qfii() const
{
   return ie_.qfii;
}

inline uint8_t DownlinkDataServiceInformationIE::paging_plcy_indctn_val() const
{
   return ie_.paging_plcy_indctn_val;
}

inline uint8_t DownlinkDataServiceInformationIE::qfi() const
{
   return ie_.qfi;
}

inline DownlinkDataServiceInformationIE &DownlinkDataServiceInformationIE::paging_plcy_indctn_val(Bool val)
{
   ie_.paging_plcy_indctn_val = val; ie_.ppi = 1;
   setLength();
   return *this;
}

inline DownlinkDataServiceInformationIE &DownlinkDataServiceInformationIE::qfi(Bool val)
{
   ie_.qfi = val; ie_.qfii = 1;
   setLength();
   return *this;
}

inline pfcp_dnlnk_data_svc_info_ie_t &DownlinkDataServiceInformationIE::data()
{
   return ie_;
}

inline uint16_t DownlinkDataServiceInformationIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.ppi ? 1 : 0) +
      (ie_.qfii ? 1 : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline DownlinkDataNotificationDelayIE::DownlinkDataNotificationDelayIE(pfcp_dnlnk_data_notif_delay_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DNLNK_DATA_NOTIF_DELAY, parent),
     ie_(ie)
{
}

inline uint8_t DownlinkDataNotificationDelayIE::delay_value() const
{
   return ie_.delay_val_in_integer_multiples_of_50_millisecs_or_zero;
}

inline DownlinkDataNotificationDelayIE &DownlinkDataNotificationDelayIE::delay_value(uint8_t val)
{
   ie_.delay_val_in_integer_multiples_of_50_millisecs_or_zero = val;
   setLength();
   return *this;
}
   
inline pfcp_dnlnk_data_notif_delay_ie_t &DownlinkDataNotificationDelayIE::data()
{
   return ie_;
}

inline uint16_t DownlinkDataNotificationDelayIE::calculateLength()
{
   return sizeof(pfcp_dnlnk_data_notif_delay_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline DlBufferingDurationIE::DlBufferingDurationIE(pfcp_dl_buf_dur_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DL_BUF_DUR, parent),
     ie_(ie)
{
}

inline DlBufferingDurationTimerUnitEnum DlBufferingDurationIE::timer_unit() const
{
   return (ie_.timer_unit == 5 || ie_.timer_unit == 6) ?
      DlBufferingDurationTimerUnitEnum::one_minute : 
      static_cast<DlBufferingDurationTimerUnitEnum>(ie_.timer_unit);
}

inline uint8_t DlBufferingDurationIE::timer_value() const
{
   return ie_.timer_value;
}

inline DlBufferingDurationIE &DlBufferingDurationIE::timer_unit(DlBufferingDurationTimerUnitEnum val)
{
   ie_.timer_unit = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline DlBufferingDurationIE &DlBufferingDurationIE::timer_unit(uint8_t val)
{
   ie_.timer_value = val;
   setLength();
   return *this;
}

inline pfcp_dl_buf_dur_ie_t &DlBufferingDurationIE::data()
{
   return ie_;
}

inline uint16_t DlBufferingDurationIE::calculateLength()
{
   return sizeof(pfcp_dl_buf_dur_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline DlBufferingSuggestedPacketCountIE::DlBufferingSuggestedPacketCountIE(pfcp_dl_buf_suggstd_pckt_cnt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DL_BUF_SUGGSTD_PCKT_CNT, parent),
     ie_(ie)
{
}

inline uint64_t DlBufferingSuggestedPacketCountIE::pckt_cnt_val() const
{
   return ie_.pckt_cnt_val;
}

inline DlBufferingSuggestedPacketCountIE &DlBufferingSuggestedPacketCountIE::pckt_cnt_val(uint16_t val)
{
   ie_.pckt_cnt_val = val;
   setLength();
   return *this;
}

inline pfcp_dl_buf_suggstd_pckt_cnt_ie_t &DlBufferingSuggestedPacketCountIE::data()
{
   return ie_;
}

inline uint16_t DlBufferingSuggestedPacketCountIE::calculateLength()
{
   return ie_.pckt_cnt_val < 256 ? 1 : 2;
}

////////////////////////////////////////////////////////////////////////////////

inline PfcpSmReqFlagsIE::PfcpSmReqFlagsIE(pfcp_pfcpsmreq_flags_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_PFCPSMREQ_FLAGS, parent),
     ie_(ie)
{
}

inline Bool PfcpSmReqFlagsIE::drobu() const
{
   return ie_.drobu;
}

inline Bool PfcpSmReqFlagsIE::sndem() const
{
   return ie_.sndem;
}

inline Bool PfcpSmReqFlagsIE::qaurr() const
{
   return ie_.qaurr;
}

inline PfcpSmReqFlagsIE &PfcpSmReqFlagsIE::drobu(Bool val)
{
   ie_.drobu = val ? 1 : 0;
   setLength();
   return *this;
}

inline PfcpSmReqFlagsIE &PfcpSmReqFlagsIE::sndem(Bool val)
{
   ie_.sndem = val ? 1 : 0;
   setLength();
   return *this;
}

inline PfcpSmReqFlagsIE &PfcpSmReqFlagsIE::qaurr(Bool val)
{
   ie_.qaurr = val ? 1 : 0;
   setLength();
   return *this;
}
   
inline pfcp_pfcpsmreq_flags_ie_t &PfcpSmReqFlagsIE::data()
{
   return ie_;
}

inline uint16_t PfcpSmReqFlagsIE::calculateLength()
{
   return sizeof(pfcp_pfcpsmreq_flags_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline PfcpSrRspFlagsIE::PfcpSrRspFlagsIE(pfcp_pfcpsrrsp_flags_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_PFCPSRRSP_FLAGS, parent),
     ie_(ie)
{
}

inline Bool PfcpSrRspFlagsIE::drobu() const
{
   return ie_.drobu;
}

inline PfcpSrRspFlagsIE &PfcpSrRspFlagsIE::drobu(Bool val)
{
   ie_.drobu = val ? 1 : 0;
   setLength();
   return *this;
}
   
inline pfcp_pfcpsrrsp_flags_ie_t &PfcpSrRspFlagsIE::data()
{
   return ie_;
}

inline uint16_t PfcpSrRspFlagsIE::calculateLength()
{
   return sizeof(pfcp_pfcpsrrsp_flags_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline SequenceNumberIE::SequenceNumberIE(pfcp_sequence_number_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SEQUENCE_NUMBER, parent),
     ie_(ie)
{
}

inline uint32_t SequenceNumberIE::sequence_number() const
{
   return ie_.sequence_number;
}

inline SequenceNumberIE &SequenceNumberIE::sequence_number(uint32_t val)
{
   ie_.sequence_number = val;
   setLength();
   return *this;
}
   
inline pfcp_sequence_number_ie_t &SequenceNumberIE::data()
{
   return ie_;
}

inline uint16_t SequenceNumberIE::calculateLength()
{
   return sizeof(pfcp_sequence_number_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline MetricIE::MetricIE(pfcp_metric_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_METRIC, parent),
     ie_(ie)
{
}

inline uint8_t MetricIE::metric() const
{
   return ie_.metric > 100 ? 0 : ie_.metric;
}

inline MetricIE &MetricIE::metric(uint8_t val)
{
   ie_.metric = val > 100 ? 0 : val;
   setLength();
   return *this;
}
   
inline pfcp_metric_ie_t &MetricIE::data()
{
   return ie_;
}

inline uint16_t MetricIE::calculateLength()
{
   return sizeof(pfcp_metric_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline TimerTimerUnitEnum TimerIE::timer_unit() const
{
   return (ie_.timer_unit == 5 || ie_.timer_unit == 6) ?
      TimerTimerUnitEnum::one_minute : 
      static_cast<TimerTimerUnitEnum>(ie_.timer_unit);
}

inline uint8_t TimerIE::timer_value() const
{
   return ie_.timer_value;
}

inline TimerIE &TimerIE::timer_unit(TimerTimerUnitEnum val)
{
   ie_.timer_unit = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline TimerIE &TimerIE::timer_unit(uint8_t val)
{
   ie_.timer_value = val;
   setLength();
   return *this;
}

inline pfcp_timer_ie_t &TimerIE::data()
{
   return ie_;
}

inline TimerIE::TimerIE(pfcp_timer_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DL_BUF_DUR, parent),
     ie_(ie)
{
}

inline uint16_t TimerIE::calculateLength()
{
   return sizeof(pfcp_timer_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint16_t PdrIdIE::rule_id() const
{
   return ie_.rule_id;
}

inline PdrIdIE &PdrIdIE::rule_id(uint16_t val)
{
   ie_.rule_id = val;
   setLength();
   return *this;
}
   
inline pfcp_pdr_id_ie_t &PdrIdIE::data()
{
   return ie_;
}

inline PdrIdIE::PdrIdIE(pfcp_pdr_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_PDR_ID, parent),
     ie_(ie)
{
}

inline uint16_t PdrIdIE::calculateLength()
{
   return sizeof(pfcp_pdr_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool FSeidIE::v4() const
{
   return ie_.v4;
}

inline Bool FSeidIE::v6() const
{
   return ie_.v6;
}

inline uint64_t FSeidIE::seid() const
{
   return ie_.seid;
}

inline const in_addr &FSeidIE::ipv4_address() const
{
   AliasPointer u;
   u.uint32_t_ptr = &ie_.ipv4_address;
   return *u.in_addr_ptr;
}

inline const in6_addr &FSeidIE::ipv6_address() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.ipv6_address;
   return *u.in6_addr_ptr;
}

inline FSeidIE &FSeidIE::seid(uint64_t val)
{
   ie_.seid = val;
   setLength();
   return *this;
}

inline FSeidIE &FSeidIE::ip_address(const ESocket::Address &val)
{
   if (val.getFamily() == ESocket::Family::INET)
      return ip_address(val.getInet().sin_addr);
   else if (val.getFamily() == ESocket::Family::INET6)
      return ip_address(val.getInet6().sin6_addr);
   return *this;
}

inline FSeidIE &FSeidIE::ip_address(const EIpAddress &val)
{
   if (val.family() == AF_INET)
      return ip_address(val.ipv4Address());
   else if (val.family() == AF_INET6)
      return ip_address(val.ipv6Address());
   return *this;
}

inline FSeidIE &FSeidIE::ip_address(const in_addr &val)
{
   ie_.v4 = 1;
   ie_.ipv4_address = val.s_addr;
   setLength();
   return *this;
}

inline FSeidIE &FSeidIE::ip_address(const in6_addr &val)
{
   ie_.v6 = 1;
   std::memcpy(ie_.ipv6_address, val.s6_addr, sizeof(ie_.ipv6_address));
   setLength();
   return *this;
}
   
inline pfcp_fseid_ie_t &FSeidIE::data()
{
   return ie_;
}

inline FSeidIE::FSeidIE(pfcp_fseid_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FSEID, parent),
     ie_(ie)
{
}
inline uint16_t FSeidIE::calculateLength()
{
   return
      1 + /* octet 5 */
      sizeof(ie_.seid) +
      (ie_.v4 ? sizeof(ie_.ipv4_address) : 0) +
      (ie_.v6 ? sizeof(ie_.ipv6_address) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline NodeIdTypeEnum NodeIdIE::node_id_type() const
{
   return static_cast<NodeIdTypeEnum>(ie_.node_id_type);
}

inline const uint8_t *NodeIdIE::node_id_value() const
{
   return ie_.node_id_value;
}

inline NodeIdIE &NodeIdIE::node_id_value(const ESocket::Address &val)
{
   if (val.getFamily() == ESocket::Family::INET)
      return node_id_value(val.getInet().sin_addr);
   else if (val.getFamily() == ESocket::Family::INET6)
      return node_id_value(val.getInet6().sin6_addr);
   return *this;
}

inline NodeIdIE &NodeIdIE::node_id_value(const EIpAddress &val)
{
   if (val.family() == AF_INET)
      return node_id_value(val.ipv4Address());
   else if (val.family() == AF_INET6)
      return node_id_value(val.ipv6Address());
   return *this;
}

inline NodeIdIE &NodeIdIE::node_id_value(const in_addr &val)
{
   ie_.node_id_type = static_cast<uint8_t>(NodeIdTypeEnum::ipv4_address);
   std::memcpy(ie_.node_id_value, &val, sizeof(val));
   setLength();
   return *this;
}

inline NodeIdIE &NodeIdIE::node_id_value(const in6_addr &val)
{
   ie_.node_id_type = static_cast<uint8_t>(NodeIdTypeEnum::ipv6_address);
   std::memcpy(ie_.node_id_value, val.s6_addr, sizeof(val));
   setLength();
   return *this;
}

inline NodeIdIE &NodeIdIE::node_id_value(const uint8_t *val, uint8_t len, NodeIdTypeEnum type)
{
   if (len > sizeof(ie_.node_id_value))
      len = sizeof(ie_.node_id_value);
   ie_.node_id_type = static_cast<uint8_t>(type);
   std::memcpy(ie_.node_id_value, val, len);
   ie_.header.len = 1 + len;
   return *this;
}
   
inline pfcp_node_id_ie_t &NodeIdIE::data()
{
   return ie_;
}

inline NodeIdIE::NodeIdIE(pfcp_node_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_NODE_ID, parent),
     ie_(ie)
{
}

inline uint16_t NodeIdIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (
         ie_.node_id_type == 0 ? sizeof(in_addr) :
         ie_.node_id_type == 1 ? sizeof(in6_addr) :
            ie_.header.len - 1
      );
}

////////////////////////////////////////////////////////////////////////////////

#define REPLACE_DATA(flag,srcofs,len_of,dest,destofs,destsize,value,value_length)   \
{                                                                                   \
   if (destofs + value_length > destsize)                                           \
      return *this;                                                                 \
   srcofs += flag ? len_of : 0;                                                     \
   flag = 1;                                                                        \
   len_of = value_length;                                                           \
   std::memcpy(&dest[destofs], value, value_length);                                \
   destofs += value_length;                                                         \
}

#define ADD_DATA(flag,src,srcofs,len_of,dest,destofs,destsize,value,value_length) \
{                                                                                 \
   AliasPointer u;                                                                \
   if (destofs + value_length + sizeof(uint16_t) > destsize)                      \
      return *this;                                                               \
   if (flag) {                                                                    \
      std::memcpy(&dest[destofs], &src[srcofs], len_of);                          \
      srcofs += len_of;                                                           \
      destofs += len_of;                                                          \
   } else {                                                                       \
      len_of = 0;                                                                 \
      flag = 1;                                                                   \
   }                                                                              \
   len_of += value_length + sizeof(uint16_t);                                     \
   u.uint8_t_ptr = &dest[destofs];                                                \
   *u.uint16_t_ptr = htons(value_length);                                         \
   destofs += sizeof(uint16_t);                                                   \
   std::memcpy(&dest[destofs], value, value_length);                              \
   destofs += value_length;                                                       \
}

inline Bool PfdContentsIE::adnp() const
{
   return ie_.adnp;
}

inline Bool PfdContentsIE::aurl() const
{
   return ie_.aurl;
}

inline Bool PfdContentsIE::afd() const
{
   return ie_.afd;
}

inline Bool PfdContentsIE::dnp() const
{
   return ie_.dnp;
}

inline Bool PfdContentsIE::cp() const
{
   return ie_.cp;
}

inline Bool PfdContentsIE::dn() const
{
   return ie_.dn;
}

inline Bool PfdContentsIE::url() const
{
   return ie_.url;
}

inline Bool PfdContentsIE::fd() const
{
   return ie_.fd;
}

inline uint16_t PfdContentsIE::len_of_flow_desc() const
{
   return ie_.len_of_flow_desc;
}

inline const uint8_t *PfdContentsIE::flow_desc() const
{
   return ie_.flow_desc;
}

inline uint16_t PfdContentsIE::length_of_url() const
{
   return ie_.length_of_url;
}

inline const uint8_t *PfdContentsIE::url2() const
{
   return ie_.url2;
}

inline uint16_t PfdContentsIE::len_of_domain_nm() const
{
   return ie_.len_of_domain_nm;
}

inline const uint8_t *PfdContentsIE::domain_name() const
{
   return ie_.domain_name;
}

inline uint16_t PfdContentsIE::len_of_cstm_pfd_cntnt() const
{
   return ie_.len_of_cstm_pfd_cntnt;
}

inline const uint8_t *PfdContentsIE::cstm_pfd_cntnt() const
{
   return ie_.cstm_pfd_cntnt;
}

inline uint16_t PfdContentsIE::len_of_domain_nm_prot() const
{
   return ie_.len_of_domain_nm_prot;
}

inline const uint8_t *PfdContentsIE::domain_name_prot() const
{
   return ie_.domain_name_prot;
}

inline uint16_t PfdContentsIE::len_of_add_flow_desc() const
{
   return ie_.len_of_add_flow_desc;
}

inline const uint8_t *PfdContentsIE::add_flow_desc() const
{
   return ie_.add_flow_desc;
}

inline uint16_t PfdContentsIE::len_of_add_url() const
{
   return ie_.len_of_add_url;
}

inline const uint8_t *PfdContentsIE::add_url() const
{
   return ie_.add_url;
}

inline uint16_t PfdContentsIE::len_of_add_domain_nm_prot() const
{
   return ie_.len_of_add_domain_nm_prot;
}

inline const uint8_t *PfdContentsIE::add_domain_nm_prot() const
{
   return ie_.add_domain_nm_prot;
}

inline PfdContentsIE &PfdContentsIE::flow_desc(const uint8_t *val, uint16_t len)
{
   uint16_t tmpofs = 0;
   uint16_t dataofs = 0;
   uint8_t tmp[sizeof(ie_.data)];

   REPLACE_DATA(ie_.fd, dataofs, ie_.len_of_flow_desc, tmp, tmpofs, sizeof(ie_.data), val, len);
   
   if (!move(ie_.url,  tmp, tmpofs, ie_.data, dataofs, ie_.length_of_url,             sizeof(ie_.data)) ||
         !move(ie_.dn,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm,          sizeof(ie_.data)) ||
         !move(ie_.cp,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_cstm_pfd_cntnt,     sizeof(ie_.data)) ||
         !move(ie_.dnp,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm_prot,     sizeof(ie_.data)) ||
         !move(ie_.afd,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_flow_desc,      sizeof(ie_.data)) ||
         !move(ie_.aurl, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_url,            sizeof(ie_.data)) ||
         !move(ie_.adnp, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_domain_nm_prot, sizeof(ie_.data)))
      return *this;

   std::memcpy(ie_.data, tmp, tmpofs);

   updatePointers();

   return *this;
}

inline PfdContentsIE &PfdContentsIE::url2(const uint8_t *val, uint16_t len)
{
   uint16_t tmpofs = 0;
   uint16_t dataofs = 0;
   uint8_t tmp[sizeof(ie_.data)];

   if (!move(ie_.fd,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_flow_desc,          sizeof(ie_.data)))
      return *this;

   REPLACE_DATA(ie_.url, dataofs, ie_.length_of_url, tmp, tmpofs, sizeof(ie_.data), val, len);
   
   if (!move(ie_.dn,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm,          sizeof(ie_.data)) ||
         !move(ie_.cp,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_cstm_pfd_cntnt,     sizeof(ie_.data)) ||
         !move(ie_.dnp,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm_prot,     sizeof(ie_.data)) ||
         !move(ie_.afd,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_flow_desc,      sizeof(ie_.data)) ||
         !move(ie_.aurl, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_url,            sizeof(ie_.data)) ||
         !move(ie_.adnp, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_domain_nm_prot, sizeof(ie_.data)))
      return *this;

   std::memcpy(ie_.data, tmp, tmpofs);

   updatePointers();

   return *this;
}

inline PfdContentsIE &PfdContentsIE::domain_name(const uint8_t *val, uint16_t len)
{
   uint16_t tmpofs = 0;
   uint16_t dataofs = 0;
   uint8_t tmp[sizeof(ie_.data)];

   if (!move(ie_.fd,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_flow_desc,          sizeof(ie_.data)) ||
         !move(ie_.url,  tmp, tmpofs, ie_.data, dataofs, ie_.length_of_url,             sizeof(ie_.data)))
      return *this;

   REPLACE_DATA(ie_.dn, dataofs, ie_.len_of_domain_nm, tmp, tmpofs, sizeof(ie_.data), val, len);
   
   if (!move(ie_.cp,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_cstm_pfd_cntnt,     sizeof(ie_.data)) ||
         !move(ie_.dnp,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm_prot,     sizeof(ie_.data)) ||
         !move(ie_.afd,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_flow_desc,      sizeof(ie_.data)) ||
         !move(ie_.aurl, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_url,            sizeof(ie_.data)) ||
         !move(ie_.adnp, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_domain_nm_prot, sizeof(ie_.data)))
      return *this;

   std::memcpy(ie_.data, tmp, tmpofs);

   updatePointers();

   return *this;
}

inline PfdContentsIE &PfdContentsIE::cstm_pfd_cntnt(const uint8_t *val, uint16_t len)
{
   uint16_t tmpofs = 0;
   uint16_t dataofs = 0;
   uint8_t tmp[sizeof(ie_.data)];

   if (!move(ie_.fd,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_flow_desc,          sizeof(ie_.data)) ||
         !move(ie_.url,  tmp, tmpofs, ie_.data, dataofs, ie_.length_of_url,             sizeof(ie_.data)) ||
         !move(ie_.dn,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm,          sizeof(ie_.data)))
      return *this;

   REPLACE_DATA(ie_.cp, dataofs, ie_.len_of_cstm_pfd_cntnt, tmp, tmpofs, sizeof(ie_.data), val, len);
   
   if (!move(ie_.dnp,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm_prot,     sizeof(ie_.data)) ||
         !move(ie_.afd,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_flow_desc,      sizeof(ie_.data)) ||
         !move(ie_.aurl, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_url,            sizeof(ie_.data)) ||
         !move(ie_.adnp, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_domain_nm_prot, sizeof(ie_.data)))
      return *this;

   std::memcpy(ie_.data, tmp, tmpofs);

   updatePointers();

   return *this;
}

inline PfdContentsIE &PfdContentsIE::domain_name_prot(const uint8_t *val, uint16_t len)
{
   uint16_t tmpofs = 0;
   uint16_t dataofs = 0;
   uint8_t tmp[sizeof(ie_.data)];

   if (!move(ie_.fd,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_flow_desc,          sizeof(ie_.data)) ||
         !move(ie_.url,  tmp, tmpofs, ie_.data, dataofs, ie_.length_of_url,             sizeof(ie_.data)) ||
         !move(ie_.dn,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm,          sizeof(ie_.data)) ||
         !move(ie_.cp,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_cstm_pfd_cntnt,     sizeof(ie_.data)))
      return *this;

   REPLACE_DATA(ie_.dnp, dataofs, ie_.len_of_domain_nm_prot, tmp, tmpofs, sizeof(ie_.data), val, len);
   
   if (!move(ie_.afd,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_flow_desc,      sizeof(ie_.data)) ||
         !move(ie_.aurl, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_url,            sizeof(ie_.data)) ||
         !move(ie_.adnp, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_domain_nm_prot, sizeof(ie_.data)))
      return *this;

   std::memcpy(ie_.data, tmp, tmpofs);

   updatePointers();

   return *this;
}

inline PfdContentsIE &PfdContentsIE::add_flow_desc(const uint8_t *val, uint16_t len)
{
   uint16_t tmpofs = 0;
   uint16_t dataofs = 0;
   uint8_t tmp[sizeof(ie_.data)];

   if (!move(ie_.fd,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_flow_desc,          sizeof(ie_.data)) ||
         !move(ie_.url,  tmp, tmpofs, ie_.data, dataofs, ie_.length_of_url,             sizeof(ie_.data)) ||
         !move(ie_.dn,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm,          sizeof(ie_.data)) ||
         !move(ie_.cp,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_cstm_pfd_cntnt,     sizeof(ie_.data)) ||
         !move(ie_.dnp,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm_prot,     sizeof(ie_.data)))
      return *this;

   ADD_DATA(ie_.afd, ie_.data, dataofs, ie_.len_of_add_flow_desc, tmp, tmpofs, sizeof(ie_.data), val, len);
   
   if (!move(ie_.aurl, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_url,            sizeof(ie_.data)) ||
         !move(ie_.adnp, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_domain_nm_prot, sizeof(ie_.data)))
      return *this;

   std::memcpy(ie_.data, tmp, tmpofs);

   updatePointers();

   return *this;
}

inline PfdContentsIE &PfdContentsIE::add_url(const uint8_t *val, uint16_t len)
{
   uint16_t tmpofs = 0;
   uint16_t dataofs = 0;
   uint8_t tmp[sizeof(ie_.data)];

   if (!move(ie_.fd,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_flow_desc,          sizeof(ie_.data)) ||
         !move(ie_.url,  tmp, tmpofs, ie_.data, dataofs, ie_.length_of_url,             sizeof(ie_.data)) ||
         !move(ie_.dn,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm,          sizeof(ie_.data)) ||
         !move(ie_.cp,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_cstm_pfd_cntnt,     sizeof(ie_.data)) ||
         !move(ie_.dnp,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm_prot,     sizeof(ie_.data)) ||
         !move(ie_.afd,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_flow_desc,      sizeof(ie_.data)))
      return *this;

   ADD_DATA(ie_.aurl, ie_.data, dataofs, ie_.len_of_add_url, tmp, tmpofs, sizeof(ie_.data), val, len);
   
   if (!move(ie_.adnp, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_domain_nm_prot, sizeof(ie_.data)))
      return *this;

   std::memcpy(ie_.data, tmp, tmpofs);

   updatePointers();

   return *this;
}

inline PfdContentsIE &PfdContentsIE::add_domain_nm_prot(const uint8_t *val, uint16_t len)
{
   uint16_t tmpofs = 0;
   uint16_t dataofs = 0;
   uint8_t tmp[sizeof(ie_.data)];

   if (!move(ie_.fd,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_flow_desc,          sizeof(ie_.data)) ||
         !move(ie_.url,  tmp, tmpofs, ie_.data, dataofs, ie_.length_of_url,             sizeof(ie_.data)) ||
         !move(ie_.dn,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm,          sizeof(ie_.data)) ||
         !move(ie_.cp,   tmp, tmpofs, ie_.data, dataofs, ie_.len_of_cstm_pfd_cntnt,     sizeof(ie_.data)) ||
         !move(ie_.dnp,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_domain_nm_prot,     sizeof(ie_.data)) ||
         !move(ie_.afd,  tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_flow_desc,      sizeof(ie_.data)) ||
         !move(ie_.aurl, tmp, tmpofs, ie_.data, dataofs, ie_.len_of_add_url,            sizeof(ie_.data)))
      return *this;

   ADD_DATA(ie_.adnp, ie_.data, dataofs, ie_.len_of_add_domain_nm_prot, tmp, tmpofs, sizeof(ie_.data), val, len);

   std::memcpy(ie_.data, tmp, tmpofs);

   updatePointers();

   return *this;
}

inline pfcp_pfd_contents_ie_t &PfdContentsIE::data()
{
   return ie_;
}

inline PfdContentsIE::PfdContentsIE(pfcp_pfd_contents_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_PFD_CONTENTS, parent),
      ie_(ie)
{
}

inline uint16_t PfdContentsIE::calculateLength()
{
   return
      2 + /* octets 5 & 6 */
      (ie_.fd ? ie_.len_of_flow_desc : 0) +
      (ie_.url ? ie_.length_of_url : 0) +
      (ie_.dn ? ie_.len_of_domain_nm : 0) +
      (ie_.cp ? ie_.len_of_cstm_pfd_cntnt : 0) +
      (ie_.dnp ? ie_.len_of_domain_nm_prot : 0) +
      (ie_.afd ? ie_.len_of_add_flow_desc : 0) +
      (ie_.aurl ? ie_.len_of_add_url : 0) +
      (ie_.adnp ? ie_.len_of_add_domain_nm_prot : 0)
      ;
}

inline Bool PfdContentsIE::move(Bool flag, uint8_t *dest, uint16_t &destofs, uint8_t *src, uint16_t &srcofs, uint16_t len, uint16_t maxlen)
{
   if (flag)
   {
      if (destofs + len < maxlen)
         return False;
      std::memcpy(&dest[destofs], &src[srcofs], len);
      destofs += len;
      srcofs += len;
   }
   return True;
}

inline Void PfdContentsIE::updatePointer(Bool flag, uint8_t* &ptr, uint8_t *data, uint16_t &ofs, uint16_t len)
{
   if (flag)
   {
      ptr = &data[ofs];
      ofs += len;
   }
   else
   {
      ptr = nullptr;
   }
}

inline Void PfdContentsIE::updatePointers()
{
   uint16_t dataofs = 0;
   updatePointer(ie_.fd,   ie_.flow_desc,          ie_.data, dataofs, ie_.len_of_flow_desc);
   updatePointer(ie_.url,  ie_.url2,               ie_.data, dataofs, ie_.length_of_url);
   updatePointer(ie_.dn,   ie_.domain_name,        ie_.data, dataofs, ie_.len_of_domain_nm);
   updatePointer(ie_.cp,   ie_.cstm_pfd_cntnt,     ie_.data, dataofs, ie_.len_of_cstm_pfd_cntnt);
   updatePointer(ie_.dnp,  ie_.domain_name_prot,   ie_.data, dataofs, ie_.len_of_domain_nm_prot);
   updatePointer(ie_.afd,  ie_.add_flow_desc,      ie_.data, dataofs, ie_.len_of_add_flow_desc);
   updatePointer(ie_.aurl, ie_.add_url,            ie_.data, dataofs, ie_.len_of_add_url);
   updatePointer(ie_.adnp, ie_.add_domain_nm_prot, ie_.data, dataofs, ie_.len_of_add_domain_nm_prot);
}

#undef REPLACE_DATA
#undef ADD_DATA

////////////////////////////////////////////////////////////////////////////////

inline Bool MeasurementMethodIE::durat() const
{
   return ie_.durat;
}

inline Bool MeasurementMethodIE::volum() const
{
   return ie_.volum;
}

inline Bool MeasurementMethodIE::event() const
{
   return ie_.event;
}

inline MeasurementMethodIE &MeasurementMethodIE::durat(Bool val)
{
   ie_.durat = val ? 1 : 0; setLength();
   return *this;
}

inline MeasurementMethodIE &MeasurementMethodIE::volum(Bool val)
{
   ie_.volum = val ? 1 : 0; setLength();
   return *this;
}

inline MeasurementMethodIE &MeasurementMethodIE::event(Bool val)
{
   ie_.event = val ? 1 : 0; setLength();
   return *this;
}
   
inline pfcp_meas_mthd_ie_t &MeasurementMethodIE::data()
{
   return ie_;
}

inline MeasurementMethodIE::MeasurementMethodIE(pfcp_meas_mthd_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_MEAS_MTHD, parent),
     ie_(ie)
{
}

inline uint16_t MeasurementMethodIE::calculateLength()
{
   return sizeof(pfcp_meas_mthd_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool UsageReportTriggerIE::immer() const
{
   return ie_.immer;
}

inline Bool UsageReportTriggerIE::droth() const
{
   return ie_.droth;
}

inline Bool UsageReportTriggerIE::stopt() const
{
   return ie_.stopt;
}

inline Bool UsageReportTriggerIE::start() const
{
   return ie_.start;
}

inline Bool UsageReportTriggerIE::quhti() const
{
   return ie_.quhti;
}

inline Bool UsageReportTriggerIE::timth() const
{
   return ie_.timth;
}

inline Bool UsageReportTriggerIE::volth() const
{
   return ie_.volth;
}

inline Bool UsageReportTriggerIE::perio() const
{
   return ie_.perio;
}

inline Bool UsageReportTriggerIE::eveth() const
{
   return ie_.eveth;
}

inline Bool UsageReportTriggerIE::macar() const
{
   return ie_.macar;
}

inline Bool UsageReportTriggerIE::envcl() const
{
   return ie_.envcl;
}

inline Bool UsageReportTriggerIE::monit() const
{
   return ie_.monit;
}

inline Bool UsageReportTriggerIE::termr() const
{
   return ie_.termr;
}

inline Bool UsageReportTriggerIE::liusa() const
{
   return ie_.liusa;
}

inline Bool UsageReportTriggerIE::timqu() const
{
   return ie_.timqu;
}

inline Bool UsageReportTriggerIE::volqu() const
{
   return ie_.volqu;
}

inline Bool UsageReportTriggerIE::evequ() const
{
   return ie_.evequ;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::immer(Bool val)
{
   ie_.immer = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::droth(Bool val)
{
   ie_.droth = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::stopt(Bool val)
{
   ie_.stopt = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::start(Bool val)
{
   ie_.start = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::quhti(Bool val)
{
   ie_.quhti = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::timth(Bool val)
{
   ie_.timth = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::volth(Bool val)
{
   ie_.volth = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::perio(Bool val)
{
   ie_.perio = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::eveth(Bool val)
{
   ie_.eveth = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::macar(Bool val)
{
   ie_.macar = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::envcl(Bool val)
{
   ie_.envcl = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::monit(Bool val)
{
   ie_.monit = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::termr(Bool val)
{
   ie_.termr = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::liusa(Bool val)
{
   ie_.liusa = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::timqu(Bool val)
{
   ie_.timqu = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::volqu(Bool val)
{
   ie_.volqu = val; setLength();
   return *this;
}

inline UsageReportTriggerIE &UsageReportTriggerIE::evequ(Bool val)
{
   ie_.evequ = val; setLength();
   return *this;
}

inline pfcp_usage_rpt_trig_ie_t &UsageReportTriggerIE::data()
{
   return ie_;
}

inline UsageReportTriggerIE::UsageReportTriggerIE(pfcp_usage_rpt_trig_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_USAGE_RPT_TRIG, parent),
     ie_(ie)
{
}

inline uint16_t UsageReportTriggerIE::calculateLength()
{
   return sizeof(pfcp_usage_rpt_trig_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t MeasurementPeriodIE::meas_period() const
{
   return ie_.meas_period;
}

inline MeasurementPeriodIE &MeasurementPeriodIE::meas_period(uint32_t val)
{
   ie_.meas_period = val;
   setLength();
   return *this;
}
   
inline pfcp_meas_period_ie_t &MeasurementPeriodIE::data()
{
   return ie_;
}

inline MeasurementPeriodIE::MeasurementPeriodIE(pfcp_meas_period_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_MEAS_PERIOD, parent),
     ie_(ie)
{
}

inline uint16_t MeasurementPeriodIE::calculateLength()
{
   return sizeof(pfcp_meas_period_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline FqCsidNodeIdTypeEnum FqCsidIE::fqcsid_node_id_type() const
{
   return static_cast<FqCsidNodeIdTypeEnum>(ie_.fqcsid_node_id_type);
}

inline uint8_t FqCsidIE::number_of_csids() const
{
   return ie_.number_of_csids;
}

inline const in_addr &FqCsidIE::ipv4_node_address() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.node_address;
   return *u.in_addr_ptr;
}

inline const in6_addr &FqCsidIE::ipv6_node_address() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.node_address;
   return *u.in6_addr_ptr;
}

inline uint32_t FqCsidIE::mcc_mnc_id_node_address() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.node_address;
   return ntohl(*u.uint32_t_ptr);
}

inline uint16_t FqCsidIE::pdn_conn_set_ident(uint8_t idx) const
{
   return ie_.pdn_conn_set_ident[idx];
}

inline FqCsidIE &FqCsidIE::number_of_csids(uint8_t val)
{
   ie_.number_of_csids = val;
   setLength();
   return *this;
}

inline FqCsidIE &FqCsidIE::node_address(const ESocket::Address &val)
{
   if (val.getFamily() == ESocket::Family::INET)
      return node_address(val.getInet().sin_addr);
   else if (val.getFamily() == ESocket::Family::INET6)
      return node_address(val.getInet6().sin6_addr);
   return *this;
}

inline FqCsidIE &FqCsidIE::node_address(const EIpAddress &val)
{
   if (val.family() == AF_INET)
      return node_address(val.ipv4Address());
   else if (val.family() == AF_INET6)
      return node_address(val.ipv6Address());
   return *this;
}

inline FqCsidIE &FqCsidIE::node_address(const in_addr &val)
{
   AliasPointer u;
   ie_.fqcsid_node_id_type = static_cast<uint8_t>(FqCsidNodeIdTypeEnum::ipv4);
   u.uint8_t_ptr = ie_.node_address;
   u.in_addr_ptr->s_addr = val.s_addr;
   setLength();
   return *this;
}

inline FqCsidIE &FqCsidIE::node_address(const in6_addr &val)
{
   ie_.fqcsid_node_id_type = static_cast<uint8_t>(FqCsidNodeIdTypeEnum::ipv6);
   std::memcpy(ie_.node_address, val.s6_addr, sizeof(ie_.node_address));
   setLength();
   return *this;
}

inline FqCsidIE &FqCsidIE::node_address(uint32_t val)
{
   AliasPointer u;
   ie_.fqcsid_node_id_type = static_cast<uint8_t>(FqCsidNodeIdTypeEnum::mcc_mnc_id);
   u.uint8_t_ptr = ie_.node_address;
   *u.uint32_t_ptr = htonl(val);
   setLength();
   return *this;
}
   
inline pfcp_fqcsid_ie_t &FqCsidIE::data()
{
   return ie_;
}

inline FqCsidIE::FqCsidIE(pfcp_fqcsid_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FQCSID, parent),
     ie_(ie)
{
}

inline uint16_t FqCsidIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.fqcsid_node_id_type == 1 ? IPV6_ADDRESS_LEN : sizeof(uint32_t)) +
      (ie_.number_of_csids * sizeof(uint32_t))
      ;
}

////////////////////////////////////////////////////////////////////////////////

inline Bool VolumeMeasurementIE::tovol() const
{
   return ie_.tovol;
}

inline Bool VolumeMeasurementIE::ulvol() const
{
   return ie_.ulvol;
}

inline Bool VolumeMeasurementIE::dlvol() const
{
   return ie_.dlvol;
}

inline uint64_t VolumeMeasurementIE::total_volume() const
{
   return ie_.total_volume;
}

inline uint64_t VolumeMeasurementIE::uplink_volume() const
{
   return ie_.uplink_volume;
}

inline uint64_t VolumeMeasurementIE::downlink_volume() const
{
   return ie_.downlink_volume;
}

inline VolumeMeasurementIE &VolumeMeasurementIE::total_volume(uint64_t val)
{
   ie_.tovol = 1;
   ie_.total_volume = val;
   setLength();
   return *this;
}

inline VolumeMeasurementIE &VolumeMeasurementIE::uplink_volume(uint64_t val)
{
   ie_.ulvol = 1;
   ie_.uplink_volume = val;
   setLength();
   return *this;
}

inline VolumeMeasurementIE &VolumeMeasurementIE::downlink_volume(uint64_t val)
{
   ie_.dlvol = 1;
   ie_.downlink_volume = val;
   setLength();
   return *this;
}

inline pfcp_vol_meas_ie_t &VolumeMeasurementIE::data()
{
   return ie_;
}

inline VolumeMeasurementIE::VolumeMeasurementIE(pfcp_vol_meas_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_VOL_MEAS, parent),
      ie_(ie)
{
}

inline uint16_t VolumeMeasurementIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.tovol ? sizeof(ie_.total_volume) : 0) +
      (ie_.ulvol ? sizeof(ie_.uplink_volume) : 0) +
      (ie_.dlvol ? sizeof(ie_.downlink_volume) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t DurationMeasurementIE::duration_value() const
{
   return ie_.duration_value;
}

inline DurationMeasurementIE &DurationMeasurementIE::duration_value(uint32_t val)
{
   ie_.duration_value = val;
   setLength();
   return *this;
}
   
inline pfcp_dur_meas_ie_t &DurationMeasurementIE::data()
{
   return ie_;
}

inline DurationMeasurementIE::DurationMeasurementIE(pfcp_dur_meas_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DUR_MEAS, parent),
     ie_(ie)
{
}

inline uint16_t DurationMeasurementIE::calculateLength()
{
   return sizeof(pfcp_dur_meas_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline ETime TimeOfFirstPacketIE::time_of_frst_pckt() const
{
   ETime t;
   t.setNTPTime(ie_.time_of_frst_pckt);
   return t;
}

inline TimeOfFirstPacketIE &TimeOfFirstPacketIE::time_of_frst_pckt(ETime &val)
{
   ie_.time_of_frst_pckt = val.getNTPTimeSeconds();
   setLength();
   return *this;
}

inline TimeOfFirstPacketIE &TimeOfFirstPacketIE::time_of_frst_pckt(uint32_t val)
{
   ie_.time_of_frst_pckt = val;
   setLength();
   return *this;
}

inline pfcp_time_of_frst_pckt_ie_t &TimeOfFirstPacketIE::data()
{
   return ie_;
}

inline TimeOfFirstPacketIE::TimeOfFirstPacketIE(pfcp_time_of_frst_pckt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_TIME_OF_FRST_PCKT, parent),
     ie_(ie)
{
}

inline uint16_t TimeOfFirstPacketIE::calculateLength()
{
   return sizeof(pfcp_time_of_frst_pckt_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline ETime TimeOfLastPacketIE::time_of_lst_pckt() const
{
   ETime t;
   t.setNTPTime(ie_.time_of_lst_pckt);
   return t;
}

inline TimeOfLastPacketIE &TimeOfLastPacketIE::time_of_frst_pckt(ETime &val)
{
   ie_.time_of_lst_pckt = val.getNTPTimeSeconds();
   setLength();
   return *this;
}

inline TimeOfLastPacketIE &TimeOfLastPacketIE::time_of_frst_pckt(uint32_t val)
{
   ie_.time_of_lst_pckt = val;
   setLength();
   return *this;
}

inline pfcp_time_of_lst_pckt_ie_t &TimeOfLastPacketIE::data()
{
   return ie_;
}

inline TimeOfLastPacketIE::TimeOfLastPacketIE(pfcp_time_of_lst_pckt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_TIME_OF_LST_PCKT, parent),
     ie_(ie)
{
}

inline uint16_t TimeOfLastPacketIE::calculateLength()
{
   return sizeof(pfcp_time_of_lst_pckt_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t QuotaHoldingTimeIE::quota_hldng_time_val() const
{
   return ie_.quota_hldng_time_val;
}

inline QuotaHoldingTimeIE &QuotaHoldingTimeIE::quota_hldng_time_val(uint32_t val)
{
   ie_.quota_hldng_time_val = val;
   setLength();
   return *this;
}
   
inline pfcp_quota_hldng_time_ie_t &QuotaHoldingTimeIE::data()
{
   return ie_;
}

inline QuotaHoldingTimeIE::QuotaHoldingTimeIE(pfcp_quota_hldng_time_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_QUOTA_HLDNG_TIME, parent),
     ie_(ie)
{
}

inline uint16_t QuotaHoldingTimeIE::calculateLength()
{
   return sizeof(pfcp_quota_hldng_time_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool DroppedDlTrafficThresholdIE::dlpa() const
{
   return ie_.dlpa;
}

inline Bool DroppedDlTrafficThresholdIE::dlby() const
{
   return ie_.dlby;
}

inline DroppedDlTrafficThresholdIE &DroppedDlTrafficThresholdIE::dnlnk_pckts(uint64_t val)
{
   ie_.dlpa = 1;
   ie_.dnlnk_pckts = val;
   setLength();
   return *this;
}
   
inline DroppedDlTrafficThresholdIE &DroppedDlTrafficThresholdIE::nbr_of_bytes_of_dnlnk_data(uint64_t val)
{
   ie_.dlby = 1;
   ie_.nbr_of_bytes_of_dnlnk_data = val;
   setLength();
   return *this;
}
   
inline pfcp_drpd_dl_traffic_thresh_ie_t &DroppedDlTrafficThresholdIE::data()
{
   return ie_;
}

inline DroppedDlTrafficThresholdIE::DroppedDlTrafficThresholdIE(pfcp_drpd_dl_traffic_thresh_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DRPD_DL_TRAFFIC_THRESH, parent),
      ie_(ie)
{
}

inline uint16_t DroppedDlTrafficThresholdIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.dlpa ? sizeof(ie_.dnlnk_pckts) : 0) +
      (ie_.dlby ? sizeof(ie_.nbr_of_bytes_of_dnlnk_data) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline Bool VolumeQuotaIE::tovol() const
{
   return ie_.tovol;
}

inline Bool VolumeQuotaIE::ulvol() const
{
   return ie_.ulvol;
}

inline Bool VolumeQuotaIE::dlvol() const
{
   return ie_.dlvol;
}

inline VolumeQuotaIE &VolumeQuotaIE::total_volume(uint64_t val)
{
   ie_.tovol = 1;
   ie_.total_volume = val;
   setLength();
   return *this;
}
   
inline VolumeQuotaIE &VolumeQuotaIE::uplink_volume(uint64_t val)
{
   ie_.ulvol = 1;
   ie_.uplink_volume = val;
   setLength();
   return *this;
}
   
inline VolumeQuotaIE &VolumeQuotaIE::downlink_volume(uint64_t val)
{
   ie_.dlvol = 1;
   ie_.downlink_volume = val;
   setLength();
   return *this;
}
   
inline pfcp_volume_quota_ie_t &VolumeQuotaIE::data()
{
   return ie_;
}

inline VolumeQuotaIE::VolumeQuotaIE(pfcp_volume_quota_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_VOLUME_QUOTA, parent),
     ie_(ie)
{
}

inline uint16_t VolumeQuotaIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.tovol ? sizeof(ie_.total_volume) : 0) +
      (ie_.ulvol ? sizeof(ie_.uplink_volume) : 0) +
      (ie_.dlvol ? sizeof(ie_.downlink_volume) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t TimeQuotaIE::time_quota_val() const
{
   return ie_.time_quota_val;
}

inline TimeQuotaIE &TimeQuotaIE::time_quota_val(uint32_t val)
{
   ie_.time_quota_val = val;
   setLength();
   return *this;
}

inline pfcp_time_quota_ie_t &TimeQuotaIE::data()
{
   return ie_;
}

inline TimeQuotaIE::TimeQuotaIE(pfcp_time_quota_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_TIME_QUOTA, parent),
     ie_(ie)
{
}

inline uint16_t TimeQuotaIE::calculateLength()
{
   return sizeof(pfcp_time_quota_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline ETime StartTimeIE::start_time() const
{
   ETime t;
   t.setNTPTime(ie_.start_time);
   return t;
}

inline StartTimeIE &StartTimeIE::start_time(const ETime &val)
{
   ie_.start_time = val.getNTPTimeSeconds();
   setLength();
   return *this;
}

inline StartTimeIE &StartTimeIE::start_time(uint32_t val)
{
   ie_.start_time = val;
   setLength();
   return *this;
}

inline pfcp_start_time_ie_t &StartTimeIE::data()
{
   return ie_;
}

inline StartTimeIE::StartTimeIE(pfcp_start_time_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_START_TIME, parent),
     ie_(ie)
{
}

inline uint16_t StartTimeIE::calculateLength()
{
   return sizeof(pfcp_start_time_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline ETime EndTimeIE::end_time() const
{
   ETime t;
   t.setNTPTime(ie_.end_time);
   return t;
}

inline EndTimeIE &EndTimeIE::end_time(const ETime &val)
{
   ie_.end_time = val.getNTPTimeSeconds();
   setLength();
   return *this;
}

inline EndTimeIE &EndTimeIE::end_time(uint32_t val)
{
   ie_.end_time = val;
   setLength();
   return *this;
}

inline pfcp_end_time_ie_t &EndTimeIE::data()
{
   return ie_;
}

inline EndTimeIE::EndTimeIE(pfcp_end_time_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_END_TIME, parent),
      ie_(ie)
{
}

inline uint16_t EndTimeIE::calculateLength()
{
   return sizeof(pfcp_end_time_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t UrrIdIE::urr_id_value() const
{
   return ie_.urr_id_value;
}

inline UrrIdIE &UrrIdIE::urr_id_value(uint32_t val)
{
   ie_.urr_id_value = val;
   setLength();
   return *this;
}
   
inline pfcp_urr_id_ie_t &UrrIdIE::data()
{
   return ie_;
}

inline UrrIdIE::UrrIdIE(pfcp_urr_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_URR_ID, parent),
      ie_(ie)
{
}

inline uint16_t UrrIdIE::calculateLength()
{
   return sizeof(pfcp_urr_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t LinkedUrrIdIE::lnkd_urr_id_val() const
{
   return ie_.lnkd_urr_id_val;
}

inline LinkedUrrIdIE &LinkedUrrIdIE::lnkd_urr_id_val(uint32_t val)
{
   ie_.lnkd_urr_id_val = val;
   setLength();
   return *this;
}
   
inline pfcp_linked_urr_id_ie_t &LinkedUrrIdIE::data()
{
   return ie_;
}

inline LinkedUrrIdIE::LinkedUrrIdIE(pfcp_linked_urr_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_LINKED_URR_ID, parent),
      ie_(ie)
{
}

inline uint16_t LinkedUrrIdIE::calculateLength()
{
   return sizeof(pfcp_linked_urr_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool OuterHeaderCreationIE::gtpu_udp_ipv4() const
{
   return ie_.outer_hdr_creation_desc.gtpu_udp_ipv4;
}

inline Bool OuterHeaderCreationIE::gtpu_udp_ipv6() const
{
   return ie_.outer_hdr_creation_desc.gtpu_udp_ipv6;
}

inline Bool OuterHeaderCreationIE::udp_ipv4() const
{
   return ie_.outer_hdr_creation_desc.udp_ipv4;
}

inline Bool OuterHeaderCreationIE::udp_ipv6() const
{
   return ie_.outer_hdr_creation_desc.udp_ipv6;
}

inline Bool OuterHeaderCreationIE::ipv4() const
{
   return ie_.outer_hdr_creation_desc.ipv4;
}

inline Bool OuterHeaderCreationIE::ipv6() const
{
   return ie_.outer_hdr_creation_desc.ipv6;
}

inline Bool OuterHeaderCreationIE::ctag() const
{
   return ie_.outer_hdr_creation_desc.ctag;
}

inline Bool OuterHeaderCreationIE::stag() const
{
   return ie_.outer_hdr_creation_desc.stag;
}

inline uint32_t OuterHeaderCreationIE::teid() const
{
   return ie_.teid;
}

inline const in_addr &OuterHeaderCreationIE::ipv4_address() const
{
   AliasPointer u;
   u.uint32_t_ptr = &ie_.ipv4_address;
   return *u.in_addr_ptr;
}

inline const in6_addr &OuterHeaderCreationIE::ipv6_address() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.ipv6_address;
   return *u.in6_addr_ptr;
}

inline uint32_t OuterHeaderCreationIE::port_number() const
{
   return ie_.port_number;
}

inline const uint8_t *OuterHeaderCreationIE::ctag_value() const
{
   return ie_.ctag;
}

inline const uint8_t *OuterHeaderCreationIE::stag_value() const
{
   return ie_.stag;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::ip_address_teid(const ESocket::Address &addr, uint32_t teid)
{
   if (addr.getFamily() == ESocket::Family::INET)
      ip_address_teid(addr.getInet().sin_addr, teid);
   else if (addr.getFamily() == ESocket::Family::INET6)
      ip_address_teid(addr.getInet6().sin6_addr, teid);
   return *this;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::ip_address_teid(const EIpAddress &addr, uint32_t teid)
{
   if (addr.family() == AF_INET)
      ip_address_teid(addr.ipv4Address(), teid);
   else if (addr.family() == AF_INET6)
      ip_address_teid(addr.ipv6Address(), teid);
   return *this;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::ip_address_teid(const in_addr &addr, uint32_t teid)
{
   ie_.outer_hdr_creation_desc.gtpu_udp_ipv4 = 1;
   ie_.outer_hdr_creation_desc.udp_ipv4 = 0;
   ie_.outer_hdr_creation_desc.udp_ipv6 = 0;
   ie_.outer_hdr_creation_desc.ipv4 = 0;
   ie_.outer_hdr_creation_desc.ipv6 = 0;
   ie_.outer_hdr_creation_desc.ctag = 0;
   ie_.outer_hdr_creation_desc.stag = 0;
   ie_.teid = teid;
   ie_.ipv4_address = addr.s_addr;
   setLength();
   return *this;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::ip_address_teid(const in6_addr &addr, uint32_t teid)
{
   ie_.outer_hdr_creation_desc.gtpu_udp_ipv6 = 1;
   ie_.outer_hdr_creation_desc.udp_ipv4 = 0;
   ie_.outer_hdr_creation_desc.udp_ipv6 = 0;
   ie_.outer_hdr_creation_desc.ipv4 = 0;
   ie_.outer_hdr_creation_desc.ipv6 = 0;
   ie_.outer_hdr_creation_desc.ctag = 0;
   ie_.outer_hdr_creation_desc.stag = 0;
   ie_.teid = teid;
   std::memcpy(ie_.ipv6_address, addr.s6_addr, sizeof(ie_.ipv6_address));
   setLength();
   return *this;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::ip_address_port(const ESocket::Address &addr, uint16_t port, InternetProtocolEnum protocol)
{
   if (addr.getFamily() == ESocket::Family::INET)
      ip_address_port(addr.getInet().sin_addr, port, protocol);
   else if (addr.getFamily() == ESocket::Family::INET6)
      ip_address_port(addr.getInet6().sin6_addr, port, protocol);
   return *this;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::ip_address_port(const EIpAddress &addr, uint16_t port, InternetProtocolEnum protocol)
{
   if (addr.family() == AF_INET)
      ip_address_port(addr.ipv4Address(), port, protocol);
   else if (addr.family() == AF_INET6)
      ip_address_port(addr.ipv6Address(), port, protocol);
   return *this;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::ip_address_port(const in_addr &addr, uint16_t port, InternetProtocolEnum protocol)
{
   if (protocol == InternetProtocolEnum::TCP)
   {
      ie_.outer_hdr_creation_desc.udp_ipv4 = 0;
      ie_.outer_hdr_creation_desc.udp_ipv6 = 0;
      ie_.outer_hdr_creation_desc.ipv4 = 1;
      ie_.outer_hdr_creation_desc.ipv6 = 0;
   }
   else if (protocol == InternetProtocolEnum::UDP)
   {
      ie_.outer_hdr_creation_desc.udp_ipv4 = 1;
      ie_.outer_hdr_creation_desc.udp_ipv6 = 0;
      ie_.outer_hdr_creation_desc.ipv4 = 0;
      ie_.outer_hdr_creation_desc.ipv6 = 0;
   }
   else
   {
      return *this;
   }
   ie_.port_number = port;
   ie_.ipv4_address = addr.s_addr;
   setLength();
   return *this;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::ip_address_port(const in6_addr &addr, uint16_t port, InternetProtocolEnum protocol)
{
   if (protocol == InternetProtocolEnum::TCP)
   {
      ie_.outer_hdr_creation_desc.udp_ipv4 = 0;
      ie_.outer_hdr_creation_desc.udp_ipv6 = 0;
      ie_.outer_hdr_creation_desc.ipv4 = 0;
      ie_.outer_hdr_creation_desc.ipv6 = 1;
   }
   else if (protocol == InternetProtocolEnum::UDP)
   {
      ie_.outer_hdr_creation_desc.udp_ipv4 = 0;
      ie_.outer_hdr_creation_desc.udp_ipv6 = 1;
      ie_.outer_hdr_creation_desc.ipv4 = 0;
      ie_.outer_hdr_creation_desc.ipv6 = 0;
   }
   else
   {
      return *this;
   }
   ie_.port_number = port;
   std::memcpy(ie_.ipv6_address, addr.s6_addr, sizeof(ie_.ipv6_address));
   setLength();
   return *this;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::ctag(const uint8_t *val)
{
   ie_.outer_hdr_creation_desc.ctag = 1;
   std::memcpy(ie_.ctag, val, sizeof(ie_.ctag));
   setLength();
   return *this;
}

inline OuterHeaderCreationIE &OuterHeaderCreationIE::stag(const uint8_t *val)
{
   ie_.outer_hdr_creation_desc.stag = 1;
   std::memcpy(ie_.stag, val, sizeof(ie_.stag));
   setLength();
   return *this;
}

inline pfcp_outer_hdr_creation_ie_t &OuterHeaderCreationIE::data()
{
   return ie_;
}

inline OuterHeaderCreationIE::OuterHeaderCreationIE(pfcp_outer_hdr_creation_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_OUTER_HDR_CREATION, parent),
     ie_(ie)
{
}

inline uint16_t OuterHeaderCreationIE::calculateLength()
{
   return
      sizeof(ie_.outer_hdr_creation_desc) +
      (ie_.outer_hdr_creation_desc.gtpu_udp_ipv4 || ie_.outer_hdr_creation_desc.gtpu_udp_ipv6 ? sizeof(ie_.teid) : 0) +
      (ie_.outer_hdr_creation_desc.gtpu_udp_ipv4 ? sizeof(ie_.ipv4_address) : 0) +
      (ie_.outer_hdr_creation_desc.gtpu_udp_ipv6 ? sizeof(ie_.ipv6_address) : 0) +
      (ie_.outer_hdr_creation_desc.udp_ipv4 ? sizeof(ie_.ipv4_address) : 0) +
      (ie_.outer_hdr_creation_desc.udp_ipv6 ? sizeof(ie_.ipv6_address) : 0) +
      (ie_.outer_hdr_creation_desc.ipv4 ? sizeof(ie_.ipv4_address) : 0) +
      (ie_.outer_hdr_creation_desc.ipv6 ? sizeof(ie_.ipv6_address) : 0) +
      (ie_.outer_hdr_creation_desc.udp_ipv4 || ie_.outer_hdr_creation_desc.udp_ipv6 ||
         ie_.outer_hdr_creation_desc.ipv4 || ie_.outer_hdr_creation_desc.ipv6 ? sizeof(ie_.port_number) : 0) +
      (ie_.outer_hdr_creation_desc.ctag ? sizeof(ie_.ctag) : 0) +
      (ie_.outer_hdr_creation_desc.stag ? sizeof(ie_.stag) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline uint8_t BarIdIE::bar_id_value() const
{
   return ie_.bar_id_value;
}

inline BarIdIE &BarIdIE::bar_id_value(uint8_t val)
{
   ie_.bar_id_value = val;
   setLength();
   return *this;
}
   
inline pfcp_bar_id_ie_t &BarIdIE::data()
{
   return ie_;
}

inline BarIdIE::BarIdIE(pfcp_bar_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_BAR_ID, parent),
     ie_(ie)
{
}

inline uint16_t BarIdIE::calculateLength()
{
   return sizeof(pfcp_bar_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool CpFunctionFeaturesIE::load() const
{
   return ie_.sup_feat.load;
}

inline Bool CpFunctionFeaturesIE::ovrl() const
{
   return ie_.sup_feat.ovrl;
}

inline CpFunctionFeaturesIE &CpFunctionFeaturesIE::load(Bool val)
{
   ie_.sup_feat.load = val ? 1 : 0; setLength();
   return *this;
}

inline CpFunctionFeaturesIE &CpFunctionFeaturesIE::ovrl(Bool val)
{
   ie_.sup_feat.ovrl = val ? 1 : 0; setLength();
   return *this;
}
   
inline pfcp_cp_func_feat_ie_t &CpFunctionFeaturesIE::data()
{
   return ie_;
}

inline CpFunctionFeaturesIE::CpFunctionFeaturesIE(pfcp_cp_func_feat_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_CP_FUNC_FEAT, parent),
      ie_(ie)
{
}

inline uint16_t CpFunctionFeaturesIE::calculateLength()
{
   return sizeof(pfcp_cp_func_feat_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool UsageInformationIE::bef() const
{
   return ie_.bef;
}

inline Bool UsageInformationIE::aft() const
{
   return ie_.aft;
}

inline Bool UsageInformationIE::uae() const
{
   return ie_.uae;
}

inline Bool UsageInformationIE::ube() const
{
   return ie_.ube;
}

inline UsageInformationIE &UsageInformationIE::bef(Bool val)
{
   ie_.bef = val ? 1 : 0; setLength();
   return *this;
}

inline UsageInformationIE &UsageInformationIE::aft(Bool val)
{
   ie_.aft = val ? 1 : 0; setLength();
   return *this;
}

inline UsageInformationIE &UsageInformationIE::uae(Bool val)
{
   ie_.uae = val ? 1 : 0; setLength();
   return *this;
}

inline UsageInformationIE &UsageInformationIE::ube(Bool val)
{
   ie_.ube = val ? 1 : 0; setLength();
   return *this;
}

inline pfcp_usage_info_ie_t &UsageInformationIE::data()
{
   return ie_;
}

inline UsageInformationIE::UsageInformationIE(pfcp_usage_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_USAGE_INFO, parent),
     ie_(ie)
{
}

inline uint16_t UsageInformationIE::calculateLength()
{
   return sizeof(pfcp_usage_info_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline const uint8_t *ApplicationInstanceIdIE::app_inst_ident() const
{
   return ie_.app_inst_ident;
 }

inline ApplicationInstanceIdIE &ApplicationInstanceIdIE::app_inst_ident(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.app_inst_ident))
      len = sizeof(ie_.app_inst_ident);
   std::memcpy(ie_.app_inst_ident, val, len);
   ie_.header.len = len;
   return *this;
}
   
inline pfcp_app_inst_id_ie_t &ApplicationInstanceIdIE::data()
{
   return ie_;
}

inline ApplicationInstanceIdIE::ApplicationInstanceIdIE(pfcp_app_inst_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_APP_INST_ID, parent),
      ie_(ie)
{
}

inline uint16_t ApplicationInstanceIdIE::calculateLength()
{
   return ie_.header.len;
}

////////////////////////////////////////////////////////////////////////////////

inline FlowDirectionEnum FlowInformationIE::flow_direction() const
{
   return static_cast<FlowDirectionEnum>(ie_.flow_direction);
}

inline FlowInformationIE &FlowInformationIE::flow_direction(FlowDirectionEnum val)
{
   ie_.flow_direction = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline FlowInformationIE &FlowInformationIE::flow_desc(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.flow_desc))
      len = sizeof(ie_.flow_desc);
   ie_.len_of_flow_desc = len;
   std::memcpy(ie_.flow_desc, val, len);
   setLength();
   return *this;
}

inline pfcp_flow_info_ie_t &FlowInformationIE::data()
{
   return ie_;
}

inline FlowInformationIE::FlowInformationIE(pfcp_flow_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FLOW_INFO, parent),
      ie_(ie)
{
}

inline uint16_t FlowInformationIE::calculateLength()
{
   return
      1 + /* octet 5 */
      sizeof(ie_.len_of_flow_desc) +
      ie_.len_of_flow_desc
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline Bool UeIpAddressIE::v6() const
{
   return ie_.v6;
}

inline Bool UeIpAddressIE::v4() const
{
   return ie_.v4;
}

inline Bool UeIpAddressIE::sd() const
{
   return ie_.sd;
}

inline Bool UeIpAddressIE::ipv6d() const
{
   return ie_.ipv6d;
}

inline const in_addr &UeIpAddressIE::ipv4_address() const
{
   AliasPointer u;
   u.uint32_t_ptr = &ie_.ipv4_address;
   return *u.in_addr_ptr;
}

inline const in6_addr &UeIpAddressIE::ipv6_address() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.ipv6_address;
   return *u.in6_addr_ptr;
}

inline const uint8_t UeIpAddressIE::ipv6_pfx_dlgtn_bits() const
{
   return ie_.ipv6_pfx_dlgtn_bits;
}

inline UeIpAddressIE &UeIpAddressIE::sd(Bool val)
{
   ie_.sd = val ? 1 : 0;
   setLength();
   return *this;
}

inline UeIpAddressIE &UeIpAddressIE::ip_address(const ESocket::Address &val)
{
   if (val.getFamily() == ESocket::Family::INET)
      return ip_address(val.getInet().sin_addr);
   else if (val.getFamily() == ESocket::Family::INET6)
      return ip_address(val.getInet6().sin6_addr);
   return *this;
}

inline UeIpAddressIE &UeIpAddressIE::ip_address(const EIpAddress &val)
{
   if (val.family() == AF_INET)
      return ip_address(val.ipv4Address());
   else if (val.family() == AF_INET6)
      return ip_address(val.ipv6Address());
   return *this;
}

inline UeIpAddressIE &UeIpAddressIE::ip_address(const in_addr &val)
{
   ie_.v4 = 1;
   ie_.ipv4_address = val.s_addr;
   setLength();
   return *this;
}

inline UeIpAddressIE &UeIpAddressIE::ip_address(const in6_addr &val)
{
   ie_.v6 = 1;
   std::memcpy(ie_.ipv6_address, val.s6_addr, sizeof(ie_.ipv6_address));
   setLength();
   return *this;
}    
   
inline UeIpAddressIE &UeIpAddressIE::ipv6_pfx_dlgtn_bits(uint8_t val)
{
   ie_.ipv6d = 1;
   ie_.ipv6_pfx_dlgtn_bits = val;
   setLength();
   return *this;
}

inline pfcp_ue_ip_address_ie_t &UeIpAddressIE::data()
{
   return ie_;
}

inline UeIpAddressIE::UeIpAddressIE(pfcp_ue_ip_address_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_UE_IP_ADDRESS, parent),
      ie_(ie)
{
}

inline uint16_t UeIpAddressIE::calculateLength()
{
   return sizeof(pfcp_ue_ip_address_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool PacketRateIE::ulpr() const
{
   return ie_.ulpr;
}

inline Bool PacketRateIE::dlpr() const
{
   return ie_.dlpr;
}

inline UplinkDownlinkTimeUnitEnum PacketRateIE::uplnk_time_unit() const
{
   return static_cast<UplinkDownlinkTimeUnitEnum>(ie_.uplnk_time_unit);
}

inline uint16_t PacketRateIE::max_uplnk_pckt_rate() const
{
   return ie_.max_uplnk_pckt_rate;
}

inline UplinkDownlinkTimeUnitEnum PacketRateIE::dnlnk_time_unit() const
{
   return static_cast<UplinkDownlinkTimeUnitEnum>(ie_.dnlnk_time_unit);
}

inline uint16_t PacketRateIE::max_dnlnk_pckt_rate() const
{
   return ie_.max_dnlnk_pckt_rate;
}

inline PacketRateIE &PacketRateIE::max_uplnk_pckt_rate(uint16_t val, UplinkDownlinkTimeUnitEnum tu)
{
   ie_.ulpr = 1;
   ie_.uplnk_time_unit = static_cast<uint8_t>(tu);
   ie_.max_uplnk_pckt_rate = val;
   setLength();
   return *this;
}

inline PacketRateIE &PacketRateIE::max_dnlnk_pckt_rate(uint16_t val, UplinkDownlinkTimeUnitEnum tu)
{
   ie_.dlpr = 1;
   ie_.dnlnk_time_unit = static_cast<uint8_t>(tu);
   ie_.max_dnlnk_pckt_rate = val;
   setLength();
   return *this;
}
   
inline pfcp_packet_rate_ie_t &PacketRateIE::data()
{
   return ie_;
}

inline PacketRateIE::PacketRateIE(pfcp_packet_rate_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_PACKET_RATE, parent),
     ie_(ie)
{
}

inline uint16_t PacketRateIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.ulpr ? 3 : 0) + /* uplink time unit + maximum uplink packet rate */
      (ie_.dlpr ? 3 : 0)   /* downlink time unit + maximum downlink packet rate */
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline OuterHeaderRemovalEnum OuterHeaderRemovalIE::outer_hdr_removal_desc() const
{
   return static_cast<OuterHeaderRemovalEnum>(ie_.outer_hdr_removal_desc);
}

inline Bool OuterHeaderRemovalIE::pdu_session_container() const
{
   return ie_.gtpu_ext_hdr_del.pdu_session_container;
}

inline OuterHeaderRemovalIE &OuterHeaderRemovalIE::pdu_session_container(Bool val)
{
   ie_.gtpu_ext_hdr_del.pdu_session_container = val ? 1 : 0;
   setLength();
   return *this;
}

inline pfcp_outer_hdr_removal_ie_t &OuterHeaderRemovalIE::data()
{
   return ie_;
}

inline OuterHeaderRemovalIE::OuterHeaderRemovalIE(pfcp_outer_hdr_removal_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_OUTER_HDR_REMOVAL, parent),
     ie_(ie)
{
}

inline uint16_t OuterHeaderRemovalIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.gtpu_ext_hdr_del.pdu_session_container ? sizeof(ie_.gtpu_ext_hdr_del) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline ETime RecoveryTimeStampIE::rcvry_time_stmp_val() const
{
   ETime t;
   t.setNTPTime(ie_.rcvry_time_stmp_val);
   return t;
}

inline RecoveryTimeStampIE &RecoveryTimeStampIE::rcvry_time_stmp_val(const ETime &val)
{
   ie_.rcvry_time_stmp_val = val.getNTPTimeSeconds();
   setLength();
   return *this;
}
   
inline pfcp_rcvry_time_stmp_ie_t &RecoveryTimeStampIE::data()
{
   return ie_;
}

inline RecoveryTimeStampIE::RecoveryTimeStampIE(pfcp_rcvry_time_stmp_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_RCVRY_TIME_STMP, parent),
     ie_(ie)
{
}

inline uint16_t RecoveryTimeStampIE::calculateLength()
{
   return sizeof(pfcp_rcvry_time_stmp_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool DlFlowLevelMarkingIE::ttc() const
{
   return ie_.ttc;
}

inline Bool DlFlowLevelMarkingIE::sci() const
{
   return ie_.sci;
}

inline const uint8_t *DlFlowLevelMarkingIE::tostraffic_cls() const
{
   return ie_.tostraffic_cls;
}

inline const uint8_t *DlFlowLevelMarkingIE::svc_cls_indctr() const
{
   return ie_.svc_cls_indctr;
}

inline DlFlowLevelMarkingIE &DlFlowLevelMarkingIE::tostraffic_cls(const uint8_t *val)
{
   ie_.ttc = 1;
   memcpy(ie_.tostraffic_cls, val, sizeof(ie_.tostraffic_cls));
   setLength();
   return *this;
}

inline DlFlowLevelMarkingIE &DlFlowLevelMarkingIE::svc_cls_indctr(const uint8_t *val)
{
   ie_.sci = 1;
   memcpy(ie_.svc_cls_indctr, val, sizeof(ie_.svc_cls_indctr));
   setLength();
   return *this;
}

inline pfcp_dl_flow_lvl_marking_ie_t &DlFlowLevelMarkingIE::data()
{
   return ie_;
}

inline DlFlowLevelMarkingIE::DlFlowLevelMarkingIE(pfcp_dl_flow_lvl_marking_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DL_FLOW_LVL_MARKING, parent),
     ie_(ie)
{
}

inline uint16_t DlFlowLevelMarkingIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.ttc ? sizeof(ie_.svc_cls_indctr) : 0) +
      (ie_.sci ? sizeof(ie_.svc_cls_indctr) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline HeaderTypeEnum HeaderEnrichmentIE::header_type() const
{
   return static_cast<HeaderTypeEnum>(ie_.header_type);
}

inline uint8_t HeaderEnrichmentIE::len_of_hdr_fld_nm() const
{
   return ie_.len_of_hdr_fld_nm;
}

inline const uint8_t *HeaderEnrichmentIE::hdr_fld_nm() const
{
   return ie_.hdr_fld_nm;
}

inline uint8_t HeaderEnrichmentIE::len_of_hdr_fld_val() const
{
   return ie_.len_of_hdr_fld_val;
}

inline const uint8_t *HeaderEnrichmentIE::hdr_fld_val() const
{
   return ie_.hdr_fld_val;
}

inline HeaderEnrichmentIE &HeaderEnrichmentIE::header_type(HeaderTypeEnum val)
{
   ie_.header_type = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline HeaderEnrichmentIE &HeaderEnrichmentIE::header_fld_nm(const uint8_t *val, uint8_t len)
{
   if (len > sizeof(ie_.hdr_fld_nm))
      len = sizeof(ie_.hdr_fld_nm);
   ie_.len_of_hdr_fld_nm = len;
   std::memcpy(ie_.hdr_fld_nm, val, len);
   setLength();
   return *this;
}

inline HeaderEnrichmentIE &HeaderEnrichmentIE::header_fld_val(const uint8_t *val, uint8_t len)
{
   if (len > sizeof(ie_.hdr_fld_val))
      len = sizeof(ie_.hdr_fld_val);
   ie_.len_of_hdr_fld_val = len;
   std::memcpy(ie_.hdr_fld_val, val, len);
   setLength();
   return *this;
}

inline pfcp_hdr_enrchmt_ie_t &HeaderEnrichmentIE::data()
{
   return ie_;
}

inline HeaderEnrichmentIE::HeaderEnrichmentIE(pfcp_hdr_enrchmt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_HDR_ENRCHMT, parent),
     ie_(ie)
{
}

inline uint16_t HeaderEnrichmentIE::calculateLength()
{
   return
      1 + /* octet 5 */
      sizeof(ie_.len_of_hdr_fld_nm) +
      ie_.len_of_hdr_fld_nm +
      sizeof(ie_.len_of_hdr_fld_val) +
      ie_.len_of_hdr_fld_val
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline Bool MeasurementInformationIE::mbqe() const
{
   return ie_.mbqe;
}

inline Bool MeasurementInformationIE::inam() const
{
   return ie_.inam;
}

inline Bool MeasurementInformationIE::radi() const
{
   return ie_.radi;
}

inline Bool MeasurementInformationIE::istm() const
{
   return ie_.istm;
}

inline MeasurementInformationIE &MeasurementInformationIE::mbqe(Bool val)
{
   ie_.mbqe = val ? 1 : 0;
   setLength();
   return *this;
}

inline MeasurementInformationIE &MeasurementInformationIE::inam(Bool val)
{
   ie_.inam = val ? 1 : 0;
   setLength();
   return *this;
}

inline MeasurementInformationIE &MeasurementInformationIE::radi(Bool val)
{
   ie_.radi = val ? 1 : 0;
   setLength();
   return *this;
}

inline MeasurementInformationIE &MeasurementInformationIE::istm(Bool val)
{
   ie_.istm = val ? 1 : 0;
   setLength();
   return *this;
}

inline pfcp_meas_info_ie_t &MeasurementInformationIE::data()
{
   return ie_;
}

inline MeasurementInformationIE::MeasurementInformationIE(pfcp_meas_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_MEAS_INFO, parent),
     ie_(ie)
{
}

inline uint16_t MeasurementInformationIE::calculateLength()
{
   return sizeof(pfcp_meas_info_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool NodeReportTypeIE::upfr() const
{
   return ie_.upfr;
}

inline NodeReportTypeIE &NodeReportTypeIE::upfr(Bool val)
{
   ie_.upfr = val;
   setLength();
   return *this;
}
   
inline pfcp_node_rpt_type_ie_t &NodeReportTypeIE::data()
{
   return ie_;
}

inline NodeReportTypeIE::NodeReportTypeIE(pfcp_node_rpt_type_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_NODE_RPT_TYPE, parent),
     ie_(ie)
{
}

inline uint16_t NodeReportTypeIE::calculateLength()
{
   return sizeof(pfcp_node_rpt_type_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool RemoteGTPUPeerIE::v6() const
{
   return ie_.v6;
}

inline Bool RemoteGTPUPeerIE::v4() const
{
   return ie_.v4;
}

inline Bool RemoteGTPUPeerIE::di() const
{
   return ie_.di;
}

inline Bool RemoteGTPUPeerIE::ni() const
{
   return ie_.ni;
}

inline const in_addr &RemoteGTPUPeerIE::ipv4_address() const
{
   AliasPointer u;
   u.uint32_t_ptr = &ie_.ipv4_address;
   return *u.in_addr_ptr;
}

inline const in6_addr &RemoteGTPUPeerIE::ipv6_address() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.ipv6_address;
   return *u.in6_addr_ptr;
}

inline uint16_t RemoteGTPUPeerIE::len_of_dst_intfc_fld() const
{
   return ie_.len_of_dst_intfc_fld;
}

inline DestinationInterfaceEnum RemoteGTPUPeerIE::dst_intfc_value() const
{
   return static_cast<DestinationInterfaceEnum>(ie_.dst_intfc.intfc_value);
}

inline uint16_t RemoteGTPUPeerIE::len_of_ntwk_intfc_fld() const
{
   return ie_.len_of_ntwk_instc_fld;
}

inline const uint8_t *RemoteGTPUPeerIE::ntwk_instc() const
{
   return ie_.ntwk_instc;
}

inline RemoteGTPUPeerIE &RemoteGTPUPeerIE::ip_address(const ESocket::Address &val)
{
   if (val.getFamily() == ESocket::Family::INET)
      return ip_address(val.getInet().sin_addr);
   else if (val.getFamily() == ESocket::Family::INET6)
      return ip_address(val.getInet6().sin6_addr);
   return *this;
}

inline RemoteGTPUPeerIE &RemoteGTPUPeerIE::ip_address(const EIpAddress &val)
{
   if (val.family() == AF_INET)
      return ip_address(val.ipv4Address());
   else if (val.family() == AF_INET6)
      return ip_address(val.ipv6Address());
   return *this;
}

inline RemoteGTPUPeerIE &RemoteGTPUPeerIE::ip_address(const in_addr &val)
{
   ie_.v4 = 1;
   ie_.ipv4_address = val.s_addr;
   setLength();
   return *this;
}

inline RemoteGTPUPeerIE &RemoteGTPUPeerIE::ip_address(const in6_addr &val)
{
   ie_.v6 = 1;
   std::memcpy(ie_.ipv6_address, val.s6_addr, sizeof(ie_.ipv6_address));
   setLength();
   return *this;
}    

inline RemoteGTPUPeerIE &RemoteGTPUPeerIE::dst_intfc_fld(DestinationInterfaceEnum val)
{
   ie_.di = 1;
   ie_.len_of_dst_intfc_fld = sizeof(ie_.dst_intfc);
   ie_.dst_intfc.intfc_value = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline RemoteGTPUPeerIE &RemoteGTPUPeerIE::ntwk_instc(const uint8_t *val, uint16_t len)
{
   ie_.ni = 1;
   if (len > sizeof(ie_.ntwk_instc))
      len = sizeof(ie_.ntwk_instc);
   ie_.len_of_ntwk_instc_fld = len;
   std::memcpy(ie_.ntwk_instc, val, len);
   setLength();
   return *this;
}

inline pfcp_rmt_gtpu_peer_ie_t &RemoteGTPUPeerIE::data()
{
   return ie_;
}

inline RemoteGTPUPeerIE::RemoteGTPUPeerIE(pfcp_rmt_gtpu_peer_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_RMT_GTPU_PEER, parent),
     ie_(ie)
{
}

inline uint16_t RemoteGTPUPeerIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.v4 ? sizeof(ie_.ipv4_address) : 0) +
      (ie_.v6 ? sizeof(ie_.ipv6_address) : 0) +
      (ie_.di ? sizeof(ie_.len_of_dst_intfc_fld) + ie_.len_of_dst_intfc_fld : 0) +
      (ie_.ni ? sizeof(ie_.len_of_ntwk_instc_fld) + ie_.len_of_ntwk_instc_fld : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t UrSeqnIE::urseqn() const
{
   return ie_.urseqn;
}

inline UrSeqnIE &UrSeqnIE::urseqn(uint32_t val)
{
   ie_.urseqn = val;
   setLength();
   return *this;
}
   
inline pfcp_urseqn_ie_t &UrSeqnIE::data()
{
   return ie_;
}

inline UrSeqnIE::UrSeqnIE(pfcp_urseqn_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_URSEQN, parent),
     ie_(ie)
{
}

inline uint16_t UrSeqnIE::calculateLength()
{
   return sizeof(pfcp_urseqn_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint16_t ActivatePredefinedRulesIE::predef_rules_nm_len() const
{
   return ie_.header.len;
}

inline const uint8_t *ActivatePredefinedRulesIE::predef_rules_nm() const
{
   return ie_.predef_rules_nm;
}

inline ActivatePredefinedRulesIE &ActivatePredefinedRulesIE::predef_rules_nm(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.predef_rules_nm))
      len = sizeof(ie_.predef_rules_nm);
   std::memcpy(ie_.predef_rules_nm, val, len);
   ie_.header.len = len;
   return *this;
}
   
inline pfcp_actvt_predef_rules_ie_t &ActivatePredefinedRulesIE::data()
{
   return ie_;
}

inline ActivatePredefinedRulesIE::ActivatePredefinedRulesIE(pfcp_actvt_predef_rules_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_ACTVT_PREDEF_RULES, parent),
     ie_(ie)
{
}

inline uint16_t ActivatePredefinedRulesIE::calculateLength()
{
   return ie_.header.len;
}

////////////////////////////////////////////////////////////////////////////////

inline uint16_t DeactivatePredefinedRulesIE::predef_rules_nm_len() const
{
   return ie_.header.len;
}
inline const uint8_t *DeactivatePredefinedRulesIE::predef_rules_nm() const
{
   return ie_.predef_rules_nm;
}

inline DeactivatePredefinedRulesIE &DeactivatePredefinedRulesIE::predef_rules_nm(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.predef_rules_nm))
      len = sizeof(ie_.predef_rules_nm);
   std::memcpy(ie_.predef_rules_nm, val, len);
   ie_.header.len = len;
   return *this;
}
   
inline pfcp_deact_predef_rules_ie_t &DeactivatePredefinedRulesIE::data()
{
   return ie_;
}

inline DeactivatePredefinedRulesIE::DeactivatePredefinedRulesIE(pfcp_deact_predef_rules_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_DEACT_PREDEF_RULES, parent),
     ie_(ie)
{
}

inline uint16_t DeactivatePredefinedRulesIE::calculateLength()
{
   return ie_.header.len;
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t FarIdIE::far_id_value() const
{
   return ie_.far_id_value;
}

inline FarIdIE &FarIdIE::far_id_value(uint32_t val)
{
   ie_.far_id_value = val;
   setLength();
   return *this;
}
   
inline pfcp_far_id_ie_t &FarIdIE::data()
{
   return ie_;
}

inline FarIdIE::FarIdIE(pfcp_far_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FAR_ID, parent),
     ie_(ie)
{
}

inline uint16_t FarIdIE::calculateLength()
{
   return sizeof(pfcp_far_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t QerIdIE::qer_id_value() const
{
   return ie_.qer_id_value;
}

inline QerIdIE &QerIdIE::qer_id_value(uint32_t val)
{
   ie_.qer_id_value = val;
   setLength();
   return *this;
}
   
inline pfcp_qer_id_ie_t &QerIdIE::data()
{
   return ie_;
}

inline QerIdIE::QerIdIE(pfcp_qer_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_QER_ID, parent),
     ie_(ie)
{
}

inline uint16_t QerIdIE::calculateLength()
{
   return sizeof(pfcp_qer_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////
inline Bool OciFlagsIE::aoci() const
{
   return ie_.aoci;
}

inline OciFlagsIE &OciFlagsIE::aoci(Bool val)
{
   ie_.aoci = val ? 1 : 0;
   setLength();
   return *this;
}
   
inline pfcp_oci_flags_ie_t &OciFlagsIE::data()
{
   return ie_;
}

inline OciFlagsIE::OciFlagsIE(pfcp_oci_flags_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_OCI_FLAGS, parent),
     ie_(ie)
{
}

inline uint16_t OciFlagsIE::calculateLength()
{
   return ie_.aoci ? 
      sizeof(pfcp_oci_flags_ie_t) - sizeof(pfcp_ie_header_t) :
      0
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline Bool AssociationReleaseRequestIE::sarr() const
{
   return ie_.sarr;
}

inline AssociationReleaseRequestIE &AssociationReleaseRequestIE::sarr(Bool val)
{
   ie_.sarr = val ? 1 : 0;
   setLength();
   return *this;
}

inline pfcp_up_assn_rel_req_ie_t &AssociationReleaseRequestIE::data()
{
   return ie_;
}

inline AssociationReleaseRequestIE::AssociationReleaseRequestIE(pfcp_up_assn_rel_req_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_UP_ASSN_REL_REQ, parent),
     ie_(ie)
{
}

inline uint16_t AssociationReleaseRequestIE::calculateLength()
{
   return ie_.sarr ?
      sizeof(pfcp_up_assn_rel_req_ie_t) - sizeof(pfcp_ie_header_t) :
      0
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline GracefulReleasePeriodTimerUnitEnum GracefulReleasePeriodIE::timer_unit() const
{
   return static_cast<GracefulReleasePeriodTimerUnitEnum>(ie_.timer_unit);
}

inline uint8_t GracefulReleasePeriodIE::timer_value() const
{
   return ie_.timer_value;
}

inline GracefulReleasePeriodIE &GracefulReleasePeriodIE::timer_value(uint8_t val, GracefulReleasePeriodTimerUnitEnum tu)
{
   ie_.timer_unit = static_cast<uint8_t>(tu);
   ie_.timer_value = val;
   setLength();
   return *this;
}
   
inline pfcp_graceful_rel_period_ie_t &GracefulReleasePeriodIE::data()
{
   return ie_;
}

inline GracefulReleasePeriodIE::GracefulReleasePeriodIE(pfcp_graceful_rel_period_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_GRACEFUL_REL_PERIOD, parent),
     ie_(ie)
{
}

inline uint16_t GracefulReleasePeriodIE::calculateLength()
{
   return sizeof(pfcp_graceful_rel_period_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline PdnTypeEnum PdnTypeIE::pdn_type() const
{
   return static_cast<PdnTypeEnum>(ie_.pdn_type);
}

inline PdnTypeIE &PdnTypeIE::pdn_type(PdnTypeEnum val)
{
   ie_.pdn_type = static_cast<uint8_t>(val);
   setLength();
   return *this;
}
   
inline pfcp_pdn_type_ie_t &PdnTypeIE::data()
{
   return ie_;
}

inline PdnTypeIE::PdnTypeIE(pfcp_pdn_type_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_PDN_TYPE, parent),
     ie_(ie)
{
}

inline uint16_t PdnTypeIE::calculateLength()
{
   return sizeof(pfcp_pdn_type_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline RuleIdTypeEnum FailedRuleIdIE::rule_id_type() const
{
   return static_cast<RuleIdTypeEnum>(ie_.rule_id_type);
}

inline uint32_t FailedRuleIdIE::rule_id_value() const
{
   return ie_.rule_id_value;
}

inline FailedRuleIdIE &FailedRuleIdIE::rule_id_value(uint32_t val, RuleIdTypeEnum rt)
{
   ie_.rule_id_type = static_cast<uint8_t>(rt);
   if (rt == RuleIdTypeEnum::pdr)
      ie_.rule_id_value = val & 0x0000ffff;
   else if (rt == RuleIdTypeEnum::bar)
      ie_.rule_id_value = val & 0x000000ff;
   else
      ie_.rule_id_value = val;
   setLength();
   return *this;
}
   
inline pfcp_failed_rule_id_ie_t &FailedRuleIdIE::data()
{
   return ie_;
}

inline FailedRuleIdIE::FailedRuleIdIE(pfcp_failed_rule_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FAILED_RULE_ID, parent),
     ie_(ie)
{
}

inline uint16_t FailedRuleIdIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.rule_id_type == 0 ? 2 :
         ie_.rule_id_type == 4 ? 1 : 4)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline BaseTimeIntervalTypeEnum TimeQuotaMechanismIE::btit() const
{
   return static_cast<BaseTimeIntervalTypeEnum>(ie_.btit);
}

inline uint32_t TimeQuotaMechanismIE::base_time_int() const
{
   return ie_.base_time_int;
}

inline TimeQuotaMechanismIE &TimeQuotaMechanismIE::base_time_int(uint32_t val, BaseTimeIntervalTypeEnum btit)
{
   ie_.btit = static_cast<uint8_t>(btit);
   ie_.base_time_int = val;
   setLength();
   return *this;
}

inline pfcp_time_quota_mech_ie_t &TimeQuotaMechanismIE::data()
{
   return ie_;
}

inline TimeQuotaMechanismIE::TimeQuotaMechanismIE(pfcp_time_quota_mech_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_TIME_QUOTA_MECH, parent),
     ie_(ie)
{
}

inline uint16_t TimeQuotaMechanismIE::calculateLength()
{
   return sizeof(pfcp_time_quota_mech_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool UserPlaneIpResourceInformationIE::v4() const
{
   return ie_.v4;
}

inline Bool UserPlaneIpResourceInformationIE::v6() const
{
   return ie_.v6;
}

inline uint8_t UserPlaneIpResourceInformationIE::teidri() const
{
   return ie_.teidri;
}

inline Bool UserPlaneIpResourceInformationIE::assoni() const
{
   return ie_.assoni;
}

inline Bool UserPlaneIpResourceInformationIE::assosi() const
{
   return ie_.assosi;
}

inline uint8_t UserPlaneIpResourceInformationIE::teid_range() const
{
   return ie_.teid_range;
}

inline UserPlaneIpResourceInformationIE &UserPlaneIpResourceInformationIE::teid_range(uint8_t bits, uint8_t val)
{
   ie_.teidri = bits;
   ie_.teid_range = val;
   return *this;
}
inline const in_addr &UserPlaneIpResourceInformationIE::ipv4_address() const
{
   AliasPointer u;
   u.uint32_t_ptr = &ie_.ipv4_address;
   return *u.in_addr_ptr;
}

inline const in6_addr &UserPlaneIpResourceInformationIE::ipv6_address() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.ipv6_address;
   return *u.in6_addr_ptr;
}

inline uint8_t UserPlaneIpResourceInformationIE::ntwk_inst_len() const
{
   return ie_.ntwk_inst_len;
}

inline const uint8_t *UserPlaneIpResourceInformationIE::ntwk_inst() const
{
   return ie_.ntwk_inst;
}

inline SourceInterfaceEnum UserPlaneIpResourceInformationIE::src_intfc() const
{
   return static_cast<SourceInterfaceEnum>(ie_.src_intfc);
}

inline UserPlaneIpResourceInformationIE &UserPlaneIpResourceInformationIE::ip_address(const ESocket::Address &val)
{
   if (val.getFamily() == ESocket::Family::INET)
      return ip_address(val.getInet().sin_addr);
   else if (val.getFamily() == ESocket::Family::INET6)
      return ip_address(val.getInet6().sin6_addr);
   return *this;
}

inline UserPlaneIpResourceInformationIE &UserPlaneIpResourceInformationIE::ip_address(const EIpAddress &val)
{
   if (val.family() == AF_INET)
      return ip_address(val.ipv4Address());
   else if (val.family() == AF_INET6)
      return ip_address(val.ipv6Address());
   return *this;
}

inline UserPlaneIpResourceInformationIE &UserPlaneIpResourceInformationIE::ip_address(const in_addr &val)
{
   ie_.v4 = 1;
   ie_.ipv4_address = val.s_addr;
   setLength();
   return *this;
}

inline UserPlaneIpResourceInformationIE &UserPlaneIpResourceInformationIE::ip_address(const in6_addr &val)
{
   ie_.v6 = 1;
   std::memcpy(ie_.ipv6_address, val.s6_addr, sizeof(ie_.ipv6_address));
   setLength();
   return *this;
}    

inline UserPlaneIpResourceInformationIE &UserPlaneIpResourceInformationIE::ntwk_inst(const uint8_t *val, uint8_t len)
{
   ie_.assoni = 1;
   if (len > sizeof(ie_.ntwk_inst))
      len = sizeof(ie_.ntwk_inst);
   ie_.ntwk_inst_len = len;
   std::memcpy(ie_.ntwk_inst, val, ie_.ntwk_inst_len);
   setLength();
   return *this;
}

inline UserPlaneIpResourceInformationIE &UserPlaneIpResourceInformationIE::src_intfc(SourceInterfaceEnum val)
{
   ie_.assosi = 1;
   ie_.src_intfc = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline pfcp_user_plane_ip_rsrc_info_ie_t &UserPlaneIpResourceInformationIE::data()
{
   return ie_;
}

inline UserPlaneIpResourceInformationIE::UserPlaneIpResourceInformationIE(pfcp_user_plane_ip_rsrc_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_USER_PLANE_IP_RSRC_INFO, parent),
     ie_(ie)
{
}

inline uint16_t UserPlaneIpResourceInformationIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.teidri > 0 ? sizeof(ie_.teid_range) : 0) +
      (ie_.v4 ? sizeof(ie_.ipv4_address) : 0) +
      (ie_.v6 ? sizeof(ie_.ipv6_address) : 0) +
      (ie_.assoni ? ie_.ntwk_inst_len : 0) +
      (ie_.assosi ? 1 : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////
inline uint32_t UserPlaneInactivityTimerIE::user_plane_inact_timer() const
{
   return ie_.user_plane_inact_timer;
}

inline UserPlaneInactivityTimerIE &UserPlaneInactivityTimerIE::user_plane_inact_timer(uint32_t val)
{
   ie_.user_plane_inact_timer = val;
   setLength();
   return *this;
}
   
inline pfcp_user_plane_inact_timer_ie_t &UserPlaneInactivityTimerIE::data()
{
   return ie_;
}

inline UserPlaneInactivityTimerIE::UserPlaneInactivityTimerIE(pfcp_user_plane_inact_timer_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_USER_PLANE_INACT_TIMER, parent),
     ie_(ie)
{
}

inline uint16_t UserPlaneInactivityTimerIE::calculateLength()
{
   return sizeof(pfcp_user_plane_inact_timer_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline int64_t MultiplierIE::value_digits() const
{
   return ie_.value_digits;
}

inline int32_t MultiplierIE::exponent() const
{
   return ie_.exponent;
}

inline MultiplierIE &MultiplierIE::value_digits(int64_t val)
{
   ie_.value_digits = val;
   setLength();
   return *this;
}

inline MultiplierIE &MultiplierIE::exponent(int32_t val)
{
   ie_.exponent = val;
   setLength();
   return *this;
}
   
inline pfcp_multiplier_ie_t &MultiplierIE::data()
{
   return ie_;
}

inline MultiplierIE::MultiplierIE(pfcp_multiplier_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_MULTIPLIER, parent),
     ie_(ie)
{
}

inline uint16_t MultiplierIE::calculateLength()
{
   return sizeof(pfcp_multiplier_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t AggregatedUrrIdIE::urr_id_value() const
{
   return ie_.urr_id_value;
}

inline AggregatedUrrIdIE &AggregatedUrrIdIE::urr_id_value(uint32_t val)
{
   ie_.urr_id_value = val;
   setLength();
   return *this;
}
   
inline pfcp_agg_urr_id_ie_t &AggregatedUrrIdIE::data()
{
   return ie_;
}

inline AggregatedUrrIdIE::AggregatedUrrIdIE(pfcp_agg_urr_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_AGG_URR_ID, parent),
     ie_(ie)
{
}

inline uint16_t AggregatedUrrIdIE::calculateLength()
{
   return sizeof(pfcp_agg_urr_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool SubsequentVolumeQuotaIE::tovol() const
{
   return ie_.tovol;
}

inline Bool SubsequentVolumeQuotaIE::ulvol() const
{
   return ie_.ulvol;
}

inline Bool SubsequentVolumeQuotaIE::dlvol() const
{
   return ie_.dlvol;
}

inline SubsequentVolumeQuotaIE &SubsequentVolumeQuotaIE::total_volume(uint64_t val)
{
   ie_.tovol = 1;
   ie_.total_volume = val;
   setLength();
   return *this;
}

inline SubsequentVolumeQuotaIE &SubsequentVolumeQuotaIE::uplink_volume(uint64_t val)
{
   ie_.ulvol = 1;
   ie_.uplink_volume = val;
   setLength();
   return *this;
}

inline SubsequentVolumeQuotaIE &SubsequentVolumeQuotaIE::downlink_volume(uint64_t val)
{
   ie_.dlvol = 1;
   ie_.downlink_volume = val;
   setLength();
   return *this;
}

inline pfcp_sbsqnt_vol_quota_ie_t &SubsequentVolumeQuotaIE::data()
{
   return ie_;
}

inline SubsequentVolumeQuotaIE::SubsequentVolumeQuotaIE(pfcp_sbsqnt_vol_quota_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SBSQNT_VOL_QUOTA, parent),
     ie_(ie)
{
}

inline uint16_t SubsequentVolumeQuotaIE::calculateLength()
{
   return 
      1 + /* octet 5 */
      (ie_.tovol ? sizeof(ie_.total_volume) : 0) +
      (ie_.ulvol ? sizeof(ie_.uplink_volume) : 0) +
      (ie_.dlvol ? sizeof(ie_.downlink_volume) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t SubsequentTimeQuotaIE::time_quota_val() const
{
   return ie_.time_quota_val;
}

inline SubsequentTimeQuotaIE &SubsequentTimeQuotaIE::time_quota_val(uint32_t val)
{
   ie_.time_quota_val = val;
   setLength();
   return *this;
}
   
inline pfcp_sbsqnt_time_quota_ie_t &SubsequentTimeQuotaIE::data()
{
   return ie_;
}

inline SubsequentTimeQuotaIE::SubsequentTimeQuotaIE(pfcp_sbsqnt_time_quota_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SBSQNT_TIME_QUOTA, parent),
     ie_(ie)
{
}

inline uint16_t SubsequentTimeQuotaIE::calculateLength()
{
   return sizeof(pfcp_sbsqnt_time_quota_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool RqiIE::rqi() const
{
   return ie_.rqi;
}

inline RqiIE &RqiIE::rqi(Bool val)
{
   ie_.rqi = val ? 1 : 0;
   setLength();
   return *this;
}
   
inline pfcp_rqi_ie_t &RqiIE::data()
{
   return ie_;
}

inline RqiIE::RqiIE(pfcp_rqi_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_RQI, parent),
      ie_(ie)
{
}

inline uint16_t RqiIE::calculateLength()
{
   return sizeof(pfcp_rqi_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint8_t QfiIE::qfi_value() const
{
   return ie_.qfi_value;
}

inline QfiIE &QfiIE::qfi_value(uint8_t val)
{
   ie_.qfi_value = val;
   setLength();
   return *this;
}
   
inline pfcp_qfi_ie_t &QfiIE::data()
{
   return ie_;
}

inline QfiIE::QfiIE(pfcp_qfi_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_QFI, parent),
     ie_(ie)
{
}

inline uint16_t QfiIE::calculateLength()
{
   return sizeof(pfcp_qfi_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t QueryUrrReferenceIE::query_urr_ref_val() const
{
   return ie_.query_urr_ref_val;
}

inline QueryUrrReferenceIE &QueryUrrReferenceIE::query_urr_ref_val(uint32_t val)
{
   ie_.query_urr_ref_val = val;
   setLength();
   return *this;
}
   
inline pfcp_query_urr_ref_ie_t &QueryUrrReferenceIE::data()
{
   return ie_;
}

inline QueryUrrReferenceIE::QueryUrrReferenceIE(pfcp_query_urr_ref_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_QUERY_URR_REF, parent),
      ie_(ie)
{
}

inline uint16_t QueryUrrReferenceIE::calculateLength()
{
   return sizeof(pfcp_query_urr_ref_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool AdditionalUsageReportsInformationIE::auri() const
{
   return ie_.auri;
}

inline uint16_t AdditionalUsageReportsInformationIE::nbr_of_add_usage_rpts_val() const
{
   return ie_.nbr_of_add_usage_rpts_val;
}

inline AdditionalUsageReportsInformationIE &AdditionalUsageReportsInformationIE::auri(Bool val)
{
   ie_.auri = val ? 1 : 0;
   setLength();
   return *this;
}

inline AdditionalUsageReportsInformationIE &AdditionalUsageReportsInformationIE::nbr_of_add_usage_rpts_val(uint16_t val)
{
   ie_.nbr_of_add_usage_rpts_val = val;
   setLength();
   return *this;
}
   
inline pfcp_add_usage_rpts_info_ie_t &AdditionalUsageReportsInformationIE::data()
{
   return ie_;
}

inline AdditionalUsageReportsInformationIE::AdditionalUsageReportsInformationIE(pfcp_add_usage_rpts_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_ADD_USAGE_RPTS_INFO, parent),
     ie_(ie)
{
}

inline uint16_t AdditionalUsageReportsInformationIE::calculateLength()
{
   return sizeof(pfcp_add_usage_rpts_info_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint8_t TrafficEndpointIdIE::traffic_endpt_id_val() const
{
   return ie_.traffic_endpt_id_val;
}

inline TrafficEndpointIdIE &TrafficEndpointIdIE::traffic_endpt_id_val(uint8_t val)
{
   ie_.traffic_endpt_id_val = val;
   setLength();
   return *this;
}
   
inline pfcp_traffic_endpt_id_ie_t &TrafficEndpointIdIE::data()
{
   return ie_;
}

inline TrafficEndpointIdIE::TrafficEndpointIdIE(pfcp_traffic_endpt_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_TRAFFIC_ENDPT_ID, parent),
      ie_(ie)
{
}

inline uint16_t TrafficEndpointIdIE::calculateLength()
{
   return sizeof(pfcp_traffic_endpt_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool MacAddressIE::sour() const
{
   return ie_.sour;
}

inline Bool MacAddressIE::dest() const
{
   return ie_.dest;
}

inline Bool MacAddressIE::usou() const
{
   return ie_.usou;
}

inline Bool MacAddressIE::udes() const
{
   return ie_.udes;
}

inline const uint8_t *MacAddressIE::src_mac_addr_val() const
{
   return ie_.src_mac_addr_val;
}

inline const uint8_t *MacAddressIE::dst_mac_addr_val() const
{
   return ie_.dst_mac_addr_val;
}

inline const uint8_t *MacAddressIE::upr_src_mac_addr_val() const
{
   return ie_.upr_src_mac_addr_val;
}

inline const uint8_t *MacAddressIE::upr_dst_mac_addr_val() const
{
   return ie_.upr_dst_mac_addr_val;
}

inline MacAddressIE &MacAddressIE::src_mac_addr_val(const uint8_t *val)
{
   ie_.sour = 1;
   std::memcpy(ie_.src_mac_addr_val, val, sizeof(ie_.src_mac_addr_val));
   setLength();
   return *this;
}
   
inline MacAddressIE &MacAddressIE::dst_mac_addr_val(const uint8_t *val)
{
   ie_.dest = 1;
   std::memcpy(ie_.dst_mac_addr_val, val, sizeof(ie_.dst_mac_addr_val));
   setLength();
   return *this;
}
   
inline MacAddressIE &MacAddressIE::upr_dst_mac_addr_val(const uint8_t *val)
{
   ie_.usou = 1;
   std::memcpy(ie_.upr_dst_mac_addr_val, val, sizeof(ie_.upr_dst_mac_addr_val));
   setLength();
   return *this;
}
   
inline MacAddressIE &MacAddressIE::upr_src_mac_addr_val(const uint8_t *val)
{
   ie_.udes = 1;
   std::memcpy(ie_.upr_src_mac_addr_val, val, sizeof(ie_.upr_src_mac_addr_val));
   setLength();
   return *this;
}
   
inline pfcp_mac_address_ie_t &MacAddressIE::data()
{
   return ie_;
}

inline MacAddressIE::MacAddressIE(pfcp_mac_address_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_MAC_ADDRESS, parent),
     ie_(ie)
{
}

inline uint16_t MacAddressIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.sour ? sizeof(ie_.src_mac_addr_val) : 0) +
      (ie_.dest ? sizeof(ie_.dst_mac_addr_val) : 0) +
      (ie_.usou ? sizeof(ie_.upr_src_mac_addr_val) : 0) +
      (ie_.udes ? sizeof(ie_.upr_dst_mac_addr_val) : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline Bool CTagIE::pcp() const
{
   return ie_.ctag_pcp;
}

inline Bool CTagIE::dei() const
{
   return ie_.ctag_dei;
}

inline Bool CTagIE::vid() const
{
   return ie_.ctag_vid;
}

inline uint8_t CTagIE::pcp_value() const
{
   return ie_.ctag_pcp_value;
}

inline Bool CTagIE::dei_flag() const
{
   return ie_.ctag_dei_flag;
}

inline uint8_t CTagIE::cvid_value() const
{
   return ie_.cvid_value;
}

inline uint8_t CTagIE::cvid_value2() const
{
   return ie_.cvid_value2;
}

inline CTagIE &CTagIE::pcp(Bool val)
{
   ie_.ctag_pcp = val ? 1 : 0;
   setLength();
   return *this;
}

inline CTagIE &CTagIE::dei(Bool val)
{
   ie_.ctag_dei = val ? 1 : 0;
   setLength();
   return *this;
}

inline CTagIE &CTagIE::vid(Bool val)
{
   ie_.ctag_vid = val ? 1 : 0;
   setLength();
   return *this;
}

inline CTagIE &CTagIE::pcp_value(uint8_t val)
{
   ie_.ctag_pcp_value = val;
   setLength();
   return *this;
}

inline CTagIE &CTagIE::dei_flag(Bool val)
{
   ie_.ctag_dei_flag = val ? 1 : 0;
   setLength();
   return *this;
}

inline CTagIE &CTagIE::cvid_value(uint8_t val)
{
   ie_.cvid_value = val;
   setLength();
   return *this;
}

inline CTagIE &CTagIE::cvid_value2(uint8_t val)
{
   ie_.cvid_value2 = val;
   setLength();
   return *this;
}

inline pfcp_ctag_ie_t &CTagIE::data()
{
   return ie_;
}

inline CTagIE::CTagIE(pfcp_ctag_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_CTAG, parent),
     ie_(ie)
{
}

inline uint16_t CTagIE::calculateLength()
{
   return sizeof(pfcp_ctag_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool STagIE::pcp() const
{
   return ie_.stag_pcp;
}

inline Bool STagIE::dei() const
{
   return ie_.stag_dei;
}

inline Bool STagIE::vid() const
{
   return ie_.stag_vid;
}

inline uint8_t STagIE::pcp_value() const
{
   return ie_.stag_pcp_value;
}

inline Bool STagIE::dei_flag() const
{
   return ie_.stag_dei_flag;
}

inline uint8_t STagIE::svid_value() const
{
   return ie_.svid_value;
}

inline uint8_t STagIE::svid_value2() const
{
   return ie_.svid_value2;
}

inline STagIE &STagIE::pcp(Bool val)
{
   ie_.stag_pcp = val ? 1 : 0;
   setLength();
   return *this;
}

inline STagIE &STagIE::dei(Bool val)
{
   ie_.stag_dei = val ? 1 : 0;
   setLength();
   return *this;
}

inline STagIE &STagIE::vid(Bool val)
{
   ie_.stag_vid = val ? 1 : 0;
   setLength();
   return *this;
}

inline STagIE &STagIE::pcp_value(uint8_t val)
{
   ie_.stag_pcp_value = val;
   setLength();
   return *this;
}

inline STagIE &STagIE::dei_flag(Bool val)
{
   ie_.stag_dei_flag = val ? 1 : 0;
   setLength();
   return *this;
}

inline STagIE &STagIE::svid_value(uint8_t val)
{
   ie_.svid_value = val;
   setLength();
   return *this;
}

inline STagIE &STagIE::svid_value2(uint8_t val)
{
   ie_.svid_value2 = val;
   setLength();
   return *this;
}
   
inline pfcp_stag_ie_t &STagIE::data()
{
   return ie_;
}

inline STagIE::STagIE(pfcp_stag_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_STAG, parent),
     ie_(ie)
{
}

inline uint16_t STagIE::calculateLength()
{
   return sizeof(pfcp_stag_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint16_t EthertypeIE::ethertype() const
{
   return ie_.ethertype;
}

inline EthertypeIE &EthertypeIE::ethertype(uint16_t val)
{
   ie_.ethertype = val;
   setLength();
   return *this;
}

inline pfcp_ethertype_ie_t &EthertypeIE::data()
{
   return ie_;
}

inline EthertypeIE::EthertypeIE(pfcp_ethertype_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_ETHERTYPE, parent),
     ie_(ie)
{
}

inline uint16_t EthertypeIE::calculateLength()
{
   return sizeof(pfcp_ethertype_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool ProxyingIE::arp() const
{
   return ie_.arp;
}

inline Bool ProxyingIE::ins() const
{
   return ie_.ins;
}

inline ProxyingIE &ProxyingIE::arp(Bool val)
{
   ie_.arp = val ? 1 : 0;
   setLength();
   return *this;
}

inline ProxyingIE &ProxyingIE::ins(Bool val)
{
   ie_.ins = val ? 1 : 0;
   setLength();
   return *this;
}

inline pfcp_proxying_ie_t &ProxyingIE::data()
{
   return ie_;
}

inline ProxyingIE::ProxyingIE(pfcp_proxying_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_PROXYING, parent),
     ie_(ie)
{
}

inline uint16_t ProxyingIE::calculateLength()
{
   return sizeof(pfcp_proxying_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t EthernetFilterIdIE::eth_fltr_id_val() const
{
   return ie_.eth_fltr_id_val;
}

inline EthernetFilterIdIE &EthernetFilterIdIE::eth_fltr_id_val(uint32_t val)
{
   ie_.eth_fltr_id_val = val;
   setLength();
   return *this;
}

inline pfcp_eth_fltr_id_ie_t &EthernetFilterIdIE::data()
{
   return ie_;
}

inline EthernetFilterIdIE::EthernetFilterIdIE(pfcp_eth_fltr_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_ETH_FLTR_ID, parent),
     ie_(ie)
{
}

inline uint16_t EthernetFilterIdIE::calculateLength()
{
   return sizeof(pfcp_eth_fltr_id_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool EthernetFilterPropertiesIE::bide() const
{
   return ie_.bide;
}

inline EthernetFilterPropertiesIE &EthernetFilterPropertiesIE::bide(Bool val)
{
   ie_.bide = val ? 1 : 0;
   setLength();
   return *this;
}
   
inline pfcp_eth_fltr_props_ie_t &EthernetFilterPropertiesIE::data()
{
   return ie_;
}

inline EthernetFilterPropertiesIE::EthernetFilterPropertiesIE(pfcp_eth_fltr_props_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_ETH_FLTR_PROPS, parent),
     ie_(ie)
{
}

inline uint16_t EthernetFilterPropertiesIE::calculateLength()
{
   return sizeof(pfcp_eth_fltr_props_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint8_t SuggestedBufferingPacketsCountIE::pckt_cnt_val() const
{
   return ie_.pckt_cnt_val;
}

inline SuggestedBufferingPacketsCountIE &SuggestedBufferingPacketsCountIE::pckt_cnt_val(uint8_t val)
{
   ie_.pckt_cnt_val = val;
   setLength();
   return *this;
}
   
inline pfcp_suggstd_buf_pckts_cnt_ie_t &SuggestedBufferingPacketsCountIE::data()
{
   return ie_;
}

inline SuggestedBufferingPacketsCountIE::SuggestedBufferingPacketsCountIE(pfcp_suggstd_buf_pckts_cnt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SUGGSTD_BUF_PCKTS_CNT, parent),
     ie_(ie)
{
}

inline uint16_t SuggestedBufferingPacketsCountIE::calculateLength()
{
   return sizeof(pfcp_suggstd_buf_pckts_cnt_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline Bool UserIdIE::imsif() const
{
   return ie_.imsif;
}

inline Bool UserIdIE::imeif() const
{
   return ie_.imeif;
}

inline Bool UserIdIE::msisdnf() const
{
   return ie_.msisdnf;
}

inline Bool UserIdIE::naif() const
{
   return ie_.naif;
}

inline uint8_t UserIdIE::length_of_imsi() const
{
   return ie_.length_of_imsi;
}

inline uint8_t UserIdIE::length_of_imei() const
{
   return ie_.length_of_imei;
}

inline uint8_t UserIdIE::len_of_msisdn() const
{
   return ie_.len_of_msisdn;
}

inline uint8_t UserIdIE::length_of_nai() const
{
   return ie_.length_of_nai;
}

inline const uint8_t *UserIdIE::imsi() const
{
   return ie_.imsi;
}

inline const uint8_t *UserIdIE::imei() const
{
   return ie_.imei;
}

inline const uint8_t *UserIdIE::msisdn() const
{
   return ie_.msisdn;
}

inline const uint8_t *UserIdIE::nai() const
{
   return ie_.nai;
}

inline UserIdIE &UserIdIE::imsi(const uint8_t *val, uint8_t len)
{
   ie_.imsif = 1;
   if (len > sizeof(ie_.imsi))
      len = sizeof(ie_.imsi);
   ie_.length_of_imsi = len;
   std::memcpy(ie_.imsi, val, len);
   return *this;
}

inline UserIdIE &UserIdIE::imei(const uint8_t *val, uint8_t len)
{
   ie_.imeif = 1;
   if (len > sizeof(ie_.imei))
      len = sizeof(ie_.imei);
   ie_.length_of_imei = len;
   std::memcpy(ie_.imei, val, len);
   return *this;
}

inline UserIdIE &UserIdIE::msisdn(const uint8_t *val, uint8_t len)
{
   ie_.msisdnf = 1;
   if (len > sizeof(ie_.msisdn))
      len = sizeof(ie_.msisdn);
   ie_.len_of_msisdn = len;
   std::memcpy(ie_.msisdn, val, len);
   return *this;
}

inline UserIdIE &UserIdIE::nai(const uint8_t *val, uint8_t len)
{
   ie_.naif = 1;
   if (len > sizeof(ie_.nai))
      len = sizeof(ie_.nai);
   ie_.length_of_nai = len;
   std::memcpy(ie_.nai, val, len);
   return *this;
}
   
inline pfcp_user_id_ie_t &UserIdIE::data()
{
   return ie_;
}

inline UserIdIE::UserIdIE(pfcp_user_id_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_USER_ID, parent),
      ie_(ie)
{
}

inline uint16_t UserIdIE::calculateLength()
{
   return
      1 + /* octet 5 */
      (ie_.imsif ? sizeof(ie_.length_of_imsi) + ie_.length_of_imsi : 0) +
      (ie_.imeif ? sizeof(ie_.length_of_imei) + ie_.length_of_imei : 0) +
      (ie_.msisdnf ? sizeof(ie_.len_of_msisdn) + ie_.len_of_msisdn : 0) +
      (ie_.naif ? sizeof(ie_.length_of_nai) + ie_.length_of_nai : 0)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline Bool EthernetPduSessionInformationIE::ethi() const
{
   return ie_.ethi;
}

inline EthernetPduSessionInformationIE &EthernetPduSessionInformationIE::ethi(Bool val)
{
   ie_.ethi = val ? 1 : 0;
   setLength();
   return *this;
}
   
inline pfcp_eth_pdu_sess_info_ie_t &EthernetPduSessionInformationIE::data()
{
   return ie_;
}

inline EthernetPduSessionInformationIE::EthernetPduSessionInformationIE(pfcp_eth_pdu_sess_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_ETH_PDU_SESS_INFO, parent),
      ie_(ie)
{
}

inline uint16_t EthernetPduSessionInformationIE::calculateLength()
{
   return sizeof(pfcp_eth_pdu_sess_info_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint8_t MacAddressesDetectedIE::nbr_of_mac_addrs() const
{
   return ie_.nbr_of_mac_addrs;
}

inline const uint8_t *MacAddressesDetectedIE::mac_addr_val(uint8_t idx)
{
   if (idx > MAC_ADDR_VAL_LEN)
      return nullptr;
   return ie_.mac_addr_val[idx];
}

inline MacAddressesDetectedIE &MacAddressesDetectedIE::mac_addr_val(const uint8_t *val)
{
   if (ie_.nbr_of_mac_addrs >= MAC_ADDR_VAL_LEN)
      return *this;
   std::memcpy(ie_.mac_addr_val[ie_.nbr_of_mac_addrs], val, 6);
   ie_.nbr_of_mac_addrs++;
   setLength();
   return *this;
}
   
inline pfcp_mac_addrs_detctd_ie_t &MacAddressesDetectedIE::data()
{
   return ie_;
}

inline MacAddressesDetectedIE::MacAddressesDetectedIE(pfcp_mac_addrs_detctd_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_MAC_ADDRS_DETCTD, parent),
     ie_(ie)
{
}

inline uint16_t MacAddressesDetectedIE::calculateLength()
{
   return
      sizeof(ie_.nbr_of_mac_addrs) +
      (ie_.nbr_of_mac_addrs * 6)
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline uint8_t MacAddressesRemovedIE::nbr_of_mac_addrs() const
{
   return ie_.nbr_of_mac_addrs;
}

inline const uint8_t *MacAddressesRemovedIE::mac_addr_val(uint8_t idx)
{
   if (idx > MAC_ADDR_VAL_LEN)
      return nullptr;
   return ie_.mac_addr_val[idx];
}

inline MacAddressesRemovedIE &MacAddressesRemovedIE::mac_addr_val(const uint8_t *val)
{
   if (ie_.nbr_of_mac_addrs >= MAC_ADDR_VAL_LEN)
      return *this;
   std::memcpy(ie_.mac_addr_val[ie_.nbr_of_mac_addrs], val, 6);
   ie_.nbr_of_mac_addrs++;
   setLength();
   return *this;
}

inline pfcp_mac_addrs_rmvd_ie_t &MacAddressesRemovedIE::data()
{
   return ie_;
}

inline MacAddressesRemovedIE::MacAddressesRemovedIE(pfcp_mac_addrs_rmvd_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_MAC_ADDRS_RMVD, parent),
     ie_(ie)
{
}

inline uint16_t MacAddressesRemovedIE::calculateLength()
{
   return sizeof(pfcp_mac_addrs_rmvd_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t EthernetInactivityTimerIE::eth_inact_timer() const
{
   return ie_.eth_inact_timer;
}

inline EthernetInactivityTimerIE &EthernetInactivityTimerIE::eth_inact_timer(uint32_t val)
{
   ie_.eth_inact_timer = val;
   setLength();
   return *this;
}
   
inline pfcp_eth_inact_timer_ie_t &EthernetInactivityTimerIE::data()
{
   return ie_;
}

inline EthernetInactivityTimerIE::EthernetInactivityTimerIE(pfcp_eth_inact_timer_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_ETH_INACT_TIMER, parent),
      ie_(ie)
{
}

inline uint16_t EthernetInactivityTimerIE::calculateLength()
{
   return sizeof(pfcp_eth_inact_timer_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t SubsequentEventQuotaIE::sbsqnt_evnt_quota() const
{
   return ie_.sbsqnt_evnt_quota;
}

inline SubsequentEventQuotaIE &SubsequentEventQuotaIE::sbsqnt_evnt_quota(uint32_t val)
{
   ie_. sbsqnt_evnt_quota = val;
   setLength();
   return *this;
}
   
inline pfcp_sbsqnt_evnt_quota_ie_t &SubsequentEventQuotaIE::data()
{
   return ie_;
}

inline SubsequentEventQuotaIE::SubsequentEventQuotaIE(pfcp_sbsqnt_evnt_quota_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SBSQNT_EVNT_QUOTA, parent),
     ie_(ie)
{
}

inline uint16_t SubsequentEventQuotaIE::calculateLength()
{
   return sizeof(pfcp_sbsqnt_evnt_quota_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t SubsequentEventThresholdIE::sbsqnt_evnt_thresh() const
{
   return ie_.sbsqnt_evnt_thresh;
}

inline SubsequentEventThresholdIE &SubsequentEventThresholdIE::sbsqnt_evnt_thresh(uint32_t val)
{
   ie_.sbsqnt_evnt_thresh = val;
   setLength();
   return *this;
}
   
inline pfcp_sbsqnt_evnt_thresh_ie_t &SubsequentEventThresholdIE::data()
{
   return ie_;
}

inline SubsequentEventThresholdIE::SubsequentEventThresholdIE(pfcp_sbsqnt_evnt_thresh_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_SBSQNT_EVNT_THRESH, parent),
      ie_(ie)
{
}

inline uint16_t SubsequentEventThresholdIE::calculateLength()
{
   return sizeof(pfcp_sbsqnt_evnt_thresh_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint8_t TraceInformationIE::mcc_digit_1() const
{
   return ie_.mcc_digit_1;
}

inline uint8_t TraceInformationIE::mcc_digit_2() const
{
   return ie_.mcc_digit_2;
}

inline uint8_t TraceInformationIE::mcc_digit_3() const
{
   return ie_.mcc_digit_3;
}

inline uint8_t TraceInformationIE::mnc_digit_1() const
{
   return ie_.mnc_digit_1;
}

inline uint8_t TraceInformationIE::mnc_digit_2() const
{
   return ie_.mnc_digit_2;
}

inline uint8_t TraceInformationIE::mnc_digit_3() const
{
   return ie_.mnc_digit_3;
}

inline uint32_t TraceInformationIE::trace_id() const
{
   return ie_.trace_id;
}

inline uint8_t TraceInformationIE::len_of_trigrng_evnts() const
{
   return ie_.len_of_trigrng_evnts;
}

inline const uint8_t *TraceInformationIE::trigrng_evnts() const
{
   return ie_.trigrng_evnts;
}

inline uint16_t TraceInformationIE::sess_trc_depth() const
{
   return ie_.sess_trc_depth;
}

inline uint32_t TraceInformationIE::len_of_list_of_intfcs() const
{
   return ie_.len_of_list_of_intfcs;
}

inline const uint8_t *TraceInformationIE::list_of_intfcs() const
{
   return ie_.list_of_intfcs;
}

inline uint16_t TraceInformationIE::len_of_ip_addr_of_trc_coll_ent() const
{
   return ie_.len_of_ip_addr_of_trc_coll_ent;
}

inline in_addr &TraceInformationIE::ipv4_addr_of_trc_coll_ent() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.ip_addr_of_trc_coll_ent;
   return *u.in_addr_ptr;
}

inline in6_addr &TraceInformationIE::ipv6_addr_of_trc_coll_ent() const
{
   AliasPointer u;
   u.uint8_t_ptr = ie_.ip_addr_of_trc_coll_ent;
   return *u.in6_addr_ptr;
}

inline TraceInformationIE &TraceInformationIE::mcc(const char *val, uint8_t len)
{
   if (len != 3)
      return *this;
   ie_.mcc_digit_1 = val[0] - '0';
   ie_.mcc_digit_2 = val[1] - '0';
   ie_.mcc_digit_3 = val[2] - '0';
   setLength();
   return *this;
}

inline TraceInformationIE &TraceInformationIE::mnc(const char *val, uint8_t len)
{
   if (len < 2 || len > 3)
      return *this;
   ie_.mnc_digit_1 = val[0];
   ie_.mnc_digit_2 = val[1];
   ie_.mnc_digit_3 = len == 2 ? 15 : val[2];
   setLength();
   return *this;
}

inline TraceInformationIE &TraceInformationIE::plmnid(const uint8_t *val)
{
   std::memcpy(reinterpret_cast<uint8_t*>(&ie_) + sizeof(pfcp_ie_header_t), val, 3);
   setLength();
   return *this;
}

inline TraceInformationIE &TraceInformationIE::trace_id(uint32_t val)
{
   ie_.trace_id = val;
   setLength();
   return *this;
}

inline TraceInformationIE &TraceInformationIE::trigrng_evnts(const uint8_t *val)
{
   ie_.len_of_trigrng_evnts = sizeof(ie_.trigrng_evnts);
   std::memcpy(ie_.trigrng_evnts, val, sizeof(ie_.trigrng_evnts));
   setLength();
   return *this;
}

inline TraceInformationIE &TraceInformationIE::list_of_intfcs(const uint8_t *val)
{
   ie_.len_of_list_of_intfcs = sizeof(ie_.list_of_intfcs);
   std::memcpy(ie_.list_of_intfcs, val, sizeof(ie_.list_of_intfcs));
   setLength();
   return *this;
}

inline TraceInformationIE &TraceInformationIE::ip_addr_of_trc_coll_ent(const ESocket::Address &val)
{
   if (val.getFamily() == ESocket::Family::INET)
      return ip_addr_of_trc_coll_ent(val.getInet().sin_addr);
   else if (val.getFamily() == ESocket::Family::INET6)
      return ip_addr_of_trc_coll_ent(val.getInet6().sin6_addr);
   return *this;
}

inline TraceInformationIE &TraceInformationIE::ip_addr_of_trc_coll_ent(const EIpAddress &val)
{
   if (val.family() == AF_INET)
      return ip_addr_of_trc_coll_ent(val.ipv4Address());
   else if (val.family() == AF_INET6)
      return ip_addr_of_trc_coll_ent(val.ipv6Address());
   return *this;
}

inline TraceInformationIE &TraceInformationIE::ip_addr_of_trc_coll_ent(const in_addr &val)
{
   ie_.len_of_ip_addr_of_trc_coll_ent = sizeof(in_addr);
   std::memcpy(ie_.ip_addr_of_trc_coll_ent, &val, sizeof(in_addr));
   setLength();
   return *this;
}

inline TraceInformationIE &TraceInformationIE::ip_addr_of_trc_coll_ent(const in6_addr &val)
{
   ie_.len_of_ip_addr_of_trc_coll_ent = sizeof(in6_addr);
   std::memcpy(ie_.ip_addr_of_trc_coll_ent, &val, sizeof(in6_addr));
   setLength();
   return *this;
}

inline pfcp_trc_info_ie_t &TraceInformationIE::data()
{
   return ie_;
}

inline TraceInformationIE::TraceInformationIE(pfcp_trc_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_TRC_INFO, parent),
      ie_(ie)
{
}

inline uint16_t TraceInformationIE::calculateLength()
{
   return
      3 + /* mcc/mnc */
      3 + /* trace id */
      sizeof(ie_.len_of_trigrng_evnts) + ie_.len_of_trigrng_evnts +
      sizeof(ie_.len_of_list_of_intfcs) + ie_.len_of_list_of_intfcs +
      sizeof(ie_.len_of_ip_addr_of_trc_coll_ent) + ie_.len_of_ip_addr_of_trc_coll_ent
   ;
}

////////////////////////////////////////////////////////////////////////////////

inline uint16_t FramedRouteIE::framed_route_len() const
{
   return ie_.header.len;
}

inline const uint8_t *FramedRouteIE::framed_route() const
{
   return ie_.framed_route;
}

inline FramedRouteIE &FramedRouteIE::framed_route(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.framed_route))
      len = sizeof(ie_.framed_route);
   std::memcpy(ie_.framed_route, val, len);
   ie_.header.len = len;
   return *this;
}
   
inline pfcp_framed_route_ie_t &FramedRouteIE::data()
{
   return ie_;
}

inline FramedRouteIE::FramedRouteIE(pfcp_framed_route_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FRAMED_ROUTE, parent),
      ie_(ie)
{
}

inline uint16_t FramedRouteIE::calculateLength()
{
   return sizeof(pfcp_framed_route_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline FramedRoutingEnum FramedRoutingIE::framed_routing() const
{
   return static_cast<FramedRoutingEnum>(ie_.framed_routing);
}

inline FramedRoutingIE &FramedRoutingIE::framed_routing(FramedRoutingEnum val)
{
   ie_.framed_routing = static_cast<uint32_t>(val);
   setLength();
   return *this;
}
   
inline pfcp_framed_routing_ie_t &FramedRoutingIE::data()
{
   return ie_;
}

inline FramedRoutingIE::FramedRoutingIE(pfcp_framed_routing_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FRAMED_ROUTING, parent),
     ie_(ie)
{
}

inline uint16_t FramedRoutingIE::calculateLength()
{
   return sizeof(pfcp_framed_routing_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint16_t FramedIpv6RouteIE::frmd_ipv6_rte_len() const
{
   return ie_.header.len;
}

inline const uint8_t *FramedIpv6RouteIE::frmd_ipv6_rte() const
{
   return ie_.frmd_ipv6_rte;
}

inline FramedIpv6RouteIE &FramedIpv6RouteIE::frmd_ipv6_rte(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.frmd_ipv6_rte))
      len = sizeof(ie_.frmd_ipv6_rte);
   std::memcpy(ie_.frmd_ipv6_rte, val, len);
   ie_.header.len = len;
   return *this;
}

inline pfcp_frmd_ipv6_rte_ie_t &FramedIpv6RouteIE::data()
{
   return ie_;
}

inline FramedIpv6RouteIE::FramedIpv6RouteIE(pfcp_frmd_ipv6_rte_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_FRMD_IPV6_RTE, parent),
    ie_(ie)
{
}

inline uint16_t FramedIpv6RouteIE::calculateLength()
{
   return sizeof(pfcp_frmd_ipv6_rte_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t EventQuotaIE::sbsqnt_evnt_quota() const
{
   return ie_.event_quota;
}

inline EventQuotaIE &EventQuotaIE::sbsqnt_evnt_quota(uint32_t val)
{
   ie_.event_quota = val;
   setLength();
   return *this;
}
   
inline pfcp_event_quota_ie_t &EventQuotaIE::data()
{
   return ie_;
}

inline EventQuotaIE::EventQuotaIE(pfcp_event_quota_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_EVENT_QUOTA, parent),
     ie_(ie)
{
}

inline uint16_t EventQuotaIE::calculateLength()
{
   return sizeof(pfcp_event_quota_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t EventThresholdIE::event_threshold() const
{
   return ie_.event_threshold;
}

inline EventThresholdIE &EventThresholdIE::event_threshold(uint32_t val)
{
   ie_.event_threshold = val;
   setLength();
   return *this;
}
   
inline pfcp_event_threshold_ie_t &EventThresholdIE::data()
{
   return ie_;
}

inline EventThresholdIE::EventThresholdIE(pfcp_event_threshold_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_EVENT_THRESHOLD, parent),
     ie_(ie)
{
}

inline uint16_t EventThresholdIE::calculateLength()
{
   return sizeof(pfcp_event_threshold_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline ETime EventTimeStampIE::evnt_time_stmp() const
{
   ETime t;
   t.setNTPTime(ie_.evnt_time_stmp);
   return t;
}

inline EventTimeStampIE &EventTimeStampIE::evnt_time_stmp(const ETime &val)
{
   ie_.evnt_time_stmp = val.getNTPTimeSeconds();
   setLength();
   return *this;
}
   
inline pfcp_evnt_time_stmp_ie_t &EventTimeStampIE::data()
{
   return ie_;
}

inline EventTimeStampIE::EventTimeStampIE(pfcp_evnt_time_stmp_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_EVNT_TIME_STMP, parent),
     ie_(ie)
{
}

inline uint16_t EventTimeStampIE::calculateLength()
{
   return sizeof(pfcp_evnt_time_stmp_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint32_t AveragingWindowIE::avgng_wnd() const
{
   return ie_.avgng_wnd;
}

inline AveragingWindowIE &AveragingWindowIE::avgng_wnd(uint32_t val)
{
   ie_.avgng_wnd = val;
   setLength();
   return *this;
}
   
inline pfcp_avgng_wnd_ie_t &AveragingWindowIE::data()
{
   return ie_;
}

inline AveragingWindowIE::AveragingWindowIE(pfcp_avgng_wnd_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_AVGNG_WND, parent),
     ie_(ie)
{
}

inline uint16_t AveragingWindowIE::calculateLength()
{
   return sizeof(pfcp_avgng_wnd_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint8_t PagingPolicyIndicatorIE::ppi_value() const
{
   return ie_.ppi_value;
}

inline PagingPolicyIndicatorIE &PagingPolicyIndicatorIE::ppi_value(uint8_t val)
{
   ie_.ppi_value = val;
   setLength();
   return *this;
}
   
inline pfcp_paging_plcy_indctr_ie_t &PagingPolicyIndicatorIE::data()
{
   return ie_;
}

inline PagingPolicyIndicatorIE::PagingPolicyIndicatorIE(pfcp_paging_plcy_indctr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_PAGING_PLCY_INDCTR, parent),
     ie_(ie)
{
}

inline uint16_t PagingPolicyIndicatorIE::calculateLength()
{
   return sizeof(pfcp_paging_plcy_indctr_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline uint16_t ApnDnnIE::apn_dnn_len() const
{
   return ie_.header.len;
}

inline const uint8_t *ApnDnnIE::apn_dnn() const
{
   return ie_.apn_dnn;
}

inline ApnDnnIE &ApnDnnIE::apn_dnn(const uint8_t *val, uint16_t len)
{
   if (len > sizeof(ie_.apn_dnn))
      len = sizeof(ie_.apn_dnn);
   std::memcpy(ie_.apn_dnn, val, len);
   ie_.header.len = len;
   return *this;
}
   
inline pfcp_apn_dnn_ie_t &ApnDnnIE::data()
{
   return ie_;
}

inline ApnDnnIE::ApnDnnIE(pfcp_apn_dnn_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_APN_DNN, parent),
     ie_(ie)
{
}

inline uint16_t ApnDnnIE::calculateLength()
{
   return sizeof(pfcp_apn_dnn_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline TgppInterfaceTypeEnum TgppInterfaceTypeIE::intfc_type_val() const
{
   return static_cast<TgppInterfaceTypeEnum>(ie_.intfc_type_val);
}

inline TgppInterfaceTypeIE &TgppInterfaceTypeIE::intfc_type_val(TgppInterfaceTypeEnum val)
{
   ie_.intfc_type_val = static_cast<uint8_t>(val);
   setLength();
   return *this;
}

inline pfcp_3gpp_intfc_type_ie_t &TgppInterfaceTypeIE::data()
{
   return ie_;
}

inline TgppInterfaceTypeIE::TgppInterfaceTypeIE(pfcp_3gpp_intfc_type_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, PFCP_IE_3GPP_INTFC_TYPE, parent),
      ie_(ie)
{
}

inline uint16_t TgppInterfaceTypeIE::calculateLength()
{
   return sizeof(pfcp_3gpp_intfc_type_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////
//// Grouped IE Classes
////////////////////////////////////////////////////////////////////////////////

inline PfdContentsIE &PfdContextIE::pfdContents(uint8_t idx)
{
   return pfdcnts_[idx];
}

inline pfcp_pfd_context_ie_t &PfdContextIE::data()
{
   return ie_;
}

inline PfdContextIE::PfdContextIE(pfcp_pfd_context_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_PFD_CONTEXT, parent),
     ie_(ie)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
      pfdcnts_.push_back(PfdContentsIE(ie_.pfd_contents[i],this));
}

inline uint16_t PfdContextIE::calculateLength()
{
   uint16_t len = 0;
   for (int idx=0; idx<ie_.pfd_contents_count; idx++)
      len += pfdcnts_[idx].packedLength();
   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline ApplicationIdIE &ApplicationIdsPfdsIE::applicationId()
{
   return appid_;
}

inline PfdContextIE &ApplicationIdsPfdsIE::pfdContext(uint8_t idx)
{
   return pfdctxts_[idx];
}

inline int ApplicationIdsPfdsIE::nextPfdContext()
{
   return (ie_.pfd_context_count < MAX_LIST_SIZE) ?
      ie_.pfd_context_count++ : -1;
}
   
inline pfcp_app_ids_pfds_ie_t &ApplicationIdsPfdsIE::data()
{
   return ie_;
}

inline ApplicationIdsPfdsIE::ApplicationIdsPfdsIE(pfcp_app_ids_pfds_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_APP_IDS_PFDS, parent),
      ie_(ie),
      appid_(ie_.application_id, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
      pfdctxts_.push_back(PfdContextIE(ie_.pfd_context[i], this));
}

inline uint16_t ApplicationIdsPfdsIE::calculateLength()
{
   uint16_t len = appid_.packedLength();
   for (int i=0; i<ie_.pfd_context_count; i++)
      len += pfdctxts_[i].packedLength();
   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline TrafficEndpointIdIE &CreateTrafficEndpointIE::traffic_endpt_id()
{
   return teid_;
}

inline FTeidIE &CreateTrafficEndpointIE::local_fteid()
{
   return lfteid_;
}

inline NetworkInstanceIE &CreateTrafficEndpointIE::ntwk_inst()
{
   return ni_;
}

inline UeIpAddressIE &CreateTrafficEndpointIE::ue_ip_address()
{
   return ueip_;
}

inline EthernetPduSessionInformationIE &CreateTrafficEndpointIE::eth_pdu_sess_info()
{
   return epsi_;
}

inline FramedRouteIE &CreateTrafficEndpointIE::framed_route(uint8_t idx)
{
   return fr_[idx];
}

inline FramedRoutingIE &CreateTrafficEndpointIE::framed_routing()
{
   return fring_;
}

inline FramedIpv6RouteIE &CreateTrafficEndpointIE::frmd_ipv6_rte(uint8_t idx)
{
   return fr6_[idx];
}

inline int CreateTrafficEndpointIE::next_framed_route()
{
   return (ie_.framed_route_count < MAX_LIST_SIZE) ?
      ie_.framed_route_count++ : -1;
}

inline int CreateTrafficEndpointIE::next_frmd_ipv6_rte()
{
   return (ie_.frmd_ipv6_rte_count < MAX_LIST_SIZE) ?
      ie_.frmd_ipv6_rte_count++ : -1;
}

inline pfcp_create_traffic_endpt_ie_t &CreateTrafficEndpointIE::data()
{
   return ie_;
}

inline CreateTrafficEndpointIE::CreateTrafficEndpointIE(pfcp_create_traffic_endpt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_CREATE_TRAFFIC_ENDPT, parent),
     ie_(ie),
     teid_(ie_.traffic_endpt_id, this),
     lfteid_(ie_.local_fteid, this),
     ni_(ie_.ntwk_inst, this),
     ueip_(ie_.ue_ip_address, this),
     epsi_(ie_.eth_pdu_sess_info, this),
     fring_(ie_.framed_routing, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
      fr_.push_back(FramedRouteIE(ie_.framed_route[i],this));
   for (int i=0; i<MAX_LIST_SIZE; i++)
      fr6_.push_back(FramedIpv6RouteIE(ie_.frmd_ipv6_rte[i],this));
}

inline uint16_t CreateTrafficEndpointIE::calculateLength()
{
   uint16_t len = 0;

   len += teid_.packedLength();
   len += lfteid_.packedLength();
   len += ni_.packedLength();
   len += ueip_.packedLength();
   len += epsi_.packedLength();
   for (int i=0; i<ie_.framed_route_count; i++)
      len += fr_[i].packedLength();
   len += fring_.packedLength();
   for (int i=0; i<ie_.frmd_ipv6_rte_count; i++)
      len += fr6_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline BarIdIE &CreateBarIE::bar_id()
{
   return barid_;
}

inline DownlinkDataNotificationDelayIE &CreateBarIE::dnlnk_data_notif_delay()
{
   return ddndelay_;
}

inline SuggestedBufferingPacketsCountIE &CreateBarIE::suggstd_buf_pckts_cnt()
{
   return sbpc_;
}
   
inline pfcp_create_bar_ie_t &CreateBarIE::data()
{
   return ie_;
}

inline CreateBarIE::CreateBarIE(pfcp_create_bar_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_CREATE_BAR, parent),
     ie_(ie),
     barid_(ie_.bar_id, this),
     ddndelay_(ie_.dnlnk_data_notif_delay, this),
     sbpc_(ie_.suggstd_buf_pckts_cnt, this)
{
}

inline uint16_t CreateBarIE::calculateLength()
{
   return sizeof(pfcp_create_bar_ie_t) - sizeof(pfcp_ie_header_t);
}

////////////////////////////////////////////////////////////////////////////////

inline QerIdIE &CreateQerIE::qer_id()
{
   return qerid_;
}

inline QerCorrelationIdIE &CreateQerIE::qer_corr_id()
{
   return qci_;
}

inline GateStatusIE &CreateQerIE::gate_status()
{
   return gs_;
}

inline MbrIE &CreateQerIE::maximum_bitrate()
{
   return mbr_;
}

inline GbrIE &CreateQerIE::guaranteed_bitrate()
{
   return gbr_;
}

inline PacketRateIE &CreateQerIE::packet_rate()
{
   return pr_;
}

inline DlFlowLevelMarkingIE &CreateQerIE::dl_flow_lvl_marking()
{
   return dfm_;
}

inline QfiIE &CreateQerIE::qos_flow_ident()
{
   return qfi_;
}

inline RqiIE &CreateQerIE::reflective_qos()
{
   return rqi_;
}

inline PagingPolicyIndicatorIE &CreateQerIE::paging_plcy_indctr()
{
   return ppi_;
}

inline AveragingWindowIE &CreateQerIE::avgng_wnd()
{
   return aw_;
}
   
inline pfcp_create_qer_ie_t &CreateQerIE::data()
{
   return ie_;
}

inline CreateQerIE::CreateQerIE(pfcp_create_qer_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_CREATE_QER, parent),
     ie_(ie),
     qerid_(ie_.qer_id, this),
     qci_(ie_.qer_corr_id, this),
     gs_(ie_.gate_status, this),
     mbr_(ie_.maximum_bitrate, this),
     gbr_(ie_.guaranteed_bitrate, this),
     pr_(ie_.packet_rate, this),
     dfm_(ie_.dl_flow_lvl_marking, this),
     qfi_(ie_.qos_flow_ident, this),
     rqi_(ie_.reflective_qos, this),
     ppi_(ie_.paging_plcy_indctr, this),
     aw_(ie_.avgng_wnd, this)
{
}

inline uint16_t CreateQerIE::calculateLength()
{
   return
      qerid_.packedLength() +
      qci_.packedLength() +
      gs_.packedLength() +
      mbr_.packedLength() +
      gbr_.packedLength() +
      pr_.packedLength() +
      dfm_.packedLength() +
      qfi_.packedLength() +
      rqi_.packedLength() +
      ppi_.packedLength() +
      aw_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline MonitoringTimeIE &AdditionalMonitoringTimeIE::monitoring_time()
{
   return mt_;
}

inline SubsequentVolumeThresholdIE &AdditionalMonitoringTimeIE::sbsqnt_vol_thresh()
{
   return svt_;
}

inline SubsequentTimeThresholdIE &AdditionalMonitoringTimeIE::sbsqnt_time_thresh()
{
   return stt_;
}

inline SubsequentVolumeQuotaIE &AdditionalMonitoringTimeIE::sbsqnt_vol_quota()
{
   return svq_;
}

inline SubsequentTimeQuotaIE &AdditionalMonitoringTimeIE::sbsqnt_time_quota()
{
   return stq_;
}

inline SubsequentEventThresholdIE &AdditionalMonitoringTimeIE::sbsqnt_evnt_thresh()
{
   return set_;
}

inline SubsequentEventQuotaIE &AdditionalMonitoringTimeIE::sbsqnt_evnt_quota()
{
   return seq_;
}
   
inline pfcp_add_mntrng_time_ie_t &AdditionalMonitoringTimeIE::data()
{
   return ie_;
}

inline AdditionalMonitoringTimeIE::AdditionalMonitoringTimeIE(pfcp_add_mntrng_time_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_ADD_MNTRNG_TIME, parent),
     ie_(ie),
     mt_(ie_.monitoring_time, this),
     svt_(ie_.sbsqnt_vol_thresh, this),
     stt_(ie_.sbsqnt_time_thresh, this),
     svq_(ie_.sbsqnt_vol_quota, this),
     stq_(ie_.sbsqnt_time_quota, this),
     set_(ie_.sbsqnt_evnt_thresh, this),
     seq_(ie_.sbsqnt_evnt_quota, this)
{
}

inline uint16_t AdditionalMonitoringTimeIE::calculateLength()
{
   return
      mt_.packedLength() +
      svt_.packedLength() +
      stt_.packedLength() +
      svq_.packedLength() +
      stq_.packedLength() +
      set_.packedLength() +
      seq_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline AggregatedUrrIdIE &AggregatedUrrsIE::agg_urr_id()
{
   return aui_;
}

inline MultiplierIE &AggregatedUrrsIE::multiplier()
{
   return m_;
}

inline pfcp_aggregated_urrs_ie_t &AggregatedUrrsIE::data()
{
   return ie_;
}

inline AggregatedUrrsIE::AggregatedUrrsIE(pfcp_aggregated_urrs_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_AGGREGATED_URRS, parent),
     ie_(ie),
     aui_(ie_.agg_urr_id, this),
     m_(ie_.multiplier, this)
{
}

inline uint16_t AggregatedUrrsIE::calculateLength()
{
   return
      aui_.packedLength() +
      m_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline UrrIdIE &CreateUrrIE::urr_id()
{
   return ui_;
}

inline MeasurementMethodIE &CreateUrrIE::meas_mthd()
{
   return mm_;
}

inline ReportingTriggersIE &CreateUrrIE::rptng_triggers()
{
   return rt_;
}

inline MeasurementPeriodIE &CreateUrrIE::meas_period()
{
   return mp_;
}

inline VolumeThresholdIE &CreateUrrIE::vol_thresh()
{
   return vt_;
}

inline VolumeQuotaIE &CreateUrrIE::volume_quota()
{
   return vq_;
}

inline EventThresholdIE &CreateUrrIE::event_threshold()
{
   return et_;
}

inline EventQuotaIE &CreateUrrIE::event_quota()
{
   return eq_;
}

inline TimeThresholdIE &CreateUrrIE::time_threshold()
{
   return tt_;
}

inline TimeQuotaIE &CreateUrrIE::time_quota()
{
   return tq_;
}

inline QuotaHoldingTimeIE &CreateUrrIE::quota_hldng_time()
{
   return qht_;
}

inline DroppedDlTrafficThresholdIE &CreateUrrIE::drpd_dl_traffic_thresh()
{
   return ddtt_;
}

inline MonitoringTimeIE &CreateUrrIE::monitoring_time()
{
   return mt_;
}

inline SubsequentVolumeThresholdIE &CreateUrrIE::sbsqnt_vol_thresh()
{
   return svt_;
}

inline SubsequentTimeThresholdIE &CreateUrrIE::sbsqnt_time_thresh()
{
   return stt_;
}

inline SubsequentVolumeQuotaIE &CreateUrrIE::sbsqnt_vol_quota()
{
   return svq_;
}

inline SubsequentTimeQuotaIE &CreateUrrIE::sbsqnt_time_quota()
{
   return stq_;
}

inline SubsequentEventThresholdIE &CreateUrrIE::sbsqnt_evnt_thresh()
{
   return set_;
}

inline SubsequentEventQuotaIE &CreateUrrIE::sbsqnt_evnt_quota()
{
   return seq_;
}

inline InactivityDetectionTimeIE &CreateUrrIE::inact_det_time()
{
   return idt_;
}

inline LinkedUrrIdIE &CreateUrrIE::linked_urr_id(uint8_t idx)
{
   return lui_[idx];
}

inline MeasurementInformationIE &CreateUrrIE::meas_info()
{
   return mi_;
}

inline TimeQuotaMechanismIE &CreateUrrIE::time_quota_mech()
{
   return tqm_;
}

inline AggregatedUrrsIE &CreateUrrIE::aggregated_urrs(uint8_t idx)
{
   return aus_[idx];
}

inline FarIdIE &CreateUrrIE::far_id_for_quota_act()
{
   return fiqa_;
}

inline EthernetInactivityTimerIE &CreateUrrIE::eth_inact_timer()
{
   return eit_;
}

inline AdditionalMonitoringTimeIE &CreateUrrIE::add_mntrng_time(uint8_t idx)
{
   return amt_[idx];
}

inline int CreateUrrIE::next_linked_urr_id()
{
   return (ie_.linked_urr_id_count < MAX_LIST_SIZE) ?
      ie_.linked_urr_id_count++ : -1;
}

inline int CreateUrrIE::next_aggregated_urrs()
{
   return (ie_.aggregated_urrs_count < MAX_LIST_SIZE) ?
      ie_.aggregated_urrs_count++ : -1;
}

inline int CreateUrrIE::next_add_mntrng_time()
{
   return (ie_.add_mntrng_time_count < MAX_LIST_SIZE) ?
      ie_.add_mntrng_time_count++ : -1;
}

inline pfcp_create_urr_ie_t &CreateUrrIE::data()
{
   return ie_;
}

inline CreateUrrIE::CreateUrrIE(pfcp_create_urr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_CREATE_URR, parent),
     ie_(ie),
     ui_(ie_.urr_id, this),
     mm_(ie_.meas_mthd, this),
     rt_(ie_.rptng_triggers, this),
     mp_(ie_.meas_period, this),
     vt_(ie_.vol_thresh, this),
     vq_(ie_.volume_quota, this),
     et_(ie_.event_threshold, this),
     eq_(ie_.event_quota, this),
     tt_(ie_.time_threshold, this),
     tq_(ie_.time_quota, this),
     qht_(ie_.quota_hldng_time, this),
     ddtt_(ie_.drpd_dl_traffic_thresh, this),
     mt_(ie_.monitoring_time, this),
     svt_(ie_.sbsqnt_vol_thresh, this),
     stt_(ie_.sbsqnt_time_thresh, this),
     svq_(ie_.sbsqnt_vol_quota, this),
     stq_(ie_.sbsqnt_time_quota, this),
     set_(ie_.sbsqnt_evnt_thresh, this),
     seq_(ie_.sbsqnt_evnt_quota, this),
     idt_(ie_.inact_det_time, this),
     mi_(ie_.meas_info, this),
     tqm_(ie_.time_quota_mech, this),
     fiqa_(ie_.far_id_for_quota_act, this),
     eit_(ie_.eth_inact_timer, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
   {
      lui_.push_back(LinkedUrrIdIE(ie_.linked_urr_id[i], this));
      aus_.push_back(AggregatedUrrsIE(ie_.aggregated_urrs[i], this));
      amt_.push_back(AdditionalMonitoringTimeIE(ie_.add_mntrng_time[i], this));
   }
}

inline uint16_t CreateUrrIE::calculateLength()
{
   uint16_t len = 0;

   len += ui_.packedLength();
   len += mm_.packedLength();
   len += rt_.packedLength();
   len += mp_.packedLength();
   len += vt_.packedLength();
   len += vq_.packedLength();
   len += et_.packedLength();
   len += eq_.packedLength();
   len += tt_.packedLength();
   len += tq_.packedLength();
   len += qht_.packedLength();
   len += ddtt_.packedLength();
   len += mt_.packedLength();
   len += svt_.packedLength();
   len += stt_.packedLength();
   len += svq_.packedLength();
   len += stq_.packedLength();
   len += set_.packedLength();
   len += seq_.packedLength();
   len += idt_.packedLength();
   len += mi_.packedLength();
   len += tqm_.packedLength();
   len += fiqa_.packedLength();
   len += eit_.packedLength();

   for (int i=0; i<ie_.linked_urr_id_count; i++)
      len += lui_[i].packedLength();
   for (int i=0; i<ie_.aggregated_urrs_count; i++)
      len += aus_[i].packedLength();
   for (int i=0; i<ie_.add_mntrng_time_count; i++)
      len += amt_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline DestinationInterfaceIE &DuplicatingParametersIE::dst_intfc()
{
   return di_;
}

inline OuterHeaderCreationIE &DuplicatingParametersIE::outer_hdr_creation()
{
   return ohc_;
}

inline TransportLevelMarkingIE &DuplicatingParametersIE::trnspt_lvl_marking()
{
   return tlm_;
}

inline ForwardingPolicyIE &DuplicatingParametersIE::frwdng_plcy()
{
   return fp_;
}

inline pfcp_dupng_parms_ie_t &DuplicatingParametersIE::data()
{
   return ie_;
}

inline DuplicatingParametersIE::DuplicatingParametersIE(pfcp_dupng_parms_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_DUPNG_PARMS, parent),
     ie_(ie),
     di_(ie_.dst_intfc, this),
     ohc_(ie_.outer_hdr_creation, this),
     tlm_(ie_.trnspt_lvl_marking, this),
     fp_(ie_.frwdng_plcy, this)
{
}

inline uint16_t DuplicatingParametersIE::calculateLength()
{
   return
      di_.packedLength() +
      ohc_.packedLength() +
      tlm_.packedLength() +
      fp_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline DestinationInterfaceIE &ForwardingParametersIE::dst_intfc()
{
   return di_;
}

inline NetworkInstanceIE &ForwardingParametersIE::ntwk_inst()
{
   return ni_;
}

inline RedirectInformationIE &ForwardingParametersIE::redir_info()
{
   return ri_;
}

inline OuterHeaderCreationIE &ForwardingParametersIE::outer_hdr_creation()
{
   return ohc_;
}

inline TransportLevelMarkingIE &ForwardingParametersIE::trnspt_lvl_marking()
{
   return tlm_;
}

inline ForwardingPolicyIE &ForwardingParametersIE::frwdng_plcy()
{
   return fp_;
}

inline HeaderEnrichmentIE &ForwardingParametersIE::hdr_enrchmt()
{
   return he_;
}

inline TrafficEndpointIdIE &ForwardingParametersIE::lnkd_traffic_endpt_id()
{
   return ltei_;
}

inline ProxyingIE &ForwardingParametersIE::proxying()
{
   return p_;
}

inline TgppInterfaceTypeIE &ForwardingParametersIE::dst_intfc_type()
{
   return dit_;
}

inline pfcp_frwdng_parms_ie_t &ForwardingParametersIE::data()
{
   return ie_;
}

inline ForwardingParametersIE::ForwardingParametersIE(pfcp_frwdng_parms_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_FRWDNG_PARMS, parent),
     ie_(ie),
     di_(ie_.dst_intfc, this),
     ni_(ie_.ntwk_inst, this),
     ri_(ie_.redir_info, this),
     ohc_(ie_.outer_hdr_creation, this),
     tlm_(ie_.trnspt_lvl_marking, this),
     fp_(ie_.frwdng_plcy, this),
     he_(ie_.hdr_enrchmt, this),
     ltei_(ie_.lnkd_traffic_endpt_id, this),
     p_(ie_.proxying, this),
     dit_(ie_.dst_intfc_type, this)
{
}

inline uint16_t ForwardingParametersIE::calculateLength()
{
   return
      di_.packedLength() +
      ni_.packedLength() +
      ri_.packedLength() +
      ohc_.packedLength() +
      tlm_.packedLength() +
      fp_.packedLength() +
      he_.packedLength() +
      ltei_.packedLength() +
      p_.packedLength() +
      dit_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline FarIdIE &CreateFarIE::far_id()
{
   return fi_;
}

inline ApplyActionIE &CreateFarIE::apply_action()
{
   return aa_;
}

inline ForwardingParametersIE &CreateFarIE::frwdng_parms()
{
   return fp_;
}

inline DuplicatingParametersIE &CreateFarIE::dupng_parms(uint8_t idx)
{
   return dp_[idx];
}

inline BarIdIE &CreateFarIE::bar_id()
{
   return bi_;
}

inline int CreateFarIE::next_dupng_parms()
{
   return (ie_.dupng_parms_count < MAX_LIST_SIZE) ?
      ie_.dupng_parms_count++ : -1;
}

inline pfcp_create_far_ie_t &CreateFarIE::data()
{
   return ie_;
}

inline CreateFarIE::CreateFarIE(pfcp_create_far_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_CREATE_FAR, parent),
     ie_(ie),
     fi_(ie_.far_id, this),
     aa_(ie_.apply_action, this),
     fp_(ie_.frwdng_parms, this),
     bi_(ie_.bar_id, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
      dp_.push_back(DuplicatingParametersIE(ie_.dupng_parms[i], this));
}

inline uint16_t CreateFarIE::calculateLength()
{
   uint16_t len = 0;

   len += fi_.packedLength();
   len += aa_.packedLength();
   len += fp_.packedLength();
   len += bi_.packedLength();

   for (int i=0; i<ie_.dupng_parms_count; i++)
      len += dp_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline EthernetFilterIdIE &EthernetPacketFilterIE::eth_fltr_id()
{
   return efi_;
}

inline EthernetFilterPropertiesIE &EthernetPacketFilterIE::eth_fltr_props()
{
   return efp_;
}

inline MacAddressIE &EthernetPacketFilterIE::mac_address()
{
   return ma_;
}

inline EthertypeIE &EthernetPacketFilterIE::ethertype()
{
   return e_;
}

inline CTagIE &EthernetPacketFilterIE::ctag()
{
   return ctag_;
}

inline STagIE &EthernetPacketFilterIE::stag()
{
   return stag_;
}

inline SdfFilterIE &EthernetPacketFilterIE::sdf_filter(uint8_t idx)
{
   return sf_[idx];
}

inline int EthernetPacketFilterIE::next_sdf_filter()
{
   return (ie_.sdf_filter_count < MAX_LIST_SIZE) ?
      ie_.sdf_filter_count++ : -1;
}

inline pfcp_eth_pckt_fltr_ie_t &EthernetPacketFilterIE::data()
{
   return ie_;
}

inline EthernetPacketFilterIE::EthernetPacketFilterIE(pfcp_eth_pckt_fltr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_ETH_PCKT_FLTR, parent),
     ie_(ie),
     efi_(ie_.eth_fltr_id, this),
     efp_(ie_.eth_fltr_props, this),
     ma_(ie_.mac_address, this),
     e_(ie_.ethertype, this),
     ctag_(ie_.ctag, this),
     stag_(ie_.stag, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
      sf_.push_back(SdfFilterIE(ie_.sdf_filter[i], this));
}

inline uint16_t EthernetPacketFilterIE::calculateLength()
{
   uint16_t len = 0;

   len += efi_.packedLength();
   len += efp_.packedLength();
   len += ma_.packedLength();
   len += e_.packedLength();
   len += ctag_.packedLength();
   len += stag_.packedLength();

   for (int i=0; i<ie_.sdf_filter_count; i++)
      len += sf_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline SourceInterfaceIE &PdiIE::src_intfc()
{
   return si_;
}

inline FTeidIE &PdiIE::local_fteid()
{
   return lft_;
}

inline NetworkInstanceIE &PdiIE::ntwk_inst()
{
   return ni_;
}

inline UeIpAddressIE &PdiIE::ue_ip_address()
{
   return uia_;
}

inline TrafficEndpointIdIE &PdiIE::traffic_endpt_id()
{
   return tei_;
}

inline ApplicationIdIE &PdiIE::application_id()
{
   return ai_;
}

inline EthernetPduSessionInformationIE &PdiIE::eth_pdu_sess_info()
{
   return epsi_;
}

inline FramedRoutingIE &PdiIE::framed_routing()
{
   return fring_;
}

inline TgppInterfaceTypeIE &PdiIE::src_intrc_type()
{
   return sit_;
}

inline SdfFilterIE &PdiIE::sdf_filter(uint8_t idx)
{
   return sf_[idx];
}

inline EthernetPacketFilterIE &PdiIE::eth_pckt_fltr(uint8_t idx)
{
   return epf_[idx];
}

inline QfiIE &PdiIE::qfi(uint8_t idx)
{
   return qfi_[idx];
}

inline FramedRouteIE &PdiIE::framed_route(uint8_t idx)
{
   return fr_[idx];
}

inline FramedIpv6RouteIE &PdiIE::frmd_ipv6_rte(uint8_t idx)
{
   return fr6_[idx];
}

inline int PdiIE::next_sdf_filter()
{
   return (ie_.sdf_filter_count < MAX_LIST_SIZE) ?
      ie_.sdf_filter_count++ : -1;
}

inline int PdiIE::next_eth_pckt_fltr()
{
   return (ie_.eth_pckt_fltr_count < MAX_LIST_SIZE) ?
      ie_.eth_pckt_fltr_count++ : -1;
}

inline int PdiIE::next_qfi()
{
   return (ie_.qfi_count < MAX_LIST_SIZE) ?
      ie_.qfi_count++ : -1;
}

inline int PdiIE::next_framed_route()
{
   return (ie_.framed_route_count < MAX_LIST_SIZE) ?
      ie_.framed_route_count++ : -1;
}

inline int PdiIE::next_frmd_ipv6_rte()
{
   return (ie_.frmd_ipv6_rte_count < MAX_LIST_SIZE) ?
      ie_.frmd_ipv6_rte_count++ : -1;
}

inline pfcp_pdi_ie_t &PdiIE::data()
{
   return ie_;
}

inline PdiIE::PdiIE(pfcp_pdi_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_PDI, parent),
     ie_(ie),
     si_(ie_.src_intfc, this),
     lft_(ie_.local_fteid, this),
     ni_(ie_.ntwk_inst, this),
     uia_(ie_.ue_ip_address, this),
     tei_(ie_.traffic_endpt_id, this),
     ai_(ie_.application_id, this),
     epsi_(ie_.eth_pdu_sess_info, this),
     fring_(ie_.framed_routing, this),
     sit_(ie_.src_intrc_type, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
   {
      sf_.push_back(SdfFilterIE(ie_.sdf_filter[i], this));
      epf_.push_back(EthernetPacketFilterIE(ie_.eth_pckt_fltr[i], this));
      qfi_.push_back(QfiIE(ie_.qfi[i], this));
      fr_.push_back(FramedRouteIE(ie_.framed_route[i], this));
      fr6_.push_back(FramedIpv6RouteIE(ie_.frmd_ipv6_rte[i], this));
   }
}

inline uint16_t PdiIE::calculateLength()
{
   uint16_t len = 0;

   len += si_.packedLength();
   len += lft_.packedLength();
   len += ni_.packedLength();
   len += uia_.packedLength();
   len += tei_.packedLength();
   len += ai_.packedLength();
   len += epsi_.packedLength();
   len += fring_.packedLength();
   len += sit_.packedLength();

   for (int i=0; i<ie_.sdf_filter_count; i++)
      len += sf_[i].packedLength();
   for (int i=0; i<ie_.eth_pckt_fltr_count; i++)
      len += epf_[i].packedLength();
   for (int i=0; i<ie_.qfi_count; i++)
      len += qfi_[i].packedLength();
   for (int i=0; i<ie_.framed_route_count; i++)
      len += fr_[i].packedLength();
   for (int i=0; i<ie_.frmd_ipv6_rte_count; i++)
      len += fr6_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline PdrIdIE &CreatePdrIE::pdr_id()
{
   return pi_;
}

inline PrecedenceIE &CreatePdrIE::precedence()
{
   return p_;
}

inline PdiIE &CreatePdrIE::pdi()
{
   return pdi_;
}

inline OuterHeaderRemovalIE &CreatePdrIE::outer_hdr_removal()
{
   return ohr_;
}

inline FarIdIE &CreatePdrIE::far_id()
{
   return fi_;
}

inline UrrIdIE &CreatePdrIE::urr_id(uint8_t idx)
{
   return ui_[idx];
}

inline QerIdIE &CreatePdrIE::qer_id(uint8_t idx)
{
   return qi_[idx];
}

inline ActivatePredefinedRulesIE &CreatePdrIE::actvt_predef_rules(uint8_t idx)
{
   return apr_[idx];
}

inline int CreatePdrIE::next_urr_id()
{
   return (ie_.urr_id_count < MAX_LIST_SIZE) ?
      ie_.urr_id_count++ : -1;
}

inline int CreatePdrIE::next_qer_id()
{
   return (ie_.qer_id_count < MAX_LIST_SIZE) ?
      ie_.qer_id_count++ : -1;
}

inline int CreatePdrIE::next_actvt_predef_rules()
{
   return (ie_.actvt_predef_rules_count < MAX_LIST_SIZE) ?
      ie_.actvt_predef_rules_count++ : -1;
}

inline pfcp_create_pdr_ie_t &CreatePdrIE::data()
{
   return ie_;
}

inline CreatePdrIE::CreatePdrIE(pfcp_create_pdr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_CREATE_PDR, parent),
     ie_(ie),
     pi_(ie_.pdr_id, this),
     p_(ie_.precedence, this),
     pdi_(ie_.pdi, this),
     ohr_(ie_.outer_hdr_removal, this),
     fi_(ie_.far_id, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
   {
      ui_.push_back(UrrIdIE(ie_.urr_id[i], this));
      qi_.push_back(QerIdIE(ie_.qer_id[i], this));
      apr_.push_back(ActivatePredefinedRulesIE(ie_.actvt_predef_rules[i], this));
   }
}

inline uint16_t CreatePdrIE::calculateLength()
{
   uint16_t len = 0;

   len += pi_.packedLength();
   len += p_.packedLength();
   len += pdi_.packedLength();
   len += ohr_.packedLength();
   len += fi_.packedLength();

   for (int i=0; i<ie_.urr_id_count; i++)
      len += ui_[i].packedLength();
   for (int i=0; i<ie_.qer_id_count; i++)
      len += qi_[i].packedLength();
   for (int i=0; i<ie_.actvt_predef_rules_count; i++)
      len += apr_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline TrafficEndpointIdIE &CreatedTrafficEndpointIE::traffic_endpt_id()
{
   return tei_;
}

inline FTeidIE &CreatedTrafficEndpointIE::local_fteid()
{
   return lft_;
}

inline pfcp_created_traffic_endpt_ie_t &CreatedTrafficEndpointIE::data()
{
   return ie_;
}

inline CreatedTrafficEndpointIE::CreatedTrafficEndpointIE(pfcp_created_traffic_endpt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_CREATED_TRAFFIC_ENDPT, parent),
     ie_(ie),
     tei_(ie_.traffic_endpt_id, this),
     lft_(ie_.local_fteid, this)
{
}

inline uint16_t CreatedTrafficEndpointIE::calculateLength()
{
   return
      tei_.packedLength() +
      lft_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline SequenceNumberIE &OverloadControlInformationIE::ovrld_ctl_seqn_nbr()
{
   return ocsn_;
}

inline MetricIE &OverloadControlInformationIE::ovrld_reduction_metric()
{
   return orm_;
}

inline TimerIE &OverloadControlInformationIE::period_of_validity()
{
   return pov_;
}

inline OciFlagsIE &OverloadControlInformationIE::ovrld_ctl_info_flgs()
{
   return ocif_;
}

inline pfcp_ovrld_ctl_info_ie_t &OverloadControlInformationIE::data()
{
   return ie_;
}

inline OverloadControlInformationIE::OverloadControlInformationIE(pfcp_ovrld_ctl_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_OVRLD_CTL_INFO, parent),
     ie_(ie),
     ocsn_(ie_.ovrld_ctl_seqn_nbr, this),
     orm_(ie_.ovrld_reduction_metric, this),
     pov_(ie_.period_of_validity, this),
     ocif_(ie_.ovrld_ctl_info_flgs, this)
{
}

inline uint16_t OverloadControlInformationIE::calculateLength()
{
   return
      ocsn_.packedLength() +
      orm_.packedLength() +
      pov_.packedLength() +
      ocif_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline SequenceNumberIE &LoadControlInformationIE::load_ctl_seqn_nbr()
{
   return lcsn_;
}

inline MetricIE &LoadControlInformationIE::load_metric()
{
   return lm_;
}

inline pfcp_load_ctl_info_ie_t &LoadControlInformationIE::data()
{
   return ie_;
}

inline LoadControlInformationIE::LoadControlInformationIE(pfcp_load_ctl_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_LOAD_CTL_INFO, parent),
     ie_(ie),
     lcsn_(ie_.load_ctl_seqn_nbr, this),
     lm_(ie_.load_metric, this)
{
}

inline uint16_t LoadControlInformationIE::calculateLength()
{
   return
      lcsn_.packedLength() +
      lm_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline PdrIdIE &CreatedPdrIE::pdr_id()
{
   return pi_;
}

inline FTeidIE &CreatedPdrIE::local_fteid()
{
   return lft_;
}

inline pfcp_created_pdr_ie_t &CreatedPdrIE::data()
{
   return ie_;
}

inline CreatedPdrIE::CreatedPdrIE(pfcp_created_pdr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_CREATED_PDR, parent),
     ie_(ie),
     pi_(ie_.pdr_id, this),
     lft_(ie_.local_fteid, this)
{
}

inline uint16_t CreatedPdrIE::calculateLength()
{
   return
      pi_.packedLength() +
      lft_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline RemoteGTPUPeerIE &UserPlanePathFailureReportIE::rmt_gtpu_peer(uint8_t idx)
{
   return rgp_[idx];
}

inline int UserPlanePathFailureReportIE::next_rmt_gtpu_peer()
{
   return (ie_.rmt_gtpu_peer_count < MAX_LIST_SIZE) ?
      ie_.rmt_gtpu_peer_count++ : -1;
}

inline pfcp_user_plane_path_fail_rpt_ie_t &UserPlanePathFailureReportIE::data()
{
   return ie_;
}

inline UserPlanePathFailureReportIE::UserPlanePathFailureReportIE(pfcp_user_plane_path_fail_rpt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_USER_PLANE_PATH_FAIL_RPT, parent),
     ie_(ie)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
      rgp_.push_back(RemoteGTPUPeerIE(ie_.rmt_gtpu_peer[i], this));
}

inline uint16_t UserPlanePathFailureReportIE::calculateLength()
{
   uint16_t len = 0;
   for (int i=0; i<ie_.rmt_gtpu_peer_count; i++)
      len += rgp_[i].packedLength();
   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline TrafficEndpointIdIE &RemoveTrafficEndpointIE::traffic_endpt_id()
{
   return tei_;
}
   
inline pfcp_rmv_traffic_endpt_ie_t &RemoveTrafficEndpointIE::data()
{
   return ie_;
}

inline RemoveTrafficEndpointIE::RemoveTrafficEndpointIE(pfcp_rmv_traffic_endpt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_RMV_TRAFFIC_ENDPT, parent),
      ie_(ie),
      tei_(ie_.traffic_endpt_id, this)
{
}

inline uint16_t RemoveTrafficEndpointIE::calculateLength()
{
   return
      tei_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline TrafficEndpointIdIE &UpdateTrafficEndpointIE::traffic_endpt_id()
{
   return tei_;
}

inline FTeidIE &UpdateTrafficEndpointIE::local_fteid()
{
   return lft_;
}

inline NetworkInstanceIE &UpdateTrafficEndpointIE::ntwk_inst()
{
   return ni_;
}

inline UeIpAddressIE &UpdateTrafficEndpointIE::ue_ip_address()
{
   return uia_;
}

inline FramedRoutingIE &UpdateTrafficEndpointIE::framed_routing()
{
   return fring_;
}

inline FramedRouteIE &UpdateTrafficEndpointIE::framed_route(uint8_t idx)
{
   return fr_[idx];
}

inline FramedIpv6RouteIE &UpdateTrafficEndpointIE::frmd_ipv6_rte(uint8_t idx)
{
   return fr6_[idx];
}

inline int UpdateTrafficEndpointIE::next_framed_route()
{
   return (ie_.framed_route_count < MAX_LIST_SIZE) ?
      ie_.framed_route_count++ : -1;
}

inline int UpdateTrafficEndpointIE::next_frmd_ipv6_rte()
{
   return (ie_.frmd_ipv6_rte_count < MAX_LIST_SIZE) ?
      ie_.frmd_ipv6_rte_count++ : -1;
}

inline pfcp_upd_traffic_endpt_ie_t &UpdateTrafficEndpointIE::data()
{
   return ie_;
}

inline UpdateTrafficEndpointIE::UpdateTrafficEndpointIE(pfcp_upd_traffic_endpt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_UPD_TRAFFIC_ENDPT, parent),
     ie_(ie),
     tei_(ie_.traffic_endpt_id, this),
     lft_(ie_.local_fteid, this),
     ni_(ie_.ntwk_inst, this),
     uia_(ie_.ue_ip_address, this),
     fring_(ie_.framed_routing, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
   {
      fr_.push_back(FramedRouteIE(ie_.framed_route[i], this));
      fr6_.push_back(FramedIpv6RouteIE(ie_.frmd_ipv6_rte[i], this));
   }
}

inline uint16_t UpdateTrafficEndpointIE::calculateLength()
{
   uint16_t len = 0;

   len += tei_.packedLength();
   len += lft_.packedLength();
   len += ni_.packedLength();
   len += uia_.packedLength();
   len += fring_.packedLength();

   for (int i=0; i<ie_.framed_route_count; i++)
      len += fr_[i].packedLength();
   for (int i=0; i<ie_.frmd_ipv6_rte_count; i++)
      len += fr6_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline BarIdIE &RemoveBarIE::bar_id()
{
   return bi_;
}

inline pfcp_remove_bar_ie_t &RemoveBarIE::data()
{
   return ie_;
}

inline RemoveBarIE::RemoveBarIE(pfcp_remove_bar_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_REMOVE_BAR, parent),
     ie_(ie),
     bi_(ie_.bar_id, this)
{
}

inline uint16_t RemoveBarIE::calculateLength()
{
   return bi_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline BarIdIE &UpdateBarSessionModificationReqIE::bar_id()
{
   return bi_;
}

inline DownlinkDataNotificationDelayIE &UpdateBarSessionModificationReqIE::dnlnk_data_notif_delay()
{
   return ddnd_;
}

inline SuggestedBufferingPacketsCountIE &UpdateBarSessionModificationReqIE::suggstd_buf_pckts_cnt()
{
   return sbpc_;
}
   
inline pfcp_upd_bar_sess_mod_req_ie_t &UpdateBarSessionModificationReqIE::data()
{
   return ie_;
}

inline UpdateBarSessionModificationReqIE::UpdateBarSessionModificationReqIE(pfcp_upd_bar_sess_mod_req_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_UPD_BAR_SESS_MOD_REQ, parent),
     ie_(ie),
     bi_(ie_.bar_id, this),
     ddnd_(ie_.dnlnk_data_notif_delay, this),
     sbpc_(ie_.suggstd_buf_pckts_cnt, this)
{
}

inline uint16_t UpdateBarSessionModificationReqIE::calculateLength()
{
   return
      bi_.packedLength() +
      ddnd_.packedLength() +      
      sbpc_.packedLength();         
}

////////////////////////////////////////////////////////////////////////////////

inline UrrIdIE &QueryUrrIE::urr_id()
{
   return ui_;
}
   
inline pfcp_query_urr_ie_t &QueryUrrIE::data()
{
   return ie_;
}

inline QueryUrrIE::QueryUrrIE(pfcp_query_urr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_QUERY_URR, parent),
     ie_(ie),
     ui_(ie_.urr_id, this)
{
}

inline uint16_t QueryUrrIE::calculateLength()
{
   return ui_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline QerIdIE RemoveQerIE::qer_id()
{
   return qi_;
}
   
inline pfcp_remove_qer_ie_t &RemoveQerIE::data()
{
   return ie_;
}

inline RemoveQerIE::RemoveQerIE(pfcp_remove_qer_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_REMOVE_QER, parent),
     ie_(ie),
     qi_(ie_.qer_id, this)
{
}

inline uint16_t RemoveQerIE::calculateLength()
{
   return qi_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline UrrIdIE &RemoveUrrIE::urr_id()
{
   return ui_;
}
   
inline pfcp_remove_urr_ie_t &RemoveUrrIE::data()
{
   return ie_;
}

inline RemoveUrrIE::RemoveUrrIE(pfcp_remove_urr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_REMOVE_URR, parent),
     ie_(ie),
     ui_(ie_.urr_id, this)
{
}

inline uint16_t RemoveUrrIE::calculateLength()
{
   return ui_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline FarIdIE &RemoveFarIE::far_id()
{
   return fi_;
}
   
inline pfcp_remove_far_ie_t &RemoveFarIE::data()
{
   return ie_;
}

inline RemoveFarIE::RemoveFarIE(pfcp_remove_far_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_REMOVE_FAR, parent),
     ie_(ie),
     fi_(ie_.far_id, this)
{
}

inline uint16_t RemoveFarIE::calculateLength()
{
   return fi_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline PdrIdIE &RemovePdrIE::pdr_id()
{
   return pi_;
}
   
inline pfcp_remove_pdr_ie_t &RemovePdrIE::data()
{
   return ie_;
}

inline RemovePdrIE::RemovePdrIE(pfcp_remove_pdr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_REMOVE_PDR, parent),
     ie_(ie),
     pi_(ie_.pdr_id, this)
{
}

inline uint16_t RemovePdrIE::calculateLength()
{
   return pi_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline QerIdIE &UpdateQerIE::qer_id()
{
   return qi_;
}

inline QerCorrelationIdIE &UpdateQerIE::qer_corr_id()
{
   return qci_;
}

inline GateStatusIE &UpdateQerIE::gate_status()
{
   return gs_;
}

inline MbrIE &UpdateQerIE::maximum_bitrate()
{
   return mb_;
}

inline GbrIE &UpdateQerIE::guaranteed_bitrate()
{
   return gb_;
}

inline PacketRateIE &UpdateQerIE::packet_rate()
{
   return pr;
}

inline DlFlowLevelMarkingIE &UpdateQerIE::dl_flow_lvl_marking()
{
   return dflm_;
}

inline QfiIE &UpdateQerIE::qos_flow_ident()
{
   return qfi_;
}

inline RqiIE &UpdateQerIE::reflective_qos()
{
   return rqi_;
}

inline PagingPolicyIndicatorIE &UpdateQerIE::paging_plcy_indctr()
{
   return ppi_;
}

inline AveragingWindowIE &UpdateQerIE::avgng_wnd()
{
   return aw_;
}

inline pfcp_update_qer_ie_t &UpdateQerIE::data()
{
   return ie_;
}

inline UpdateQerIE::UpdateQerIE(pfcp_update_qer_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_UPDATE_QER, parent),
     ie_(ie),
     qi_(ie_.qer_id, this),
     qci_(ie_.qer_corr_id, this),
     gs_(ie_.gate_status, this),
     mb_(ie_.maximum_bitrate, this),
     gb_(ie_.guaranteed_bitrate, this),
     pr(ie_.packet_rate, this),
     dflm_(ie_.dl_flow_lvl_marking, this),
     qfi_(ie_.qos_flow_ident, this),
     rqi_(ie_.reflective_qos, this),
     ppi_(ie_.paging_plcy_indctr, this),
     aw_(ie_.avgng_wnd, this)
{
}

inline uint16_t UpdateQerIE::calculateLength()
{
   return
      qi_.packedLength() +
      qci_.packedLength() +
      gs_.packedLength() +
      mb_.packedLength() +
      gb_.packedLength() +
      pr.packedLength() +
      dflm_.packedLength() +
      qfi_.packedLength() +
      rqi_.packedLength() +
      ppi_.packedLength() +
      aw_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline UrrIdIE &UpdateUrrIE::urr_id()
{
   return ui_;
}

inline MeasurementMethodIE &UpdateUrrIE::meas_mthd()
{
   return mm_;
}

inline ReportingTriggersIE &UpdateUrrIE::rptng_triggers()
{
   return rt_;
}

inline MeasurementPeriodIE &UpdateUrrIE::meas_period()
{
   return mp_;
}

inline VolumeThresholdIE &UpdateUrrIE::vol_thresh()
{
   return vt_;
}

inline VolumeQuotaIE &UpdateUrrIE::volume_quota()
{
   return vq_;
}

inline TimeThresholdIE &UpdateUrrIE::time_threshold()
{
   return tt_;
}

inline TimeQuotaIE &UpdateUrrIE::time_quota()
{
   return tq_;
}

inline EventThresholdIE &UpdateUrrIE::event_threshold()
{
   return et_;
}

inline EventQuotaIE &UpdateUrrIE::event_quota()
{
   return eq_;
}

inline QuotaHoldingTimeIE &UpdateUrrIE::quota_hldng_time()
{
   return qht_;
}

inline DroppedDlTrafficThresholdIE &UpdateUrrIE::drpd_dl_traffic_thresh()
{
   return ddtt_;
}

inline MonitoringTimeIE &UpdateUrrIE::monitoring_time()
{
   return mt_;
}

inline SubsequentVolumeThresholdIE &UpdateUrrIE::sbsqnt_vol_thresh()
{
   return svt_;
}

inline SubsequentTimeThresholdIE &UpdateUrrIE::sbsqnt_time_thresh()
{
   return stt_;
}

inline SubsequentVolumeQuotaIE &UpdateUrrIE::sbsqnt_vol_quota()
{
   return svq_;
}

inline SubsequentTimeQuotaIE &UpdateUrrIE::sbsqnt_time_quota()
{
   return stq_;
}

inline SubsequentEventThresholdIE &UpdateUrrIE::sbsqnt_evnt_thresh()
{
   return set_;
}

inline SubsequentEventQuotaIE &UpdateUrrIE::sbsqnt_evnt_quota()
{
   return seq_;
}

inline InactivityDetectionTimeIE &UpdateUrrIE::inact_det_time()
{
   return idt_;
}

inline MeasurementInformationIE &UpdateUrrIE::meas_info()
{
   return mi_;
}

inline TimeQuotaMechanismIE &UpdateUrrIE::time_quota_mech()
{
   return tqm_;
}

inline FarIdIE &UpdateUrrIE::far_id_for_quota_act()
{
   return fifqa_;
}

inline EthernetInactivityTimerIE &UpdateUrrIE::eth_inact_timer()
{
   return eit_;
}

inline AdditionalMonitoringTimeIE &UpdateUrrIE::add_mntrng_time()
{
   return amt_;
}

inline LinkedUrrIdIE &UpdateUrrIE::linked_urr_id(uint8_t idx)
{
   return lui_[idx];
}

inline AggregatedUrrsIE &UpdateUrrIE::aggregated_urrs(uint8_t idx)
{
   return au_[idx];
}

inline int UpdateUrrIE::next_linked_urr_id()
{
   return (ie_.linked_urr_id_count < MAX_LIST_SIZE) ?
      ie_.linked_urr_id_count++ : -1;
}

inline int UpdateUrrIE::next_aggregated_urrs()
{
   return (ie_.aggregated_urrs_count < MAX_LIST_SIZE) ?
      ie_.aggregated_urrs_count++ : -1;
}

inline pfcp_update_urr_ie_t &UpdateUrrIE::data()
{
   return ie_;
}

inline UpdateUrrIE::UpdateUrrIE(pfcp_update_urr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_UPDATE_URR, parent),
     ie_(ie),
     ui_(ie_.urr_id, this),
     mm_(ie_.meas_mthd, this),
     rt_(ie_.rptng_triggers, this),
     mp_(ie_.meas_period, this),
     vt_(ie_.vol_thresh, this),
     vq_(ie_.volume_quota, this),
     tt_(ie_.time_threshold, this),
     tq_(ie_.time_quota, this),
     et_(ie_.event_threshold, this),
     eq_(ie_.event_quota, this),
     qht_(ie_.quota_hldng_time, this),
     ddtt_(ie_.drpd_dl_traffic_thresh, this),
     mt_(ie_.monitoring_time, this),
     svt_(ie_.sbsqnt_vol_thresh, this),
     stt_(ie_.sbsqnt_time_thresh, this),
     svq_(ie_.sbsqnt_vol_quota, this),
     stq_(ie_.sbsqnt_time_quota, this),
     set_(ie_.sbsqnt_evnt_thresh, this),
     seq_(ie_.sbsqnt_evnt_quota, this),
     idt_(ie_.inact_det_time, this),
     mi_(ie_.meas_info, this),
     tqm_(ie_.time_quota_mech, this),
     fifqa_(ie_.far_id_for_quota_act, this),
     eit_(ie_.eth_inact_timer, this),
     amt_(ie_.add_mntrng_time, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
   {
      lui_.push_back(LinkedUrrIdIE(ie_.linked_urr_id[i], this));
      au_.push_back(AggregatedUrrsIE(ie_.aggregated_urrs[i], this));
   }
}

inline uint16_t UpdateUrrIE::calculateLength()
{
   uint16_t len = 0;
   len += ui_.packedLength();
   len += mm_.packedLength();
   len += rt_.packedLength();
   len += mp_.packedLength();
   len += vt_.packedLength();
   len += vq_.packedLength();
   len += tt_.packedLength();
   len += tq_.packedLength();
   len += et_.packedLength();
   len += eq_.packedLength();
   len += qht_.packedLength();
   len += ddtt_.packedLength();
   len += mt_.packedLength();
   len += svt_.packedLength();
   len += stt_.packedLength();
   len += svq_.packedLength();
   len += stq_.packedLength();
   len += set_.packedLength();
   len += seq_.packedLength();
   len += idt_.packedLength();
   len += mi_.packedLength();
   len += tqm_.packedLength();
   len += fifqa_.packedLength();
   len += eit_.packedLength();
   len += amt_.packedLength();

   for (int i=0; i<ie_.linked_urr_id_count; i++)
      len += lui_[i].packedLength();
   for (int i=0; i<ie_.aggregated_urrs_count; i++)
      len += au_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline DestinationInterfaceIE &UpdateDuplicatingParametersIE::dst_intfc()
{
   return di_;
}

inline OuterHeaderCreationIE &UpdateDuplicatingParametersIE::outer_hdr_creation()
{
   return ohc_;
}

inline TransportLevelMarkingIE &UpdateDuplicatingParametersIE::trnspt_lvl_marking()
{
   return tlm_;
}

inline ForwardingPolicyIE &UpdateDuplicatingParametersIE::frwdng_plcy()
{
   return fp_;
}

inline pfcp_upd_dupng_parms_ie_t &UpdateDuplicatingParametersIE::data()
{
   return ie_;
}

inline UpdateDuplicatingParametersIE::UpdateDuplicatingParametersIE(pfcp_upd_dupng_parms_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_UPD_DUPNG_PARMS, parent),
     ie_(ie),
     di_(ie_.dst_intfc, this),
     ohc_(ie_.outer_hdr_creation, this),
     tlm_(ie_.trnspt_lvl_marking, this),
     fp_(ie_.frwdng_plcy, this)
{
}

inline uint16_t UpdateDuplicatingParametersIE::calculateLength()
{
   return 
      di_.packedLength() +
      ohc_.packedLength() +
      tlm_.packedLength() +
      fp_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline DestinationInterfaceIE &UpdateForwardingParametersIE::dst_intfc()
{
   return di_;
}

inline NetworkInstanceIE &UpdateForwardingParametersIE::ntwk_inst()
{
   return ni_;
}

inline RedirectInformationIE &UpdateForwardingParametersIE::redir_info()
{
   return ri_;
}

inline OuterHeaderCreationIE &UpdateForwardingParametersIE::outer_hdr_creation()
{
   return ohc_;
}

inline TransportLevelMarkingIE &UpdateForwardingParametersIE::trnspt_lvl_marking()
{
   return tlm_;
}

inline ForwardingPolicyIE &UpdateForwardingParametersIE::frwdng_plcy()
{
   return fp_;
}

inline HeaderEnrichmentIE &UpdateForwardingParametersIE::hdr_enrchmt()
{
   return he_;
}

inline PfcpSmReqFlagsIE &UpdateForwardingParametersIE::pfcpsmreq_flags()
{
   return f_;
}

inline TrafficEndpointIdIE &UpdateForwardingParametersIE::lnkd_traffic_endpt_id()
{
   return ltei_;
}

inline TgppInterfaceTypeIE &UpdateForwardingParametersIE::dst_intfc_type()
{
   return dit_;
}

inline pfcp_upd_frwdng_parms_ie_t &UpdateForwardingParametersIE::data()
{
   return ie_;
}

inline UpdateForwardingParametersIE::UpdateForwardingParametersIE(pfcp_upd_frwdng_parms_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_UPD_FRWDNG_PARMS, parent),
      ie_(ie),
      di_(ie_.dst_intfc, this),
      ni_(ie_.ntwk_inst, this),
      ri_(ie_.redir_info, this),
      ohc_(ie_.outer_hdr_creation, this),
      tlm_(ie_.trnspt_lvl_marking, this),
      fp_(ie_.frwdng_plcy, this),
      he_(ie_.hdr_enrchmt, this),
      f_(ie_.pfcpsmreq_flags, this),
      ltei_(ie_.lnkd_traffic_endpt_id, this),
      dit_(ie_.dst_intfc_type, this)
{
}

inline uint16_t UpdateForwardingParametersIE::calculateLength()
{
   return
      di_.packedLength() +
      ni_.packedLength() +
      ri_.packedLength() +
      ohc_.packedLength() +
      tlm_.packedLength() +
      fp_.packedLength() +
      he_.packedLength() +
      f_.packedLength() +
      ltei_.packedLength() +
      dit_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline FarIdIE &UpdateFarIE::far_id()
{
   return fi_;
}

inline ApplyActionIE &UpdateFarIE::apply_action()
{
   return aa_;
}

inline UpdateForwardingParametersIE &UpdateFarIE::upd_frwdng_parms()
{
   return ufp_;
}

inline BarIdIE &UpdateFarIE::bar_id()
{
   return bi_;
}

inline UpdateDuplicatingParametersIE &UpdateFarIE::upd_dupng_parms(uint8_t idx)
{
   return udp_[idx];
}

inline int UpdateFarIE::next_upd_dupng_parms()
{
   return (ie_.upd_dupng_parms_count < MAX_LIST_SIZE) ?
      ie_.upd_dupng_parms_count++ : -1;
}

inline pfcp_update_far_ie_t &UpdateFarIE::data()
{
   return ie_;
}

inline UpdateFarIE::UpdateFarIE(pfcp_update_far_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_UPDATE_FAR, parent),
     ie_(ie),
     fi_(ie_.far_id, this),
     aa_(ie_.apply_action, this),
     ufp_(ie_.upd_frwdng_parms, this),
     bi_(ie_.bar_id, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
      udp_.push_back(UpdateDuplicatingParametersIE(ie_.upd_dupng_parms[i], this));
}

inline uint16_t UpdateFarIE::calculateLength()
{
   uint16_t len = 0;

   len += fi_.packedLength();
   len += aa_.packedLength();
   len += ufp_.packedLength();
   len += bi_.packedLength();

   for (int i=0; i<ie_.upd_dupng_parms_count; i++)
      len += udp_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline PdrIdIE &UpdatePdrIE::pdr_id()
{
   return pi_;
}

inline OuterHeaderRemovalIE &UpdatePdrIE::outer_hdr_removal()
{
   return ohr_;
}

inline PrecedenceIE &UpdatePdrIE::precedence()
{
   return p_;
}

inline PdiIE &UpdatePdrIE::pdi()
{
   return pdi_;
}

inline FarIdIE &UpdatePdrIE::far_id()
{
   return fi_;
}

inline UrrIdIE &UpdatePdrIE::urr_id()
{
   return ui_;
}

inline QerIdIE &UpdatePdrIE::qer_id()
{
   return qi_;
}

inline ActivatePredefinedRulesIE &UpdatePdrIE::actvt_predef_rules(uint8_t idx)
{
   return apr_[idx];
}

inline DeactivatePredefinedRulesIE &UpdatePdrIE::deact_predef_rules(uint8_t idx)
{
   return dpr_[idx];
}

inline int UpdatePdrIE::next_actvt_predef_rules()
{
   return (ie_.actvt_predef_rules_count < MAX_LIST_SIZE) ?
      ie_.actvt_predef_rules_count++ : -1;
}

inline int UpdatePdrIE::next_deact_predef_rules()
{
   return (ie_.deact_predef_rules_count < MAX_LIST_SIZE) ?
      ie_.deact_predef_rules_count++ : -1;
}

inline pfcp_update_pdr_ie_t &UpdatePdrIE::data()
{
   return ie_;
}

inline UpdatePdrIE::UpdatePdrIE(pfcp_update_pdr_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_UPDATE_PDR, parent),
     ie_(ie),
     pi_(ie_.pdr_id, this),
     ohr_(ie_.outer_hdr_removal, this),
     p_(ie_.precedence, this),
     pdi_(ie_.pdi, this),
     fi_(ie_.far_id, this),
     ui_(ie_.urr_id, this),
     qi_(ie_.qer_id, this)
{
   for ( int i=0; i<MAX_LIST_SIZE; i++)
   {
      apr_.push_back(ActivatePredefinedRulesIE(ie_.actvt_predef_rules[i], this));
      dpr_.push_back(DeactivatePredefinedRulesIE(ie_.deact_predef_rules[i], this));
   }
}

inline uint16_t UpdatePdrIE::calculateLength()
{
   uint16_t len = 0;
   
   len += pi_.packedLength();
   len += ohr_.packedLength();
   len += p_.packedLength();
   len += pdi_.packedLength();
   len += fi_.packedLength();
   len += ui_.packedLength();
   len += qi_.packedLength();

   for (int i=0; i<ie_.actvt_predef_rules_count; i++)
      len += apr_[i].packedLength();
   for (int i=0; i<ie_.deact_predef_rules_count; i++)
      len += dpr_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline MacAddressesDetectedIE &EthernetTrafficInformationIE::mac_addrs_detctd()
{
   return mad_;
}

inline MacAddressesRemovedIE &EthernetTrafficInformationIE::mac_addrs_rmvd()
{
   return mar_;
}

inline pfcp_eth_traffic_info_ie_t &EthernetTrafficInformationIE::data()
{
   return ie_;
}

inline EthernetTrafficInformationIE::EthernetTrafficInformationIE(pfcp_eth_traffic_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_ETH_TRAFFIC_INFO, parent),
     ie_(ie),
     mad_(ie_.mac_addrs_detctd, this),
     mar_(ie_.mac_addrs_rmvd, this)
{
}

inline uint16_t EthernetTrafficInformationIE::calculateLength()
{
   return 
      mad_.packedLength() +
      mar_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline UrrIdIE &UsageReportSessionModificationRspIE::urr_id()
{
   return ui_;
}

inline UrSeqnIE &UsageReportSessionModificationRspIE::urseqn()
{
   return us_;
}

inline UsageReportTriggerIE &UsageReportSessionModificationRspIE::usage_rpt_trig()
{
   return urt_;
}

inline StartTimeIE &UsageReportSessionModificationRspIE::start_time()
{
   return st_;
}

inline EndTimeIE &UsageReportSessionModificationRspIE::end_time()
{
   return et_;
}

inline VolumeMeasurementIE &UsageReportSessionModificationRspIE::vol_meas()
{
   return vm_;
}

inline DurationMeasurementIE &UsageReportSessionModificationRspIE::dur_meas()
{
   return dm_;
}

inline TimeOfFirstPacketIE &UsageReportSessionModificationRspIE::time_of_frst_pckt()
{
   return tofp_;
}

inline TimeOfLastPacketIE &UsageReportSessionModificationRspIE::time_of_lst_pckt()
{
   return tolp_;
}

inline UsageInformationIE &UsageReportSessionModificationRspIE::usage_info()
{
   return uinfo_;
}

inline QueryUrrReferenceIE &UsageReportSessionModificationRspIE::query_urr_ref()
{
   return qur_;
}

inline EthernetTrafficInformationIE &UsageReportSessionModificationRspIE::eth_traffic_info()
{
   return eti_;
}

inline pfcp_usage_rpt_sess_mod_rsp_ie_t &UsageReportSessionModificationRspIE::data()
{
   return ie_;
}

inline UsageReportSessionModificationRspIE::UsageReportSessionModificationRspIE(pfcp_usage_rpt_sess_mod_rsp_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_USAGE_RPT_SESS_MOD_RSP, parent),
     ie_(ie),
     ui_(ie_.urr_id, this),
     us_(ie_.urseqn, this),
     urt_(ie_.usage_rpt_trig, this),
     st_(ie_.start_time, this),
     et_(ie_.end_time, this),
     vm_(ie_.vol_meas, this),
     dm_(ie_.dur_meas, this),
     tofp_(ie_.time_of_frst_pckt, this),
     tolp_(ie_.time_of_lst_pckt, this),
     uinfo_(ie_.usage_info, this),
     qur_(ie_.query_urr_ref, this),
     eti_(ie_.eth_traffic_info, this)
{
}

inline uint16_t UsageReportSessionModificationRspIE::calculateLength()
{
   return 
      ui_.packedLength() +
      us_.packedLength() +
      urt_.packedLength() +
      st_.packedLength() +
      et_.packedLength() +
      vm_.packedLength() +
      dm_.packedLength() +
      tofp_.packedLength() +
      tolp_.packedLength() +
      uinfo_.packedLength() +
      qur_.packedLength() +
      eti_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline UrrIdIE &UsageReportSessionDeletionRspIE::urr_id()
{
   return ui_;
}

inline UrSeqnIE &UsageReportSessionDeletionRspIE::urseqn()
{
   return us_;
}

inline UsageReportTriggerIE &UsageReportSessionDeletionRspIE::usage_rpt_trig()
{
   return urt_;
}

inline StartTimeIE &UsageReportSessionDeletionRspIE::start_time()
{
   return st_;
}

inline EndTimeIE &UsageReportSessionDeletionRspIE::end_time()
{
   return et_;
}

inline VolumeMeasurementIE &UsageReportSessionDeletionRspIE::vol_meas()
{
   return vm_;
}

inline DurationMeasurementIE &UsageReportSessionDeletionRspIE::dur_meas()
{
   return dm_;
}

inline TimeOfFirstPacketIE &UsageReportSessionDeletionRspIE::time_of_frst_pckt()
{
   return tofp_;
}

inline TimeOfLastPacketIE &UsageReportSessionDeletionRspIE::time_of_lst_pckt()
{
   return tolp_;
}

inline UsageInformationIE &UsageReportSessionDeletionRspIE::usage_info()
{
   return uinfo_;
}

inline EthernetTrafficInformationIE &UsageReportSessionDeletionRspIE::eth_traffic_info()
{
   return eti_;
}

inline pfcp_usage_rpt_sess_del_rsp_ie_t &UsageReportSessionDeletionRspIE::data()
{
   return ie_;
}

inline UsageReportSessionDeletionRspIE::UsageReportSessionDeletionRspIE(pfcp_usage_rpt_sess_del_rsp_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_USAGE_RPT_SESS_DEL_RSP, parent),
     ie_(ie),
     ui_(ie_.urr_id, this),
     us_(ie_.urseqn, this),
     urt_(ie_.usage_rpt_trig, this),
     st_(ie_.start_time, this),
     et_(ie_.end_time, this),
     vm_(ie_.vol_meas, this),
     dm_(ie_.dur_meas, this),
     tofp_(ie_.time_of_frst_pckt, this),
     tolp_(ie_.time_of_lst_pckt, this),
     uinfo_(ie_.usage_info, this),
     eti_(ie_.eth_traffic_info, this)
{
}

inline uint16_t UsageReportSessionDeletionRspIE::calculateLength()
{
   return 
      ui_.packedLength() +
      us_.packedLength() +
      urt_.packedLength() +
      st_.packedLength() +
      et_.packedLength() +
      vm_.packedLength() +
      dm_.packedLength() +
      tofp_.packedLength() +
      tolp_.packedLength() +
      uinfo_.packedLength() +
      eti_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline FTeidIE &ErrorIndicationReportIE::remote_fteid(uint8_t idx)
{
   return rft_[idx];
}

inline int ErrorIndicationReportIE::next_remote_fteid()
{
   return (ie_.remote_fteid_count < MAX_LIST_SIZE) ?
      ie_.remote_fteid_count++ : -1;
}

inline pfcp_err_indctn_rpt_ie_t &ErrorIndicationReportIE::data()
{
   return ie_;
}

inline ErrorIndicationReportIE::ErrorIndicationReportIE(pfcp_err_indctn_rpt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_ERR_INDCTN_RPT, parent),
     ie_(ie)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
      rft_.push_back(FTeidIE(ie_.remote_fteid[i], this));
}

inline uint16_t ErrorIndicationReportIE::calculateLength()
{
   uint16_t len = 0;
   for (int i=0; i<ie_.remote_fteid_count; i++)
      len += rft_[i].packedLength();
   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline ApplicationIdIE &ApplicationDetectionInformationIE::application_id()
{
   return ai_;
}

inline ApplicationInstanceIdIE &ApplicationDetectionInformationIE::app_inst_id()
{
   return aii_;
}

inline FlowInformationIE &ApplicationDetectionInformationIE::flow_info()
{
   return fi_;
}

inline pfcp_app_det_info_ie_t &ApplicationDetectionInformationIE::data()
{
   return ie_;
}

inline ApplicationDetectionInformationIE::ApplicationDetectionInformationIE(pfcp_app_det_info_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_APP_DET_INFO, parent),
     ie_(ie),
     ai_(ie_.application_id, this),
     aii_(ie_.app_inst_id, this),
     fi_(ie_.flow_info, this)
{
}

inline uint16_t ApplicationDetectionInformationIE::calculateLength()
{
   return 
      ai_.packedLength() +
      aii_.packedLength() +
      fi_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////

inline UrrIdIE &UsageReportSessionReportReqIE::urr_id()
{
   return ui_;
}

inline UrSeqnIE &UsageReportSessionReportReqIE::urseqn()
{
   return us_;
}

inline UsageReportTriggerIE &UsageReportSessionReportReqIE::usage_rpt_trig()
{
   return urt_;
}

inline StartTimeIE &UsageReportSessionReportReqIE::start_time()
{
   return st_;
}

inline EndTimeIE &UsageReportSessionReportReqIE::end_time()
{
   return et_;
}

inline VolumeMeasurementIE &UsageReportSessionReportReqIE::vol_meas()
{
   return vm_;
}

inline DurationMeasurementIE &UsageReportSessionReportReqIE::dur_meas()
{
   return dm_;
}

inline ApplicationDetectionInformationIE &UsageReportSessionReportReqIE::app_det_info()
{
   return adi_;
}

inline UeIpAddressIE &UsageReportSessionReportReqIE::ue_ip_address()
{
   return uia_;
}

inline NetworkInstanceIE &UsageReportSessionReportReqIE::ntwk_inst()
{
   return ni_;
}

inline TimeOfFirstPacketIE &UsageReportSessionReportReqIE::time_of_frst_pckt()
{
   return tofp_;
}

inline TimeOfLastPacketIE &UsageReportSessionReportReqIE::time_of_lst_pckt()
{
   return tolp_;
}

inline UsageInformationIE &UsageReportSessionReportReqIE::usage_info()
{
   return uinfo_;
}

inline QueryUrrReferenceIE &UsageReportSessionReportReqIE::query_urr_ref()
{
   return qur_;
}

inline EthernetTrafficInformationIE &UsageReportSessionReportReqIE::eth_traffic_info()
{
   return eti_;
}


inline EventTimeStampIE &UsageReportSessionReportReqIE::evnt_time_stmp(uint8_t idx)
{
   return ets_[idx];
}

inline int UsageReportSessionReportReqIE::next_evnt_time_stmp()
{
   return (ie_.evnt_time_stmp_count < MAX_LIST_SIZE) ?
      ie_.evnt_time_stmp_count++ : -1;
}

inline pfcp_usage_rpt_sess_rpt_req_ie_t &UsageReportSessionReportReqIE::data()
{
   return ie_;
}

inline UsageReportSessionReportReqIE::UsageReportSessionReportReqIE(pfcp_usage_rpt_sess_rpt_req_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_USAGE_RPT_SESS_RPT_REQ, parent),
     ie_(ie),
     ui_(ie_.urr_id, this),
     us_(ie_.urseqn, this),
     urt_(ie_.usage_rpt_trig, this),
     st_(ie_.start_time, this),
     et_(ie_.end_time, this),
     vm_(ie_.vol_meas, this),
     dm_(ie_.dur_meas, this),
     adi_(ie_.app_det_info, this),
     uia_(ie_.ue_ip_address, this),
     ni_(ie_.ntwk_inst, this),
     tofp_(ie_.time_of_frst_pckt, this),
     tolp_(ie_.time_of_lst_pckt, this),
     uinfo_(ie_.usage_info, this),
     qur_(ie_.query_urr_ref, this),
     eti_(ie_.eth_traffic_info, this)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
      ets_.push_back(EventTimeStampIE(ie_.evnt_time_stmp[i], this));
}

inline uint16_t UsageReportSessionReportReqIE::calculateLength()
{
   uint16_t len = 0;

   len += ui_.packedLength();
   len += us_.packedLength();
   len += urt_.packedLength();
   len += st_.packedLength();
   len += et_.packedLength();
   len += vm_.packedLength();
   len += dm_.packedLength();
   len += adi_.packedLength();
   len += uia_.packedLength();
   len += ni_.packedLength();
   len += tofp_.packedLength();
   len += tolp_.packedLength();
   len += uinfo_.packedLength();
   len += qur_.packedLength();
   len += eti_.packedLength();

   for (int i=0; i<ie_.evnt_time_stmp_count; i++)
      len += ets_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline PdrIdIE &DownlinkDataReportIE::pdr_id(uint8_t idx)
{
   return pi_[idx];
}

inline DownlinkDataServiceInformationIE &DownlinkDataReportIE::dnlnk_data_svc_info(uint8_t idx)
{
   return ddsi_[idx];
}

inline int DownlinkDataReportIE::next_pdr_id()
{
   return (ie_.pdr_id_count < MAX_LIST_SIZE) ?
      ie_.pdr_id_count++ : -1;
}

inline int DownlinkDataReportIE::next_dnlnk_data_svc_info()
{
   return (ie_.dnlnk_data_svc_info_count < MAX_LIST_SIZE) ?
      ie_.dnlnk_data_svc_info_count++ : -1;
}

inline pfcp_dnlnk_data_rpt_ie_t &DownlinkDataReportIE::data()
{
   return ie_;
}

inline DownlinkDataReportIE::DownlinkDataReportIE(pfcp_dnlnk_data_rpt_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_DNLNK_DATA_RPT, parent),
     ie_(ie)
{
   for (int i=0; i<MAX_LIST_SIZE; i++)
   {
      pi_.push_back(PdrIdIE(ie_.pdr_id[i], this));
      ddsi_.push_back(DownlinkDataServiceInformationIE(ie_.dnlnk_data_svc_info[i], this));
   }
}

inline uint16_t DownlinkDataReportIE::calculateLength()
{
   uint16_t len = 0;

   for (int i=0; i<ie_.pdr_id_count; i++)
      len += pi_[i].packedLength();
   for (int i=0; i<ie_.dnlnk_data_svc_info_count; i++)
      len += ddsi_[i].packedLength();

   return len;
}

////////////////////////////////////////////////////////////////////////////////

inline BarIdIE &UpdateBarSessionReportRspIE::bar_id()
{
   return bi_;
}

inline DownlinkDataNotificationDelayIE &UpdateBarSessionReportRspIE::dnlnk_data_notif_delay()
{
   return ddnd_;
}

inline DlBufferingDurationIE &UpdateBarSessionReportRspIE::dl_buf_dur()
{
   return dbd_;
}

inline DlBufferingSuggestedPacketCountIE &UpdateBarSessionReportRspIE::dl_buf_suggstd_pckt_cnt()
{
   return dbspc_;
}

inline SuggestedBufferingPacketsCountIE &UpdateBarSessionReportRspIE::suggstd_buf_pckts_cnt()
{
   return sbpc_;
}

inline pfcp_upd_bar_sess_rpt_rsp_ie_t &UpdateBarSessionReportRspIE::data()
{
   return ie_;
}

inline UpdateBarSessionReportRspIE::UpdateBarSessionReportRspIE(pfcp_upd_bar_sess_rpt_rsp_ie_t &ie, LengthCalculator *parent)
   : IEHeader(ie.header, IE_UPD_BAR_SESS_RPT_RSP, parent),
     ie_(ie),
     bi_(ie_.bar_id, this),
     ddnd_(ie_.dnlnk_data_notif_delay, this),
     dbd_(ie_.dl_buf_dur, this),
     dbspc_(ie_.dl_buf_suggstd_pckt_cnt, this),
     sbpc_(ie_.suggstd_buf_pckts_cnt, this)
{
}

inline uint16_t UpdateBarSessionReportRspIE::calculateLength()
{
   return 
      bi_.packedLength() +
      ddnd_.packedLength() +
      dbd_.packedLength() +
      dbspc_.packedLength() +
      sbpc_.packedLength();
}

////////////////////////////////////////////////////////////////////////////////
//// Message Classes
////////////////////////////////////////////////////////////////////////////////

inline HeartbeatReq::HeartbeatReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({}),
     rts_(data_.rcvry_time_stmp, nullptr)
{
   // data_ = {};
   setMsgType(PFCP_HRTBEAT_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t HeartbeatReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t HeartbeatReq::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline HeartbeatReq &HeartbeatReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline RecoveryTimeStampIE &HeartbeatReq::recoveryTimeStamp()
{
   return rts_;
}

inline HeartbeatReq &HeartbeatReq::encode(uint8_t *dest)
{
   UShort len = encode_pfcp_hrtbeat_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_hrtbeat_req_t &HeartbeatReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline HeartbeatRsp::HeartbeatRsp()
   : data_({}),
     rts_(data_.rcvry_time_stmp, nullptr)
{
   setMsgType(PFCP_HRTBEAT_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t HeartbeatRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t HeartbeatRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline HeartbeatRsp &HeartbeatRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline RecoveryTimeStampIE &HeartbeatRsp::recoveryTimeStamp()
{
   return rts_;
}

inline HeartbeatRsp &HeartbeatRsp::encode(uint8_t *dest)
{
   UShort len = encode_pfcp_hrtbeat_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_hrtbeat_rsp_t &HeartbeatRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline PfdMgmtReq::PfdMgmtReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({})
{
   setMsgType(PFCP_PFD_MGMT_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
      appids_.push_back(ApplicationIdsPfdsIE(data_.app_ids_pfds[i],NULL));
}

inline uint16_t PfdMgmtReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t PfdMgmtReq::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline PfdMgmtReq &PfdMgmtReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline ApplicationIdsPfdsIE &PfdMgmtReq::app_ids_pfds(uint8_t idx)
{
   return appids_[idx];
}

inline PfdMgmtReq &PfdMgmtReq::encode(uint8_t *dest)
{
   UShort len = encode_pfcp_pfd_mgmt_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_pfd_mgmt_req_t &PfdMgmtReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline PfdMgmtRsp::PfdMgmtRsp()
   : data_({}),
     c_(data_.cause, nullptr),
     oi_(data_.offending_ie, nullptr)
{
   setMsgType(PFCP_PFD_MGMT_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t PfdMgmtRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t PfdMgmtRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline PfdMgmtRsp &PfdMgmtRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline CauseIE &PfdMgmtRsp::cause()
{
   return c_;
}

inline OffendingIeIE &PfdMgmtRsp::offending_ie()
{
   return oi_;
}

inline PfdMgmtRsp &PfdMgmtRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_pfd_mgmt_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_pfd_mgmt_rsp_t &PfdMgmtRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline AssnSetupReq::AssnSetupReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({}),
     ni_(data_.node_id, nullptr),
     rts_(data_.rcvry_time_stmp, nullptr),
     uff_(data_.up_func_feat, nullptr),
     cff_(data_.cp_func_feat, nullptr)
{
   setMsgType(PFCP_ASSN_SETUP_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
      upiri_.push_back(UserPlaneIpResourceInformationIE(data_.user_plane_ip_rsrc_info[i], nullptr));
}

inline uint16_t AssnSetupReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t AssnSetupReq::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline AssnSetupReq &AssnSetupReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline NodeIdIE &AssnSetupReq::node_id()
{
   return ni_;
}

inline RecoveryTimeStampIE &AssnSetupReq::rcvry_time_stmp()
{
   return rts_;
}

inline UpFunctionFeaturesIE &AssnSetupReq::up_func_feat()
{
   return uff_;
}

inline CpFunctionFeaturesIE &AssnSetupReq::cp_func_feat()
{
   return cff_;
}

inline UserPlaneIpResourceInformationIE &AssnSetupReq::user_plane_ip_rsrc_info(uint8_t idx)
{
   return upiri_[idx];
}

inline int AssnSetupReq::next_user_plane_ip_rsrc_info()
{
   return (data_.user_plane_ip_rsrc_info_count < MAX_LIST_SIZE) ?
      data_.user_plane_ip_rsrc_info_count++ : -1;
}

inline AssnSetupReq &AssnSetupReq::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_assn_setup_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_assn_setup_req_t &AssnSetupReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline AssnSetupRsp::AssnSetupRsp()
   : data_({}),
     ni_(data_.node_id, nullptr),
     c_(data_.cause, nullptr),
     rts_(data_.rcvry_time_stmp, nullptr),
     uff_(data_.up_func_feat, nullptr),
     cff_(data_.cp_func_feat, nullptr)
{
   setMsgType(PFCP_ASSN_SETUP_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
      upiri_.push_back(UserPlaneIpResourceInformationIE(data_.user_plane_ip_rsrc_info[i], nullptr));
}

inline uint16_t AssnSetupRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t AssnSetupRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline AssnSetupRsp &AssnSetupRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline NodeIdIE &AssnSetupRsp::node_id()
{
   return ni_;
}

inline CauseIE &AssnSetupRsp::cause()
{
   return c_;
}

inline RecoveryTimeStampIE &AssnSetupRsp::rcvry_time_stmp()
{
   return rts_;
}

inline UpFunctionFeaturesIE &AssnSetupRsp::up_func_feat()
{
   return uff_;
}

inline CpFunctionFeaturesIE &AssnSetupRsp::cp_func_feat()
{
   return cff_;
}

inline UserPlaneIpResourceInformationIE &AssnSetupRsp::user_plane_ip_rsrc_info(uint8_t idx)
{
   return upiri_[idx];
}

inline int AssnSetupRsp::next_user_plane_ip_rsrc_info()
{
   return (data_.user_plane_ip_rsrc_info_count < MAX_LIST_SIZE) ?
      data_.user_plane_ip_rsrc_info_count++ : -1;
}

inline AssnSetupRsp &AssnSetupRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_assn_setup_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_assn_setup_rsp_t &AssnSetupRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline AssnUpdateReq::AssnUpdateReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({}),
     ni_(data_.node_id, nullptr),
     uff_(data_.up_func_feat, nullptr),
     cff_(data_.cp_func_feat, nullptr),
     arr_(data_.up_assn_rel_req, nullptr),
     grp_(data_.graceful_rel_period, nullptr)
{
   setMsgType(PFCP_ASSN_UPD_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
      upiri_.push_back(UserPlaneIpResourceInformationIE(data_.user_plane_ip_rsrc_info[i], nullptr));
}

inline uint16_t AssnUpdateReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t AssnUpdateReq::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline AssnUpdateReq &AssnUpdateReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline NodeIdIE &AssnUpdateReq::node_id()
{
   return ni_;
}

inline UpFunctionFeaturesIE &AssnUpdateReq::up_func_feat()
{
   return uff_;
}

inline CpFunctionFeaturesIE &AssnUpdateReq::cp_func_feat()
{
   return cff_;
}

inline AssociationReleaseRequestIE &AssnUpdateReq::up_assn_rel_req()
{
   return arr_;
}

inline GracefulReleasePeriodIE &AssnUpdateReq::graceful_rel_period()
{
   return grp_;
}

inline UserPlaneIpResourceInformationIE &AssnUpdateReq::user_plane_ip_rsrc_info(uint8_t idx)
{
   return  upiri_[idx];
}

inline int AssnUpdateReq::next_user_plane_ip_rsrc_info()
{
   return (data_.user_plane_ip_rsrc_info_count < MAX_LIST_SIZE) ?
      data_.user_plane_ip_rsrc_info_count++ : -1;
}

inline AssnUpdateReq &AssnUpdateReq::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_assn_upd_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_assn_upd_req_t &AssnUpdateReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline AssnUpdateRsp::AssnUpdateRsp()
   : data_({}),
     ni_(data_.node_id, nullptr),
     c_(data_.cause, nullptr),
     uff_(data_.up_func_feat, nullptr),
     cff_(data_.cp_func_feat, nullptr)
{
   setMsgType(PFCP_ASSN_UPD_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t AssnUpdateRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t AssnUpdateRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline AssnUpdateRsp &AssnUpdateRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline NodeIdIE &AssnUpdateRsp::node_id()
{
   return ni_;
}

inline CauseIE &AssnUpdateRsp::cause()
{
   return c_;
}

inline UpFunctionFeaturesIE &AssnUpdateRsp::up_func_feat()
{
   return uff_;
}

inline CpFunctionFeaturesIE &AssnUpdateRsp::cp_func_feat()
{
   return cff_;
}

inline AssnUpdateRsp &AssnUpdateRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_assn_upd_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_assn_upd_rsp_t &AssnUpdateRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline AssnReleaseReq::AssnReleaseReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
      data_({}),
      ni_(data_.node_id, nullptr)
{
   setMsgType(PFCP_ASSN_REL_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t AssnReleaseReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t AssnReleaseReq::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline AssnReleaseReq &AssnReleaseReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline NodeIdIE &AssnReleaseReq::node_id()
{
   return ni_;
}

inline AssnReleaseReq &AssnReleaseReq::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_assn_rel_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_assn_rel_req_t &AssnReleaseReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline AssnReleaseRsp::AssnReleaseRsp()
   : data_({}),
     ni_(data_.node_id, nullptr),
     c_(data_.cause, nullptr)
{
   setMsgType(PFCP_ASSN_REL_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t AssnReleaseRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t AssnReleaseRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline AssnReleaseRsp &AssnReleaseRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline NodeIdIE &AssnReleaseRsp::node_id()
{
   return ni_;
}

inline CauseIE &AssnReleaseRsp::cause()
{
   return c_;
}

inline AssnReleaseRsp &AssnReleaseRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_assn_rel_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_assn_rel_rsp_t &AssnReleaseRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline VersionNotSupportedRsp::VersionNotSupportedRsp()
   : data_({})
{
   setMsgType(PFCP_VERSION_NOT_SUPPORTED);
   data_.message_type = msgType();
   data_.version = 1;
}

inline uint16_t VersionNotSupportedRsp::length() const
{
   return data_.message_len;
}

inline uint32_t VersionNotSupportedRsp::sequenceNumber() const
{
   return data_.seid_seqno.no_seid.seq_no;
}

inline VersionNotSupportedRsp &VersionNotSupportedRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline VersionNotSupportedRsp &VersionNotSupportedRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_header_t(&data_, dest);
   data_.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.message_len - 4);
   return *this;
}

inline pfcp_header_t &VersionNotSupportedRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline NodeReportReq::NodeReportReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({}),
     ni_(data_.node_id, nullptr),
     nrt_(data_.node_rpt_type, nullptr),
     uprfr_(data_.user_plane_path_fail_rpt, nullptr)
{
   setMsgType(PFCP_NODE_RPT_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t NodeReportReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t NodeReportReq::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline NodeReportReq &NodeReportReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline NodeIdIE &NodeReportReq::node_id()
{
   return ni_;
}

inline NodeReportTypeIE &NodeReportReq::node_rpt_type()
{
   return nrt_;
}

inline UserPlanePathFailureReportIE &NodeReportReq::user_plane_path_fail_rpt()
{
   return uprfr_;
}

inline NodeReportReq &NodeReportReq::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_node_rpt_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_node_rpt_req_t &NodeReportReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline NodeReportRsp::NodeReportRsp()
   : data_({}),
     ni_(data_.node_id, nullptr),
     c_(data_.cause, nullptr),
     oi_(data_.offending_ie, nullptr)
{
   setMsgType(PFCP_NODE_RPT_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t NodeReportRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t NodeReportRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.no_seid.seq_no;
}

inline NodeReportRsp &NodeReportRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline NodeIdIE &NodeReportRsp::node_id()
{
   return ni_;
}

inline CauseIE &NodeReportRsp::cause()
{
   return c_;
}

inline OffendingIeIE &NodeReportRsp::offending_ie()
{
   return oi_;
}

inline NodeReportRsp &NodeReportRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_node_rpt_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_node_rpt_rsp_t &NodeReportRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionSetDeletionReq::SessionSetDeletionReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({}),
     ni_(data_.node_id, nullptr),
     sc_(data_.sgw_c_fqcsid, nullptr),
     pc_(data_.pgw_c_fqcsid, nullptr),
     u_(data_.up_fqcsid, nullptr),
     t_(data_.twan_fqcsid, nullptr),
     e_(data_.epdg_fqcsid, nullptr),
     m_(data_.mme_fqcsid, nullptr)
{
   setMsgType(PFCP_SESS_SET_DEL_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t SessionSetDeletionReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionSetDeletionReq::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionSetDeletionReq &SessionSetDeletionReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline uint64_t SessionSetDeletionReq::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionSetDeletionReq &SessionSetDeletionReq::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline NodeIdIE &SessionSetDeletionReq::node_id()
{
   return ni_;
}

inline FqCsidIE &SessionSetDeletionReq::sgw_c_fqcsid()
{
   return sc_;
}

inline FqCsidIE &SessionSetDeletionReq::pgw_c_fqcsid()
{
   return pc_;
}

inline FqCsidIE &SessionSetDeletionReq::up_fqcsid()
{
   return u_;
}

inline FqCsidIE &SessionSetDeletionReq::twan_fqcsid()
{
   return t_;
}

inline FqCsidIE &SessionSetDeletionReq::epdg_fqcsid()
{
   return e_;
}

inline FqCsidIE &SessionSetDeletionReq::mme_fqcsid()
{
   return m_;
}

inline SessionSetDeletionReq &SessionSetDeletionReq::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_sess_set_del_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_set_del_req_t &SessionSetDeletionReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionSetDeletionRsp::SessionSetDeletionRsp()
   : data_({}),
     ni_(data_.node_id, nullptr),
     c_(data_.cause, nullptr),
     oi_(data_.offending_ie, nullptr)
{
   setMsgType(PFCP_SESS_SET_DEL_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t SessionSetDeletionRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionSetDeletionRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionSetDeletionRsp &SessionSetDeletionRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}
inline uint64_t SessionSetDeletionRsp::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionSetDeletionRsp &SessionSetDeletionRsp::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline NodeIdIE &SessionSetDeletionRsp::node_id()
{
   return ni_;
}

inline CauseIE &SessionSetDeletionRsp::cause()
{
   return c_;
}

inline OffendingIeIE &SessionSetDeletionRsp::offending_ie()
{
   return oi_;
}

inline SessionSetDeletionRsp &SessionSetDeletionRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_sess_set_del_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_set_del_rsp_t &SessionSetDeletionRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionEstablishmentReq::SessionEstablishmentReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({}),
     ni_(data_.node_id, nullptr),
     cpf_(data_.cp_fseid, nullptr),
     cb_(data_.create_bar, nullptr),
     pt_(data_.pdn_type, nullptr),
     sfc_(data_.sgw_c_fqcsid, nullptr),
     mfc_(data_.mme_fqcsid, nullptr),
     pfc_(data_.pgw_c_fqcsid, nullptr),
     efc_(data_.epdg_fqcsid, nullptr),
     tfc_(data_.twan_fqcsid, nullptr),
     upit_(data_.user_plane_inact_timer, nullptr),
     ui_(data_.user_id, nullptr),
     ti_(data_.trc_info, nullptr),
     ad_(data_.apn_dnn, nullptr)
{
   setMsgType(PFCP_SESS_ESTAB_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
   {
      cp_.push_back(CreatePdrIE(data_.create_pdr[i], nullptr));
      cf_.push_back(CreateFarIE(data_.create_far[i], nullptr));
      cu_.push_back(CreateUrrIE(data_.create_urr[i], nullptr));
      cq_.push_back(CreateQerIE(data_.create_qer[i], nullptr));
      cte_.push_back(CreateTrafficEndpointIE(data_.create_traffic_endpt[i], nullptr));
   }
}

inline uint16_t SessionEstablishmentReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionEstablishmentReq::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionEstablishmentReq &SessionEstablishmentReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.has_seid.seq_no = val;
   return *this;
}

inline uint64_t SessionEstablishmentReq::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionEstablishmentReq &SessionEstablishmentReq::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline NodeIdIE &SessionEstablishmentReq::node_id()
{
   return ni_;
}

inline FSeidIE &SessionEstablishmentReq::cp_fseid()
{
   return cpf_;
}

inline CreateBarIE &SessionEstablishmentReq::create_bar()
{
   return cb_;
}

inline PdnTypeIE &SessionEstablishmentReq::pdn_type()
{
   return pt_;
}

inline FqCsidIE &SessionEstablishmentReq::sgw_c_fqcsid()
{
   return sfc_;
}

inline FqCsidIE &SessionEstablishmentReq::mme_fqcsid()
{
   return mfc_;
}

inline FqCsidIE &SessionEstablishmentReq::pgw_c_fqcsid()
{
   return pfc_;
}

inline FqCsidIE &SessionEstablishmentReq::epdg_fqcsid()
{
   return efc_;
}

inline FqCsidIE &SessionEstablishmentReq::twan_fqcsid()
{
   return tfc_;
}

inline UserPlaneInactivityTimerIE &SessionEstablishmentReq::user_plane_inact_timer()
{
   return upit_;
}

inline UserIdIE &SessionEstablishmentReq::user_id()
{
   return ui_;
}

inline TraceInformationIE &SessionEstablishmentReq::trc_info()
{
   return ti_;
}

inline ApnDnnIE &SessionEstablishmentReq::apn_dnn()
{
   return ad_;
}

inline CreatePdrIE &SessionEstablishmentReq::create_pdr(uint8_t idx)
{
   return cp_[idx];
}

inline CreateFarIE &SessionEstablishmentReq::create_far(uint8_t idx)
{
   return cf_[idx];
}

inline CreateUrrIE &SessionEstablishmentReq::create_urr(uint8_t idx)
{
   return cu_[idx];
}

inline CreateQerIE &SessionEstablishmentReq::create_qer(uint8_t idx)
{
   return cq_[idx];
}

inline CreateTrafficEndpointIE &SessionEstablishmentReq::create_traffic_endpt(uint8_t idx)
{
   return cte_[idx];
}

inline int SessionEstablishmentReq::next_create_pdr()
{
   return (data_.create_pdr_count < MAX_LIST_SIZE) ?
      data_.create_pdr_count++ : -1;
}

inline int SessionEstablishmentReq::next_create_far()
{
   return (data_.create_far_count < MAX_LIST_SIZE) ?
      data_.create_far_count++ : -1;
}

inline int SessionEstablishmentReq::next_create_urr()
{
   return (data_.create_urr_count < MAX_LIST_SIZE) ?
      data_.create_urr_count++ : -1;
}

inline int SessionEstablishmentReq::next_create_qer()
{
   return (data_.create_qer_count < MAX_LIST_SIZE) ?
      data_.create_qer_count++ : -1;
}

inline int SessionEstablishmentReq::next_create_traffic_endpt()
{
   return (data_.create_traffic_endpt_count < MAX_LIST_SIZE) ?
      data_.create_traffic_endpt_count++ : -1;
}

inline SessionEstablishmentReq &SessionEstablishmentReq::encode(uint8_t *dest)
{
   UShort len = encode_pfcp_sess_estab_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_estab_req_t &SessionEstablishmentReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionEstablishmentRsp::SessionEstablishmentRsp()
   : data_({}),
     ni_(data_.node_id, nullptr),
     c_(data_.cause, nullptr),
     oi_(data_.offending_ie, nullptr),
     ufs_(data_.up_fseid, nullptr),
     lci_(data_.load_ctl_info, nullptr),
     oci_(data_.ovrld_ctl_info, nullptr),
     ufc_(data_.up_fqcsid, nullptr),
     fri_(data_.failed_rule_id, nullptr)
{
   setMsgType(PFCP_SESS_ESTAB_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
   {
      cp_.push_back(CreatedPdrIE(data_.created_pdr[i], nullptr));
      cte_.push_back(CreatedTrafficEndpointIE(data_.created_traffic_endpt[i], nullptr));
   }
}

inline uint16_t SessionEstablishmentRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionEstablishmentRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionEstablishmentRsp &SessionEstablishmentRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.has_seid.seq_no = val;
   return *this;
}

inline uint64_t SessionEstablishmentRsp::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionEstablishmentRsp &SessionEstablishmentRsp::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline NodeIdIE &SessionEstablishmentRsp::node_id()
{
   return ni_;
}

inline CauseIE &SessionEstablishmentRsp::cause()
{
   return c_;
}

inline OffendingIeIE &SessionEstablishmentRsp::offending_ie()
{
   return oi_;
}

inline FSeidIE &SessionEstablishmentRsp::up_fseid()
{
   return ufs_;
}

inline LoadControlInformationIE &SessionEstablishmentRsp::load_ctl_info()
{
   return lci_;
}

inline OverloadControlInformationIE &SessionEstablishmentRsp::ovrld_ctl_info()
{
   return oci_;
}

inline FqCsidIE &SessionEstablishmentRsp::up_fqcsid()
{
   return ufc_;
}

inline FailedRuleIdIE &SessionEstablishmentRsp::failed_rule_id()
{
   return fri_;
}

inline CreatedPdrIE &SessionEstablishmentRsp::created_pdr(uint8_t idx)
{
   return cp_[idx];
}

inline CreatedTrafficEndpointIE &SessionEstablishmentRsp::created_traffic_endpt(uint8_t idx)
{
   return cte_[idx];
}

inline int SessionEstablishmentRsp::next_created_pdr()
{
   return (data_.created_pdr_count < MAX_LIST_SIZE) ?
      data_.created_pdr_count++ : -1;
}

inline int SessionEstablishmentRsp::next_created_traffic_endpt()
{
   return (data_.created_traffic_endpt_count < MAX_LIST_SIZE) ?
      data_.created_traffic_endpt_count++ : -1;
}

inline SessionEstablishmentRsp &SessionEstablishmentRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_sess_estab_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_estab_rsp_t &SessionEstablishmentRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionModificationReq::SessionModificationReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({}),
     cfs_(data_.cp_fseid, nullptr),
     rb_(data_.remove_bar, nullptr),
     rte_(data_.rmv_traffic_endpt, nullptr),
     cb_(data_.create_bar, nullptr),
     cte_(data_.create_traffic_endpt, nullptr),
     ub_(data_.update_bar, nullptr),
     ute_(data_.upd_traffic_endpt, nullptr),
     f_(data_.pfcpsmreq_flags, nullptr),
     pcfc_(data_.pgw_c_fqcsid, nullptr),
     scfc_(data_.sgw_c_fqcsid, nullptr),
     mfc_(data_.mme_fqcsid, nullptr),
     efc_(data_.epdg_fqcsid, nullptr),
     tfc_(data_.twan_fqcsid, nullptr),
     upit_(data_.user_plane_inact_timer, nullptr),
     qur_(data_.query_urr_ref, nullptr),
     ti_(data_.trc_info, nullptr)
{
   setMsgType(PFCP_SESS_MOD_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
   {
      rp_.push_back(RemovePdrIE(data_.remove_pdr[i], nullptr));
      rf_.push_back(RemoveFarIE(data_.remove_far[i], nullptr));
      ru_.push_back(RemoveUrrIE(data_.remove_urr[i], nullptr));
      rq_.push_back(RemoveQerIE(data_.remove_qer[i], nullptr));
      cp_.push_back(CreatePdrIE(data_.create_pdr[i], nullptr));
      cf_.push_back(CreateFarIE(data_.create_far[i], nullptr));
      cu_.push_back(CreateUrrIE(data_.create_urr[i], nullptr));
      cq_.push_back(CreateQerIE(data_.create_qer[i], nullptr));
      up_.push_back(UpdatePdrIE(data_.update_pdr[i], nullptr));
      uf_.push_back(UpdateFarIE(data_.update_far[i], nullptr));
      uu_.push_back(UpdateUrrIE(data_.update_urr[i], nullptr));
      uq_.push_back(UpdateQerIE(data_.update_qer[i], nullptr));
      qu_.push_back(QueryUrrIE(data_.query_urr[i], nullptr));
   }
}

inline uint16_t SessionModificationReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionModificationReq::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionModificationReq &SessionModificationReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline uint64_t SessionModificationReq::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionModificationReq &SessionModificationReq::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline FSeidIE &SessionModificationReq::cp_fseid()
{
   return cfs_;
}

inline RemoveBarIE &SessionModificationReq::remove_bar()
{
   return rb_;
}

inline RemoveTrafficEndpointIE &SessionModificationReq::rmv_traffic_endpt()
{
   return rte_;
}

inline CreateBarIE &SessionModificationReq::create_bar()
{
   return cb_;
}

inline CreateTrafficEndpointIE &SessionModificationReq::create_traffic_endpt()
{
   return cte_;
}

inline UpdateBarSessionModificationReqIE &SessionModificationReq::update_bar()
{
   return ub_;
}

inline UpdateTrafficEndpointIE &SessionModificationReq::upd_traffic_endpt()
{
   return ute_;
}

inline PfcpSmReqFlagsIE &SessionModificationReq::pfcpsmreq_flags()
{
   return f_;
}

inline FqCsidIE &SessionModificationReq::pgw_c_fqcsid()
{
   return pcfc_;
}

inline FqCsidIE &SessionModificationReq::sgw_c_fqcsid()
{
   return scfc_;
}

inline FqCsidIE &SessionModificationReq::mme_fqcsid()
{
   return mfc_;
}

inline FqCsidIE &SessionModificationReq::epdg_fqcsid()
{
   return efc_;
}

inline FqCsidIE &SessionModificationReq::twan_fqcsid()
{
   return tfc_;
}

inline UserPlaneInactivityTimerIE &SessionModificationReq::user_plane_inact_timer()
{
   return upit_;
}

inline QueryUrrReferenceIE &SessionModificationReq::query_urr_ref()
{
   return qur_;
}

inline TraceInformationIE &SessionModificationReq::trc_info()
{
   return ti_;
}

inline RemovePdrIE &SessionModificationReq::remove_pdr(uint8_t idx)
{
   return rp_[idx];
}

inline RemoveFarIE &SessionModificationReq::remove_far(uint8_t idx)
{
   return rf_[idx];
}

inline RemoveUrrIE &SessionModificationReq::remove_urr(uint8_t idx)
{
   return ru_[idx];
}

inline RemoveQerIE &SessionModificationReq::remove_qer(uint8_t idx)
{
   return rq_[idx];
}

inline CreatePdrIE &SessionModificationReq::create_pdr(uint8_t idx)
{
   return cp_[idx];
}

inline CreateFarIE &SessionModificationReq::create_far(uint8_t idx)
{
   return cf_[idx];
}

inline CreateUrrIE &SessionModificationReq::create_urr(uint8_t idx)
{
   return cu_[idx];
}

inline CreateQerIE &SessionModificationReq::create_qer(uint8_t idx)
{
   return cq_[idx];
}

inline UpdatePdrIE &SessionModificationReq::update_pdr(uint8_t idx)
{
   return up_[idx];
}

inline UpdateFarIE &SessionModificationReq::update_far(uint8_t idx)
{
   return uf_[idx];
}

inline UpdateUrrIE &SessionModificationReq::update_urr(uint8_t idx)
{
   return uu_[idx];
}

inline UpdateQerIE &SessionModificationReq::update_qer(uint8_t idx)
{
   return uq_[idx];
}

inline QueryUrrIE &SessionModificationReq::query_urr(uint8_t idx)
{
   return qu_[idx];
}

inline int SessionModificationReq::next_remove_pdr()
{
   return (data_.remove_pdr_count < MAX_LIST_SIZE) ?
      data_.remove_pdr_count++ : -1;
}

inline int SessionModificationReq::next_remove_far()
{
   return (data_.remove_far_count < MAX_LIST_SIZE) ?
      data_.remove_far_count++ : -1;
}

inline int SessionModificationReq::next_remove_urr()
{
   return (data_.remove_urr_count < MAX_LIST_SIZE) ?
      data_.remove_urr_count++ : -1;
}

inline int SessionModificationReq::next_remove_qer()
{
   return (data_.remove_qer_count < MAX_LIST_SIZE) ?
      data_.remove_qer_count++ : -1;
}

inline int SessionModificationReq::next_create_pdr()
{
   return (data_.create_pdr_count < MAX_LIST_SIZE) ?
      data_.create_pdr_count++ : -1;
}

inline int SessionModificationReq::next_create_far()
{
   return (data_.create_far_count < MAX_LIST_SIZE) ?
      data_.create_far_count++ : -1;
}

inline int SessionModificationReq::next_create_urr()
{
   return (data_.create_urr_count < MAX_LIST_SIZE) ?
      data_.create_urr_count++ : -1;
}

inline int SessionModificationReq::next_create_qer()
{
   return (data_.create_qer_count < MAX_LIST_SIZE) ?
      data_.create_qer_count++ : -1;
}

inline int SessionModificationReq::next_update_pdr()
{
   return (data_.update_pdr_count < MAX_LIST_SIZE) ?
      data_.update_pdr_count++ : -1;
}

inline int SessionModificationReq::next_update_far()
{
   return (data_.update_far_count < MAX_LIST_SIZE) ?
      data_.update_far_count++ : -1;
}

inline int SessionModificationReq::next_update_urr()
{
   return (data_.update_urr_count < MAX_LIST_SIZE) ?
      data_.update_urr_count++ : -1;
}

inline int SessionModificationReq::next_update_qer()
{
   return (data_.update_qer_count < MAX_LIST_SIZE) ?
      data_.update_qer_count++ : -1;
}

inline int SessionModificationReq::next_query_urr()
{
   return (data_.query_urr_count < MAX_LIST_SIZE) ?
      data_.query_urr_count++ : -1;
}

inline SessionModificationReq &SessionModificationReq::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_sess_mod_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_mod_req_t &SessionModificationReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionModificationRsp::SessionModificationRsp()
   : data_({}),
     c_(data_.cause, nullptr),
     oi_(data_.offending_ie, nullptr),
     cp_(data_.created_pdr, nullptr),
     lci_(data_.load_ctl_info, nullptr),
     oci_(data_.ovrld_ctl_info, nullptr),
     fri_(data_.failed_rule_id, nullptr),
     auri_(data_.add_usage_rpts_info, nullptr),
     cute_(data_.createdupdated_traffic_endpt, nullptr)
{
   setMsgType(PFCP_SESS_MOD_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
      ur_.push_back(UsageReportSessionModificationRspIE(data_.usage_report[i], nullptr));
}

inline uint16_t SessionModificationRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionModificationRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionModificationRsp &SessionModificationRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline uint64_t SessionModificationRsp::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionModificationRsp &SessionModificationRsp::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline CauseIE &SessionModificationRsp::cause()
{
   return c_;
}

inline OffendingIeIE &SessionModificationRsp::offending_ie()
{
   return oi_;
}

inline CreatedPdrIE &SessionModificationRsp::created_pdr()
{
   return cp_;
}

inline LoadControlInformationIE &SessionModificationRsp::load_ctl_info()
{
   return lci_;
}

inline OverloadControlInformationIE &SessionModificationRsp::ovrld_ctl_info()
{
   return oci_;
}

inline FailedRuleIdIE &SessionModificationRsp::failed_rule_id()
{
   return fri_;
}

inline AdditionalUsageReportsInformationIE &SessionModificationRsp::add_usage_rpts_info()
{
   return auri_;
}

inline CreatedTrafficEndpointIE &SessionModificationRsp::createdupdated_traffic_endpt()
{
   return cute_;
}

inline UsageReportSessionModificationRspIE &SessionModificationRsp::usage_report(uint8_t idx)
{
   return ur_[idx];
}

inline int SessionModificationRsp::next_usage_report()
{
   return (data_.usage_report_count < MAX_LIST_SIZE) ?
      data_.usage_report_count++ : -1;
}

inline SessionModificationRsp &SessionModificationRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_sess_mod_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_mod_rsp_t &SessionModificationRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionDeletionReq::SessionDeletionReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({})
{
   setMsgType(PFCP_SESS_DEL_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t SessionDeletionReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionDeletionReq::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionDeletionReq &SessionDeletionReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline uint64_t SessionDeletionReq::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionDeletionReq &SessionDeletionReq::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline SessionDeletionReq &SessionDeletionReq::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_sess_del_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_del_req_t &SessionDeletionReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionDeletionRsp::SessionDeletionRsp()
   : data_({}),
     c_(data_.cause, nullptr),
     oi_(data_.offending_ie, nullptr),
     lci_(data_.load_ctl_info, nullptr),
     oci_(data_.ovrld_ctl_info, nullptr)
{
   setMsgType(PFCP_SESS_DEL_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
      ur_.push_back(UsageReportSessionDeletionRspIE(data_.usage_report[i], nullptr));
}

inline uint16_t SessionDeletionRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionDeletionRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionDeletionRsp &SessionDeletionRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline uint64_t SessionDeletionRsp::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionDeletionRsp &SessionDeletionRsp::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline CauseIE &SessionDeletionRsp::cause()
{
   return c_;
}

inline OffendingIeIE &SessionDeletionRsp::offending_ie()
{
   return oi_;
}

inline LoadControlInformationIE &SessionDeletionRsp::load_ctl_info()
{
   return lci_;
}

inline OverloadControlInformationIE &SessionDeletionRsp::ovrld_ctl_info()
{
   return oci_;
}

inline UsageReportSessionDeletionRspIE &SessionDeletionRsp::usage_report(uint8_t idx)
{
   return ur_[idx];
}

inline int SessionDeletionRsp::next_usage_report()
{
   return (data_.usage_report_count < MAX_LIST_SIZE) ?
      data_.usage_report_count++ : -1;
}

inline SessionDeletionRsp &SessionDeletionRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_sess_del_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_del_rsp_t &SessionDeletionRsp::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionReportReq::SessionReportReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
   : PFCP::AppMsgReq(ln,rn),
     data_({}),
     rt_(data_.report_type, nullptr),
     ddr_(data_.dnlnk_data_rpt, nullptr),
     eir_(data_.err_indctn_rpt, nullptr),
     lci_(data_.load_ctl_info, nullptr),
     oci_(data_.ovrld_ctl_info, nullptr),
     auri_(data_.add_usage_rpts_info, nullptr)
{
   setMsgType(PFCP_SESS_RPT_REQ);
   data_.header.message_type = msgType();
   data_.header.version = 1;

   for (int i=0; i<MAX_LIST_SIZE; i++)
      ur_.push_back(UsageReportSessionReportReqIE(data_.usage_report[i], nullptr));
}

inline uint16_t SessionReportReq::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionReportReq::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionReportReq &SessionReportReq::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline uint64_t SessionReportReq::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionReportReq &SessionReportReq::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline ReportTypeIE &SessionReportReq::report_type()
{
   return rt_;
}

inline DownlinkDataReportIE &SessionReportReq::dnlnk_data_rpt()
{
   return ddr_;
}

inline ErrorIndicationReportIE &SessionReportReq::err_indctn_rpt()
{
   return eir_;
}

inline LoadControlInformationIE &SessionReportReq::load_ctl_info()
{
   return lci_;
}

inline OverloadControlInformationIE &SessionReportReq::ovrld_ctl_info()
{
   return oci_;
}

inline AdditionalUsageReportsInformationIE &SessionReportReq::add_usage_rpts_info()
{
   return auri_;
}

inline UsageReportSessionReportReqIE &SessionReportReq::usage_report(uint8_t idx)
{
   return ur_[idx];
}

inline SessionReportReq &SessionReportReq::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_sess_rpt_req_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_rpt_req_t &SessionReportReq::data()
{
   return data_;
}

////////////////////////////////////////////////////////////////////////////////

inline SessionReportRsp::SessionReportRsp()
   : data_({}),
     c_(data_.cause, nullptr),
     oi_(data_.offending_ie, nullptr),
     ub_(data_.update_bar, nullptr),
     flags_(data_.sxsrrsp_flags, nullptr)
{
   setMsgType(PFCP_SESS_RPT_RSP);
   data_.header.message_type = msgType();
   data_.header.version = 1;
}

inline uint16_t SessionReportRsp::length() const
{
   return data_.header.message_len;
}

inline uint32_t SessionReportRsp::sequenceNumber() const
{
   return data_.header.seid_seqno.has_seid.seq_no;
}

inline SessionReportRsp &SessionReportRsp::sequenceNumber(uint32_t val)
{
   setSeqNbr(val);
   data_.header.seid_seqno.no_seid.seq_no = val;
   return *this;
}

inline uint64_t SessionReportRsp::seid() const
{
   return data_.header.seid_seqno.has_seid.seid;
}

inline SessionReportRsp &SessionReportRsp::seid(uint64_t val)
{
   setSeid(val);
   data_.header.seid_seqno.has_seid.seid = val;
   return *this;
}

inline CauseIE &SessionReportRsp::cause()
{
   return c_;
}

inline OffendingIeIE &SessionReportRsp::offending_ie()
{
   return oi_;
}

inline UpdateBarSessionReportRspIE &SessionReportRsp::update_bar()
{
   return ub_;
}

inline PfcpSrRspFlagsIE &SessionReportRsp::sxsrrsp_flags()
{
   return flags_;
}

inline SessionReportRsp &SessionReportRsp::encode(uint8_t *dest)
{
   uint16_t len = encode_pfcp_sess_rpt_rsp_t(&data_, dest);
   data_.header.message_len = len;
   reinterpret_cast<pfcp_header_t*>(dest)->message_len = htons(data_.header.message_len - 4);
   return *this;
}

inline pfcp_sess_rpt_rsp_t &SessionReportRsp::data()
{
   return data_;
}

} // namespace PFCP_R15

#endif // __PFCPR15INL_H