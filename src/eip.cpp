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

#include <regex>

#include "epctools.h"
#include "eutil.h"
#include "eip.h"

EIpFilterRule_InvalidAction::EIpFilterRule_InvalidAction(cpStr msg)
{
   setSevere();
   setText(msg);
}

EIpFilterRule_InvalidDirection::EIpFilterRule_InvalidDirection(cpStr msg)
{
   setSevere();
   setText(msg);
}

EIpFilterRule_InvalidProtocol::EIpFilterRule_InvalidProtocol(cpStr msg)
{
   setSevere();
   setText(msg);
}

EIpFilterRule_InvalidIpAddress::EIpFilterRule_InvalidIpAddress(cpStr msg)
{
   setSevere();
   setText(msg);
}

EIpFilterRule_InvalidMaskWidth::EIpFilterRule_InvalidMaskWidth(cpStr msg)
{
   setSevere();
   setText(msg);
}

EIpFilterRule_InvalidPort::EIpFilterRule_InvalidPort(cpStr msg)
{
   setSevere();
   setText(msg);
}

EIpFilterRule_InvalidOption::EIpFilterRule_InvalidOption(cpStr msg)
{
   setSevere();
   setText(msg);
}

EIpFilterRule_InvalidOptionMissingSpec::EIpFilterRule_InvalidOptionMissingSpec(cpStr msg)
{
   setSevere();
   setText(msg);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

EIpFilterRule &EIpFilterRule::parse(const std::string &raw)
{
   //
   // match[0] - the complete matched string
   // match[1] - action
   // match[2] - direction
   // match[3] - protocol
   // match[4] - source IP address with optional mask width
   // match[5] - optional source port/port range list
   // match[6] - destination IP address with optional mask width
   // match[7] - optional destination port/port range list
   // match[8] - list of options
   cpStr pattern = "(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+from\\s+(\\S+)(?:(?:\\s+)([0-9,\\-]+))?\\s+to\\s+(\\S+)(?:(?:\\s+)([0-9,\\-]+))?(?:(?:\\s+)(.*))?";
   std::regex re(pattern, std::regex::ECMAScript | std::regex::icase);
   std::sregex_iterator next(raw.begin(), raw.end(), re);
   std::sregex_iterator end;

   if (next != end)
   {
      std::smatch match = *next;
      setOriginal(match.str());
      setAction(parseAction(match[1].str()));
      setDirection(parseDirection(match[2].str()));
      setProtocol(parseProtocol(match[3].str()));
      setSource(parseIpAddress(match[4].str()));
      parsePorts(match[5].str(), srcPorts_);
      setDestination(parseIpAddress(match[6].str()));
      parsePorts(match[7].str(), dstPorts_);
      parseOptions(match[8].str());
   }

   return *this;
}

EIpFilterRule::Action EIpFilterRule::parseAction(const EString &str)
{
   EString strlower(str);
   strlower.tolower();
   if (strlower.compare("permit") == 0)
      return EIpFilterRule::Action::Permit;
   if (strlower.compare("deny") == 0)
      return EIpFilterRule::Action::Deny;
   throw EIpFilterRule_InvalidAction(str);
}

EIpFilterRule::Direction EIpFilterRule::parseDirection(const EString &str)
{
   EString strlower = str;
   strlower.tolower();
   if (strlower.compare("in") == 0)
      return EIpFilterRule::Direction::In;
   if (strlower.compare("out") == 0)
      return EIpFilterRule::Direction::Out;
   return EIpFilterRule::Direction::Undefined;
   throw EIpFilterRule_InvalidDirection(str);
}

EIpFilterRule::Protocol EIpFilterRule::parseProtocol(const EString &str)
{
   EString strlower = str;
   strlower.tolower();
   if (strlower.compare("ip") == 0)
      return EIpFilterRule::Protocol::IP;
   Int val = std::stoi(strlower);
   if (val < static_cast<Int>(EIpFilterRule::Protocol::ProtoMin) ||
       val > static_cast<Int>(EIpFilterRule::Protocol::ProtoMax))
      throw EIpFilterRule_InvalidProtocol(str);
   return static_cast<EIpFilterRule::Protocol>(val);
}

EIpAddress EIpFilterRule::parseIpAddress(const EString &str)
{
   static cpStr pattern = "([0-9a-f\\.:]+)(?:(?:/)(\\d+))?";
   EString strlower = str;
   strlower.tolower();
   if (strlower.compare("any") == 0)
      return EIpAddress().setAny(AF_INET);

   std::regex re(pattern, std::regex::ECMAScript | std::regex::icase);
   std::sregex_iterator next(str.begin(), str.end(), re);
   std::sregex_iterator end;

   if (next != end)
   {
      std::smatch match = *next;
      if (match.size() != 3)
         throw EIpFilterRule_InvalidIpAddress(str);
      EIpAddress addr(match[1].str());
      if (addr.family() == AF_UNSPEC)
         throw EIpFilterRule_InvalidIpAddress(str);
      EString mwstr = match[2].str();
      mwstr.trim();
      if (mwstr.size() > 0 && std::isdigit(mwstr[0]))
      {
         Int mw = std::stoi(mwstr);
         if (mw < 1 || mw > (addr.family() == AF_INET ? 32 : 128))
            throw EIpFilterRule_InvalidMaskWidth(str);
         addr.setMaskWidth(static_cast<UChar>(mw));
      }
      return addr;
   }
   throw EIpFilterRule_InvalidIpAddress(str);
}

Void EIpFilterRule::parsePorts(const EString &str, PortVec &ports)
{
   static cpStr pattern = "(\\d+)(?:-(\\d+))?";
   std::regex re(pattern, std::regex::ECMAScript, std::regex::icase);
   std::sregex_iterator end;
   EStringVec strs = EUtility::split(str, ",");

   ports.clear();
   for (EString &s : strs)
   {
      std::sregex_iterator next(s.begin(), s.end(), re);
      if (next != end)
      {
         std::smatch match = *next;
         if (match.size() != 3)
            throw EIpFilterRule_InvalidPort(str);
         UShort first = std::stoi(match[1]);
         UShort last = first;
         if (match[2].length() > 0)
            last = std::stoi(match[2]);
         ports.push_back(Port(first, last));
      }
   }
}

Void EIpFilterRule::parseOptions(const EString &str)
{
   EStringVec strs = EUtility::split(str, " ");

   for (size_t idx = 0; idx < strs.size(); idx++)
   {
      EString &s(strs[idx]);
      s.tolower();
      if (s.compare("frag") == 0)
      {
         options_.push_back(make_shared<Fragment>());
      }
      else if (s.compare("ipoptions") == 0)
      {
         idx++;
         if (idx >= strs.size())
            throw EIpFilterRule_InvalidOptionMissingSpec(str);
         options_.push_back(make_shared<IpOptions>(strs[idx]));
      }
      else if (s.compare("tcpoptions") == 0)
      {
         idx++;
         if (idx >= strs.size())
            throw EIpFilterRule_InvalidOptionMissingSpec(str);
         options_.push_back(make_shared<TcpOptions>(strs[idx]));
      }
      else if (s.compare("established") == 0)
      {
         options_.push_back(make_shared<Established>());
      }
      else if (s.compare("setup") == 0)
      {
         options_.push_back(make_shared<Setup>());
      }
      else if (s.compare("tcpflags") == 0)
      {
         idx++;
         if (idx >= strs.size())
            throw EIpFilterRule_InvalidOptionMissingSpec(str);
         options_.push_back(make_shared<TcpFlags>(strs[idx]));
      }
      else if (s.compare("icmptypes") == 0)
      {
         idx++;
         if (idx >= strs.size())
            throw EIpFilterRule_InvalidOptionMissingSpec(str);
         options_.push_back(make_shared<IcmpTypes>(strs[idx]));
      }
      else
      {
         throw EIpFilterRule_InvalidOption(s);
      }
   }
}

Void EIpFilterRule::dump(cpStr padding)
{
   EString str;

   std::cout << padding << "Original          - " << original() << std::endl;
   std::cout << padding << "Action            - ";
   switch (action())
   {
      case Action::Undefined: { std::cout << "undefined" << std::endl; break; }
      case Action::Permit:    { std::cout << "permit"    << std::endl; break; }
      case Action::Deny:      { std::cout << "deny"      << std::endl; break; }
      default:                { break; }
   }
   std::cout << padding << "Direction         - ";
   switch (direction())
   {
      case Direction::Undefined: { std::cout << "undefined" << std::endl; break; }
      case Direction::In:        { std::cout << "in"        << std::endl; break; }
      case Direction::Out:       { std::cout << "out"       << std::endl; break; }
      default:                   { break; }
   }
   std::cout << padding << "Protocol          - " << static_cast<Int>(protocol()) << std::endl;
   std::cout << padding << "Source            - " << source().address() << std::endl;
   std::cout << padding << "Source Ports      - ";
   Int cnt=0;
   for (auto port : sourcePorts())
   {
      std::cout << (cnt++?",":"") << port.first();
       if (port.first() != port.last())
         std::cout << "-" << port.last();
   }
   std::cout << std::endl;
   std::cout << padding << "Destination       - " << destination().address() << std::endl;
   std::cout << padding << "Destination Ports - ";
   cnt=0;
   for (auto port : destinationPorts())
   {
      std::cout << (cnt++?",":"") << port.first();
       if (port.first() != port.last())
         std::cout << "-" << port.last();
   }
   std::cout << std::endl;
   std::cout << padding << "Options           - " << std::endl;
   for (auto option : options())
   {
      std::cout << padding << "  ";
      switch (option->type())
      {
         case EIpFilterRule::OptionType::frag:        { std::cout << "type=Fragment"; break; }
         case EIpFilterRule::OptionType::ipoptions:   { std::cout << "type=IpOptions spec=" << std::dynamic_pointer_cast<EIpFilterRule::IpOptions>(option)->spec(); break; }
         case EIpFilterRule::OptionType::tcpoptions:  { std::cout << "type=TcpOptions spec=" << std::dynamic_pointer_cast<EIpFilterRule::TcpOptions>(option)->spec(); break; }
         case EIpFilterRule::OptionType::established: { std::cout << "type=Established"; break; }
         case EIpFilterRule::OptionType::setup:       { std::cout << "type=Setup"; break; }
         case EIpFilterRule::OptionType::tcpflags:    { std::cout << "type=TcpFlags flags=" << std::dynamic_pointer_cast<EIpFilterRule::TcpFlags>(option)->flags(); break; }
         case EIpFilterRule::OptionType::icmptypes:   { std::cout << "type=IcmpTypes types=" << std::dynamic_pointer_cast<EIpFilterRule::IcmpTypes>(option)->types(); break; }
         default:                                     { break; }
      }
      std::cout << std::endl;
   }
   std::cout << padding << "Final             - " << final() << std::endl;
   std::cout << padding << "Hash              - " << std::hash<EIpFilterRule>{}(*this) << std::endl;
}

EString EIpFilterRule::final()
{
   Int cnt;
   std::stringstream ss;

   switch (action())
   {
      case Action::Undefined: { ss << "undefined "; break; }
      case Action::Permit:    { ss << "permit"; break; }
      case Action::Deny:      { ss << "deny"; break; }
      default:                { break; }
   }
   switch (direction())
   {
      case Direction::Undefined: { ss << " undefined"; break; }
      case Direction::In:        { ss << " in"; break; }
      case Direction::Out:       { ss << " out"; break; }
      default:                   { break; }
   }
   if (protocol() == EIpFilterRule::Protocol::IP)
      ss << " ip";
   else
      ss << " " << static_cast<Int>(protocol());
   ss << " from " << source().address();
   cnt = 0;
   for (auto port : sourcePorts())
   {
      ss << (cnt++?",":" ") << port.first();
      if (port.first() != port.last())
         ss << "-" << port.last();
   }
   ss << " to " << destination().address();
   cnt = 0;
   for (auto port : destinationPorts())
   {
      ss << (cnt++?",":" ") << port.first();
      if (port.first() != port.last())
         ss << "-" << port.last();
   }

   for (auto option : options())
   {
      switch (option->type())
      {
         case EIpFilterRule::OptionType::frag:        { ss << " frag"; break; }
         case EIpFilterRule::OptionType::ipoptions:   { ss << " ipoptions " << std::dynamic_pointer_cast<EIpFilterRule::IpOptions>(option)->spec(); break; }
         case EIpFilterRule::OptionType::tcpoptions:  { ss << " tcpoptions " << std::dynamic_pointer_cast<EIpFilterRule::TcpOptions>(option)->spec(); break; }
         case EIpFilterRule::OptionType::established: { ss << " established"; break; }
         case EIpFilterRule::OptionType::setup:       { ss << " setup"; break; }
         case EIpFilterRule::OptionType::tcpflags:    { ss << " tcpflags " << std::dynamic_pointer_cast<EIpFilterRule::TcpFlags>(option)->flags(); break; }
         case EIpFilterRule::OptionType::icmptypes:   { ss << " icmptypes " << std::dynamic_pointer_cast<EIpFilterRule::IcmpTypes>(option)->types(); break; }
         default:                                     { break; }
      }
   }

   return EString(ss.str());
}
