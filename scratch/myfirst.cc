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
#include "ns3/network-module.h"
#include "ns3/ipv4.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

    int
main (int argc, char *argv[])
{
    CommandLine cmd;
    cmd.Parse (argc, argv);
    if(argc < 2){
        cout<<"not enough parameters\n";
        exit(-1);
    }
    int num_nodes = atoi(argv[1]);
    if (num_nodes == 0)
    {
        cout<<"parameter error\n";
        exit(-1);
    }
    Time::SetResolution (Time::NS);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    vector<Ptr<Node>> nodeList;
    for(int j=0;j<num_nodes;j++){
        Ptr<Node> node = CreateObject<Node>();
        InternetStackHelper stack;
        stack.Install(node);
        if(j==0){
            UdpEchoServerHelper echoServer (9);
            ApplicationContainer serverApps = echoServer.Install (node);
            serverApps.Start (Seconds (1.0));
            serverApps.Stop (Seconds (30.0));
        }
        nodeList.push_back(node);
    }

    for(int j=0;j<num_nodes;j++)
        cout << nodeList.at(j)->GetId()<<"\n";

    for(int i=0;i<num_nodes;i++){
        for(int j=i+1;j<num_nodes;j++){

            NodeContainer node_container;
            node_container.Add(nodeList.at(i));
            node_container.Add(nodeList.at(j));

            PointToPointHelper p2p_channel_helper;
            p2p_channel_helper.SetDeviceAttribute("DataRate", StringValue ("5Mbps"));
            p2p_channel_helper.SetChannelAttribute("Delay", StringValue ("2ms"));

            NetDeviceContainer nd_container;
            nd_container = p2p_channel_helper.Install(node_container);
            Ipv4InterfaceContainer interfaces = address.Assign (nd_container);
        }
    }
    /*        if(i!=0 && j!=0){
                if(nodeList.at(i)->GetNApplications() == 0){
                    ApplicationContainer clientApps = echoClient.Install (nodeList.at(i));
                    clientApps.Start (Seconds (2.0));
                    clientApps.Stop (Seconds (10.0));
                }

                if(nodeList.at(j)->GetNApplications() == 0){
                    ApplicationContainer clientApps = echoClient.Install (nodeList.at(j));
                    clientApps.Start (Seconds (2.0));
                    clientApps.Stop (Seconds (10.0));
                }
            }
            else if(i!=0){
                if(nodeList.at(i)->GetNApplications() == 0){
                    UdpEchoClientHelper echoClient (interfaces.GetAddress(1), 9);
                    echoClient.SetAttribute ("MaxPackets", UintegerValue (3));
                    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1)));
                    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

                    ApplicationContainer clientApps = echoClient.Install (nodeList.at(i));
                    clientApps.Start (Seconds (2.0+j));
                    clientApps.Stop (Seconds (20.0));
                }
            }
            else{
        */
        cout << "NumDevs: " << nodeList[0]->GetNDevices() << "\n";
        for(int j=1;j<num_nodes;j++){
                    UdpEchoClientHelper echoClient (Ipv4Address("10.1.1.1"), 9);
                    echoClient.SetAttribute ("MaxPackets", UintegerValue (3));
                    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1)));
                    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

                    ApplicationContainer clientApps = echoClient.Install (nodeList.at(j));
                    clientApps.Start (Seconds (2.0+j));
                    clientApps.Stop (Seconds (20.0));
            
        }
    /*
       ifstream infile;
       infile.open(argv[1]);
       if(!infile){
       cout<<"Unable to open file\n";
       exit(-1);
       }
       cout<<argv[1]<<"\n";
       int num_nodes = count(istreambuf_iterator<char>(infile),istreambuf_iterator<char>(),'\n');
       cout << "number of nodes being added : "<< num_nodes << "\n";*/

    /*NodeContainer n0n1_node_container;
      n0n1_node_container.Create (2);

      NodeContainer n0n2_node_container;
      n0n2_node_container.Create (2);

      NodeContainer n0n3_node_container;
      n0n3_node_container.Create (2);

      NodeContainer n1n2_node_container;
      n1n2_node_container.Create (2);

      NodeContainer n1n3_node_container;
      n1n3_node_container.Create (2);

      NodeContainer n2n3_node_container;
      n2n3_node_container.Create (2);


      PointToPointHelper n0n1;
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

      PointToPointHelper n0n2;
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

      PointToPointHelper n0n3;
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

      PointToPointHelper n1n2;
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

      PointToPointHelper n1n3;
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

      PointToPointHelper n2n3;
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));



      NetDeviceContainer n0n1_nd_container;
      devices = n0n1_nd_container.Install (nodes);

      InternetStackHelper stack;
      stack.Install (nodes);
      */

    /*UdpEchoServerHelper echoServer (9);

    ApplicationContainer serverApps = echoServer.Install (nodes.Get (0));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (20.0));
    for(int i=1;i<5;i++){
        UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
        echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
        echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
        echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

        ApplicationContainer clientApps = echoClient.Install (nodes.Get (i));
        clientApps.Start (Seconds (2.0));
        clientApps.Stop (Seconds (10.0));
    }
*/
    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
