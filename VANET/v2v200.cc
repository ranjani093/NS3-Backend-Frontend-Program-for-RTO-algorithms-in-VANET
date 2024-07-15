// ****************************************************************************************
//Self Organizing Networking Group (SONG) @ Kongu Engineering College (KEC) 
//kecsong2013@gmail.com		 kecsong@gmail.com
//*****************************************************************************************
// Program V : Creating TCP ON-OFF Application for VANET scenario (Mobility Model Included)

#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/aodv-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-module.h" 
#include "ns3/yans-wifi-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/internet-module.h"
#include "ns3/tcp-header.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/netanim-module.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("v2v");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
 // bool tracing = true;
  bool enableFlowMonitor = true;
  std::string phyMode ("OfdmRate27MbpsBW10MHz");
  std::string traceFile = "scratch/mob200.ns_movements";
  uint32_t maxBytes = 0;

  CommandLine cmd;

  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.AddValue ("maxBytes",
                "Total number of bytes for application to send", maxBytes);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
 // cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.Parse (argc,argv);
 Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpBbr"));



 // if (verbose)
 //   {
  //    LogComponentEnable ("TcpCubic", LOG_LEVEL_INFO);
//    }
  

  Ns2MobilityHelper ns2mobility = Ns2MobilityHelper (traceFile);
  NodeContainer vanetnodes;
  vanetnodes.Create (100);
  ns2mobility.Install ();


  // The below set of helpers will help us to put together the wifi NICs we want
  //YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetErrorRateModel ("ns3::NistErrorRateModel");
  //YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  
  //wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  YansWifiChannelHelper wifiChannel;
wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
wifiChannel.AddPropagationLoss ("ns3::NakagamiPropagationLossModel");

  //wifiPhy.SetChannel (wifiChannel.Create ());
  
  
  Ptr<YansWifiChannel> channel = wifiChannel.Create ();
  wifiPhy.SetChannel (channel);
  // ns-3 supports generate a pcap trace
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
  NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
   wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue (phyMode),
                                      "ControlMode",StringValue (phyMode));
  NetDeviceContainer vanetdevices = wifi80211p.Install (wifiPhy, wifi80211pMac, vanetnodes);


 // if (verbose)
    //{
    // wifi80211p.EnableLogComponents ();      // Turn on all Wifi 802.11p logging
    //}

  // Enable AODV
  AodvHelper aodv;
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv);
  stack.Install (vanetnodes);
     

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (vanetdevices);

 // Choosing Source and Destination Node for tcp traffic Flow 

  NS_LOG_INFO ("Create Applications.");
 // uint16_t port = 9;   
 
// 8 traffic
//1

  Ptr<Node> appSource1 = NodeList::GetNode (41);
  Ptr<Node> appSink1 = NodeList::GetNode (27);
  Ipv4Address remoteAddr1 = appSink1->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal ();

  BulkSendHelper source1 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr1, 1));
  source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer apps1 = source1.Install (vanetnodes.Get  (41));
  apps1.Start (Seconds (1));
  apps1.Stop (Seconds (1000));
  PacketSinkHelper sink1 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 1));
  apps1 = sink1.Install (vanetnodes.Get (27));
