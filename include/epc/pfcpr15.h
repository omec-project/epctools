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

#ifndef __PFCPR15_H
#define __PFCPR15_H

#include "epfcp.h"
#include "eostring.h"

#include "pfcp_messages.h"
#include "pfcp_messages_encoder.h"
#include "pfcp_messages_decoder.h"

namespace PFCP_R15
{
////////////////////////////////////////////////////////////////////////////////
//// IE Classes
////////////////////////////////////////////////////////////////////////////////

class LengthCalculator
{
public:
   LengthCalculator(LengthCalculator *parent)
      : parent_(parent)
   {
   }

   virtual Void setLength()
   {
      if (parent_)
         parent_->calculateLength();
   }

   virtual uint16_t length() const = 0;
   virtual uint16_t packedLength() const = 0;

protected:
   virtual uint16_t calculateLength() = 0;

private:
   LengthCalculator *parent_;
};

class IEHeader : public LengthCalculator
{
public:
   uint16_t type() const               { return hdr_.type; }
   IEHeader &type(uint16_t type)       { hdr_.type = type; return *this; }

   uint16_t length() const             { return hdr_.len; }
   uint16_t packedLength() const       { return hdr_.len > 0 ? hdr_.len + sizeof(hdr_) : 0; }
   Void setLength()                    { hdr_.len = calculateLength(); LengthCalculator::setLength(); }
   Bool present() const                { return hdr_.len != 0; }

   pfcp_ie_header_t &header() { return hdr_; }

protected:
   IEHeader(pfcp_ie_header_t &hdr, uint16_t type, LengthCalculator *parent)
      : LengthCalculator(parent),
        hdr_(hdr)
   {
      IEHeader::type(type);
   }

