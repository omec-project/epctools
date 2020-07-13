#include <stdlib.h>
#include <vector>
#include <signal.h>

#include "Packet.h"
#include "UdpLayer.h"
#include "PcapFileDevice.h"
#include "pfcpr15.h"
#include "epctools.h"
#include "epfcp.h"
#include"PayloadLayer.h"

#define LOG_SYSTEM 1
#define LOG_PFCP 2

void ParsePcap()
{
   // use the IFileReaderDevice interface to automatically identify file type (pcap/pcap-ng)
   // and create an interface instance that both readers implement
   pcpp::IFileReaderDevice *reader = pcpp::IFileReaderDevice::getReader("pfcp-pfd-management-request-baseline.pcap");

   // verify that a reader interface was indeed created
   if (reader == NULL)
      throw EError(EError::Error, errno, "Cannot determine reader for file type");

   // open the reader for reading
   if (!reader->open())
      throw EError(EError::Error, errno, "Cannot open input pcap for reading");

   // read the first (and only) packet from the file
   pcpp::RawPacket rawPacket;
   if (!reader->getNextPacket(rawPacket))
      throw EError(EError::Error, errno, "Couldn't read the first packet in the file");

   // close the file reader, we don't need it anymore
   reader->close();

   // parse the raw packet into a parsed packet
   pcpp::Packet parsedPacket(&rawPacket);

   pcpp::UdpLayer *udpLayer = parsedPacket.getLayerOfType<pcpp::UdpLayer>();
   if (udpLayer == nullptr)
      throw EError(EError::Error, errno, "Something went wrong, couldn't find UDP layer");

   std::vector<uint8_t> payload;
   payload.assign(udpLayer->getLayerPayload(), udpLayer->getLayerPayload() + udpLayer->getLayerPayloadSize());

   // Dest port in UDP packet 8085 is PFCP

   PFCP_R15::Translator trans;

   PFCP::TranslatorMsgInfo info;
   trans.getMsgInfo(info, payload.data(), payload.size());

   PFCP::ReqInPtr msg_in = new PFCP::ReqIn();
   msg_in->setSeid(info.seid());
   msg_in->setSeqNbr(info.seqNbr());
   msg_in->setMsgType(info.msgType());
   msg_in->setIsReq(info.isReq());
   msg_in->setVersion(info.version());
   msg_in->assign(payload.data(), payload.size());

   PFCP::AppMsgReqPtr app_msg = trans.decodeReq(msg_in);

   switch(app_msg->msgType()) {
      case PFCP_PFD_MGMT_REQ: {
         PFCP_R15::PfdMgmtReq &pfd_mgmt_req = *(static_cast<PFCP_R15::PfdMgmtReq *>(app_msg));
         ELogger::log(LOG_PFCP).info("Parsed PfdMgmtReq");
         // // TEMP fixup
         // pfd_mgmt_req.data().header.message_len += 1;
         // pfd_mgmt_req.data().app_ids_pfds[0].header.len += 1;
         // pfd_mgmt_req.data().app_ids_pfds[0].pfd_context[0].header.len += 1;
         // pfd_mgmt_req.data().app_ids_pfds[0].pfd_context[0].pfd_contents[0].header.len += 1;
         break;
      }
      default:
         ELogger::log(LOG_PFCP).major("Unhandled PFCP message type {}", app_msg->msgType());
   }

   PFCP::ReqOutPtr msg_out = trans.encodeReq(app_msg);
   
   std::vector<uint8_t> payload_out;
   payload_out.assign(msg_out->data(), msg_out->data() + msg_out->len());

   parsedPacket.removeAllLayersAfter(udpLayer);

   pcpp::PayloadLayer *new_layer = new pcpp::PayloadLayer(payload_out.data(), payload_out.size(), false);
   parsedPacket.addLayer(new_layer, true);

   // compute all calculated fields
   parsedPacket.computeCalculateFields();

   // write the modified packet to a pcap file
   pcpp::PcapFileWriterDevice writer("pfcp-pfd-management-request-baseline2.pcap", parsedPacket.getRawPacket()->getLinkLayerType());
   writer.open();
   writer.writePacket(*(parsedPacket.getRawPacket()));
   writer.close();
}

Void usage()
{
   std::cout << "USAGE:  Tutorial-PacketParsing [--help] [--file optionfile]" << std::endl;
}

int main(int argc, char *argv[])
{
   EGetOpt::Option options[] = {
       {"-h", "--help", EGetOpt::no_argument, EGetOpt::dtNone},
       {"-f", "--file", EGetOpt::required_argument, EGetOpt::dtString},
       {"", "", EGetOpt::no_argument, EGetOpt::dtNone},
   };

   EGetOpt opt;
   EString optfile;

   try
   {
      opt.loadCmdLine(argc, argv, options);
      if (opt.getCmdLine("-h,--help", false))
      {
         usage();
         return 0;
      }

      optfile.format("%s.json", argv[0]);
      if (EUtility::file_exists(optfile))
         opt.loadFile(optfile);

      optfile = opt.getCmdLine("-f,--file", "__unknown__");
      if (EUtility::file_exists(optfile))
         opt.loadFile(optfile);

      if (opt.getCmdLine("-p,--print", false))
         opt.print();
   }
   catch (const std::exception &e)
   {
      std::cerr << e.what() << std::endl;
      return 1;
   }

   try
   {
      EpcTools::Initialize(opt);
      ELogger::log(LOG_SYSTEM).startup("EpcTools initialization complete");

      try
      {
         PFCP::Configuration::setLogger(ELogger::log(LOG_PFCP));

         ParsePcap();
      }
      catch (const std::exception &e)
      {
         ELogger::log(LOG_SYSTEM).major(e.what());
      }

      ELogger::log(LOG_SYSTEM).startup("Shutting down EpcTools");
      EpcTools::UnInitialize();
   }
   catch (const std::exception &e)
   {
      std::cerr << e.what() << std::endl;
      return 2;
   }

   return 0;
}