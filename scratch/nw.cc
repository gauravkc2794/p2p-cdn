/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <stdint.h>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  int numNodes = 8;

  CommandLine cmd;
  cmd.AddValue("numNodes", "Number of nodes in the network", numNodes);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpPeerClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (numNodes + 1);
  
  InternetStackHelper stack;
  stack.Install (nodes);

  PointToPointHelper p2ps[numNodes+1];
  for(int i = 0; i < numNodes; i++) {
		PointToPointHelper p2p;
		p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
		p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
		p2ps[i] = p2p;
  }

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  NetDeviceContainer devices;
  Ipv4Address serverAdd;
  for(int i = 0; i < numNodes; i++) {
		devices = p2ps[0].Install(nodes.Get(i), nodes.Get(numNodes));
		Ipv4InterfaceContainer interfaces = address.Assign(devices);
		if(i == 0) serverAdd = interfaces.GetAddress(0);
		std::cout << interfaces.GetAddress(0) << "\n";
        address.NewNetwork();		
  }
			

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();
  UdpCDNServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get(0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (100.0));

  for(int i = 1; i < numNodes; i++) {
  	UdpPeerClientHelper echoClient (serverAdd, 9);
	//__gnu_cxx::cout << list[i] << "\n";
  	echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  	echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    //echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
    //uint32_t temp=0;
    

  	ApplicationContainer clientApps = echoClient.Install (nodes.Get (i));
    echoClient.SetFill(clientApps.Get(0),"false;false;1000");
  	clientApps.Start (Seconds (10.0*i));
  	clientApps.Stop (Seconds (1000.0));
  }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