/*  
//2

  Ptr<Node> appSource2 = NodeList::GetNode (30);
  Ptr<Node> appSink2 = NodeList::GetNode (170);
  Ipv4Address remoteAddr2 = appSink2->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal ();

  BulkSendHelper source2("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr2,2));
  source2.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  
  ApplicationContainer apps2 = source2.Install (vanetnodes.Get  (30));
  apps2.Start (Seconds (1));
  apps2.Stop (Seconds (1000));
  PacketSinkHelper sink2 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (),2));
  apps2 = sink2.Install (vanetnodes.Get (170));
 
//3 
  
  Ptr<Node> appSource3 = NodeList::GetNode (3);
  Ptr<Node> appSink3 = NodeList::GetNode (86);
  Ipv4Address remoteAddr3 = appSink3->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal ();


  // Creating a socket and Linking Source Aplication with Destination 

  BulkSendHelper source3 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr3,3));
  source3.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  //source2.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps3 = source3.Install (vanetnodes.Get  (3));
  apps3.Start (Seconds (1));
  apps3.Stop (Seconds (1000));
  PacketSinkHelper sink3 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (),3));
  apps3 = sink3.Install (vanetnodes.Get (86));
  
//4
  Ptr<Node> appSource4 = NodeList::GetNode (23);
  Ptr<Node> appSink4 = NodeList::GetNode (141);
  Ipv4Address remoteAddr4 = appSink4->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal ();


  // Creating a socket and Linking Source Aplication with Destination 

  BulkSendHelper source4 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr4, 4));
  source4.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
 // source3.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps4 = source4.Install (vanetnodes.Get  (23));
  apps4.Start (Seconds (1));
  apps4.Stop (Seconds (1000));
  PacketSinkHelper sink4 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 4));
  apps4 = sink4.Install (vanetnodes.Get (141));

//5 
  Ptr<Node> appSource5 = NodeList::GetNode (139);
  Ptr<Node> appSink5 = NodeList::GetNode (216);
  Ipv4Address remoteAddr5 = appSink5->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal ();


  BulkSendHelper source5 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr5, 5));
  source5.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  //source4.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps5 = source5.Install (vanetnodes.Get  (139));
  apps5.Start (Seconds (1));
  apps5.Stop (Seconds (1000));
  PacketSinkHelper sink5 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 5));
  apps5 = sink5.Install (vanetnodes.Get (216));
 
//6
  Ptr<Node> appSource6 = NodeList::GetNode (229);
  Ptr<Node> appSink6 = NodeList::GetNode (244);
  Ipv4Address remoteAddr6 = appSink6->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal ();


  BulkSendHelper source6 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr6, 6));
  source6.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  //source5.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps6 = source6.Install (vanetnodes.Get  (229));
  apps6.Start (Seconds (1));
  apps6.Stop (Seconds (1000));
  PacketSinkHelper sink6 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 6));
  apps6 = sink6.Install (vanetnodes.Get (244));
 
//7
  Ptr<Node> appSource7 = NodeList::GetNode (46);
  Ptr<Node> appSink7 = NodeList::GetNode (164);
  Ipv4Address remoteAddr7 = appSink7->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal ();


  BulkSendHelper source7 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr7, 7));
  source7.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  //source6.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps7 = source7.Install (vanetnodes.Get  (46));
  apps7.Start (Seconds (1));
  apps7.Stop (Seconds (1000));
  PacketSinkHelper sink7 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 7));
  apps7 = sink7.Install (vanetnodes.Get (164));
 
//8
  Ptr<Node> appSource8 = NodeList::GetNode (166);
  Ptr<Node> appSink8 = NodeList::GetNode (209);
  Ipv4Address remoteAddr8 = appSink8->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal ();


  BulkSendHelper source8 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr8, 8));
  source8.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  //source7.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps8 = source8.Install (vanetnodes.Get  (166));
  apps8.Start (Seconds (1));
  apps8.Stop (Seconds (1000));
  PacketSinkHelper sink8 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 8));
  apps8 = sink8.Install (vanetnodes.Get (209));
*/
  //wifiPhy.EnablePcap ("v2vwb5_s40",vanetnodes.Get(40)->GetId(),0);
 // wifiPhy.EnablePcap ("v2vwb5_r24",vanetnodes.Get(24)->GetId(),0);
  //wifiPhy.EnablePcap ("v2vwb5_s6",vanetnodes.Get(6)->GetId(),0);
 // wifiPhy.EnablePcap ("v2vwb5_r9",vanetnodes.Get(9)->GetId(),0); 

 
// Tracing
  //wifiPhy.EnablePcap ("vanet-aodv-mobility", vanetdevices);
  
AnimationInterface anim("v2v200.xml");
// Installing Flow Monitor
  FlowMonitorHelper flowmonHelper;
 if (enableFlowMonitor)
    {
      flowmonHelper.InstallAll ();
    }
  Simulator::Stop (Seconds (1000));
  Simulator::Run ();

// Enabling the XML Flow Monitor
 if (enableFlowMonitor)
    {
      flowmonHelper.SerializeToXmlFile ("v2v200.flowmon", false, false);
    }
  Simulator::Destroy ();
  return 0;
}
