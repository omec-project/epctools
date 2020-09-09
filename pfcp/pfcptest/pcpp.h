/*
* Copyright (c) 2020 T-Mobile
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

#ifndef __pfcptest_pcpp_h_included
#define __pfcptest_pcpp_h_included

#include "Packet.h"
#include "Device.h"

#include "epfcp.h"
#include "estring.h"

#include <vector>

namespace PFCP_R15
{
   class Translator;
}

namespace PFCPTest
{
   PFCP_R15::Translator &GetTranslator();
   pcpp::RawPacketVector GetPackets(const EString &pcap);
   std::vector<uint8_t> ExtractPFCPPayload(const pcpp::Packet &packet);
   void ReplacePFCPPayload(pcpp::Packet &packet, const std::vector<uint8_t> &payload);
   std::unique_ptr<PFCP::AppMsg> DecodeAppMsg(const std::vector<uint8_t> &payload);
   std::vector<uint8_t> EncodeAppMsg(PFCP::AppMsgPtr appMsg);
   void WritePcap(const EString &pcap, pcpp::RawPacketVector &packets);
   bool ComparePackets(const EString &baselinePcap, const EString &resultPcap);
} // namespace PFCPTest

#endif // #define __pfcptest_pcpp_h_included