   virtual uint16_t calculateLength() = 0;

private:
   IEHeader();
   pfcp_ie_header_t &hdr_;
};

////////////////////////////////////////////////////////////////////////////////

enum class CauseEnum : uint8_t
{
   Reserved                         = 0,
   RequestAccepted                  = 1,
   RequestRejected                  = 64,
   SessionContextNotFound           = 65,
   MandatoryIeMissing               = 66,
   ConditionalIeMissing             = 67,
   InvalidLength                    = 68,
   MandatoryIeIncorrect             = 69,
   InvalidForwardingPolicy          = 70,
   InvalidFTeidAllocationOption     = 71,
   NoEstablishedPfcpAssociation     = 72,
   RuleCreationModificationFailure  = 73,
   PfcpEntityInCongestion           = 74,
   NoResourcesAvailable             = 75,
   ServiceNotSupported              = 76,
   SystemFailure                    = 77
};

class CauseIE : public IEHeader
{
   friend class PfdMgmtRsp;
   friend class AssnSetupRsp;
   friend class AssnUpdateRsp;
   friend class AssnReleaseRsp;
   friend class NodeReportRsp;
   friend class SessionSetDeletionRsp;
   friend class SessionEstablishmentRsp;
   friend class SessionModificationRsp;
   friend class SessionDeletionRsp;
   friend class SessionReportRsp;
public:
   CauseEnum cause() const;
   CauseIE &cause(CauseEnum val);
   pfcp_cause_ie_t &data();

protected:
   CauseIE(pfcp_cause_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();
   
private:
   CauseIE();
   pfcp_cause_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class SourceInterfaceEnum : uint8_t
{
   Access      = 0,
   Core        = 1,
   SGILanN6Lan = 2,
   CPFunction  = 3
};

class SourceInterfaceIE : public IEHeader
{
   friend class PdiIE;
public:
   SourceInterfaceEnum interface_value() const;
   SourceInterfaceIE &interface_value(SourceInterfaceEnum val);
   pfcp_src_intfc_ie_t &data();

protected:
   SourceInterfaceIE(pfcp_src_intfc_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SourceInterfaceIE();
   pfcp_src_intfc_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class FTeidIE : public IEHeader
{
   friend class CreateTrafficEndpointIE;
   friend class PdiIE;
   friend class CreatedTrafficEndpointIE;
   friend class CreatedPdrIE;
   friend class UpdateTrafficEndpointIE;
   friend class ErrorIndicationReportIE;
public:
   Bool chid() const;
   Bool ch() const;
   Bool v6() const;
   Bool v4() const;
   uint32_t teid() const;
   const in_addr &ipv4_address() const;
   const in6_addr &ipv6_address() const;
   uint8_t choose_id() const;

   FTeidIE &teid(uint32_t val);
   FTeidIE &ip_address(const ESocket::Address &val);
   FTeidIE &ip_address(const EIpAddress &val);
   FTeidIE &ip_address(const in_addr &val);
   FTeidIE &ip_address(const in6_addr &val);
   FTeidIE &ch(Bool ch);
   FTeidIE &choose_id(uint8_t val);
   pfcp_fteid_ie_t &data();
   
protected:
   FTeidIE(pfcp_fteid_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   FTeidIE();
   pfcp_fteid_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class NetworkInstanceIE : public IEHeader
{
   friend class CreateTrafficEndpointIE;
   friend class ForwardingParametersIE;
   friend class EthernetPacketFilterIE;
   friend class PdiIE;
   friend class UpdateTrafficEndpointIE;
   friend class UpdateForwardingParametersIE;
   friend class UsageReportSessionReportReqIE;
public:
   const uint8_t *ntwk_inst() const;
   const uint16_t ntwk_inst_len() const;

   NetworkInstanceIE &ntwk_inst(const EOctetString &val);
   NetworkInstanceIE &ntwk_inst(const uint8_t *val, uint16_t len);
   pfcp_ntwk_inst_ie_t &data();

protected:
   NetworkInstanceIE(pfcp_ntwk_inst_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   NetworkInstanceIE();
   pfcp_ntwk_inst_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class SdfFilterIE : public IEHeader
{
   friend class EthernetPacketFilterIE;
   friend class PdiIE;
public:
   Bool fd() const;
   Bool ttc() const;
   Bool spi() const;
   Bool fl() const;
   Bool bid() const;

   const uint8_t *flow_desc() const;
   uint16_t len_of_flow_desc() const;
   const uint8_t *tos_traffic_cls() const;
   const uint8_t *secur_parm_idx() const;
   const uint8_t *flow_label() const;
   uint32_t sdf_filter_id() const;

   SdfFilterIE &bid(Bool bid);
   SdfFilterIE &flow_desc(const uint8_t *flowDesc, uint16_t flowDescLen);
   SdfFilterIE &tos_traffic_cls(const uint8_t *tosTrafficClass);
   SdfFilterIE &secur_parm_idx(const uint8_t *securityParameterIndex);
   SdfFilterIE &flow_label(const uint8_t *flowLabel);
   SdfFilterIE &sdf_filter_id(const uint32_t sdfFilterId);
   pfcp_sdf_filter_ie_t &data();

protected:
   SdfFilterIE(pfcp_sdf_filter_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SdfFilterIE();
   pfcp_sdf_filter_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class ApplicationIdIE : public IEHeader
{
   friend class ApplicationIdsPfdsIE;
   friend class PdiIE;
   friend class ApplicationDetectionInformationIE;
public:
   const uint8_t *app_ident() const;
   ApplicationIdIE &app_ident(const uint8_t *app_id, uint16_t len);
   pfcp_application_id_ie_t &data();

protected:
   ApplicationIdIE(pfcp_application_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ApplicationIdIE();
   pfcp_application_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class DownlinkGateEnum : uint8_t
{
   Open = 0,
   Closed = 1,
   Future1 = 2,
   Future2 = 3
};

enum class UplinkGateEnum : uint8_t
{
   Open = 0,
   Closed = 1,
   Future1 = 2,
   Future2 = 3
};

class GateStatusIE : public IEHeader
{
   friend class CreateQerIE;
   friend class UpdateQerIE;
public:
   DownlinkGateEnum dl_gate() const;
   UplinkGateEnum ul_gate() const;
   GateStatusIE &dl_gate(DownlinkGateEnum dl);
   GateStatusIE &ul_gate(UplinkGateEnum ul);
   pfcp_gate_status_ie_t &data();

protected:
   GateStatusIE(pfcp_gate_status_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   GateStatusIE();
   pfcp_gate_status_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class MbrIE : public IEHeader
{
   friend class CreateQerIE;
   friend class UpdateQerIE;
public:
   uint64_t ul_mbr() const;
   uint64_t dl_mbr() const;
   MbrIE &ul_mbr(uint64_t mbr);
   MbrIE &dl_mbr(uint64_t mbr);
   pfcp_mbr_ie_t &data();

protected:
   MbrIE(pfcp_mbr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   MbrIE();
   pfcp_mbr_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class GbrIE : public IEHeader
{
   friend class CreateQerIE;
   friend class UpdateQerIE;
public:
   uint64_t ul_gbr() const;
   uint64_t dl_gbr() const;
   GbrIE &ul_gbr(uint64_t mbr);
   GbrIE &dl_gbr(uint64_t mbr);
   pfcp_gbr_ie_t &data();
   
protected:
   GbrIE(pfcp_gbr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   GbrIE();
   pfcp_gbr_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class QerCorrelationIdIE : public IEHeader
{
   friend class CreateQerIE;
   friend class UpdateQerIE;
public:
   uint32_t qer_corr_id_val() const;
   QerCorrelationIdIE &qer_corr_id_val(uint32_t id);
   pfcp_qer_corr_id_ie_t &data();

protected:
   QerCorrelationIdIE(pfcp_qer_corr_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   QerCorrelationIdIE();
   pfcp_qer_corr_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class PrecedenceIE : public IEHeader
{
   friend class CreatePdrIE;
   friend class UpdatePdrIE;
public:
   uint32_t prcdnc_val() const;
   PrecedenceIE &prcdnc_val(uint32_t val);
   pfcp_precedence_ie_t &data();

protected:
   PrecedenceIE(pfcp_precedence_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PrecedenceIE();
   pfcp_precedence_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class TransportLevelMarkingIE : public IEHeader
{
   friend class DuplicatingParametersIE;
   friend class ForwardingParametersIE;
   friend class UpdateDuplicatingParametersIE;
   friend class UpdateForwardingParametersIE;
public:
   const uint8_t *tostraffic_cls() const;
   TransportLevelMarkingIE &tostraffic_cls(const uint8_t *val);
   pfcp_trnspt_lvl_marking_ie_t &data();

protected:
   TransportLevelMarkingIE(pfcp_trnspt_lvl_marking_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TransportLevelMarkingIE();
   pfcp_trnspt_lvl_marking_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class VolumeThresholdIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   Bool tovol() const;
   Bool ulvol() const;
   Bool dlvol() const;
   uint64_t total_volume() const;
   uint64_t uplink_volume() const;
   uint64_t downlink_volume() const;
   VolumeThresholdIE &total_volume(uint64_t val);
   VolumeThresholdIE &uplink_volume(uint64_t val);
   VolumeThresholdIE &downlink_volume(uint64_t val);
   pfcp_vol_thresh_ie_t &data();

protected:
   VolumeThresholdIE(pfcp_vol_thresh_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   VolumeThresholdIE();
   pfcp_vol_thresh_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class TimeThresholdIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t time_threshold() const;
   TimeThresholdIE &time_threshold(uint32_t val);
   pfcp_time_threshold_ie_t &data();

protected:
   TimeThresholdIE(pfcp_time_threshold_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TimeThresholdIE();
   pfcp_time_threshold_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class MonitoringTimeIE : public IEHeader
{
   friend class AdditionalMonitoringTimeIE;
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   const ETime monitoring_time() const;
   MonitoringTimeIE &monitoring_time(uint32_t val);
   MonitoringTimeIE &monitoring_time(const ETime &val);
   pfcp_monitoring_time_ie_t &data();

protected:
   MonitoringTimeIE(pfcp_monitoring_time_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   MonitoringTimeIE();
   pfcp_monitoring_time_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class SubsequentVolumeThresholdIE : public IEHeader
{
   friend class AdditionalMonitoringTimeIE;
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   Bool tovol() const;
   Bool ulvol() const;
   Bool dlvol() const;
   uint64_t total_volume() const;
   uint64_t uplink_volume() const;
   uint64_t downlink_volume() const;
   SubsequentVolumeThresholdIE &total_volume(uint64_t val);
   SubsequentVolumeThresholdIE &uplink_volume(uint64_t val);
   SubsequentVolumeThresholdIE &downlink_volume(uint64_t val);
   pfcp_sbsqnt_vol_thresh_ie_t &data();

protected:
   SubsequentVolumeThresholdIE(pfcp_sbsqnt_vol_thresh_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SubsequentVolumeThresholdIE();
   pfcp_sbsqnt_vol_thresh_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class SubsequentTimeThresholdIE : public IEHeader
{
   friend class AdditionalMonitoringTimeIE;
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t sbsqnt_time_thresh() const;
   SubsequentTimeThresholdIE &sbsqnt_time_thresh(uint32_t val);
   pfcp_sbsqnt_time_thresh_ie_t &data();

protected:
   SubsequentTimeThresholdIE(pfcp_sbsqnt_time_thresh_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SubsequentTimeThresholdIE();
   pfcp_sbsqnt_time_thresh_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class InactivityDetectionTimeIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t inact_det_time() const;
   InactivityDetectionTimeIE &inact_det_time(uint32_t val);
   pfcp_inact_det_time_ie_t &data();

protected:
   InactivityDetectionTimeIE(pfcp_inact_det_time_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   InactivityDetectionTimeIE();
   pfcp_inact_det_time_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class ReportingTriggersIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   Bool perio() const;
   Bool volth() const;
   Bool timth() const;
   Bool quhti() const;
   Bool start() const;
   Bool stopt() const;
   Bool droth() const;
   Bool liusa() const;
   Bool volqu() const;
   Bool timqu() const;
   Bool envcl() const;
   Bool macar() const;
   Bool eveth() const;
   Bool evequ() const;
   
   ReportingTriggersIE &setPerio(Bool val);
   ReportingTriggersIE &setVolth(Bool val);
   ReportingTriggersIE &setTimth(Bool val);
   ReportingTriggersIE &setQuhti(Bool val);
   ReportingTriggersIE &setStart(Bool val);
   ReportingTriggersIE &setStopt(Bool val);
   ReportingTriggersIE &setDroth(Bool val);
   ReportingTriggersIE &setLiusa(Bool val);
   ReportingTriggersIE &setVolqu(Bool val);
   ReportingTriggersIE &setTimqu(Bool val);
   ReportingTriggersIE &setEnvcl(Bool val);
   ReportingTriggersIE &setMacar(Bool val);
   ReportingTriggersIE &setEveth(Bool val);
   ReportingTriggersIE &setEvequ(Bool val);

   pfcp_rptng_triggers_ie_t &data();

protected:
   ReportingTriggersIE(pfcp_rptng_triggers_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ReportingTriggersIE();
   pfcp_rptng_triggers_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class RedirectAddressTypeEnum : uint8_t
{
   ipv4_address            = 0,
   ipv6_address            = 1,
   url                     = 2,
   sip_uri                 = 3,
   ipv4_and_ipv6_address   = 4
};

class RedirectInformationIE : public IEHeader
{
   friend class ForwardingParametersIE;
   friend class UpdateForwardingParametersIE;
public:
   RedirectAddressTypeEnum redir_addr_type() const;
   uint16_t redir_svr_addr_len() const;
   const uint8_t *redir_svr_addr() const;
   uint16_t other_redir_svr_addr_len() const;
   const uint8_t *other_redir_svr_addr() const;
   RedirectInformationIE &redir_addr_type(RedirectAddressTypeEnum val);
   RedirectInformationIE &redir_svr_addr(const uint8_t *val, uint16_t len);
   RedirectInformationIE &other_redir_svr_addr(const uint8_t *val, uint16_t len);
   pfcp_redir_info_ie_t &data();

protected:
   RedirectInformationIE(pfcp_redir_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RedirectInformationIE();
   pfcp_redir_info_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class ReportTypeIE : public IEHeader
{
   friend class SessionReportReq;
public:
   Bool dldr() const;
   Bool usar() const;
   Bool erir() const;
   Bool upir() const;
   ReportTypeIE &dldr(Bool val);
   ReportTypeIE &usar(Bool val);
   ReportTypeIE &erir(Bool val);
   ReportTypeIE &upir(Bool val);    
   pfcp_report_type_ie_t &data();

protected:
   ReportTypeIE(pfcp_report_type_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ReportTypeIE();
   pfcp_report_type_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class OffendingIeIE : public IEHeader
{
   friend class PfdMgmtRsp;
   friend class NodeReportRsp;
   friend class SessionSetDeletionRsp;
   friend class SessionEstablishmentRsp;
   friend class SessionModificationRsp;
   friend class SessionDeletionRsp;
   friend class SessionReportRsp;
public:
   uint16_t type_of_the_offending_ie() const;
   OffendingIeIE &type_of_the_offending_ie(uint16_t val);
   pfcp_offending_ie_ie_t &data();

protected:
   OffendingIeIE(pfcp_offending_ie_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   OffendingIeIE();
   pfcp_offending_ie_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class ForwardingPolicyIE : public IEHeader
{
   friend class DuplicatingParametersIE;
   friend class ForwardingParametersIE;
   friend class UpdateDuplicatingParametersIE;
   friend class UpdateForwardingParametersIE;
public:
   uint8_t frwdng_plcy_ident_len() const;
   const uint8_t *frwdng_plcy_ident() const;
   ForwardingPolicyIE &frwdng_plcy_ident(const uint8_t *val, uint8_t len);
   pfcp_frwdng_plcy_ie_t &data();

protected:
   ForwardingPolicyIE(pfcp_frwdng_plcy_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ForwardingPolicyIE();
   pfcp_frwdng_plcy_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class DestinationInterfaceEnum : uint8_t
{
   access         = 0,
   core           = 1,
   sgi_lan_n6_lan = 2,
   cp_function    = 3,
   li_function    = 4
};

class DestinationInterfaceIE : public IEHeader
{
   friend class DuplicatingParametersIE;
   friend class ForwardingParametersIE;
   friend class UpdateDuplicatingParametersIE;
   friend class UpdateForwardingParametersIE;
public:
   DestinationInterfaceEnum interface_value() const;
   DestinationInterfaceIE &interface_value(DestinationInterfaceEnum val);
   pfcp_dst_intfc_ie_t &data();

protected:
   DestinationInterfaceIE(pfcp_dst_intfc_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DestinationInterfaceIE();
   pfcp_dst_intfc_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class UpFunctionFeaturesIE : public IEHeader
{
   friend class AssnSetupReq;
   friend class AssnSetupRsp;
   friend class AssnUpdateReq;
   friend class AssnUpdateRsp;
public:
   Bool bucp() const;
   Bool ddnd() const;
   Bool dlbd() const;
   Bool trst() const;
   Bool ftup() const;
   Bool pfdm() const;
   Bool heeu() const;
   Bool treu() const;
   Bool empu() const;
   Bool pdiu() const;
   Bool udbc() const;
   Bool quoac() const;
   Bool trace() const;
   Bool frrt() const;
   Bool pfde() const;
   UpFunctionFeaturesIE &bucp(Bool val);
   UpFunctionFeaturesIE &ddnd(Bool val);
   UpFunctionFeaturesIE &dlbd(Bool val);
   UpFunctionFeaturesIE &trst(Bool val);
   UpFunctionFeaturesIE &ftup(Bool val);
   UpFunctionFeaturesIE &pfdm(Bool val);
   UpFunctionFeaturesIE &heeu(Bool val);
   UpFunctionFeaturesIE &treu(Bool val);
   UpFunctionFeaturesIE &empu(Bool val);
   UpFunctionFeaturesIE &pdiu(Bool val);
   UpFunctionFeaturesIE &udbc(Bool val);
   UpFunctionFeaturesIE &quoac(Bool val);
   UpFunctionFeaturesIE &trace(Bool val);
   UpFunctionFeaturesIE &frrt(Bool val);
   UpFunctionFeaturesIE &pfde(Bool val);

   pfcp_up_func_feat_ie_t &data();

protected:
   UpFunctionFeaturesIE(pfcp_up_func_feat_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpFunctionFeaturesIE();
   pfcp_up_func_feat_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class ApplyActionIE : public IEHeader
{
   friend class CreateFarIE;
   friend class UpdateFarIE;
public:
   Bool drop() const;
   Bool forw() const;
   Bool buff() const;
   Bool nocp() const;
   Bool dupl() const;

   ApplyActionIE &drop(Bool val);
   ApplyActionIE &forw(Bool val);
   ApplyActionIE &buff(Bool val);
   ApplyActionIE &nocp(Bool val);
   ApplyActionIE &dupl(Bool val);
    
   pfcp_apply_action_ie_t &data();

protected:
   ApplyActionIE(pfcp_apply_action_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ApplyActionIE();
   pfcp_apply_action_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class DownlinkDataServiceInformationIE : public IEHeader
{
   friend class DownlinkDataReportIE;
public:
   Bool ppi() const;
   Bool qfii() const;
   uint8_t paging_plcy_indctn_val() const;
   uint8_t qfi() const;
   DownlinkDataServiceInformationIE &paging_plcy_indctn_val(Bool val);
   DownlinkDataServiceInformationIE &qfi(Bool val);

   pfcp_dnlnk_data_svc_info_ie_t &data();

protected:
   DownlinkDataServiceInformationIE(pfcp_dnlnk_data_svc_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DownlinkDataServiceInformationIE();
   pfcp_dnlnk_data_svc_info_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class DownlinkDataNotificationDelayIE : public IEHeader
{
   friend class CreateBarIE;
   friend class UpdateBarSessionModificationReqIE;
   friend class SessionModificationReq;
   friend class UpdateBarSessionReportRspIE;
public:
   uint8_t delay_value() const;
   DownlinkDataNotificationDelayIE &delay_value(uint8_t val);
   pfcp_dnlnk_data_notif_delay_ie_t &data();

protected:
   DownlinkDataNotificationDelayIE(pfcp_dnlnk_data_notif_delay_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DownlinkDataNotificationDelayIE();
   pfcp_dnlnk_data_notif_delay_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class DlBufferingDurationTimerUnitEnum : uint8_t
{
   two_seconds = 0,
   one_minute  = 1,
   ten_minutes = 2,
   one_hour    = 3,
   ten_hours   = 4,
   infinite    = 7
};

class DlBufferingDurationIE : public IEHeader
{
   friend class UpdateBarSessionReportRspIE;
public:
   DlBufferingDurationTimerUnitEnum timer_unit() const;
   uint8_t timer_value() const;
   DlBufferingDurationIE &timer_unit(DlBufferingDurationTimerUnitEnum val);
   DlBufferingDurationIE &timer_unit(uint8_t val);
   pfcp_dl_buf_dur_ie_t &data();

protected:
   DlBufferingDurationIE(pfcp_dl_buf_dur_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DlBufferingDurationIE();
   pfcp_dl_buf_dur_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class DlBufferingSuggestedPacketCountIE : public IEHeader
{
   friend class UpdateBarSessionReportRspIE;
public:
   uint64_t pckt_cnt_val() const;
   DlBufferingSuggestedPacketCountIE &pckt_cnt_val(uint16_t val);
   pfcp_dl_buf_suggstd_pckt_cnt_ie_t &data();

protected:
   DlBufferingSuggestedPacketCountIE(pfcp_dl_buf_suggstd_pckt_cnt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DlBufferingSuggestedPacketCountIE();
   pfcp_dl_buf_suggstd_pckt_cnt_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class PfcpSmReqFlagsIE : public IEHeader
{
   friend class UpdateForwardingParametersIE;
   friend class SessionModificationReq;
public:
   Bool drobu() const;
   Bool sndem() const;
   Bool qaurr() const;
   PfcpSmReqFlagsIE &drobu(Bool val);
   PfcpSmReqFlagsIE &sndem(Bool val);
   PfcpSmReqFlagsIE &qaurr(Bool val);
   pfcp_pfcpsmreq_flags_ie_t &data();

protected:
   PfcpSmReqFlagsIE(pfcp_pfcpsmreq_flags_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PfcpSmReqFlagsIE();
   pfcp_pfcpsmreq_flags_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class PfcpSrRspFlagsIE : public IEHeader
{
   friend class SessionReportRsp;
public:
   Bool drobu() const;
   PfcpSrRspFlagsIE &drobu(Bool val);
   pfcp_pfcpsrrsp_flags_ie_t &data();

protected:
   PfcpSrRspFlagsIE(pfcp_pfcpsrrsp_flags_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PfcpSrRspFlagsIE();
   pfcp_pfcpsrrsp_flags_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class SequenceNumberIE : public IEHeader
{
   friend class OverloadControlInformationIE;
   friend class LoadControlInformationIE;
public:
   uint32_t sequence_number() const;
   SequenceNumberIE &sequence_number(uint32_t val);
   pfcp_sequence_number_ie_t &data();

protected:
   SequenceNumberIE(pfcp_sequence_number_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SequenceNumberIE();
   pfcp_sequence_number_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class MetricIE : public IEHeader
{
   friend class OverloadControlInformationIE;
   friend class LoadControlInformationIE;
public:
   uint8_t metric() const;
   MetricIE &metric(uint8_t val);
   pfcp_metric_ie_t &data();

protected:
   MetricIE(pfcp_metric_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();
private:
   MetricIE();
   pfcp_metric_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class TimerTimerUnitEnum : uint8_t
{
   two_seconds = 0,
   one_minute  = 1,
   ten_minutes = 2,
   one_hour    = 3,
   ten_hours   = 4,
   infinite    = 7
};

class TimerIE : public IEHeader
{
   friend class OverloadControlInformationIE;
public:
   TimerTimerUnitEnum timer_unit() const;
   uint8_t timer_value() const;
   TimerIE &timer_unit(TimerTimerUnitEnum val);
   TimerIE &timer_unit(uint8_t val);
   pfcp_timer_ie_t &data();

protected:
   TimerIE(pfcp_timer_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TimerIE();
   pfcp_timer_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class PdrIdIE : public IEHeader
{
   friend class CreatePdrIE;
   friend class CreatedPdrIE;
   friend class RemovePdrIE;
   friend class UpdatePdrIE;
   friend class DownlinkDataReportIE;
public:
   uint16_t rule_id() const;
   PdrIdIE &rule_id(uint16_t val);
   pfcp_pdr_id_ie_t &data();

protected:
   PdrIdIE(pfcp_pdr_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PdrIdIE();
   pfcp_pdr_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class FSeidIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionEstablishmentRsp;
   friend class SessionModificationReq;
public:
   Bool v4() const;
   Bool v6() const;
   uint64_t seid() const;
   const in_addr &ipv4_address() const;
   const in6_addr &ipv6_address() const;
   FSeidIE &seid(uint64_t val);
   FSeidIE &ip_address(const ESocket::Address &val);
   FSeidIE &ip_address(const EIpAddress &val);
   FSeidIE &ip_address(const in_addr &val);
   FSeidIE &ip_address(const in6_addr &val);
   pfcp_fseid_ie_t &data();

protected:
   FSeidIE(pfcp_fseid_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   FSeidIE();
   pfcp_fseid_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class NodeIdTypeEnum : uint8_t
{
   ipv4_address   = 0,
   ipv6_address   = 1,
   FQDN           = 2
};

class NodeIdIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionEstablishmentRsp;
   friend class AssnSetupReq;
   friend class AssnSetupRsp;
   friend class AssnUpdateReq;
   friend class AssnUpdateRsp;
   friend class AssnReleaseReq;
   friend class AssnReleaseRsp;
   friend class NodeReportReq;
   friend class NodeReportRsp;
   friend class SessionSetDeletionReq;
   friend class SessionSetDeletionRsp;
public:
   NodeIdTypeEnum node_id_type() const;
   const uint8_t *node_id_value() const;
   NodeIdIE &node_id_value(const ESocket::Address &val);
   NodeIdIE &node_id_value(const EIpAddress &val);
   NodeIdIE &node_id_value(const in_addr &val);
   NodeIdIE &node_id_value(const in6_addr &val);
   NodeIdIE &node_id_value(const uint8_t *val, uint8_t len, NodeIdTypeEnum type=NodeIdTypeEnum::FQDN);
   pfcp_node_id_ie_t &data();

protected:
   NodeIdIE(pfcp_node_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   NodeIdIE();
   pfcp_node_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class PfdContentsIE : public IEHeader
{
   friend class PfdContextIE;
public:
   Bool adnp() const;
   Bool aurl() const;
   Bool afd() const;
   Bool dnp() const;
   Bool cp() const;
   Bool dn() const;
   Bool url() const;
   Bool fd() const;

   uint16_t len_of_flow_desc() const;
   const uint8_t *flow_desc() const;
   uint16_t length_of_url() const;
   const uint8_t *url2() const;
   uint16_t len_of_domain_nm() const;
   const uint8_t *domain_name() const;
   uint16_t len_of_cstm_pfd_cntnt() const;
   const uint8_t *cstm_pfd_cntnt() const;
   uint16_t len_of_domain_nm_prot() const;
   const uint8_t *domain_name_prot() const;
   uint16_t len_of_add_flow_desc() const;
   const uint8_t *add_flow_desc() const;
   uint16_t len_of_add_url() const;
   const uint8_t *add_url() const;
   uint16_t len_of_add_domain_nm_prot() const;
   const uint8_t *add_domain_nm_prot() const;

   PfdContentsIE &flow_desc(const uint8_t *val, uint16_t len);
   PfdContentsIE &url2(const uint8_t *val, uint16_t len);
   PfdContentsIE &domain_name(const uint8_t *val, uint16_t len);
   PfdContentsIE &cstm_pfd_cntnt(const uint8_t *val, uint16_t len);
   PfdContentsIE &domain_name_prot(const uint8_t *val, uint16_t len);
   PfdContentsIE &add_flow_desc(const uint8_t *val, uint16_t len);
   PfdContentsIE &add_url(const uint8_t *val, uint16_t len);
   PfdContentsIE &add_domain_nm_prot(const uint8_t *val, uint16_t len);
   pfcp_pfd_contents_ie_t &data();

protected:
   PfdContentsIE(pfcp_pfd_contents_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PfdContentsIE();
   pfcp_pfd_contents_ie_t &ie_;

   Bool move(Bool flag, uint8_t *dest, uint16_t &destofs, uint8_t *src, uint16_t &srcofs, uint16_t len, uint16_t maxlen);
   Void updatePointer(Bool flag, uint8_t* &ptr, uint8_t *data, uint16_t &ofs, uint16_t len);
   Void updatePointers();
};

////////////////////////////////////////////////////////////////////////////////

class MeasurementMethodIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   Bool durat() const;
   Bool volum() const;
   Bool event() const;

   MeasurementMethodIE &durat(Bool val);
   MeasurementMethodIE &volum(Bool val);
   MeasurementMethodIE &event(Bool val);
    
   pfcp_meas_mthd_ie_t &data();

protected:
   MeasurementMethodIE(pfcp_meas_mthd_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   MeasurementMethodIE();
   pfcp_meas_mthd_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class UsageReportTriggerIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   Bool immer() const;
   Bool droth() const;
   Bool stopt() const;
   Bool start() const;
   Bool quhti() const;
   Bool timth() const;
   Bool volth() const;
   Bool perio() const;
   Bool eveth() const;
   Bool macar() const;
   Bool envcl() const;
   Bool monit() const;
   Bool termr() const;
   Bool liusa() const;
   Bool timqu() const;
   Bool volqu() const;
   Bool evequ() const;

   UsageReportTriggerIE &immer(Bool val);
   UsageReportTriggerIE &droth(Bool val);
   UsageReportTriggerIE &stopt(Bool val);
   UsageReportTriggerIE &start(Bool val);
   UsageReportTriggerIE &quhti(Bool val);
   UsageReportTriggerIE &timth(Bool val);
   UsageReportTriggerIE &volth(Bool val);
   UsageReportTriggerIE &perio(Bool val);
   UsageReportTriggerIE &eveth(Bool val);
   UsageReportTriggerIE &macar(Bool val);
   UsageReportTriggerIE &envcl(Bool val);
   UsageReportTriggerIE &monit(Bool val);
   UsageReportTriggerIE &termr(Bool val);
   UsageReportTriggerIE &liusa(Bool val);
   UsageReportTriggerIE &timqu(Bool val);
   UsageReportTriggerIE &volqu(Bool val);
   UsageReportTriggerIE &evequ(Bool val);

   pfcp_usage_rpt_trig_ie_t &data();

protected:
   UsageReportTriggerIE(pfcp_usage_rpt_trig_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UsageReportTriggerIE();
   pfcp_usage_rpt_trig_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class MeasurementPeriodIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t meas_period() const;
   MeasurementPeriodIE &meas_period(uint32_t val);
   pfcp_meas_period_ie_t &data();

protected:
   MeasurementPeriodIE(pfcp_meas_period_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   MeasurementPeriodIE();
   pfcp_meas_period_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class FqCsidNodeIdTypeEnum : uint8_t
{
   ipv4       = 0,
   ipv6       = 1,
   mcc_mnc_id = 2
};

class FqCsidIE : public IEHeader
{
   friend class SessionSetDeletionReq;
   friend class SessionEstablishmentReq;
   friend class SessionEstablishmentRsp;
   friend class SessionModificationReq;
public:
   FqCsidNodeIdTypeEnum fqcsid_node_id_type() const;
   uint8_t number_of_csids() const;
   const in_addr &ipv4_node_address() const;
   const in6_addr &ipv6_node_address() const;
   uint32_t mcc_mnc_id_node_address() const;
   uint16_t pdn_conn_set_ident(uint8_t idx) const;

   FqCsidIE &number_of_csids(uint8_t val);
   FqCsidIE &node_address(const ESocket::Address &val);
   FqCsidIE &node_address(const EIpAddress &val);
   FqCsidIE &node_address(const in_addr &val);
   FqCsidIE &node_address(const in6_addr &val);
   FqCsidIE &node_address(uint32_t val);
   pfcp_fqcsid_ie_t &data();

protected:
   FqCsidIE(pfcp_fqcsid_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   FqCsidIE();
   pfcp_fqcsid_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class VolumeMeasurementIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   Bool tovol() const;
   Bool ulvol() const;
   Bool dlvol() const;

   uint64_t total_volume() const;
   uint64_t uplink_volume() const;
   uint64_t downlink_volume() const;
   VolumeMeasurementIE &total_volume(uint64_t val);
   VolumeMeasurementIE &uplink_volume(uint64_t val);
   VolumeMeasurementIE &downlink_volume(uint64_t val);
   pfcp_vol_meas_ie_t &data();

protected:
   VolumeMeasurementIE(pfcp_vol_meas_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   VolumeMeasurementIE();
   pfcp_vol_meas_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class DurationMeasurementIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   uint32_t duration_value() const;
   DurationMeasurementIE &duration_value(uint32_t val);
   pfcp_dur_meas_ie_t &data();

protected:
   DurationMeasurementIE(pfcp_dur_meas_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DurationMeasurementIE();
   pfcp_dur_meas_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class TimeOfFirstPacketIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   ETime time_of_frst_pckt() const;
   TimeOfFirstPacketIE &time_of_frst_pckt(ETime &val);
   TimeOfFirstPacketIE &time_of_frst_pckt(uint32_t val);
   pfcp_time_of_frst_pckt_ie_t &data();

protected:
   TimeOfFirstPacketIE(pfcp_time_of_frst_pckt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TimeOfFirstPacketIE();
   pfcp_time_of_frst_pckt_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class TimeOfLastPacketIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   ETime time_of_lst_pckt() const;
   TimeOfLastPacketIE &time_of_frst_pckt(ETime &val);
   TimeOfLastPacketIE &time_of_frst_pckt(uint32_t val);
   pfcp_time_of_lst_pckt_ie_t &data();

protected:
   TimeOfLastPacketIE(pfcp_time_of_lst_pckt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TimeOfLastPacketIE();
   pfcp_time_of_lst_pckt_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class QuotaHoldingTimeIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t quota_hldng_time_val() const;
   QuotaHoldingTimeIE &quota_hldng_time_val(uint32_t val);
   pfcp_quota_hldng_time_ie_t &data();

protected:
   QuotaHoldingTimeIE(pfcp_quota_hldng_time_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   QuotaHoldingTimeIE();
   pfcp_quota_hldng_time_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class DroppedDlTrafficThresholdIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   Bool dlpa() const;
   Bool dlby() const;
   DroppedDlTrafficThresholdIE &dnlnk_pckts(uint64_t val);
   DroppedDlTrafficThresholdIE &nbr_of_bytes_of_dnlnk_data(uint64_t val);
   pfcp_drpd_dl_traffic_thresh_ie_t &data();

protected:
   DroppedDlTrafficThresholdIE(pfcp_drpd_dl_traffic_thresh_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DroppedDlTrafficThresholdIE();
   pfcp_drpd_dl_traffic_thresh_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class VolumeQuotaIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   Bool tovol() const;
   Bool ulvol() const;
   Bool dlvol() const;
   VolumeQuotaIE &total_volume(uint64_t val);
   VolumeQuotaIE &uplink_volume(uint64_t val);
   VolumeQuotaIE &downlink_volume(uint64_t val);
   pfcp_volume_quota_ie_t &data();

protected:
   VolumeQuotaIE(pfcp_volume_quota_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   VolumeQuotaIE();
   pfcp_volume_quota_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class TimeQuotaIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t time_quota_val() const;
   TimeQuotaIE &time_quota_val(uint32_t val);
   pfcp_time_quota_ie_t &data();

protected:
   TimeQuotaIE(pfcp_time_quota_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TimeQuotaIE();
   pfcp_time_quota_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class StartTimeIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   ETime start_time() const;
   StartTimeIE &start_time(const ETime &val);
   StartTimeIE &start_time(uint32_t val);
   pfcp_start_time_ie_t &data();

protected:
   StartTimeIE(pfcp_start_time_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   StartTimeIE();
   pfcp_start_time_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class EndTimeIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   ETime end_time() const;
   EndTimeIE &end_time(const ETime &val);
   EndTimeIE &end_time(uint32_t val);
   pfcp_end_time_ie_t &data();

protected:
   EndTimeIE(pfcp_end_time_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EndTimeIE();
   pfcp_end_time_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class UrrIdIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class CreatePdrIE;
   friend class QueryUrrIE;
   friend class RemoveUrrIE;
   friend class UpdateUrrIE;
   friend class UpdatePdrIE;
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   uint32_t urr_id_value() const;
   UrrIdIE &urr_id_value(uint32_t val);
   pfcp_urr_id_ie_t &data();

protected:
   UrrIdIE(pfcp_urr_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UrrIdIE();
   pfcp_urr_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class LinkedUrrIdIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t lnkd_urr_id_val() const;
   LinkedUrrIdIE &lnkd_urr_id_val(uint32_t val);
   pfcp_linked_urr_id_ie_t &data();

protected:
   LinkedUrrIdIE(pfcp_linked_urr_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   LinkedUrrIdIE();
   pfcp_linked_urr_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class InternetProtocolEnum
{
   TCP = 6,
   UDP = 17
};

class OuterHeaderCreationIE : public IEHeader
{
   friend class DuplicatingParametersIE;
   friend class ForwardingParametersIE;
   friend class UpdateDuplicatingParametersIE;
   friend class UpdateForwardingParametersIE;
public:
   Bool gtpu_udp_ipv4() const;
   Bool gtpu_udp_ipv6() const;
   Bool udp_ipv4() const;
   Bool udp_ipv6() const;
   Bool ipv4() const;
   Bool ipv6() const;
   Bool ctag() const;
   Bool stag() const;
   uint32_t teid() const;
   const in_addr &ipv4_address() const;
   const in6_addr &ipv6_address() const;
   uint32_t port_number() const;
   const uint8_t *ctag_value() const;
   const uint8_t *stag_value() const;
   OuterHeaderCreationIE &ip_address_teid(const ESocket::Address &addr, uint32_t teid);
   OuterHeaderCreationIE &ip_address_teid(const EIpAddress &addr, uint32_t teid);
   OuterHeaderCreationIE &ip_address_teid(const in_addr &addr, uint32_t teid);
   OuterHeaderCreationIE &ip_address_teid(const in6_addr &addr, uint32_t teid);
   OuterHeaderCreationIE &ip_address_port(const ESocket::Address &addr, uint16_t port, InternetProtocolEnum protocol);
   OuterHeaderCreationIE &ip_address_port(const EIpAddress &addr, uint16_t port, InternetProtocolEnum protocol);
   OuterHeaderCreationIE &ip_address_port(const in_addr &addr, uint16_t port, InternetProtocolEnum protocol);
   OuterHeaderCreationIE &ip_address_port(const in6_addr &addr, uint16_t port, InternetProtocolEnum protocol);
   OuterHeaderCreationIE &ctag(const uint8_t *val);
   OuterHeaderCreationIE &stag(const uint8_t *val);
   pfcp_outer_hdr_creation_ie_t &data();

protected:
   OuterHeaderCreationIE(pfcp_outer_hdr_creation_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   OuterHeaderCreationIE();
   pfcp_outer_hdr_creation_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class BarIdIE : public IEHeader
{
   friend class CreateBarIE;
   friend class CreateFarIE;
   friend class RemoveBarIE;
   friend class UpdateBarSessionModificationReqIE;
   friend class UpdateFarIE;
   friend class UpdateBarSessionReportRspIE;
public:
   uint8_t bar_id_value() const;
   BarIdIE &bar_id_value(uint8_t val);
   pfcp_bar_id_ie_t &data();

protected:
   BarIdIE(pfcp_bar_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   BarIdIE();
   pfcp_bar_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class CpFunctionFeaturesIE : public IEHeader
{
   friend class AssnSetupReq;
   friend class AssnSetupRsp;
   friend class AssnUpdateReq;
   friend class AssnUpdateRsp;
public:
   Bool load() const;
   Bool ovrl() const;
   CpFunctionFeaturesIE &load(Bool val);
   CpFunctionFeaturesIE &ovrl(Bool val);
   pfcp_cp_func_feat_ie_t &data();

protected:
   CpFunctionFeaturesIE(pfcp_cp_func_feat_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CpFunctionFeaturesIE();
   pfcp_cp_func_feat_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class UsageInformationIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   Bool bef() const;
   Bool aft() const;
   Bool uae() const;
   Bool ube() const;
   UsageInformationIE &bef(Bool val);
   UsageInformationIE &aft(Bool val);
   UsageInformationIE &uae(Bool val);
   UsageInformationIE &ube(Bool val);
   pfcp_usage_info_ie_t &data();

protected:
   UsageInformationIE(pfcp_usage_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UsageInformationIE();
   pfcp_usage_info_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class ApplicationInstanceIdIE : public IEHeader
{
   friend class ApplicationDetectionInformationIE;
public:

   const uint8_t *app_inst_ident() const;
   ApplicationInstanceIdIE &app_inst_ident(const uint8_t *val, uint16_t len);
   pfcp_app_inst_id_ie_t &data();

protected:
   ApplicationInstanceIdIE(pfcp_app_inst_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ApplicationInstanceIdIE();
   pfcp_app_inst_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class FlowDirectionEnum : uint8_t
{
   unspecified    = 0,
   downlink       = 1,
   uplink         = 2,
   bidirectional  = 3
};

class FlowInformationIE : public IEHeader
{
   friend class ApplicationDetectionInformationIE;
public:
   FlowDirectionEnum flow_direction() const;
   FlowInformationIE &flow_direction(FlowDirectionEnum val);
   FlowInformationIE &flow_desc(const uint8_t *val, uint16_t len);
   pfcp_flow_info_ie_t &data();

protected:
   FlowInformationIE(pfcp_flow_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   FlowInformationIE();
   pfcp_flow_info_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class UeIpAddressIE : public IEHeader
{
   friend class CreateTrafficEndpointIE;
   friend class PdiIE;
   friend class UpdateTrafficEndpointIE;
   friend class UsageReportSessionReportReqIE;
public:
   Bool v6() const;
   Bool v4() const;
   Bool sd() const;
   Bool ipv6d() const;
   const in_addr &ipv4_address() const;
   const in6_addr &ipv6_address() const;
   const uint8_t ipv6_pfx_dlgtn_bits() const;
   UeIpAddressIE &sd(Bool val);
   UeIpAddressIE &ip_address(const ESocket::Address &val);
   UeIpAddressIE &ip_address(const EIpAddress &val);
   UeIpAddressIE &ip_address(const in_addr &val);
   UeIpAddressIE &ip_address(const in6_addr &val);
   UeIpAddressIE &ipv6_pfx_dlgtn_bits(uint8_t val);
   pfcp_ue_ip_address_ie_t &data();

protected:
   UeIpAddressIE(pfcp_ue_ip_address_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UeIpAddressIE();
   pfcp_ue_ip_address_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class UplinkDownlinkTimeUnitEnum : uint8_t
{
   minute      = 0,
   six_minutes = 1,
   hour        = 2,
   day         = 3,
   week        = 4
};

class PacketRateIE : public IEHeader
{
   friend class CreateQerIE;
   friend class UpdateQerIE;
public:
   Bool ulpr() const;
   Bool dlpr() const;
   UplinkDownlinkTimeUnitEnum uplnk_time_unit() const;
   uint16_t max_uplnk_pckt_rate() const;
   UplinkDownlinkTimeUnitEnum dnlnk_time_unit() const;
   uint16_t max_dnlnk_pckt_rate() const;
   PacketRateIE &max_uplnk_pckt_rate(uint16_t val, UplinkDownlinkTimeUnitEnum tu);
   PacketRateIE &max_dnlnk_pckt_rate(uint16_t val, UplinkDownlinkTimeUnitEnum tu);
   pfcp_packet_rate_ie_t &data();

protected:
   PacketRateIE(pfcp_packet_rate_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PacketRateIE();
   pfcp_packet_rate_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class OuterHeaderRemovalEnum : uint8_t
{
   gtpu_udp_ipv4 = 0,
   gtpu_udp_ipv6 = 1,
   udp_ipv4      = 2,
   udp_ipv6      = 3,
   ipv4          = 4,
   ipv6          = 5,
   gtpu_udp_ip   = 6,
   stag          = 7,
   stag_ctag     = 8
};

class OuterHeaderRemovalIE : public IEHeader
{
   friend class CreatePdrIE;
   friend class UpdatePdrIE;
public:
   OuterHeaderRemovalEnum outer_hdr_removal_desc() const;
   Bool pdu_session_container() const;
   OuterHeaderRemovalIE &pdu_session_container(Bool val);
   pfcp_outer_hdr_removal_ie_t &data();

protected:
   OuterHeaderRemovalIE(pfcp_outer_hdr_removal_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   OuterHeaderRemovalIE();
   pfcp_outer_hdr_removal_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class RecoveryTimeStampIE : public IEHeader
{
   friend class HeartbeatReq;
   friend class HeartbeatRsp;
   friend class AssnSetupReq;
   friend class AssnSetupRsp;
public:
   ETime rcvry_time_stmp_val() const;
   RecoveryTimeStampIE &rcvry_time_stmp_val(const ETime &val);
   pfcp_rcvry_time_stmp_ie_t &data();

protected:
   RecoveryTimeStampIE(pfcp_rcvry_time_stmp_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RecoveryTimeStampIE();
   pfcp_rcvry_time_stmp_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class DlFlowLevelMarkingIE : public IEHeader
{
   friend class CreateQerIE;
   friend class UpdateQerIE;
public:
   Bool ttc() const;
   Bool sci() const;
   const uint8_t *tostraffic_cls() const;
   const uint8_t *svc_cls_indctr() const;
   DlFlowLevelMarkingIE &tostraffic_cls(const uint8_t *val);
   DlFlowLevelMarkingIE &svc_cls_indctr(const uint8_t *val);
   pfcp_dl_flow_lvl_marking_ie_t &data();

protected:
   DlFlowLevelMarkingIE(pfcp_dl_flow_lvl_marking_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DlFlowLevelMarkingIE();
   pfcp_dl_flow_lvl_marking_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class HeaderTypeEnum : uint8_t
{
   http = 0
};

class HeaderEnrichmentIE : public IEHeader
{
   friend class ForwardingParametersIE;
   friend class UpdateForwardingParametersIE;
public:
   HeaderTypeEnum header_type() const;
   uint8_t len_of_hdr_fld_nm() const;
   const uint8_t *hdr_fld_nm() const;
   uint8_t len_of_hdr_fld_val() const;
   const uint8_t *hdr_fld_val() const;
   HeaderEnrichmentIE &header_type(HeaderTypeEnum val);
   HeaderEnrichmentIE &header_fld_nm(const uint8_t *val, uint8_t len);
   HeaderEnrichmentIE &header_fld_val(const uint8_t *val, uint8_t len);
   pfcp_hdr_enrchmt_ie_t &data();

protected:
   HeaderEnrichmentIE(pfcp_hdr_enrchmt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   HeaderEnrichmentIE();
   pfcp_hdr_enrchmt_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class MeasurementInformationIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   Bool mbqe() const;
   Bool inam() const;
   Bool radi() const;
   Bool istm() const;
   MeasurementInformationIE &mbqe(Bool val);
   MeasurementInformationIE &inam(Bool val);
   MeasurementInformationIE &radi(Bool val);
   MeasurementInformationIE &istm(Bool val);
   pfcp_meas_info_ie_t &data();

protected:
   MeasurementInformationIE(pfcp_meas_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   MeasurementInformationIE();
   pfcp_meas_info_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class NodeReportTypeIE : public IEHeader
{
   friend class NodeReportReq;
public:
   Bool upfr() const;
   NodeReportTypeIE &upfr(Bool val);    
   pfcp_node_rpt_type_ie_t &data();

protected:
   NodeReportTypeIE(pfcp_node_rpt_type_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   NodeReportTypeIE();
   pfcp_node_rpt_type_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class RemoteGTPUPeerIE : public IEHeader
{
   friend class UserPlanePathFailureReportIE;
public:
   Bool v6() const;
   Bool v4() const;
   Bool di() const;
   Bool ni() const;
   const in_addr &ipv4_address() const;
   const in6_addr &ipv6_address() const;
   uint16_t len_of_dst_intfc_fld() const;
   DestinationInterfaceEnum dst_intfc_value() const;
   uint16_t len_of_ntwk_intfc_fld() const;
   const uint8_t *ntwk_instc() const;
   RemoteGTPUPeerIE &ip_address(const ESocket::Address &val);
   RemoteGTPUPeerIE &ip_address(const EIpAddress &val);
   RemoteGTPUPeerIE &ip_address(const in_addr &val);
   RemoteGTPUPeerIE &ip_address(const in6_addr &val);
   RemoteGTPUPeerIE &dst_intfc_fld(DestinationInterfaceEnum val);
   RemoteGTPUPeerIE &ntwk_instc(const uint8_t *val, uint16_t len);
   pfcp_rmt_gtpu_peer_ie_t &data();

protected:
   RemoteGTPUPeerIE(pfcp_rmt_gtpu_peer_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RemoteGTPUPeerIE();
   pfcp_rmt_gtpu_peer_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class UrSeqnIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   uint32_t urseqn() const;
   UrSeqnIE &urseqn(uint32_t val);
   pfcp_urseqn_ie_t &data();

protected:
   UrSeqnIE(pfcp_urseqn_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UrSeqnIE();
   pfcp_urseqn_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class ActivatePredefinedRulesIE : public IEHeader
{
   friend class CreatePdrIE;
   friend class UpdatePdrIE;
public:
   uint16_t predef_rules_nm_len() const;
   const uint8_t *predef_rules_nm() const;
   ActivatePredefinedRulesIE &predef_rules_nm(const uint8_t *val, uint16_t len);
   pfcp_actvt_predef_rules_ie_t &data();

protected:
   ActivatePredefinedRulesIE(pfcp_actvt_predef_rules_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ActivatePredefinedRulesIE();
   pfcp_actvt_predef_rules_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class DeactivatePredefinedRulesIE : public IEHeader
{
   friend class UpdatePdrIE;
public:
   uint16_t predef_rules_nm_len() const;
   const uint8_t *predef_rules_nm() const;
   DeactivatePredefinedRulesIE &predef_rules_nm(const uint8_t *val, uint16_t len);
   pfcp_deact_predef_rules_ie_t &data();

protected:
   DeactivatePredefinedRulesIE(pfcp_deact_predef_rules_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DeactivatePredefinedRulesIE();
   pfcp_deact_predef_rules_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class FarIdIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class CreateFarIE;
   friend class CreatePdrIE;
   friend class RemoveFarIE;
   friend class UpdateUrrIE;
   friend class UpdateFarIE;
   friend class UpdatePdrIE;
public:
   uint32_t far_id_value() const;
   FarIdIE &far_id_value(uint32_t val);
   pfcp_far_id_ie_t &data();

protected:
   FarIdIE(pfcp_far_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   FarIdIE();
   pfcp_far_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class QerIdIE : public IEHeader
{
   friend class CreateQerIE;
   friend class CreatePdrIE;
   friend class RemoveQerIE;
   friend class UpdateQerIE;
   friend class UpdatePdrIE;
public:
   uint32_t qer_id_value() const;
   QerIdIE &qer_id_value(uint32_t val);
   pfcp_qer_id_ie_t &data();

protected:
   QerIdIE(pfcp_qer_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   QerIdIE();
   pfcp_qer_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class OciFlagsIE : public IEHeader
{
   friend class OverloadControlInformationIE;
public:
   Bool aoci() const;
   OciFlagsIE &aoci(Bool val);
   pfcp_oci_flags_ie_t &data();

protected:
   OciFlagsIE(pfcp_oci_flags_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   OciFlagsIE();
   pfcp_oci_flags_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class AssociationReleaseRequestIE : public IEHeader
{
   friend class AssnUpdateReq;
public:
   Bool sarr() const;
   AssociationReleaseRequestIE &sarr(Bool val);
   pfcp_up_assn_rel_req_ie_t &data();

protected:
   AssociationReleaseRequestIE(pfcp_up_assn_rel_req_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   AssociationReleaseRequestIE();
   pfcp_up_assn_rel_req_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class GracefulReleasePeriodTimerUnitEnum : uint8_t
{
   two_seconds = 0,
   one_minute  = 1,
   ten_minutes = 2,
   one_hour    = 3,
   ten_hours   = 4,
   infinite    = 7
};

class GracefulReleasePeriodIE : public IEHeader
{
   friend class AssnUpdateReq;
public:
   GracefulReleasePeriodTimerUnitEnum timer_unit() const;
   uint8_t timer_value() const;
   GracefulReleasePeriodIE &timer_value(uint8_t val, GracefulReleasePeriodTimerUnitEnum tu);
   pfcp_graceful_rel_period_ie_t &data();

protected:
   GracefulReleasePeriodIE(pfcp_graceful_rel_period_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   GracefulReleasePeriodIE();
   pfcp_graceful_rel_period_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class PdnTypeEnum : uint8_t
{
   ipv4     = 1,
   ipv6     = 2,
   ipv4v6   = 3,
   nonip    = 4,
   ethernet = 5
};

class PdnTypeIE : public IEHeader
{
   friend class SessionEstablishmentReq;
public:
   PdnTypeEnum pdn_type() const;
   PdnTypeIE &pdn_type(PdnTypeEnum val);
   pfcp_pdn_type_ie_t &data();

protected:
   PdnTypeIE(pfcp_pdn_type_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PdnTypeIE();
   pfcp_pdn_type_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class RuleIdTypeEnum : uint8_t
{
   pdr = 0,
   far = 1,
   qer = 2,
   urr = 3,
   bar = 4
};

class FailedRuleIdIE : public IEHeader
{
   friend class SessionEstablishmentRsp;
   friend class SessionModificationRsp;
public:
   RuleIdTypeEnum rule_id_type() const;
   uint32_t rule_id_value() const;
   FailedRuleIdIE &rule_id_value(uint32_t val, RuleIdTypeEnum rt);
   pfcp_failed_rule_id_ie_t &data();

protected:
   FailedRuleIdIE(pfcp_failed_rule_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   FailedRuleIdIE();
   pfcp_failed_rule_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class BaseTimeIntervalTypeEnum : uint8_t
{
   ctp = 0,
   dtp = 1
};

class TimeQuotaMechanismIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   BaseTimeIntervalTypeEnum btit() const;
   uint32_t base_time_int() const;
   TimeQuotaMechanismIE &base_time_int(uint32_t val, BaseTimeIntervalTypeEnum btit);
   pfcp_time_quota_mech_ie_t &data();

protected:
   TimeQuotaMechanismIE(pfcp_time_quota_mech_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TimeQuotaMechanismIE();
   pfcp_time_quota_mech_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class UserPlaneIpResourceInformationIE : public IEHeader
{
   friend class AssnSetupReq;
   friend class AssnSetupRsp;
   friend class AssnUpdateReq;
public:
   Bool v4() const;
   Bool v6() const;
   uint8_t teidri() const;
   Bool assoni() const;
   Bool assosi() const;
   uint8_t teid_range() const;
   UserPlaneIpResourceInformationIE &teid_range(uint8_t bits, uint8_t val);
   const in_addr &ipv4_address() const;
   const in6_addr &ipv6_address() const;
   uint8_t ntwk_inst_len() const;
   const uint8_t *ntwk_inst() const;
   SourceInterfaceEnum src_intfc() const;
   UserPlaneIpResourceInformationIE &ip_address(const ESocket::Address &val);
   UserPlaneIpResourceInformationIE &ip_address(const EIpAddress &val);
   UserPlaneIpResourceInformationIE &ip_address(const in_addr &val);
   UserPlaneIpResourceInformationIE &ip_address(const in6_addr &val);
   UserPlaneIpResourceInformationIE &ntwk_inst(const uint8_t *val, uint8_t len);
   UserPlaneIpResourceInformationIE &src_intfc(SourceInterfaceEnum val);
   pfcp_user_plane_ip_rsrc_info_ie_t &data();

protected:
   UserPlaneIpResourceInformationIE(pfcp_user_plane_ip_rsrc_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UserPlaneIpResourceInformationIE();
   pfcp_user_plane_ip_rsrc_info_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class UserPlaneInactivityTimerIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionModificationReq;
public:
   uint32_t user_plane_inact_timer() const;
   UserPlaneInactivityTimerIE &user_plane_inact_timer(uint32_t val);
   pfcp_user_plane_inact_timer_ie_t &data();

protected:
   UserPlaneInactivityTimerIE(pfcp_user_plane_inact_timer_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UserPlaneInactivityTimerIE();
   pfcp_user_plane_inact_timer_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class MultiplierIE : public IEHeader
{
   friend class AggregatedUrrsIE;
public:
   int64_t value_digits() const;
   int32_t exponent() const;
   MultiplierIE &value_digits(int64_t val);
   MultiplierIE &exponent(int32_t val);
   pfcp_multiplier_ie_t &data();

protected:
   MultiplierIE(pfcp_multiplier_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   MultiplierIE();
   pfcp_multiplier_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class AggregatedUrrIdIE : public IEHeader
{
   friend class AggregatedUrrsIE;
public:
   uint32_t urr_id_value() const;
   AggregatedUrrIdIE &urr_id_value(uint32_t val);
   pfcp_agg_urr_id_ie_t &data();

protected:
   AggregatedUrrIdIE(pfcp_agg_urr_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   AggregatedUrrIdIE();
   pfcp_agg_urr_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class SubsequentVolumeQuotaIE : public IEHeader
{
   friend class AdditionalMonitoringTimeIE;
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   Bool tovol() const;
   Bool ulvol() const;
   Bool dlvol() const;
   SubsequentVolumeQuotaIE &total_volume(uint64_t val);
   SubsequentVolumeQuotaIE &uplink_volume(uint64_t val);
   SubsequentVolumeQuotaIE &downlink_volume(uint64_t val);
   pfcp_sbsqnt_vol_quota_ie_t &data();

protected:
   SubsequentVolumeQuotaIE(pfcp_sbsqnt_vol_quota_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SubsequentVolumeQuotaIE();
   pfcp_sbsqnt_vol_quota_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class SubsequentTimeQuotaIE : public IEHeader
{
   friend class AdditionalMonitoringTimeIE;
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t time_quota_val() const;
   SubsequentTimeQuotaIE &time_quota_val(uint32_t val);
   pfcp_sbsqnt_time_quota_ie_t &data();

protected:
   SubsequentTimeQuotaIE(pfcp_sbsqnt_time_quota_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SubsequentTimeQuotaIE();
   pfcp_sbsqnt_time_quota_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class RqiIE : public IEHeader
{
   friend class CreateQerIE;
   friend class UpdateQerIE;
public:
   Bool rqi() const;
   RqiIE &rqi(Bool val);
   pfcp_rqi_ie_t &data();

protected:
   RqiIE(pfcp_rqi_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RqiIE();
   pfcp_rqi_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class QfiIE : public IEHeader
{
   friend class CreateQerIE;
   friend class PdiIE;
   friend class UpdateQerIE;
public:
   uint8_t qfi_value() const;
   QfiIE &qfi_value(uint8_t val);
   pfcp_qfi_ie_t &data();

protected:
   QfiIE(pfcp_qfi_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   QfiIE();
   pfcp_qfi_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class QueryUrrReferenceIE : public IEHeader
{
   friend class SessionModificationReq;
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   uint32_t query_urr_ref_val() const;
   QueryUrrReferenceIE &query_urr_ref_val(uint32_t val);
   pfcp_query_urr_ref_ie_t &data();

protected:
   QueryUrrReferenceIE(pfcp_query_urr_ref_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   QueryUrrReferenceIE();
   pfcp_query_urr_ref_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class AdditionalUsageReportsInformationIE : public IEHeader
{
   friend class SessionModificationRsp;
   friend class SessionReportReq;
public:
   Bool auri() const;
   uint16_t nbr_of_add_usage_rpts_val() const;
   AdditionalUsageReportsInformationIE &auri(Bool val);
   AdditionalUsageReportsInformationIE &nbr_of_add_usage_rpts_val(uint16_t val);
   pfcp_add_usage_rpts_info_ie_t &data();

protected:
   AdditionalUsageReportsInformationIE(pfcp_add_usage_rpts_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   AdditionalUsageReportsInformationIE();
   pfcp_add_usage_rpts_info_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class TrafficEndpointIdIE : public IEHeader
{
   friend class CreateTrafficEndpointIE;
   friend class ForwardingParametersIE;
   friend class PdiIE;
   friend class CreatedTrafficEndpointIE;
   friend class RemoveTrafficEndpointIE;
   friend class UpdateTrafficEndpointIE;
   friend class UpdateForwardingParametersIE;
public:
   uint8_t traffic_endpt_id_val() const;
   TrafficEndpointIdIE &traffic_endpt_id_val(uint8_t val);
   pfcp_traffic_endpt_id_ie_t &data();

protected:
   TrafficEndpointIdIE(pfcp_traffic_endpt_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TrafficEndpointIdIE();
   pfcp_traffic_endpt_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class MacAddressIE : public IEHeader
{
   friend class EthernetPacketFilterIE;
public:
   Bool sour() const;
   Bool dest() const;
   Bool usou() const;
   Bool udes() const;
   const uint8_t *src_mac_addr_val() const;
   const uint8_t *dst_mac_addr_val() const;
   const uint8_t *upr_src_mac_addr_val() const;
   const uint8_t *upr_dst_mac_addr_val() const;
   MacAddressIE &src_mac_addr_val(const uint8_t *val);
   MacAddressIE &dst_mac_addr_val(const uint8_t *val);
   MacAddressIE &upr_dst_mac_addr_val(const uint8_t *val);
   MacAddressIE &upr_src_mac_addr_val(const uint8_t *val);
   pfcp_mac_address_ie_t &data();

protected:
   MacAddressIE(pfcp_mac_address_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   MacAddressIE();
   pfcp_mac_address_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class CTagIE : public IEHeader
{
   friend class EthernetPacketFilterIE;
public:
   Bool pcp() const;
   Bool dei() const;
   Bool vid() const;
   uint8_t pcp_value() const;
   Bool dei_flag() const;
   uint8_t cvid_value() const;
   uint8_t cvid_value2() const;
   CTagIE &pcp(Bool val);
   CTagIE &dei(Bool val);
   CTagIE &vid(Bool val);
   CTagIE &pcp_value(uint8_t val);
   CTagIE &dei_flag(Bool val);
   CTagIE &cvid_value(uint8_t val);
   CTagIE &cvid_value2(uint8_t val);
   pfcp_ctag_ie_t &data();

protected:
   CTagIE(pfcp_ctag_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CTagIE();
   pfcp_ctag_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class STagIE : public IEHeader
{
   friend class EthernetPacketFilterIE;
public:
   Bool pcp() const;
   Bool dei() const;
   Bool vid() const;
   uint8_t pcp_value() const;
   Bool dei_flag() const;
   uint8_t svid_value() const;
   uint8_t svid_value2() const;
   STagIE &pcp(Bool val);
   STagIE &dei(Bool val);
   STagIE &vid(Bool val);
   STagIE &pcp_value(uint8_t val);
   STagIE &dei_flag(Bool val);
   STagIE &svid_value(uint8_t val);
   STagIE &svid_value2(uint8_t val);
   pfcp_stag_ie_t &data();

protected:
   STagIE(pfcp_stag_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   STagIE();
   pfcp_stag_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class EthertypeIE : public IEHeader
{
   friend class EthernetPacketFilterIE;
public:
   uint16_t ethertype() const;
   EthertypeIE &ethertype(uint16_t val);
   pfcp_ethertype_ie_t &data();

protected:
   EthertypeIE(pfcp_ethertype_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EthertypeIE();
   pfcp_ethertype_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class ProxyingIE : public IEHeader
{
   friend class ForwardingParametersIE;
public:
   Bool arp() const;
   Bool ins() const;
   ProxyingIE &arp(Bool val);
   ProxyingIE &ins(Bool val);
   pfcp_proxying_ie_t &data();

protected:
   ProxyingIE(pfcp_proxying_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ProxyingIE();
   pfcp_proxying_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class EthernetFilterIdIE : public IEHeader
{
   friend class EthernetPacketFilterIE;
public:
   uint32_t eth_fltr_id_val() const;
   EthernetFilterIdIE &eth_fltr_id_val(uint32_t val);
   pfcp_eth_fltr_id_ie_t &data();

protected:
   EthernetFilterIdIE(pfcp_eth_fltr_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EthernetFilterIdIE();
   pfcp_eth_fltr_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class EthernetFilterPropertiesIE : public IEHeader
{
   friend class EthernetPacketFilterIE;
public:
   Bool bide() const;
   EthernetFilterPropertiesIE &bide(Bool val);
   pfcp_eth_fltr_props_ie_t &data();

protected:
   EthernetFilterPropertiesIE(pfcp_eth_fltr_props_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EthernetFilterPropertiesIE();
   pfcp_eth_fltr_props_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class SuggestedBufferingPacketsCountIE : public IEHeader
{
   friend class CreateBarIE;
   friend class UpdateBarSessionModificationReqIE;
   friend class UpdateBarSessionReportRspIE;
public:
   uint8_t pckt_cnt_val() const;
   SuggestedBufferingPacketsCountIE &pckt_cnt_val(uint8_t val);
   pfcp_suggstd_buf_pckts_cnt_ie_t &data();

protected:
   SuggestedBufferingPacketsCountIE(pfcp_suggstd_buf_pckts_cnt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SuggestedBufferingPacketsCountIE();
   pfcp_suggstd_buf_pckts_cnt_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class UserIdIE : public IEHeader
{
   friend class SessionEstablishmentReq;
public:
   Bool imsif() const;
   Bool imeif() const;
   Bool msisdnf() const;
   Bool naif() const;
   uint8_t length_of_imsi() const;
   uint8_t length_of_imei() const;
   uint8_t len_of_msisdn() const;
   uint8_t length_of_nai() const;
   const uint8_t *imsi() const;
   const uint8_t *imei() const;
   const uint8_t *msisdn() const;
   const uint8_t *nai() const;
   UserIdIE &imsi(const uint8_t *val, uint8_t len);
   UserIdIE &imei(const uint8_t *val, uint8_t len);
   UserIdIE &msisdn(const uint8_t *val, uint8_t len);
   UserIdIE &nai(const uint8_t *val, uint8_t len);
   pfcp_user_id_ie_t &data();

protected:
   UserIdIE(pfcp_user_id_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UserIdIE();
   pfcp_user_id_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class EthernetPduSessionInformationIE : public IEHeader
{
   friend class CreateTrafficEndpointIE;
   friend class PdiIE;
public:
   Bool ethi() const;
   EthernetPduSessionInformationIE &ethi(Bool val);
   pfcp_eth_pdu_sess_info_ie_t &data();

protected:
   EthernetPduSessionInformationIE(pfcp_eth_pdu_sess_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EthernetPduSessionInformationIE();
   pfcp_eth_pdu_sess_info_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class MacAddressesDetectedIE : public IEHeader
{
   friend class EthernetTrafficInformationIE;
public:
   uint8_t nbr_of_mac_addrs() const;
   const uint8_t *mac_addr_val(uint8_t idx);
   MacAddressesDetectedIE &mac_addr_val(const uint8_t *val);
   pfcp_mac_addrs_detctd_ie_t &data();

protected:
   MacAddressesDetectedIE(pfcp_mac_addrs_detctd_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   MacAddressesDetectedIE();
   pfcp_mac_addrs_detctd_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class MacAddressesRemovedIE : public IEHeader
{
   friend class EthernetTrafficInformationIE;
public:
   uint8_t nbr_of_mac_addrs() const;
   const uint8_t *mac_addr_val(uint8_t idx);
   MacAddressesRemovedIE &mac_addr_val(const uint8_t *val);
   pfcp_mac_addrs_rmvd_ie_t &data();

protected:
   MacAddressesRemovedIE(pfcp_mac_addrs_rmvd_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   MacAddressesRemovedIE();
   pfcp_mac_addrs_rmvd_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class EthernetInactivityTimerIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t eth_inact_timer() const;
   EthernetInactivityTimerIE &eth_inact_timer(uint32_t val);
   pfcp_eth_inact_timer_ie_t &data();

protected:
   EthernetInactivityTimerIE(pfcp_eth_inact_timer_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EthernetInactivityTimerIE();
   pfcp_eth_inact_timer_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class SubsequentEventQuotaIE : public IEHeader
{
   friend class AdditionalMonitoringTimeIE;
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t sbsqnt_evnt_quota() const;
   SubsequentEventQuotaIE &sbsqnt_evnt_quota(uint32_t val);
   pfcp_sbsqnt_evnt_quota_ie_t &data();

protected:
   SubsequentEventQuotaIE(pfcp_sbsqnt_evnt_quota_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SubsequentEventQuotaIE();
   pfcp_sbsqnt_evnt_quota_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class SubsequentEventThresholdIE : public IEHeader
{
   friend class AdditionalMonitoringTimeIE;
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t sbsqnt_evnt_thresh() const;
   SubsequentEventThresholdIE &sbsqnt_evnt_thresh(uint32_t val);
   pfcp_sbsqnt_evnt_thresh_ie_t &data();

protected:
   SubsequentEventThresholdIE(pfcp_sbsqnt_evnt_thresh_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   SubsequentEventThresholdIE();
   pfcp_sbsqnt_evnt_thresh_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class TraceInformationIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionModificationReq;
public:
   uint8_t mcc_digit_1() const;
   uint8_t mcc_digit_2() const;
   uint8_t mcc_digit_3() const;
   uint8_t mnc_digit_1() const;
   uint8_t mnc_digit_2() const;
   uint8_t mnc_digit_3() const;
   uint32_t trace_id() const;
   uint8_t len_of_trigrng_evnts() const;
   const uint8_t *trigrng_evnts() const;
   uint16_t sess_trc_depth() const;
   uint32_t len_of_list_of_intfcs() const;
   const uint8_t *list_of_intfcs() const;
   uint16_t len_of_ip_addr_of_trc_coll_ent() const;
   in_addr &ipv4_addr_of_trc_coll_ent() const;
   in6_addr &ipv6_addr_of_trc_coll_ent() const;
   TraceInformationIE &mcc(const char *val, uint8_t len);
   TraceInformationIE &mnc(const char *val, uint8_t len);
   TraceInformationIE &plmnid(const uint8_t *val);
   TraceInformationIE &trace_id(uint32_t val);
   TraceInformationIE &trigrng_evnts(const uint8_t *val);
   TraceInformationIE &list_of_intfcs(const uint8_t *val);
   TraceInformationIE &ip_addr_of_trc_coll_ent(const ESocket::Address &val);
   TraceInformationIE &ip_addr_of_trc_coll_ent(const EIpAddress &val);
   TraceInformationIE &ip_addr_of_trc_coll_ent(const in_addr &val);
   TraceInformationIE &ip_addr_of_trc_coll_ent(const in6_addr &val);
   pfcp_trc_info_ie_t &data();

protected:
   TraceInformationIE(pfcp_trc_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TraceInformationIE();
   pfcp_trc_info_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class FramedRouteIE : public IEHeader
{
   friend class CreateTrafficEndpointIE;
   friend class PdiIE;
   friend class UpdateTrafficEndpointIE;
public:
   uint16_t framed_route_len() const;
   const uint8_t *framed_route() const;
   FramedRouteIE &framed_route(const uint8_t *val, uint16_t len);
   pfcp_framed_route_ie_t &data();

protected:
   FramedRouteIE(pfcp_framed_route_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   FramedRouteIE();
   pfcp_framed_route_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class FramedRoutingEnum : uint32_t
{
   none                       = 0,
   send_routing_packets       = 1,
   listen_for_routing_packets = 2,
   send_and_listen            = 3
};

class FramedRoutingIE : public IEHeader
{
   friend class CreateTrafficEndpointIE;
   friend class PdiIE;
   friend class UpdateTrafficEndpointIE;
public:
   FramedRoutingEnum framed_routing() const;
   FramedRoutingIE &framed_routing(FramedRoutingEnum val);
   pfcp_framed_routing_ie_t &data();

protected:
   FramedRoutingIE(pfcp_framed_routing_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   FramedRoutingIE();
   pfcp_framed_routing_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class FramedIpv6RouteIE : public IEHeader
{
   friend class CreateTrafficEndpointIE;
   friend class PdiIE;
   friend class UpdateTrafficEndpointIE;
public:
   uint16_t frmd_ipv6_rte_len() const;
   const uint8_t *frmd_ipv6_rte() const;
   FramedIpv6RouteIE &frmd_ipv6_rte(const uint8_t *val, uint16_t len);
   pfcp_frmd_ipv6_rte_ie_t &data();

protected:
   FramedIpv6RouteIE(pfcp_frmd_ipv6_rte_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   FramedIpv6RouteIE();
   pfcp_frmd_ipv6_rte_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class EventQuotaIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t sbsqnt_evnt_quota() const ;
   EventQuotaIE &sbsqnt_evnt_quota(uint32_t val);
   pfcp_event_quota_ie_t &data();

protected:
   EventQuotaIE(pfcp_event_quota_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EventQuotaIE();
   pfcp_event_quota_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class EventThresholdIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   uint32_t event_threshold() const;
   EventThresholdIE &event_threshold(uint32_t val);
   pfcp_event_threshold_ie_t &data();

protected:
   EventThresholdIE(pfcp_event_threshold_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EventThresholdIE();
   pfcp_event_threshold_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class EventTimeStampIE : public IEHeader
{
   friend class UsageReportSessionReportReqIE;
public:
   ETime evnt_time_stmp() const;
   EventTimeStampIE &evnt_time_stmp(const ETime &val);
   pfcp_evnt_time_stmp_ie_t &data();

protected:
   EventTimeStampIE(pfcp_evnt_time_stmp_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EventTimeStampIE();
   pfcp_evnt_time_stmp_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class AveragingWindowIE : public IEHeader
{
   friend class CreateQerIE;
   friend class UpdateQerIE;
public:
   uint32_t avgng_wnd() const;
   AveragingWindowIE &avgng_wnd(uint32_t val);
   pfcp_avgng_wnd_ie_t &data();

protected:
   AveragingWindowIE(pfcp_avgng_wnd_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   AveragingWindowIE();
   pfcp_avgng_wnd_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class PagingPolicyIndicatorIE : public IEHeader
{
   friend class CreateQerIE;
   friend class UpdateQerIE;
public:
   uint8_t ppi_value() const;
   PagingPolicyIndicatorIE &ppi_value(uint8_t val);
   pfcp_paging_plcy_indctr_ie_t &data();

protected:
   PagingPolicyIndicatorIE(pfcp_paging_plcy_indctr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PagingPolicyIndicatorIE();
   pfcp_paging_plcy_indctr_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

class ApnDnnIE : public IEHeader
{
   friend class SessionEstablishmentReq;
public:
   uint16_t apn_dnn_len() const;
   const uint8_t *apn_dnn() const;
   ApnDnnIE &apn_dnn(const uint8_t *val, uint16_t len);
   pfcp_apn_dnn_ie_t &data();

protected:
   ApnDnnIE(pfcp_apn_dnn_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ApnDnnIE();
   pfcp_apn_dnn_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

enum class TgppInterfaceTypeEnum : uint8_t
{
   S1U = 0,
   S5S8U = 1,
   S4u = 2,
   S11U = 3,
   S12U = 4,
   GnGpU = 5,
   S2aU = 6,
   S2bU = 7,
   eNodeB_GTPU_interface_for_DL_data_forwarding = 8,
   eNodeB_GTPU_interface_for_UL_data_forwarding = 9,
   SGW_UPF_GTPU_interface_for_DL_data_forwarding = 10,
   N3_3GPP_Access = 11,
   N3_Trusted_Non_3GPP_Access = 12,
   N3_Untrusted_Non_3GPP_Access = 13,
   N3_for_data_forwarding = 14,
   N9 = 15
};

class TgppInterfaceTypeIE : public IEHeader
{
   friend class ForwardingParametersIE;
   friend class PdiIE;
   friend class UpdateForwardingParametersIE;
public:
   TgppInterfaceTypeEnum intfc_type_val() const;
   TgppInterfaceTypeIE &intfc_type_val(TgppInterfaceTypeEnum val);
   pfcp_3gpp_intfc_type_ie_t &data();

protected:
   TgppInterfaceTypeIE(pfcp_3gpp_intfc_type_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   TgppInterfaceTypeIE();
   pfcp_3gpp_intfc_type_ie_t &ie_;
};

////////////////////////////////////////////////////////////////////////////////

/*
class IE : public IEHeader
{
public:
    
   __datatype__ &data() { return ie_; }

protected:
   IE(__datatype__ &ie)
      : IEHeader(ie.header, ),
        ie_(ie)
   {
   }
   uint16_t calculateLength()
   {
      return sizeof(__datatype__) - sizeof(pfcp_ie_header_t);
   }

private:
   IE();
   __datatype__ &ie_;
};
*/

////////////////////////////////////////////////////////////////////////////////
//// Grouped IE Classes
////////////////////////////////////////////////////////////////////////////////

class PfdContextIE : public IEHeader
{
   friend class ApplicationIdsPfdsIE;
public:
   PfdContentsIE &pfdContents(uint8_t idx);
   pfcp_pfd_context_ie_t &data();

protected:
   PfdContextIE(pfcp_pfd_context_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PfdContextIE();
   pfcp_pfd_context_ie_t &ie_;
   vector<PfdContentsIE> pfdcnts_;
};

////////////////////////////////////////////////////////////////////////////////

class ApplicationIdsPfdsIE : public IEHeader
{
   friend class PfdMgmtReq;
public:
   ApplicationIdIE &applicationId();
   PfdContextIE &pfdContext(uint8_t idx);
   int nextPfdContext();
   pfcp_app_ids_pfds_ie_t &data();

protected:
   ApplicationIdsPfdsIE(pfcp_app_ids_pfds_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ApplicationIdsPfdsIE();
   pfcp_app_ids_pfds_ie_t &ie_;
   ApplicationIdIE appid_;
   std::vector<PfdContextIE> pfdctxts_;
};

////////////////////////////////////////////////////////////////////////////////

class CreateTrafficEndpointIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionModificationReq;
public:
   TrafficEndpointIdIE &traffic_endpt_id();
   FTeidIE &local_fteid();
   NetworkInstanceIE &ntwk_inst();
   UeIpAddressIE &ue_ip_address();
   EthernetPduSessionInformationIE &eth_pdu_sess_info();
   FramedRouteIE &framed_route(uint8_t idx);
   FramedRoutingIE &framed_routing();
   FramedIpv6RouteIE &frmd_ipv6_rte(uint8_t idx);
   int next_framed_route();
   int next_frmd_ipv6_rte();
   pfcp_create_traffic_endpt_ie_t &data();

protected:
   CreateTrafficEndpointIE(pfcp_create_traffic_endpt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CreateTrafficEndpointIE();
   pfcp_create_traffic_endpt_ie_t &ie_;
   TrafficEndpointIdIE teid_;
   FTeidIE lfteid_;
   NetworkInstanceIE ni_;
   UeIpAddressIE ueip_;
   EthernetPduSessionInformationIE epsi_;
   std::vector<FramedRouteIE> fr_;
   FramedRoutingIE fring_;
   std::vector<FramedIpv6RouteIE> fr6_;
};

////////////////////////////////////////////////////////////////////////////////

class CreateBarIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionModificationReq;
public:
   BarIdIE &bar_id();
   DownlinkDataNotificationDelayIE &dnlnk_data_notif_delay();
   SuggestedBufferingPacketsCountIE &suggstd_buf_pckts_cnt();
   pfcp_create_bar_ie_t &data();

protected:
   CreateBarIE(pfcp_create_bar_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CreateBarIE();
   pfcp_create_bar_ie_t &ie_;
   BarIdIE barid_;
   DownlinkDataNotificationDelayIE ddndelay_;
   SuggestedBufferingPacketsCountIE sbpc_;
};

////////////////////////////////////////////////////////////////////////////////

class CreateQerIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionModificationReq;
public:
   QerIdIE &qer_id();
   QerCorrelationIdIE &qer_corr_id();
   GateStatusIE &gate_status();
   MbrIE &maximum_bitrate();
   GbrIE &guaranteed_bitrate();
   PacketRateIE &packet_rate();
   DlFlowLevelMarkingIE &dl_flow_lvl_marking();
   QfiIE &qos_flow_ident();
   RqiIE &reflective_qos();
   PagingPolicyIndicatorIE &paging_plcy_indctr();
   AveragingWindowIE &avgng_wnd();
   pfcp_create_qer_ie_t &data();

protected:
   CreateQerIE(pfcp_create_qer_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CreateQerIE();
   pfcp_create_qer_ie_t &ie_;
   QerIdIE qerid_;
   QerCorrelationIdIE qci_;
   GateStatusIE gs_;
   MbrIE mbr_;
   GbrIE gbr_;
   PacketRateIE pr_;
   DlFlowLevelMarkingIE dfm_;
   QfiIE qfi_;
   RqiIE rqi_;
   PagingPolicyIndicatorIE ppi_;
   AveragingWindowIE aw_;
};

////////////////////////////////////////////////////////////////////////////////

class AdditionalMonitoringTimeIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   MonitoringTimeIE &monitoring_time();
   SubsequentVolumeThresholdIE &sbsqnt_vol_thresh();
   SubsequentTimeThresholdIE &sbsqnt_time_thresh();
   SubsequentVolumeQuotaIE &sbsqnt_vol_quota();
   SubsequentTimeQuotaIE &sbsqnt_time_quota();
   SubsequentEventThresholdIE &sbsqnt_evnt_thresh();
   SubsequentEventQuotaIE &sbsqnt_evnt_quota();
   pfcp_add_mntrng_time_ie_t &data();

protected:
   AdditionalMonitoringTimeIE(pfcp_add_mntrng_time_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   AdditionalMonitoringTimeIE();
   pfcp_add_mntrng_time_ie_t &ie_;
   MonitoringTimeIE mt_;
   SubsequentVolumeThresholdIE svt_;
   SubsequentTimeThresholdIE stt_;
   SubsequentVolumeQuotaIE svq_;
   SubsequentTimeQuotaIE stq_;
   SubsequentEventThresholdIE set_;
   SubsequentEventQuotaIE seq_;
};

////////////////////////////////////////////////////////////////////////////////

class AggregatedUrrsIE : public IEHeader
{
   friend class CreateUrrIE;
   friend class UpdateUrrIE;
public:
   AggregatedUrrIdIE &agg_urr_id();
   MultiplierIE &multiplier();
   pfcp_aggregated_urrs_ie_t &data();

protected:
   AggregatedUrrsIE(pfcp_aggregated_urrs_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   AggregatedUrrsIE();
   pfcp_aggregated_urrs_ie_t &ie_;
   AggregatedUrrIdIE aui_;
   MultiplierIE m_;
};

////////////////////////////////////////////////////////////////////////////////

class CreateUrrIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionModificationReq;
public:
   UrrIdIE &urr_id();
   MeasurementMethodIE &meas_mthd();
   ReportingTriggersIE &rptng_triggers();
   MeasurementPeriodIE &meas_period();
   VolumeThresholdIE &vol_thresh();
   VolumeQuotaIE &volume_quota();
   EventThresholdIE &event_threshold();
   EventQuotaIE &event_quota();
   TimeThresholdIE &time_threshold();
   TimeQuotaIE &time_quota();
   QuotaHoldingTimeIE &quota_hldng_time();
   DroppedDlTrafficThresholdIE &drpd_dl_traffic_thresh();
   MonitoringTimeIE &monitoring_time();
   SubsequentVolumeThresholdIE &sbsqnt_vol_thresh();
   SubsequentTimeThresholdIE &sbsqnt_time_thresh();
   SubsequentVolumeQuotaIE &sbsqnt_vol_quota();
   SubsequentTimeQuotaIE &sbsqnt_time_quota();
   SubsequentEventThresholdIE &sbsqnt_evnt_thresh();
   SubsequentEventQuotaIE &sbsqnt_evnt_quota();
   InactivityDetectionTimeIE &inact_det_time();
   LinkedUrrIdIE &linked_urr_id(uint8_t idx);
   MeasurementInformationIE &meas_info();
   TimeQuotaMechanismIE &time_quota_mech();
   AggregatedUrrsIE &aggregated_urrs(uint8_t idx);
   FarIdIE &far_id_for_quota_act();
   EthernetInactivityTimerIE &eth_inact_timer();
   AdditionalMonitoringTimeIE &add_mntrng_time(uint8_t idx);
   int next_linked_urr_id();
   int next_aggregated_urrs();
   int next_add_mntrng_time();
   pfcp_create_urr_ie_t &data();

protected:
   CreateUrrIE(pfcp_create_urr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CreateUrrIE();
   pfcp_create_urr_ie_t &ie_;
   UrrIdIE ui_;
   MeasurementMethodIE mm_;
   ReportingTriggersIE rt_;
   MeasurementPeriodIE mp_;
   VolumeThresholdIE vt_;
   VolumeQuotaIE vq_;
   EventThresholdIE et_;
   EventQuotaIE eq_;
   TimeThresholdIE tt_;
   TimeQuotaIE tq_;
   QuotaHoldingTimeIE qht_;
   DroppedDlTrafficThresholdIE ddtt_;
   MonitoringTimeIE mt_;
   SubsequentVolumeThresholdIE svt_;
   SubsequentTimeThresholdIE stt_;
   SubsequentVolumeQuotaIE svq_;
   SubsequentTimeQuotaIE stq_;
   SubsequentEventThresholdIE set_;
   SubsequentEventQuotaIE seq_;
   InactivityDetectionTimeIE idt_;
   std::vector<LinkedUrrIdIE> lui_;
   MeasurementInformationIE mi_;
   TimeQuotaMechanismIE tqm_;
   std::vector<AggregatedUrrsIE> aus_;
   FarIdIE fiqa_;
   EthernetInactivityTimerIE eit_;
   std::vector<AdditionalMonitoringTimeIE> amt_;
};

////////////////////////////////////////////////////////////////////////////////

class DuplicatingParametersIE : public IEHeader
{
   friend class CreateFarIE;
public:
   DestinationInterfaceIE &dst_intfc();
   OuterHeaderCreationIE &outer_hdr_creation();
   TransportLevelMarkingIE &trnspt_lvl_marking();
   ForwardingPolicyIE &frwdng_plcy();    
   pfcp_dupng_parms_ie_t &data();

protected:
   DuplicatingParametersIE(pfcp_dupng_parms_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DuplicatingParametersIE();
   pfcp_dupng_parms_ie_t &ie_;
   DestinationInterfaceIE di_;
   OuterHeaderCreationIE ohc_;
   TransportLevelMarkingIE tlm_;
   ForwardingPolicyIE fp_;
};

////////////////////////////////////////////////////////////////////////////////

class ForwardingParametersIE : public IEHeader
{
   friend class CreateFarIE;
public:
   DestinationInterfaceIE &dst_intfc();
   NetworkInstanceIE &ntwk_inst();
   RedirectInformationIE &redir_info();
   OuterHeaderCreationIE &outer_hdr_creation();
   TransportLevelMarkingIE &trnspt_lvl_marking();
   ForwardingPolicyIE &frwdng_plcy();
   HeaderEnrichmentIE &hdr_enrchmt();
   TrafficEndpointIdIE &lnkd_traffic_endpt_id();
   ProxyingIE &proxying();
   TgppInterfaceTypeIE &dst_intfc_type();
   pfcp_frwdng_parms_ie_t &data();

protected:
   ForwardingParametersIE(pfcp_frwdng_parms_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ForwardingParametersIE();
   pfcp_frwdng_parms_ie_t &ie_;
   DestinationInterfaceIE di_;
   NetworkInstanceIE ni_;
   RedirectInformationIE ri_;
   OuterHeaderCreationIE ohc_;
   TransportLevelMarkingIE tlm_;
   ForwardingPolicyIE fp_;
   HeaderEnrichmentIE he_;
   TrafficEndpointIdIE ltei_;
   ProxyingIE p_;
   TgppInterfaceTypeIE dit_;
};

////////////////////////////////////////////////////////////////////////////////

class CreateFarIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionModificationReq;
public:
   FarIdIE &far_id();
   ApplyActionIE &apply_action();
   ForwardingParametersIE &frwdng_parms();
   DuplicatingParametersIE &dupng_parms(uint8_t idx);
   BarIdIE &bar_id();
   int next_dupng_parms();
   pfcp_create_far_ie_t &data();

protected:
   CreateFarIE(pfcp_create_far_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CreateFarIE();
   pfcp_create_far_ie_t &ie_;
   FarIdIE fi_;
   ApplyActionIE aa_;
   ForwardingParametersIE fp_;
   std::vector<DuplicatingParametersIE> dp_;
   BarIdIE bi_;
};

////////////////////////////////////////////////////////////////////////////////

class EthernetPacketFilterIE : public IEHeader
{
   friend class PdiIE;
public:
   EthernetFilterIdIE &eth_fltr_id();
   EthernetFilterPropertiesIE &eth_fltr_props();
   MacAddressIE &mac_address();
   EthertypeIE &ethertype();
   CTagIE &ctag();
   STagIE &stag();
   SdfFilterIE &sdf_filter(uint8_t idx);
   int next_sdf_filter();
   pfcp_eth_pckt_fltr_ie_t &data();

protected:
   EthernetPacketFilterIE(pfcp_eth_pckt_fltr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EthernetPacketFilterIE();
   pfcp_eth_pckt_fltr_ie_t &ie_;
   EthernetFilterIdIE efi_;
   EthernetFilterPropertiesIE efp_;
   MacAddressIE ma_;
   EthertypeIE e_;
   CTagIE ctag_;
   STagIE stag_;
   std::vector<SdfFilterIE> sf_;
};

////////////////////////////////////////////////////////////////////////////////

class PdiIE : public IEHeader
{
   friend class CreatePdrIE;
   friend class UpdatePdrIE;
public:
   SourceInterfaceIE &src_intfc();
   FTeidIE &local_fteid();
   NetworkInstanceIE &ntwk_inst();
   UeIpAddressIE &ue_ip_address();
   TrafficEndpointIdIE &traffic_endpt_id();
   ApplicationIdIE &application_id();
   EthernetPduSessionInformationIE &eth_pdu_sess_info();
   FramedRoutingIE &framed_routing();
   TgppInterfaceTypeIE &src_intrc_type();
   SdfFilterIE &sdf_filter(uint8_t idx);
   EthernetPacketFilterIE &eth_pckt_fltr(uint8_t idx);
   QfiIE &qfi(uint8_t idx);
   FramedRouteIE &framed_route(uint8_t idx);
   FramedIpv6RouteIE &frmd_ipv6_rte(uint8_t idx);
   int next_sdf_filter();
   int next_eth_pckt_fltr();
   int next_qfi();
   int next_framed_route();
   int next_frmd_ipv6_rte();
   pfcp_pdi_ie_t &data();

protected:
   PdiIE(pfcp_pdi_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   PdiIE();
   pfcp_pdi_ie_t &ie_;
   SourceInterfaceIE si_;
   FTeidIE lft_;
   NetworkInstanceIE ni_;
   UeIpAddressIE uia_;
   TrafficEndpointIdIE tei_;
   std::vector<SdfFilterIE> sf_;
   ApplicationIdIE ai_;
   EthernetPduSessionInformationIE epsi_;
   std::vector<EthernetPacketFilterIE> epf_;
   std::vector<QfiIE> qfi_;
   std::vector<FramedRouteIE> fr_;
   FramedRoutingIE fring_;
   std::vector<FramedIpv6RouteIE> fr6_;
   TgppInterfaceTypeIE sit_;
};

////////////////////////////////////////////////////////////////////////////////
class CreatePdrIE : public IEHeader
{
   friend class SessionEstablishmentReq;
   friend class SessionModificationReq;
public:
   PdrIdIE &pdr_id();
   PrecedenceIE &precedence();
   PdiIE &pdi();
   OuterHeaderRemovalIE &outer_hdr_removal();
   FarIdIE &far_id();
   UrrIdIE &urr_id(uint8_t idx);
   QerIdIE &qer_id(uint8_t idx);
   ActivatePredefinedRulesIE &actvt_predef_rules(uint8_t idx);
   int next_urr_id();
   int next_qer_id();
   int next_actvt_predef_rules();
   pfcp_create_pdr_ie_t &data();

protected:
   CreatePdrIE(pfcp_create_pdr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CreatePdrIE();
   pfcp_create_pdr_ie_t &ie_;
   PdrIdIE pi_;
   PrecedenceIE p_;
   PdiIE pdi_;
   OuterHeaderRemovalIE ohr_;
   FarIdIE fi_;
   std::vector<UrrIdIE> ui_;
   std::vector<QerIdIE> qi_;
   std::vector<ActivatePredefinedRulesIE> apr_;
};

////////////////////////////////////////////////////////////////////////////////

class CreatedTrafficEndpointIE : public IEHeader
{
   friend class SessionEstablishmentRsp;
   friend class SessionModificationReq;
   friend class SessionModificationRsp;
public:
   TrafficEndpointIdIE &traffic_endpt_id();
   FTeidIE &local_fteid();
   pfcp_created_traffic_endpt_ie_t &data();

protected:
   CreatedTrafficEndpointIE(pfcp_created_traffic_endpt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CreatedTrafficEndpointIE();
   pfcp_created_traffic_endpt_ie_t &ie_;
   TrafficEndpointIdIE tei_;
   FTeidIE lft_;
};

////////////////////////////////////////////////////////////////////////////////

class OverloadControlInformationIE : public IEHeader
{
   friend class SessionEstablishmentRsp;
   friend class SessionModificationRsp;
   friend class SessionDeletionRsp;
   friend class SessionReportReq;
public:
   SequenceNumberIE &ovrld_ctl_seqn_nbr();
   MetricIE &ovrld_reduction_metric();
   TimerIE &period_of_validity();
   OciFlagsIE &ovrld_ctl_info_flgs();
   pfcp_ovrld_ctl_info_ie_t &data();

protected:
   OverloadControlInformationIE(pfcp_ovrld_ctl_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   OverloadControlInformationIE();
   pfcp_ovrld_ctl_info_ie_t &ie_;
   SequenceNumberIE ocsn_;
   MetricIE orm_;
   TimerIE pov_;
   OciFlagsIE ocif_;
};

////////////////////////////////////////////////////////////////////////////////

class LoadControlInformationIE : public IEHeader
{
   friend class SessionEstablishmentRsp;
   friend class SessionModificationRsp;
   friend class SessionDeletionRsp;
   friend class SessionReportReq;
public:
   SequenceNumberIE &load_ctl_seqn_nbr();
   MetricIE &load_metric();
   pfcp_load_ctl_info_ie_t &data();

protected:
   LoadControlInformationIE(pfcp_load_ctl_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   LoadControlInformationIE();
   pfcp_load_ctl_info_ie_t &ie_;
   SequenceNumberIE lcsn_;
   MetricIE lm_;
};

////////////////////////////////////////////////////////////////////////////////

class CreatedPdrIE : public IEHeader
{
   friend class SessionEstablishmentRsp;
   friend class SessionModificationRsp;
public:
   PdrIdIE &pdr_id();
   FTeidIE &local_fteid();
   pfcp_created_pdr_ie_t &data();

protected:
   CreatedPdrIE(pfcp_created_pdr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   CreatedPdrIE();
   pfcp_created_pdr_ie_t &ie_;
   PdrIdIE pi_;
   FTeidIE lft_;
};

////////////////////////////////////////////////////////////////////////////////

class UserPlanePathFailureReportIE : public IEHeader
{
   friend class NodeReportReq;
public:
   RemoteGTPUPeerIE &rmt_gtpu_peer(uint8_t idx);
   int next_rmt_gtpu_peer();
   pfcp_user_plane_path_fail_rpt_ie_t &data();

protected:
   UserPlanePathFailureReportIE(pfcp_user_plane_path_fail_rpt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UserPlanePathFailureReportIE();
   pfcp_user_plane_path_fail_rpt_ie_t &ie_;
   std::vector<RemoteGTPUPeerIE> rgp_;
};

////////////////////////////////////////////////////////////////////////////////

class RemoveTrafficEndpointIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   TrafficEndpointIdIE &traffic_endpt_id();
   pfcp_rmv_traffic_endpt_ie_t &data();

protected:
   RemoveTrafficEndpointIE(pfcp_rmv_traffic_endpt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RemoveTrafficEndpointIE();
   pfcp_rmv_traffic_endpt_ie_t &ie_;
   TrafficEndpointIdIE tei_;
};

////////////////////////////////////////////////////////////////////////////////

class UpdateTrafficEndpointIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   TrafficEndpointIdIE &traffic_endpt_id();
   FTeidIE &local_fteid();
   NetworkInstanceIE &ntwk_inst();
   UeIpAddressIE &ue_ip_address();
   FramedRoutingIE &framed_routing();
   FramedRouteIE &framed_route(uint8_t idx);
   FramedIpv6RouteIE &frmd_ipv6_rte(uint8_t idx);
   int next_framed_route();
   int next_frmd_ipv6_rte();
   pfcp_upd_traffic_endpt_ie_t &data();

protected:
   UpdateTrafficEndpointIE(pfcp_upd_traffic_endpt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpdateTrafficEndpointIE();
   pfcp_upd_traffic_endpt_ie_t &ie_;
   TrafficEndpointIdIE tei_;
   FTeidIE lft_;
   NetworkInstanceIE ni_;
   UeIpAddressIE uia_;
   std::vector<FramedRouteIE> fr_;
   FramedRoutingIE fring_;
   std::vector<FramedIpv6RouteIE> fr6_;
};

////////////////////////////////////////////////////////////////////////////////

class RemoveBarIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   BarIdIE &bar_id();
   pfcp_remove_bar_ie_t &data();

protected:
   RemoveBarIE(pfcp_remove_bar_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RemoveBarIE();
   pfcp_remove_bar_ie_t &ie_;
   BarIdIE bi_;
};

////////////////////////////////////////////////////////////////////////////////

class UpdateBarSessionModificationReqIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   BarIdIE &bar_id();
   DownlinkDataNotificationDelayIE &dnlnk_data_notif_delay();
   SuggestedBufferingPacketsCountIE &suggstd_buf_pckts_cnt();    
   pfcp_upd_bar_sess_mod_req_ie_t &data();

protected:
   UpdateBarSessionModificationReqIE(pfcp_upd_bar_sess_mod_req_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpdateBarSessionModificationReqIE();
   pfcp_upd_bar_sess_mod_req_ie_t &ie_;
   BarIdIE bi_;
   DownlinkDataNotificationDelayIE ddnd_;
   SuggestedBufferingPacketsCountIE sbpc_;
};

////////////////////////////////////////////////////////////////////////////////

class QueryUrrIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   UrrIdIE &urr_id();    
   pfcp_query_urr_ie_t &data();

protected:
   QueryUrrIE(pfcp_query_urr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   QueryUrrIE();
   pfcp_query_urr_ie_t &ie_;
   UrrIdIE ui_;
};

////////////////////////////////////////////////////////////////////////////////

class RemoveQerIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   QerIdIE qer_id();
   pfcp_remove_qer_ie_t &data();

protected:
   RemoveQerIE(pfcp_remove_qer_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RemoveQerIE();
   pfcp_remove_qer_ie_t &ie_;
   QerIdIE qi_;
};

////////////////////////////////////////////////////////////////////////////////

class RemoveUrrIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   UrrIdIE &urr_id();
   pfcp_remove_urr_ie_t &data();

protected:
   RemoveUrrIE(pfcp_remove_urr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RemoveUrrIE();
   pfcp_remove_urr_ie_t &ie_;
   UrrIdIE ui_;
};

////////////////////////////////////////////////////////////////////////////////

class RemoveFarIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   FarIdIE &far_id();
   pfcp_remove_far_ie_t &data();

protected:
   RemoveFarIE(pfcp_remove_far_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RemoveFarIE();
   pfcp_remove_far_ie_t &ie_;
   FarIdIE fi_;
};

////////////////////////////////////////////////////////////////////////////////

class RemovePdrIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   PdrIdIE &pdr_id();
   pfcp_remove_pdr_ie_t &data();

protected:
   RemovePdrIE(pfcp_remove_pdr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   RemovePdrIE();
   pfcp_remove_pdr_ie_t &ie_;
   PdrIdIE pi_;
};

////////////////////////////////////////////////////////////////////////////////

class UpdateQerIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   QerIdIE &qer_id();
   QerCorrelationIdIE &qer_corr_id();
   GateStatusIE &gate_status();
   MbrIE &maximum_bitrate();
   GbrIE &guaranteed_bitrate();
   PacketRateIE &packet_rate();
   DlFlowLevelMarkingIE &dl_flow_lvl_marking();
   QfiIE &qos_flow_ident();
   RqiIE &reflective_qos();
   PagingPolicyIndicatorIE &paging_plcy_indctr();
   AveragingWindowIE &avgng_wnd();
   pfcp_update_qer_ie_t &data();

protected:
   UpdateQerIE(pfcp_update_qer_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpdateQerIE();
   pfcp_update_qer_ie_t &ie_;
   QerIdIE qi_;
   QerCorrelationIdIE qci_;
   GateStatusIE gs_;
   MbrIE mb_;
   GbrIE gb_;
   PacketRateIE pr;
   DlFlowLevelMarkingIE dflm_;
   QfiIE qfi_;
   RqiIE rqi_;
   PagingPolicyIndicatorIE ppi_;
   AveragingWindowIE aw_;
};

////////////////////////////////////////////////////////////////////////////////

class UpdateUrrIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   UrrIdIE &urr_id();
   MeasurementMethodIE &meas_mthd();
   ReportingTriggersIE &rptng_triggers();
   MeasurementPeriodIE &meas_period();
   VolumeThresholdIE &vol_thresh();
   VolumeQuotaIE &volume_quota();
   TimeThresholdIE &time_threshold();
   TimeQuotaIE &time_quota();
   EventThresholdIE &event_threshold();
   EventQuotaIE &event_quota();
   QuotaHoldingTimeIE &quota_hldng_time();
   DroppedDlTrafficThresholdIE &drpd_dl_traffic_thresh();
   MonitoringTimeIE &monitoring_time();
   SubsequentVolumeThresholdIE &sbsqnt_vol_thresh();
   SubsequentTimeThresholdIE &sbsqnt_time_thresh();
   SubsequentVolumeQuotaIE &sbsqnt_vol_quota();
   SubsequentTimeQuotaIE &sbsqnt_time_quota();
   SubsequentEventThresholdIE &sbsqnt_evnt_thresh();
   SubsequentEventQuotaIE &sbsqnt_evnt_quota();
   InactivityDetectionTimeIE &inact_det_time();
   MeasurementInformationIE &meas_info();
   TimeQuotaMechanismIE &time_quota_mech();
   FarIdIE &far_id_for_quota_act();
   EthernetInactivityTimerIE &eth_inact_timer();
   AdditionalMonitoringTimeIE &add_mntrng_time();
   LinkedUrrIdIE &linked_urr_id(uint8_t idx);
   AggregatedUrrsIE &aggregated_urrs(uint8_t idx);
   int next_linked_urr_id();
   int next_aggregated_urrs();
   pfcp_update_urr_ie_t &data();

protected:
   UpdateUrrIE(pfcp_update_urr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpdateUrrIE();
   pfcp_update_urr_ie_t &ie_;
   UrrIdIE ui_;
   MeasurementMethodIE mm_;
   ReportingTriggersIE rt_;
   MeasurementPeriodIE mp_;
   VolumeThresholdIE vt_;
   VolumeQuotaIE vq_;
   TimeThresholdIE tt_;
   TimeQuotaIE tq_;
   EventThresholdIE et_;
   EventQuotaIE eq_;
   QuotaHoldingTimeIE qht_;
   DroppedDlTrafficThresholdIE ddtt_;
   MonitoringTimeIE mt_;
   SubsequentVolumeThresholdIE svt_;
   SubsequentTimeThresholdIE stt_;
   SubsequentVolumeQuotaIE svq_;
   SubsequentTimeQuotaIE stq_;
   SubsequentEventThresholdIE set_;
   SubsequentEventQuotaIE seq_;
   InactivityDetectionTimeIE idt_;
   std::vector<LinkedUrrIdIE> lui_;
   MeasurementInformationIE mi_;
   TimeQuotaMechanismIE tqm_;
   std::vector<AggregatedUrrsIE> au_;
   FarIdIE fifqa_;
   EthernetInactivityTimerIE eit_;
   AdditionalMonitoringTimeIE amt_;
};

////////////////////////////////////////////////////////////////////////////////

class UpdateDuplicatingParametersIE : public IEHeader
{
   friend class UpdateFarIE;
public:
   DestinationInterfaceIE &dst_intfc();
   OuterHeaderCreationIE &outer_hdr_creation();
   TransportLevelMarkingIE &trnspt_lvl_marking();
   ForwardingPolicyIE &frwdng_plcy();
   pfcp_upd_dupng_parms_ie_t &data();

protected:
   UpdateDuplicatingParametersIE(pfcp_upd_dupng_parms_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpdateDuplicatingParametersIE();
   pfcp_upd_dupng_parms_ie_t &ie_;
   DestinationInterfaceIE di_;
   OuterHeaderCreationIE ohc_;
   TransportLevelMarkingIE tlm_;
   ForwardingPolicyIE fp_;
};

////////////////////////////////////////////////////////////////////////////////

class UpdateForwardingParametersIE : public IEHeader
{
   friend class UpdateFarIE;
public:
   DestinationInterfaceIE &dst_intfc();
   NetworkInstanceIE &ntwk_inst();
   RedirectInformationIE &redir_info();
   OuterHeaderCreationIE &outer_hdr_creation();
   TransportLevelMarkingIE &trnspt_lvl_marking();
   ForwardingPolicyIE &frwdng_plcy();
   HeaderEnrichmentIE &hdr_enrchmt();
   PfcpSmReqFlagsIE &pfcpsmreq_flags();
   TrafficEndpointIdIE &lnkd_traffic_endpt_id();
   TgppInterfaceTypeIE &dst_intfc_type();
   pfcp_upd_frwdng_parms_ie_t &data();

protected:
   UpdateForwardingParametersIE(pfcp_upd_frwdng_parms_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpdateForwardingParametersIE();
   pfcp_upd_frwdng_parms_ie_t &ie_;
   DestinationInterfaceIE di_;
   NetworkInstanceIE ni_;
   RedirectInformationIE ri_;
   OuterHeaderCreationIE ohc_;
   TransportLevelMarkingIE tlm_;
   ForwardingPolicyIE fp_;
   HeaderEnrichmentIE he_;
   PfcpSmReqFlagsIE f_;
   TrafficEndpointIdIE ltei_;
   TgppInterfaceTypeIE dit_;
};

////////////////////////////////////////////////////////////////////////////////

class UpdateFarIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   FarIdIE &far_id();
   ApplyActionIE &apply_action();
   UpdateForwardingParametersIE &upd_frwdng_parms();
   BarIdIE &bar_id();
   UpdateDuplicatingParametersIE &upd_dupng_parms(uint8_t idx);
   int next_upd_dupng_parms();
   pfcp_update_far_ie_t &data();

protected:
   UpdateFarIE(pfcp_update_far_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpdateFarIE();
   pfcp_update_far_ie_t &ie_;
   FarIdIE fi_;
   ApplyActionIE aa_;
   UpdateForwardingParametersIE ufp_;
   std::vector<UpdateDuplicatingParametersIE> udp_;
   BarIdIE bi_;
};

////////////////////////////////////////////////////////////////////////////////

class UpdatePdrIE : public IEHeader
{
   friend class SessionModificationReq;
public:
   PdrIdIE &pdr_id();
   OuterHeaderRemovalIE &outer_hdr_removal();
   PrecedenceIE &precedence();
   PdiIE &pdi();
   FarIdIE &far_id();
   UrrIdIE &urr_id();
   QerIdIE &qer_id();
   ActivatePredefinedRulesIE &actvt_predef_rules(uint8_t idx);
   DeactivatePredefinedRulesIE &deact_predef_rules(uint8_t idx);
   int next_actvt_predef_rules();
   int next_deact_predef_rules();
   pfcp_update_pdr_ie_t &data();

protected:
   UpdatePdrIE(pfcp_update_pdr_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpdatePdrIE();
   pfcp_update_pdr_ie_t &ie_;
   PdrIdIE pi_;
   OuterHeaderRemovalIE ohr_;
   PrecedenceIE p_;
   PdiIE pdi_;
   FarIdIE fi_;
   UrrIdIE ui_;
   QerIdIE qi_;
   std::vector<ActivatePredefinedRulesIE> apr_;
   std::vector<DeactivatePredefinedRulesIE> dpr_;
};

////////////////////////////////////////////////////////////////////////////////

class EthernetTrafficInformationIE : public IEHeader
{
   friend class UsageReportSessionModificationRspIE;
   friend class UsageReportSessionDeletionRspIE;
   friend class UsageReportSessionReportReqIE;
public:
   MacAddressesDetectedIE &mac_addrs_detctd();
   MacAddressesRemovedIE &mac_addrs_rmvd();
   pfcp_eth_traffic_info_ie_t &data();

protected:
   EthernetTrafficInformationIE(pfcp_eth_traffic_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   EthernetTrafficInformationIE();
   pfcp_eth_traffic_info_ie_t &ie_;
   MacAddressesDetectedIE mad_;
   MacAddressesRemovedIE mar_;
};

////////////////////////////////////////////////////////////////////////////////

class UsageReportSessionModificationRspIE : public IEHeader
{
   friend class SessionModificationRsp;
public:
   UrrIdIE &urr_id();
   UrSeqnIE &urseqn();
   UsageReportTriggerIE &usage_rpt_trig();
   StartTimeIE &start_time();
   EndTimeIE &end_time();
   VolumeMeasurementIE &vol_meas();
   DurationMeasurementIE &dur_meas();
   TimeOfFirstPacketIE &time_of_frst_pckt();
   TimeOfLastPacketIE &time_of_lst_pckt();
   UsageInformationIE &usage_info();
   QueryUrrReferenceIE &query_urr_ref();
   EthernetTrafficInformationIE &eth_traffic_info();
   pfcp_usage_rpt_sess_mod_rsp_ie_t &data();

protected:
   UsageReportSessionModificationRspIE(pfcp_usage_rpt_sess_mod_rsp_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UsageReportSessionModificationRspIE();
   pfcp_usage_rpt_sess_mod_rsp_ie_t &ie_;
   UrrIdIE ui_;
   UrSeqnIE us_;
   UsageReportTriggerIE urt_;
   StartTimeIE st_;
   EndTimeIE et_;
   VolumeMeasurementIE vm_;
   DurationMeasurementIE dm_;
   TimeOfFirstPacketIE tofp_;
   TimeOfLastPacketIE tolp_;
   UsageInformationIE uinfo_;
   QueryUrrReferenceIE qur_;
   EthernetTrafficInformationIE eti_;
};

////////////////////////////////////////////////////////////////////////////////

class UsageReportSessionDeletionRspIE : public IEHeader
{
   friend class SessionDeletionRsp;
public:
   UrrIdIE &urr_id();
   UrSeqnIE &urseqn();
   UsageReportTriggerIE &usage_rpt_trig();
   StartTimeIE &start_time();
   EndTimeIE &end_time();
   VolumeMeasurementIE &vol_meas();
   DurationMeasurementIE &dur_meas();
   TimeOfFirstPacketIE &time_of_frst_pckt();
   TimeOfLastPacketIE &time_of_lst_pckt();
   UsageInformationIE &usage_info();
   EthernetTrafficInformationIE &eth_traffic_info();
   pfcp_usage_rpt_sess_del_rsp_ie_t &data();

protected:
   UsageReportSessionDeletionRspIE(pfcp_usage_rpt_sess_del_rsp_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UsageReportSessionDeletionRspIE();
   pfcp_usage_rpt_sess_del_rsp_ie_t &ie_;
   UrrIdIE ui_;
   UrSeqnIE us_;
   UsageReportTriggerIE urt_;
   StartTimeIE st_;
   EndTimeIE et_;
   VolumeMeasurementIE vm_;
   DurationMeasurementIE dm_;
   TimeOfFirstPacketIE tofp_;
   TimeOfLastPacketIE tolp_;
   UsageInformationIE uinfo_;
   EthernetTrafficInformationIE eti_;
};

////////////////////////////////////////////////////////////////////////////////

class ErrorIndicationReportIE : public IEHeader
{
   friend class SessionReportReq;
public:
   FTeidIE &remote_fteid(uint8_t idx);
   int next_remote_fteid();
   pfcp_err_indctn_rpt_ie_t &data();

protected:
   ErrorIndicationReportIE(pfcp_err_indctn_rpt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ErrorIndicationReportIE();
   pfcp_err_indctn_rpt_ie_t &ie_;
   std::vector<FTeidIE> rft_;
};

////////////////////////////////////////////////////////////////////////////////

class ApplicationDetectionInformationIE : public IEHeader
{
   friend class UsageReportSessionReportReqIE;
public:
   ApplicationIdIE &application_id();
   ApplicationInstanceIdIE &app_inst_id();
   FlowInformationIE &flow_info();
   pfcp_app_det_info_ie_t &data();

protected:
   ApplicationDetectionInformationIE(pfcp_app_det_info_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   ApplicationDetectionInformationIE();
   pfcp_app_det_info_ie_t &ie_;
   ApplicationIdIE ai_;
   ApplicationInstanceIdIE aii_;
   FlowInformationIE fi_;
};

////////////////////////////////////////////////////////////////////////////////

class UsageReportSessionReportReqIE : public IEHeader
{
   friend class SessionReportReq;
public:
   UrrIdIE &urr_id();
   UrSeqnIE &urseqn();
   UsageReportTriggerIE &usage_rpt_trig();
   StartTimeIE &start_time();
   EndTimeIE &end_time();
   VolumeMeasurementIE &vol_meas();
   DurationMeasurementIE &dur_meas();
   ApplicationDetectionInformationIE &app_det_info();
   UeIpAddressIE &ue_ip_address();
   NetworkInstanceIE &ntwk_inst();
   TimeOfFirstPacketIE &time_of_frst_pckt();
   TimeOfLastPacketIE &time_of_lst_pckt();
   UsageInformationIE &usage_info();
   QueryUrrReferenceIE &query_urr_ref();
   EthernetTrafficInformationIE &eth_traffic_info();
   EventTimeStampIE &evnt_time_stmp(uint8_t idx);
   int next_evnt_time_stmp();
   pfcp_usage_rpt_sess_rpt_req_ie_t &data();

protected:
   UsageReportSessionReportReqIE(pfcp_usage_rpt_sess_rpt_req_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UsageReportSessionReportReqIE();
   pfcp_usage_rpt_sess_rpt_req_ie_t &ie_;
   UrrIdIE ui_;
   UrSeqnIE us_;
   UsageReportTriggerIE urt_;
   StartTimeIE st_;
   EndTimeIE et_;
   VolumeMeasurementIE vm_;
   DurationMeasurementIE dm_;
   ApplicationDetectionInformationIE adi_;
   UeIpAddressIE uia_;
   NetworkInstanceIE ni_;
   TimeOfFirstPacketIE tofp_;
   TimeOfLastPacketIE tolp_;
   UsageInformationIE uinfo_;
   QueryUrrReferenceIE qur_;
   std::vector<EventTimeStampIE> ets_;
   EthernetTrafficInformationIE eti_;
};

////////////////////////////////////////////////////////////////////////////////

class DownlinkDataReportIE : public IEHeader
{
   friend class SessionReportReq;
public:
   PdrIdIE &pdr_id(uint8_t idx);
   DownlinkDataServiceInformationIE &dnlnk_data_svc_info(uint8_t idx);
   int next_pdr_id();
   int next_dnlnk_data_svc_info();
   pfcp_dnlnk_data_rpt_ie_t &data();

protected:
   DownlinkDataReportIE(pfcp_dnlnk_data_rpt_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   DownlinkDataReportIE();
   pfcp_dnlnk_data_rpt_ie_t &ie_;
   std::vector<PdrIdIE> pi_;
   std::vector<DownlinkDataServiceInformationIE> ddsi_;
};

////////////////////////////////////////////////////////////////////////////////

class UpdateBarSessionReportRspIE : public IEHeader
{
   friend class SessionReportRsp;
public:
   BarIdIE &bar_id();
   DownlinkDataNotificationDelayIE &dnlnk_data_notif_delay();
   DlBufferingDurationIE &dl_buf_dur();
   DlBufferingSuggestedPacketCountIE &dl_buf_suggstd_pckt_cnt();
   SuggestedBufferingPacketsCountIE &suggstd_buf_pckts_cnt();
   pfcp_upd_bar_sess_rpt_rsp_ie_t &data();

protected:
   UpdateBarSessionReportRspIE(pfcp_upd_bar_sess_rpt_rsp_ie_t &ie, LengthCalculator *parent);
   uint16_t calculateLength();

private:
   UpdateBarSessionReportRspIE();
   pfcp_upd_bar_sess_rpt_rsp_ie_t &ie_;
   BarIdIE bi_;
   DownlinkDataNotificationDelayIE ddnd_;
   DlBufferingDurationIE dbd_;
   DlBufferingSuggestedPacketCountIE dbspc_;
   SuggestedBufferingPacketsCountIE sbpc_;
};

////////////////////////////////////////////////////////////////////////////////

/*
class GroupedIE : public IEHeader
{
public:

   int next_()
   {
      return (ie_._count < MAX_LIST_SIZE) ?
         ie_._count++ : -1;
   }

   __datatype__ &data() { return ie_; }

protected:
   GroupedIE(__datatype__ &ie, LengthCalculator *parent)
      : IEHeader(ie.header, , parent),
        ie_(ie),
   {
   }
   uint16_t calculateLength()
   {
      return 
   }

private:
   GroupedIE();
   __datatype__ &ie_;
};
*/

////////////////////////////////////////////////////////////////////////////////
//// Message Classes
////////////////////////////////////////////////////////////////////////////////

// class IE : public IEHeader
// {
// public:
    
//    __datatype__ &data() { return ie_; }

// protected:
//    IE(__datatype__ &ie)
//       : IEHeader(ie.header, ),
//         ie_(ie)
//    {
//    }
//    uint16_t calculateLength()
//    {
//       return sizeof(__datatype__) - sizeof(pfcp_ie_header_t);
//    }

// private:
//    IE();
//    __datatype__ &ie_;
// };

////////////////////////////////////////////////////////////////////////////////

class HeartbeatReq : public PFCP::AppMsgReq
{
public:
   HeartbeatReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   HeartbeatReq &sequenceNumber(uint32_t val);
   RecoveryTimeStampIE &recoveryTimeStamp();
   HeartbeatReq &encode(uint8_t *dest);
   pfcp_hrtbeat_req_t &data();

private:
   pfcp_hrtbeat_req_t data_;
   RecoveryTimeStampIE rts_;
};

////////////////////////////////////////////////////////////////////////////////

class HeartbeatRsp : public PFCP::AppMsgRsp
{
public:
   HeartbeatRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   HeartbeatRsp &sequenceNumber(uint32_t val);
   RecoveryTimeStampIE &recoveryTimeStamp();
   HeartbeatRsp &encode(uint8_t *dest);
   pfcp_hrtbeat_rsp_t &data();
private:
   pfcp_hrtbeat_rsp_t data_;
   RecoveryTimeStampIE rts_;
};

////////////////////////////////////////////////////////////////////////////////

class PfdMgmtReq : public PFCP::AppMsgReq
{
public:
   PfdMgmtReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   PfdMgmtReq &sequenceNumber(uint32_t val);
   ApplicationIdsPfdsIE &app_ids_pfds(uint8_t idx);
   PfdMgmtReq &encode(uint8_t *dest);
   pfcp_pfd_mgmt_req_t &data();
private:
   pfcp_pfd_mgmt_req_t data_;
   std::vector<ApplicationIdsPfdsIE> appids_;
};

////////////////////////////////////////////////////////////////////////////////

class PfdMgmtRsp : public PFCP::AppMsgRsp
{
public:
   PfdMgmtRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   PfdMgmtRsp &sequenceNumber(uint32_t val);
   CauseIE &cause();
   OffendingIeIE &offending_ie();
   PfdMgmtRsp &encode(uint8_t *dest);
   pfcp_pfd_mgmt_rsp_t &data();
private:
   pfcp_pfd_mgmt_rsp_t data_;
   CauseIE c_;
   OffendingIeIE oi_;
};

////////////////////////////////////////////////////////////////////////////////

class AssnSetupReq : public PFCP::AppMsgReq
{
public:
   AssnSetupReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   AssnSetupReq &sequenceNumber(uint32_t val);
   NodeIdIE &node_id();
   RecoveryTimeStampIE &rcvry_time_stmp();
   UpFunctionFeaturesIE &up_func_feat();
   CpFunctionFeaturesIE &cp_func_feat();
   UserPlaneIpResourceInformationIE &user_plane_ip_rsrc_info(uint8_t idx);
   int next_user_plane_ip_rsrc_info();
   AssnSetupReq &encode(uint8_t *dest);
   pfcp_assn_setup_req_t &data();
private:
   pfcp_assn_setup_req_t data_;
   NodeIdIE ni_;
   RecoveryTimeStampIE rts_;
   UpFunctionFeaturesIE uff_;
   CpFunctionFeaturesIE cff_;
   std::vector<UserPlaneIpResourceInformationIE> upiri_;
};

////////////////////////////////////////////////////////////////////////////////

class AssnSetupRsp : public PFCP::AppMsgRsp
{
public:
   AssnSetupRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   AssnSetupRsp &sequenceNumber(uint32_t val);
   NodeIdIE &node_id();
   CauseIE &cause();
   RecoveryTimeStampIE &rcvry_time_stmp();
   UpFunctionFeaturesIE &up_func_feat();
   CpFunctionFeaturesIE &cp_func_feat();
   UserPlaneIpResourceInformationIE &user_plane_ip_rsrc_info(uint8_t idx);
   int next_user_plane_ip_rsrc_info();
   AssnSetupRsp &encode(uint8_t *dest);
   pfcp_assn_setup_rsp_t &data();
private:
   pfcp_assn_setup_rsp_t data_;
   NodeIdIE ni_;
   CauseIE c_;
   RecoveryTimeStampIE rts_;
   UpFunctionFeaturesIE uff_;
   CpFunctionFeaturesIE cff_;
   std::vector<UserPlaneIpResourceInformationIE> upiri_;
};

////////////////////////////////////////////////////////////////////////////////

class AssnUpdateReq : public PFCP::AppMsgReq
{
public:
   AssnUpdateReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   AssnUpdateReq &sequenceNumber(uint32_t val);
   NodeIdIE &node_id();
   UpFunctionFeaturesIE &up_func_feat();
   CpFunctionFeaturesIE &cp_func_feat();
   AssociationReleaseRequestIE &up_assn_rel_req();
   GracefulReleasePeriodIE &graceful_rel_period();
   UserPlaneIpResourceInformationIE &user_plane_ip_rsrc_info(uint8_t idx);
   int next_user_plane_ip_rsrc_info();
   AssnUpdateReq &encode(uint8_t *dest);
   pfcp_assn_upd_req_t &data();
private:
   pfcp_assn_upd_req_t data_;
   NodeIdIE ni_;
   UpFunctionFeaturesIE uff_;
   CpFunctionFeaturesIE cff_;
   AssociationReleaseRequestIE arr_;
   GracefulReleasePeriodIE grp_;
   std::vector<UserPlaneIpResourceInformationIE> upiri_;
};

////////////////////////////////////////////////////////////////////////////////

class AssnUpdateRsp : public PFCP::AppMsgRsp
{
public:
   AssnUpdateRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   AssnUpdateRsp &sequenceNumber(uint32_t val);
   NodeIdIE &node_id();
   CauseIE &cause();
   UpFunctionFeaturesIE &up_func_feat();
   CpFunctionFeaturesIE &cp_func_feat();
   AssnUpdateRsp &encode(uint8_t *dest);
   pfcp_assn_upd_rsp_t &data();
private:
   pfcp_assn_upd_rsp_t data_;
   NodeIdIE ni_;
   CauseIE c_;
   UpFunctionFeaturesIE uff_;
   CpFunctionFeaturesIE cff_;
};

////////////////////////////////////////////////////////////////////////////////

class AssnReleaseReq : public PFCP::AppMsgReq
{
public:
   AssnReleaseReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   AssnReleaseReq &sequenceNumber(uint32_t val);
   NodeIdIE &node_id();
   AssnReleaseReq &encode(uint8_t *dest);
   pfcp_assn_rel_req_t &data();
private:
   pfcp_assn_rel_req_t data_;
   NodeIdIE ni_;
};

////////////////////////////////////////////////////////////////////////////////

class AssnReleaseRsp : public PFCP::AppMsgRsp
{
public:
   AssnReleaseRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   AssnReleaseRsp &sequenceNumber(uint32_t val);
   NodeIdIE &node_id();
   CauseIE &cause();
   AssnReleaseRsp &encode(uint8_t *dest);
   pfcp_assn_rel_rsp_t &data();
private:
   pfcp_assn_rel_rsp_t data_;
   NodeIdIE ni_;
   CauseIE c_;
};

////////////////////////////////////////////////////////////////////////////////

#define PFCP_VERSION_NOT_SUPPORTED (11)
class VersionNotSupportedRsp : public PFCP::AppMsgRsp
{
public:
   VersionNotSupportedRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   VersionNotSupportedRsp &sequenceNumber(uint32_t val);
   VersionNotSupportedRsp &encode(uint8_t *dest);
   pfcp_header_t &data();
private:
   pfcp_header_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class NodeReportReq : public PFCP::AppMsgReq
{
public:
   NodeReportReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   NodeReportReq &sequenceNumber(uint32_t val);
   NodeIdIE &node_id();
   NodeReportTypeIE &node_rpt_type();
   UserPlanePathFailureReportIE &user_plane_path_fail_rpt();
   NodeReportReq &encode(uint8_t *dest);
   pfcp_node_rpt_req_t &data();
private:
   pfcp_node_rpt_req_t data_;
   NodeIdIE ni_;
   NodeReportTypeIE nrt_;
   UserPlanePathFailureReportIE uprfr_;
};

////////////////////////////////////////////////////////////////////////////////

class NodeReportRsp : public PFCP::AppMsgRsp
{
public:
   NodeReportRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   NodeReportRsp &sequenceNumber(uint32_t val);
   NodeIdIE &node_id();
   CauseIE &cause();
   OffendingIeIE &offending_ie();
   NodeReportRsp &encode(uint8_t *dest);
   pfcp_node_rpt_rsp_t &data();
private:
   pfcp_node_rpt_rsp_t data_;
   NodeIdIE ni_;
   CauseIE c_;
   OffendingIeIE oi_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionSetDeletionReq : public PFCP::AppMsgReq
{
public:
   SessionSetDeletionReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionSetDeletionReq &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionSetDeletionReq &seid(uint64_t val);
   NodeIdIE &node_id();
   FqCsidIE &sgw_c_fqcsid();
   FqCsidIE &pgw_c_fqcsid();
   FqCsidIE &up_fqcsid();
   FqCsidIE &twan_fqcsid();
   FqCsidIE &epdg_fqcsid();
   FqCsidIE &mme_fqcsid();
   SessionSetDeletionReq &encode(uint8_t *dest);
   pfcp_sess_set_del_req_t &data();
private:
   pfcp_sess_set_del_req_t data_;
   NodeIdIE ni_;
   FqCsidIE sc_;
   FqCsidIE pc_;
   FqCsidIE u_;
   FqCsidIE t_;
   FqCsidIE e_;
   FqCsidIE m_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionSetDeletionRsp : public PFCP::AppMsgRsp
{
public:
   SessionSetDeletionRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionSetDeletionRsp &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionSetDeletionRsp &seid(uint64_t val);
   NodeIdIE &node_id();
   CauseIE &cause();
   OffendingIeIE &offending_ie();
   SessionSetDeletionRsp &encode(uint8_t *dest);
   pfcp_sess_set_del_rsp_t &data();
private:
   pfcp_sess_set_del_rsp_t data_;
   NodeIdIE ni_;
   CauseIE c_;
   OffendingIeIE oi_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionEstablishmentReq : public PFCP::AppMsgReq
{
public:
   SessionEstablishmentReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionEstablishmentReq &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionEstablishmentReq &seid(uint64_t val);
   NodeIdIE &node_id();
   FSeidIE &cp_fseid();
   CreateBarIE &create_bar();
   PdnTypeIE &pdn_type();
   FqCsidIE &sgw_c_fqcsid();
   FqCsidIE &mme_fqcsid();
   FqCsidIE &pgw_c_fqcsid();
   FqCsidIE &epdg_fqcsid();
   FqCsidIE &twan_fqcsid();
   UserPlaneInactivityTimerIE &user_plane_inact_timer();
   UserIdIE &user_id();
   TraceInformationIE &trc_info();
   ApnDnnIE &apn_dnn();
   CreatePdrIE &create_pdr(uint8_t idx);
   CreateFarIE &create_far(uint8_t idx);
   CreateUrrIE &create_urr(uint8_t idx);
   CreateQerIE &create_qer(uint8_t idx);
   CreateTrafficEndpointIE &create_traffic_endpt(uint8_t idx);
   int next_create_pdr();
   int next_create_far();
   int next_create_urr();
   int next_create_qer();
   int next_create_traffic_endpt();
   SessionEstablishmentReq &encode(uint8_t *dest);
   pfcp_sess_estab_req_t &data();
private:
   pfcp_sess_estab_req_t data_;
   NodeIdIE ni_;
   FSeidIE cpf_;
   std::vector<CreatePdrIE> cp_;
   std::vector<CreateFarIE> cf_;
   std::vector<CreateUrrIE> cu_;
   std::vector<CreateQerIE> cq_;
   CreateBarIE cb_;
   std::vector<CreateTrafficEndpointIE> cte_;
   PdnTypeIE pt_;
   FqCsidIE sfc_;
   FqCsidIE mfc_;
   FqCsidIE pfc_;
   FqCsidIE efc_;
   FqCsidIE tfc_;
   UserPlaneInactivityTimerIE upit_;
   UserIdIE ui_;
   TraceInformationIE ti_;
   ApnDnnIE ad_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionEstablishmentRsp : public PFCP::AppMsgRsp
{
public:
   SessionEstablishmentRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionEstablishmentRsp &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionEstablishmentRsp &seid(uint64_t val);
   NodeIdIE &node_id();
   CauseIE &cause();
   OffendingIeIE &offending_ie();
   FSeidIE &up_fseid();
   LoadControlInformationIE &load_ctl_info();
   OverloadControlInformationIE &ovrld_ctl_info();
   FqCsidIE &up_fqcsid();
   FailedRuleIdIE &failed_rule_id();
   CreatedPdrIE &created_pdr(uint8_t idx);
   CreatedTrafficEndpointIE &created_traffic_endpt(uint8_t idx);
   int next_created_pdr();
   int next_created_traffic_endpt();
   SessionEstablishmentRsp &encode(uint8_t *dest);
   pfcp_sess_estab_rsp_t &data();

private:
   pfcp_sess_estab_rsp_t data_;
   NodeIdIE ni_;
   CauseIE c_;
   OffendingIeIE oi_;
   FSeidIE ufs_;
   std::vector<CreatedPdrIE> cp_;
   LoadControlInformationIE lci_;
   OverloadControlInformationIE oci_;
   FqCsidIE ufc_;
   FailedRuleIdIE fri_;
   std::vector<CreatedTrafficEndpointIE> cte_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionModificationReq : public PFCP::AppMsgReq
{
public:
   SessionModificationReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionModificationReq &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionModificationReq &seid(uint64_t val);
   FSeidIE &cp_fseid();
   RemoveBarIE &remove_bar();
   RemoveTrafficEndpointIE &rmv_traffic_endpt();
   CreateBarIE &create_bar();
   CreateTrafficEndpointIE &create_traffic_endpt();
   UpdateBarSessionModificationReqIE &update_bar();
   UpdateTrafficEndpointIE &upd_traffic_endpt();
   PfcpSmReqFlagsIE &pfcpsmreq_flags();
   FqCsidIE &pgw_c_fqcsid();
   FqCsidIE &sgw_c_fqcsid();
   FqCsidIE &mme_fqcsid();
   FqCsidIE &epdg_fqcsid();
   FqCsidIE &twan_fqcsid();
   UserPlaneInactivityTimerIE &user_plane_inact_timer();
   QueryUrrReferenceIE &query_urr_ref();
   TraceInformationIE &trc_info();
   RemovePdrIE &remove_pdr(uint8_t idx);
   RemoveFarIE &remove_far(uint8_t idx);
   RemoveUrrIE &remove_urr(uint8_t idx);
   RemoveQerIE &remove_qer(uint8_t idx);
   CreatePdrIE &create_pdr(uint8_t idx);
   CreateFarIE &create_far(uint8_t idx);
   CreateUrrIE &create_urr(uint8_t idx);
   CreateQerIE &create_qer(uint8_t idx);
   UpdatePdrIE &update_pdr(uint8_t idx);
   UpdateFarIE &update_far(uint8_t idx);
   UpdateUrrIE &update_urr(uint8_t idx);
   UpdateQerIE &update_qer(uint8_t idx);
   QueryUrrIE &query_urr(uint8_t idx);
   int next_remove_pdr();
   int next_remove_far();
   int next_remove_urr();
   int next_remove_qer();
   int next_create_pdr();
   int next_create_far();
   int next_create_urr();
   int next_create_qer();
   int next_update_pdr();
   int next_update_far();
   int next_update_urr();
   int next_update_qer();
   int next_query_urr();
   SessionModificationReq &encode(uint8_t *dest);
   pfcp_sess_mod_req_t &data();
private:
   pfcp_sess_mod_req_t data_;
   FSeidIE cfs_;
   std::vector<RemovePdrIE> rp_;
   std::vector<RemoveFarIE> rf_;
   std::vector<RemoveUrrIE> ru_;
   std::vector<RemoveQerIE> rq_;
   RemoveBarIE rb_;
   RemoveTrafficEndpointIE rte_;
   std::vector<CreatePdrIE> cp_;
   std::vector<CreateFarIE> cf_;
   std::vector<CreateUrrIE> cu_;
   std::vector<CreateQerIE> cq_;
   CreateBarIE cb_;
   CreateTrafficEndpointIE cte_;
   std::vector<UpdatePdrIE> up_;
   std::vector<UpdateFarIE> uf_;
   std::vector<UpdateUrrIE> uu_;
   std::vector<UpdateQerIE> uq_;
   UpdateBarSessionModificationReqIE ub_;
   UpdateTrafficEndpointIE ute_;
   PfcpSmReqFlagsIE f_;
   std::vector<QueryUrrIE> qu_;
   FqCsidIE pcfc_;
   FqCsidIE scfc_;
   FqCsidIE mfc_;
   FqCsidIE efc_;
   FqCsidIE tfc_;
   UserPlaneInactivityTimerIE upit_;
   QueryUrrReferenceIE qur_;
   TraceInformationIE ti_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionModificationRsp : public PFCP::AppMsgRsp
{
public:
   SessionModificationRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionModificationRsp &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionModificationRsp &seid(uint64_t val);
   CauseIE &cause();
   OffendingIeIE &offending_ie();
   CreatedPdrIE &created_pdr();
   LoadControlInformationIE &load_ctl_info();
   OverloadControlInformationIE &ovrld_ctl_info();
   FailedRuleIdIE &failed_rule_id();
   AdditionalUsageReportsInformationIE &add_usage_rpts_info();
   CreatedTrafficEndpointIE &createdupdated_traffic_endpt();
   UsageReportSessionModificationRspIE &usage_report(uint8_t idx);
   int next_usage_report();
   SessionModificationRsp &encode(uint8_t *dest);
   pfcp_sess_mod_rsp_t &data();
private:
   pfcp_sess_mod_rsp_t data_;
   CauseIE c_;
   OffendingIeIE oi_;
   CreatedPdrIE cp_;
   LoadControlInformationIE lci_;
   OverloadControlInformationIE oci_;
   std::vector<UsageReportSessionModificationRspIE> ur_;
   FailedRuleIdIE fri_;
   AdditionalUsageReportsInformationIE auri_;
   CreatedTrafficEndpointIE cute_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionDeletionReq : public PFCP::AppMsgReq
{
public:
   SessionDeletionReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionDeletionReq &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionDeletionReq &seid(uint64_t val);
   SessionDeletionReq &encode(uint8_t *dest);
   pfcp_sess_del_req_t &data();
private:
   pfcp_sess_del_req_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionDeletionRsp : public PFCP::AppMsgRsp
{
public:
   SessionDeletionRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionDeletionRsp &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionDeletionRsp &seid(uint64_t val);
   CauseIE &cause();
   OffendingIeIE &offending_ie();
   LoadControlInformationIE &load_ctl_info();
   OverloadControlInformationIE &ovrld_ctl_info();
   UsageReportSessionDeletionRspIE &usage_report(uint8_t idx);
   int next_usage_report();
   SessionDeletionRsp &encode(uint8_t *dest);
   pfcp_sess_del_rsp_t &data();
private:
   pfcp_sess_del_rsp_t data_;
   CauseIE c_;
   OffendingIeIE oi_;
   LoadControlInformationIE lci_;
   OverloadControlInformationIE oci_;
   std::vector<UsageReportSessionDeletionRspIE> ur_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionReportReq : public PFCP::AppMsgReq
{
public:
   SessionReportReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn);
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionReportReq &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionReportReq &seid(uint64_t val);
   ReportTypeIE &report_type();
   DownlinkDataReportIE &dnlnk_data_rpt();
   ErrorIndicationReportIE &err_indctn_rpt();
   LoadControlInformationIE &load_ctl_info();
   OverloadControlInformationIE &ovrld_ctl_info();
   AdditionalUsageReportsInformationIE &add_usage_rpts_info();
   UsageReportSessionReportReqIE &usage_report(uint8_t idx);
   SessionReportReq &encode(uint8_t *dest);
   pfcp_sess_rpt_req_t &data();
private:
   pfcp_sess_rpt_req_t data_;
   ReportTypeIE rt_;
   DownlinkDataReportIE ddr_;
   std::vector<UsageReportSessionReportReqIE> ur_;
   ErrorIndicationReportIE eir_;
   LoadControlInformationIE lci_;
   OverloadControlInformationIE oci_;
   AdditionalUsageReportsInformationIE auri_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionReportRsp : public PFCP::AppMsgRsp
{
public:
   SessionReportRsp();
   uint16_t length() const;
   uint32_t sequenceNumber() const;
   SessionReportRsp &sequenceNumber(uint32_t val);
   uint64_t seid() const;
   SessionReportRsp &seid(uint64_t val);
   CauseIE &cause();
   OffendingIeIE &offending_ie();
   UpdateBarSessionReportRspIE &update_bar();
   PfcpSrRspFlagsIE &sxsrrsp_flags();
   SessionReportRsp &encode(uint8_t *dest);
   pfcp_sess_rpt_rsp_t &data();
private:
   pfcp_sess_rpt_rsp_t data_;
   CauseIE c_;
   OffendingIeIE oi_;
   UpdateBarSessionReportRspIE ub_;
   PfcpSrRspFlagsIE flags_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Translator : public PFCP::Translator
{
public:
   Translator();
   ~Translator();

   PFCP::ReqOutPtr encodeHeartbeatReq(PFCP::SndHeartbeatReqData &hb);
   PFCP::RspOutPtr encodeHeartbeatRsp(PFCP::SndHeartbeatRspData &hb);
   PFCP::RspOutPtr encodeVersionNotSupportedRsp(PFCP::ReqInPtr msg);

   PFCP::ReqOutPtr encodeReq(PFCP::AppMsgReqPtr msg);
   PFCP::RspOutPtr encodeRsp(PFCP::AppMsgRspPtr msg);
   PFCP::AppMsgReqPtr decodeReq(PFCP::ReqInPtr msg);
   PFCP::AppMsgRspPtr decodeRsp(PFCP::RspInPtr msg);
   PFCP::RcvdHeartbeatReqDataPtr decodeHeartbeatReq(PFCP::ReqInPtr msg);
   PFCP::RcvdHeartbeatRspDataPtr decodeHeartbeatRsp(PFCP::RspInPtr msg);

   Void getMsgInfo(PFCP::TranslatorMsgInfo &info, cpUChar msg, Int len);
   Bool isVersionSupported(UChar ver);
};

}

#endif